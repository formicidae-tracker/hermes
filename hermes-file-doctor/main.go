package main

import (
	"compress/gzip"
	"errors"
	"fmt"
	"io"
	"log/slog"
	"os"
	"path"
	"strings"
	"time"

	"github.com/formicidae-tracker/hermes/src/go/hermes"
	"github.com/jessevdk/go-flags"
	"github.com/lmittmann/tint"
)

// TODO: flag to cut on error, or check error path for each segment.
// TODO: checks support for uncompressed files.
// TODO: unit tests ?

type LineFixer func(*hermes.FileLine) (bool, bool, error)
type HeaderFixer func(*hermes.Header) error

func PrintHeader(h *hermes.Header) error {
	slog.Info("show data", "header", h)
	return nil
}

func PrintLine(l *hermes.FileLine) (bool, bool, error) {
	slog.Info("show data", "line", l)
	return true, false, nil
}

type Int32TimestampFixer struct {
	last      *int64
	timestamp int64
	logged    bool
}

func (f *Int32TimestampFixer) fixLine(l *hermes.FileLine) (keep bool, shouldLog bool, err error) {

	defer func() {
		if err == nil {
			return
		}
		if f.logged == false {
			shouldLog = true
			f.logged = true
			slog.Warn("all subsequent timestamp will be modified")
		}
	}()

	if l.Readout == nil {
		return true, false, nil
	}

	if f.last == nil {
		f.last = new(int64)
		*f.last = l.Readout.Timestamp
		f.timestamp = l.Readout.Timestamp
		if f.timestamp <= 0 {
			f.timestamp = 1
			l.Readout.Timestamp = 1
			return true, false, fmt.Errorf("Frame %d starts with a negative or null timestamp, forcing it to 1", l.Readout.FrameID)

		}
		return true, false, nil
	}
	prev := uint32(*f.last)
	diff := uint32(l.Readout.Timestamp) - prev
	f.timestamp += int64(diff)
	*f.last = l.Readout.Timestamp

	if f.timestamp != l.Readout.Timestamp {
		err := fmt.Errorf("Wrong timestamp %d (previous: %d, udiff: %d), rewritting it to %d",
			l.Readout.Timestamp, prev, diff, f.timestamp)
		l.Readout.Timestamp = f.timestamp
		return true, false, err
	}

	return true, false, nil
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

func UncompressedFilename(filename string) string {

	basename := path.Base(filename)
	if strings.HasPrefix(basename, "uncompressed-") == true {
		return filename
	}
	dirname := path.Dir(filename)
	return path.Join(dirname, "uncompressed-"+basename)
}

func openSegment(filename string) (io.ReadCloser, error) {
	f, err := os.Open(UncompressedFilename(filename))
	if err == nil {
		return f, nil
	}

	return openGzip(filename)
}

func (a App) RunForFile(fs *FileSequence, i int) error {
	name := fs.Segments[i].Name
	filepath := path.Join(fs.dirpath, fs.Segments[i].Name)

	file, err := openSegment(filepath)
	if err != nil {
		return err
	}

	defer file.Close()
	defer fs.Persist()
	logger := slog.With("file", fs.Segments[i].Name)
	logger.Info("start reading")

	h := &hermes.Header{}

	ok, err := hermes.ReadDelimitedMessage(file, h)
	if ok == false || err != nil {
		if err == nil {
			err = fmt.Errorf("Empty file")
		}
		return err
	}
	fixed := 0

	for _, hf := range a.headerFixers {
		err := hf(h)
		if err != nil {
			logger.Warn("header error", "error", err)
			fixed += 1
		}
	}

	lines := []*hermes.FileLine{}

	fs.ResetSegment(i)
	lineIdx := -1
	hasFooter := false
	for {
		line := &hermes.FileLine{}
		ok := true
		ok, err = hermes.ReadDelimitedMessage(file, line)
		if ok == false && err == nil {
			err = fmt.Errorf("Could not read line")
		}

		if errors.Is(err, io.EOF) {
			err = nil
			break
		}

		if err != nil {
			logger.Error("could not read complete file", "error", err)
			logger.Info("terminating file", "lines", lineIdx+1)
			lines = append(lines, &hermes.FileLine{
				Readout: nil,
				Footer: &hermes.Footer{
					Next: fs.Segments[i].Next,
				},
			})
			fixed += 1
			break
		}

		lineLogger := logger
		if line.Readout != nil {
			lineLogger = logger.With("FrameID", line.Readout.FrameID)
			lineIdx += 1
			fs.Update(name, lineIdx, line.Readout.FrameID)
		}

		keep := true
		for _, f := range a.lineFixers {
			keepIt, shouldLog, err := f(line)
			if err != nil {
				if shouldLog == true {
					lineLogger.Error("line error", "error", err)
				}
				fixed += 1
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

		hasFooter = true
		expectedNext := fs.Segments[i].Next

		if line.Footer.Next != expectedNext {

			lineLogger.Error("missing next footer", "expectedNext", expectedNext)
			fixed += 1
			line.Footer.Next = expectedNext
		}

		break
	}

	if hasFooter == false {
		logger.Error("segment is missing a footer")
		lines = append(lines, &hermes.FileLine{
			Footer: &hermes.Footer{
				Next: fs.Segments[i].Next,
			},
		})
		fixed += 1
	}

	logger.Info("segment read", "lines", lineIdx+1, "fixes", fixed)

	if fixed > 0 && a.DryRun == false {
		PushRewrite(filepath, h, lines, logger)
	}

	return nil
}

func Execute() error {

	slog.SetDefault(slog.New(
		tint.NewHandler(os.Stderr, &tint.Options{
			Level:      slog.LevelDebug,
			TimeFormat: time.TimeOnly,
		}),
	))

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

	fs, err := NewHermesFileSequence(string(*a.Args.File))
	if err != nil {
		return err
	}

	defer PopRewrite()
	defer fs.Persist()

	var errs error
	for i := range fs.Segments {
		if err := a.RunForFile(fs, i); err != nil {
			errs = errors.Join(errs, fmt.Errorf("while repairing '%s': %w", path.Join(fs.dirpath, fs.Segments[i].Name), err))
		}
	}

	return errs
}
func main() {

	if err := Execute(); err != nil {
		slog.Error("Unhandled error", "error", err)
		os.Exit(1)
	}
}
