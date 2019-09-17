package hermes

import (
	"bytes"
	"testing"

	proto "github.com/golang/protobuf/proto"
	. "gopkg.in/check.v1"
)

// Hook up gocheck into the "go test" runner.
func Test(t *testing.T) { TestingT(t) }

type StreamReaderSuite struct{}

var _ = Suite(&StreamReaderSuite{})

func (s *StreamReaderSuite) TestDelimitedMessageReading(c *C) {
	testdata := []proto.Message{
		&Header{
			Version:  &Version{Vmajor: 42, Vminor: 43},
			Previous: "nope",
		},
		&FrameReadout{
			Timestamp: 10,
			FrameID:   1,
			Ants:      []*Ant{&Ant{ID: 0, X: 10, Y: 12}},
			Quads:     2,
		},
		&FrameReadout{
			Timestamp: 20,
			FrameID:   2,
			Ants:      nil,
			Quads:     2,
			Error:     FrameReadout_ILLUMINATION_ERROR,
		},
	}

	repeats := 100

	rawBuffer := bytes.NewBuffer(nil)

	for i := 0; i < repeats; i++ {
		for _, m := range testdata {
			goBuffer := proto.NewBuffer(nil)
			c.Assert(goBuffer.EncodeMessage(m), IsNil)
			_, err := rawBuffer.Write(goBuffer.Bytes())
			c.Assert(err, IsNil)
		}
	}

	for i := 0; i < repeats; i++ {
		for _, m := range testdata {
			mm := proto.Clone(m)
			mm.Reset()
			ok, err := ReadDelimitedMessage(rawBuffer, mm)
			c.Assert(ok, Equals, true)
			c.Assert(err, IsNil)
			c.Assert(mm, DeepEquals, m)
		}
	}
}
