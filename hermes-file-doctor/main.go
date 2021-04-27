package main

import (
	"compress/gzip"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"

	"github.com/formicidae-tracker/hermes/src/go/hermes"
	"github.com/golang/protobuf/proto"
	"github.com/jessevdk/go-flags"
)

type LineFixer func(*hermes.FileLine) (bool, error)
type HeaderFixer func(*hermes.Header) error

func PrintHeader(h *hermes.Header) error {
	log.Printf("Got Header %+v", h)
	return nil
}

func PrintLine(l *hermes.FileLine) (bool, error) {
	log.Printf("Got Line %+v", l)
	return true, nil
}

type App struct {
	Verbose bool `short:"v" long:"verbose" description:"verbose mode"`
	DryRun  bool `short:"n" long:"dry-run" description:"Do not modify the file, and just report error"`
	Args    struct {
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
	input, err := ioutil.ReadFile(in)
	if err != nil {
		return err
	}
	return ioutil.WriteFile(out, input, 0644)
}

func writeHermesFile(filename string, header *hermes.Header, lines []*hermes.FileLine) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()
	compressed := gzip.NewWriter(file)
	defer compressed.Close()
	b := proto.NewBuffer(nil)
	err = b.EncodeMessage(header)
	if err != nil {
		return err
	}
	_, err = compressed.Write(b.Bytes())
	if err != nil {
		return err
	}

	for _, l := range lines {
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
			log.Printf("Got header error: %s", err)
			good = false
		}
	}

	lines := []*hermes.FileLine{}

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
		keep := true
		for _, f := range a.lineFixers {
			keepIt, err := f(line)
			if err != nil {
				good = false
				log.Printf("Got error: %s", err)
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
		nextFilename := filepath.Join(filepath.Dir(filename), line.Footer.Next)
		next, err := openGzip(nextFilename)
		if err != nil {
			line.Footer.Next = ""
			next.Close()
			break
		}
		err = a.RunForFile(nextFilename, next)
		next.Close()
		if err != nil {
			line.Footer.Next = ""
		}
	}

	if err != nil {
		good = false
		log.Printf("Got error: %s\n Creating a new termination footer", err)
		if lines[len(lines)-1].Footer == nil {
			lines = append(lines, &hermes.FileLine{Footer: &hermes.Footer{}})
		}
		lines[len(lines)-1].Footer.Next = ""
		err = nil
	}

	if a.DryRun == true {
		if good == false {
			os.Exit(1)
		}
		return nil
	}

	if good == true {
		return nil
	}

	err = copyFile(filename, filename+".bak")
	if err != nil {
		return err
	}

	return writeHermesFile(filename, h, lines)
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

	file, err := openGzip(string(*a.Args.File))
	if err != nil {
		return err
	}
	return a.RunForFile(string(*a.Args.File), file)
}
func main() {

	if err := Execute(); err != nil {
		log.Fatalf("Unhandled error: %s", err)
	}
}
