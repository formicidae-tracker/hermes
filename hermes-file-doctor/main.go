package main

import (
	"compress/gzip"
	"fmt"
	"io"
	"log/slog"
	"os"
	"path/filepath"

	"github.com/formicidae-tracker/hermes/src/go/hermes"
	"github.com/golang/protobuf/proto"
	"github.com/jessevdk/go-flags"
)

// TODO: flag to cut on error, or check error path for each segment.
// TODO: checks support for uncompressed files.
// TODO: unit tests ?

type LineFixer func(*hermes.FileLine) (bool, error)
type HeaderFixer func(*hermes.Header) error

func PrintHeader(h *hermes.Header) error {
	slog.Info("show data", "header", h)
	return nil
}

func PrintLine(l *hermes.FileLine) (bool, error) {
	slog.Info("show data", "line", l)
	return true, nil
}

type Int32TimestampFixer struct {
	last      *int64
	timestamp int64
}

func (f *Int32TimestampFixer) fixLine(l *hermes.FileLine) (bool, error) {

	if l.Readout == nil {
		return true, nil
	}

	if f.last == nil {
		f.last = new(int64)
		*f.last = l.Readout.Timestamp
		f.timestamp = l.Readout.Timestamp
		if f.timestamp < 0 {
			f.timestamp = 0
			l.Readout.Timestamp = 0
			return true, fmt.Errorf("Frame %d starts with a negative timestamp, forcing it to 0", l.Readout.FrameID)
		}
		return true, nil
	}

	diff := uint32(l.Readout.Timestamp) - uint32(*f.last)
	*f.last = l.Readout.Timestamp
	f.timestamp += int64(diff)

	if f.timestamp != l.Readout.Timestamp {
		err := fmt.Errorf("Wrong timestamp %d (previous: %d, udiff: %d), rewritting it to %d",
			l.Readout.Timestamp, *f.last, diff, f.timestamp)
		l.Readout.Timestamp = f.timestamp
		return true, err
	}

	return true, nil
}

type App struct {
	Verbose  bool `short:"v" long:"verbose" description:"verbose mode"`
	DryRun   bool `short:"n" long:"dry-run" description:"Do not modify the file, and just report error"`
	FixInt32 bool `long:"fix-int32-timestamp" description:"Fix int32 timestamp"`
	Args     struct {
		File *flags.Filename
	} `positional-args:"yes" required:"true"`

	headerFixers []HeaderFixer
	lineFixers   []LineFixer
}

type gzipFile struct {
	compressed   io.ReadCloser
	uncompressed io.ReadCloser
}

func (g *gzipFile) Read(p []byte) (int, error) {
	return g.uncompressed.Read(p)
}

func (g *gzipFile) Close() error {
	g.uncompressed.Close()
	return g.compressed.Close()
}

func openGzip(filename string) (io.ReadCloser, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}

	uncomp, err := gzip.NewReader(file)
	if err != nil {
		file.Close()
		return nil, err
	}
	return &gzipFile{compressed: file, uncompressed: uncomp}, nil
}

func copyFile(in, out string) error {
	input, err := os.ReadFile(in)
	if err != nil {
		return err
	}
	return os.WriteFile(out, input, 0644)
}

type hermesFileRewriter struct {
	filename string
	header   *hermes.Header
	lines    []*hermes.FileLine
	logger   *slog.Logger
}

var toRewrite *hermesFileRewriter = nil

func pushRewrite(filename string, header *hermes.Header, lines []*hermes.FileLine, logger *slog.Logger) {
	var err error
	if toRewrite != nil {
		err = toRewrite.rewrite()
		if err != nil {
			toRewrite.logger.Error("rewrite error", "error", err)
		}
		toRewrite = nil
	}
	toRewrite = &hermesFileRewriter{
		filename: filename,
		header:   header,
		lines:    lines,
		logger:   logger,
	}
}

func (w *hermesFileRewriter) rewrite() error {
	dest := w.filename + ".bak"
	w.logger.Info("backuping", "dest", dest)
	err := copyFile(w.filename, dest)
	if err != nil {
		return err
	}

	w.logger.Info("rewriting", "lines", len(w.lines))
	file, err := os.Create(w.filename)
	if err != nil {
		return err
	}
	defer file.Close()
	compressed := gzip.NewWriter(file)
	defer compressed.Close()
	b := proto.NewBuffer(nil)
	err = b.EncodeMessage(w.header)
	if err != nil {
		return err
	}
	_, err = compressed.Write(b.Bytes())
	if err != nil {
		return err
	}

	for _, l := range w.lines {
		b = proto.NewBuffer(nil)
		err = b.EncodeMessage(l)
		if err != nil {
			return err
		}
		_, err := compressed.Write(b.Bytes())
		if err != nil {
			return err
		}
	}
	return nil
}

func (a App) RunForFile(filename string, file io.ReadCloser) error {
	defer file.Close()
	logger := slog.With("file", filename)
	logger.Info("start reading")
	h := &hermes.Header{}
	ok, err := hermes.ReadDelimitedMessage(file, h)
	if ok == false || err != nil {
		if err == nil {
			err = fmt.Errorf("Empty file")
		}
		return err
	}

	good := true
	for _, hf := range a.headerFixers {
		err := hf(h)
		if err != nil {
			logger.Warn("header error", "error", err)
			good = false
		}
	}

	lines := []*hermes.FileLine{}
	var nextFilename string
	var next io.ReadCloser
	for {
		line := &hermes.FileLine{}
		ok := true
		ok, err = hermes.ReadDelimitedMessage(file, line)
		if ok == false || err != nil {
			if err == nil {
				err = fmt.Errorf("Could not read line")
			}
			break
		}
		lineLogger := logger
		if line.Readout != nil {
			lineLogger = logger.With("FrameID", line.Readout.FrameID)
		}

		keep := true
		for _, f := range a.lineFixers {
			keepIt, err := f(line)

			if err != nil {
				good = false
				lineLogger.Warn("line error", "error", err)
			}
			if keepIt == false {
				keep = false
			}
		}
		if keep == true {
			lines = append(lines, line)
		}

		if line.Footer == nil {
			continue
		}
		if len(line.Footer.Next) == 0 {
			break
		}

		file.Close()

		nextFilename = filepath.Join(filepath.Dir(filename), line.Footer.Next)
		next, err = openGzip(nextFilename)
		if err != nil {
			line.Footer.Next = ""
			good = false
			next.Close()
		}
		break
	}

	if good == false && a.DryRun == false {
		pushRewrite(filename, h, lines, logger)
	}

	if nextFilename != "" && next != nil {
		defer next.Close()
		return a.RunForFile(nextFilename, next)
	}

	return nil
}

func Execute() error {
	a := &App{
		headerFixers: []HeaderFixer{},
		lineFixers:   []LineFixer{},
	}
	if _, err := flags.Parse(a); err != nil {
		return err
	}

	if a.Verbose == true {
		a.headerFixers = append(a.headerFixers, PrintHeader)
		a.lineFixers = append(a.lineFixers, PrintLine)
		//add something
	}

	if a.FixInt32 == true {
		f := Int32TimestampFixer{}
		a.lineFixers = append(a.lineFixers, f.fixLine)
	}

	file, err := openGzip(string(*a.Args.File))
	if err != nil {
		return err
	}
	return a.RunForFile(string(*a.Args.File), file)
}
func main() {

	if err := Execute(); err != nil {
		slog.Error("Unhandled error", "error", err)
		os.Exit(1)
	}
}
