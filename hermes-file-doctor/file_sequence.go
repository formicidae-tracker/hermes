package main

import (
	"bufio"
	"fmt"
	"os"
	"path"
	"path/filepath"
	"regexp"
	"sort"
	"strings"
)

type SegmentInfo struct {
	Name, Next string
	Index      int
	LineCount  int
	Start      int64
	End        int64
}

type FileSequence struct {
	Segments []SegmentInfo

	indicesByName map[string]int

	dirpath      string
	updateNeeded bool
}

type By[T any] func(a, b T) bool
type sorter[T any] struct {
	toSort []T
	by     By[T]
}

func (s *sorter[T]) Len() int {
	return len(s.toSort)
}

func (s *sorter[T]) Swap(i, j int) {
	s.toSort[i], s.toSort[j] = s.toSort[j], s.toSort[i]
}

func (s *sorter[T]) Less(i, j int) bool {
	return s.by(s.toSort[i], s.toSort[j])
}

func (by By[T]) Sort(data []T) {
	sorter := &sorter[T]{
		toSort: data,
		by:     by,
	}
	sort.Sort(sorter)
}

func NewHermesFileSequence(dirpath string) (*FileSequence, error) {
	finfo, err := os.Stat(dirpath)
	if err != nil {
		return nil, fmt.Errorf("could not stat '%s': %w", dirpath, err)
	}
	if finfo.IsDir() == false {
		return nil, fmt.Errorf("'%s' is not a directory", dirpath)
	}

	if res, err := buildFromIndexFile(dirpath); err == nil {
		return res, nil
	}

	return buildFromFileListing(dirpath)
}

var segmentIndexFile = ".segment-index.hermes"

func buildFromIndexFile(dirpath string) (*FileSequence, error) {
	indexPath := filepath.Join(dirpath, segmentIndexFile)

	f, err := os.Open(indexPath)
	if err != nil {
		return nil, err
	}
	defer f.Close()
	scanner := bufio.NewScanner(f)
	var segments []SegmentInfo
	lineCount := -1
	for scanner.Scan() {
		lineCount += 1
		l := scanner.Text()
		if strings.TrimSpace(l)[0] == '#' {
			continue
		}
		var name string
		var count int
		var start, end int64
		_, err := fmt.Sscanf(l, `"%s",%d,%d,%d`, &name, &count, &start, &end)
		if err != nil {
			return nil, fmt.Errorf("could not read '%s' for at line %d: %w",
				indexPath, lineCount, err)
		}

		if len(segments) > 0 {
			segments[len(segments)-1].Next = name
		}

		segments = append(segments, SegmentInfo{
			Index:     len(segments),
			Name:      name,
			LineCount: count,
			Start:     start,
			End:       end,
		})
	}

	res := &FileSequence{
		updateNeeded: false,
		dirpath:      dirpath,
		Segments:     segments,
	}
	res.rebuild()
	return res, nil
}

var trackingFileRx = regexp.MustCompile(`\Atracking\.([0-9]{4}).hermes\z`)

var SegmentInfoByName = By[SegmentInfo](func(a, b SegmentInfo) bool {
	return a.Name < b.Name
})

func buildFromFileListing(dirpath string) (*FileSequence, error) {
	entries, err := os.ReadDir(dirpath)
	if err != nil {
		return nil, fmt.Errorf("could not read '%s': %w", dirpath, err)
	}
	segments := make([]SegmentInfo, 0, len(entries))
	for _, e := range entries {
		if e.IsDir() || trackingFileRx.MatchString(e.Name()) == false {
			continue
		}
		segments = append(segments, SegmentInfo{
			Name: e.Name(),
		})
	}
	SegmentInfoByName.Sort(segments)
	res := &FileSequence{
		Segments: segments,
		dirpath:  dirpath,
	}

	res.rebuild()
	return res, nil
}

func (fs *FileSequence) rebuild() {
	fs.indicesByName = make(map[string]int)

	for i := range fs.Segments {
		fs.indicesByName[fs.Segments[i].Name] = i

		if fs.Segments[i].Index != i {
			fs.Segments[i].Index = i
			fs.updateNeeded = true
		}

		if i < len(fs.Segments)-1 {
			next := fs.Segments[i+1].Name
			if fs.Segments[i].Next != next {
				fs.Segments[i].Next = next
				fs.updateNeeded = true
			}
		}
	}

}

func (fs *FileSequence) Update(filename string, lineIndex int, frameID int64) {
	if _, ok := fs.indicesByName[filename]; ok == false {
		fs.Segments = append(fs.Segments, SegmentInfo{
			Name: filename,
		})
		fs.rebuild()
	}

	idx, ok := fs.indicesByName[filename]
	if ok == false {
		return
	}

	if fs.Segments[idx].LineCount < lineIndex+1 {
		fs.Segments[idx].LineCount = lineIndex + 1
		fs.updateNeeded = true
	}

	if lineIndex == 0 {
		fs.Segments[idx].Start = frameID
		fs.updateNeeded = true
	}

	if fs.Segments[idx].End < frameID+1 {
		fs.Segments[idx].End = frameID + 1
		fs.updateNeeded = true
	}

}

func (fs *FileSequence) Persist() error {
	if fs.updateNeeded == false {
		return nil
	}
	indexPath := path.Join(fs.dirpath, segmentIndexFile)

	f, err := os.Create(indexPath)
	if err != nil {
		return fmt.Errorf("could not create index file '%s': %w", indexPath, err)
	}
	defer f.Close()
	fmt.Fprintf(f, "#Name,Count,Start,End\n")
	for _, s := range fs.Segments {
		fmt.Fprintf(f, "\"%s\",%d,%d,%d\n", s.Name, s.LineCount, s.Start, s.End)
	}
	fs.updateNeeded = false
	return nil
}

func (fs *FileSequence) ResetSegment(i int) {
	if i < 0 || i >= len(fs.Segments) {
		return
	}
	fs.Segments[i].LineCount = 0
	fs.Segments[i].Start = 0
	fs.Segments[i].End = 0
	fs.updateNeeded = true
}
