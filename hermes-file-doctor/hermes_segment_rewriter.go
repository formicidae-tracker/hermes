package main

import (
	"compress/gzip"
	"log/slog"
	"os"

	"github.com/formicidae-tracker/hermes/src/go/hermes"
	"github.com/golang/protobuf/proto"
)

type hermesFileRewriter struct {
	filename string
	header   *hermes.Header
	lines    []*hermes.FileLine
	logger   *slog.Logger
}

var toRewrite *hermesFileRewriter = nil

func PopRewrite() {
	if toRewrite == nil {
		return
	}

	err := toRewrite.rewrite()
	if err != nil {
		toRewrite.logger.Error("rewrite error", "error", err)
	}
	toRewrite = nil
}

func PushRewrite(filename string, header *hermes.Header, lines []*hermes.FileLine, logger *slog.Logger) {
	PopRewrite()

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

	uncompressedFilepath := UncompressedFilename(w.filename)
	if _, err := os.Stat(uncompressedFilepath); err != nil {
		w.logger.Info("removing uncompressed", "filepath", uncompressedFilepath)
	}
	return os.RemoveAll(uncompressedFilepath)

}
