package hermes

import (
	fmt "fmt"
	"io"

	"github.com/golang/protobuf/proto"
)

func ReadDelimitedMessage(stream io.Reader, pb proto.Message) (bool, error) {
	dataSize := make([]byte, 10)
	idx := 0
	for ; idx < len(dataSize); idx++ {
		n, err := stream.Read(dataSize[idx:(idx + 1)])
		if err != nil {
			return false, err
		}

		if n == 0 {
			idx--
			continue
		}
		if dataSize[idx]&0x80 == 0x00 {
			idx++
			break
		}
	}

	size, n := proto.DecodeVarint(dataSize[0:idx])
	if n == 0 || size == 0 {
		return false, nil
	}
	data := make([]byte, size)
	_, err := io.ReadFull(stream, data)
	if err != nil {
		return false, fmt.Errorf("io.ReadFull(%d bytes): %s", len(data), err)
	}
	pb.Reset()
	err = proto.Unmarshal(data, pb)
	if err != nil {
		return false, fmt.Errorf("protobuf error: %s", err)
	}
	return true, nil
}

func SafeEncode(pb proto.Message) ([]byte, error) {
	for i := 0; i < 10; i++ {
		b := proto.NewBuffer(nil)

		bSize := proto.Size(pb)

		if err := b.EncodeMessage(pb); err != nil {
			return nil, err
		}

		encSize, _ := proto.DecodeVarint(b.Bytes())

		if int(encSize) == bSize && len(b.Bytes()) == int(encSize)+proto.SizeVarint(encSize) {
			return b.Bytes(), nil
		}

	}
	return nil, fmt.Errorf("protobuf wants to make us miserable")
}
