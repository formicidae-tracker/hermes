

def _decodeVaruint32(stream):
    v = 0
    for i in range(5):
        b = stream.read(1)
        v += b[0] & 0x7f
        if b[0] & 0x80 == 0:
            break
        v = v << 7
    return v
