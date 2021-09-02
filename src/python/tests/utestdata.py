from py_fort_hermes import Tag_pb2, FrameReadout_pb2, Header_pb2
import shutil
import tempfile
import math
import os
import gzip
from dataclasses import dataclass


class HermesFileWriter(object):
    def __init__(self, filepath, dual=False):
        self.gziped = gzip.open(filepath, "wb")
        self.uncomp = None
        if (dual == True):
            self.uncomp = open(filepath + "unc", "wb")

    def close(self):
        self.gziped.close()
        if self.uncomp is not None:
            self.uncomp.close()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.close()

    def write(self, message):
        self._writeMessageToFile(self.gziped, message)
        if self.uncomp is not None:
            self._writeMessageToFile(self.uncomp, message)

    @staticmethod
    def _encodeVaruint32(v: int):
        if (v < 0):
            raise ValueError("%d must be positive" % v)
        if (v > 2 ** 32 - 1):
            raise ValueError("%d is too large (max is 2^32-1)" % v)
        res = bytearray()
        while (v > 127):
            res += ((v & 0x7f) | 0x80).to_bytes(1, byteorder='big')
            v = v >> 7
        res += (v & 0x7f).to_bytes(1, byteorder='big')
        return res

    def _writeMessageToFile(self, f, message):
        encodedMessageSize = HermesFileWriter._encodeVaruint32(
            message.ByteSize())
        f.write(encodedMessageSize)
        f.write(message.SerializeToString())


class Singleton(type):
    _instances = {}

    def __call__(cls, *args, **kwargs):
        if cls not in cls._instances:
            cls._instances[cls] = super(
                Singleton, cls).__call__(*args, **kwargs)
        return cls._instances[cls]


def _HermesFileName(basename, index):
    """Returns the conventional filename for a hermes file segment

       Args:
           basename (str): the basename for the segment
           index (int): the index for the segment

       Returns:
           str: the filename for the segment
    """
    return "%s.%04d.hermes" % (basename, index)


@dataclass
class _WriteSequenceArgs:
    """Arguments for writing a sequence of hermes file"""
    Basepath: str
    Basename: str
    Readout: FrameReadout_pb2.FrameReadout
    NumberOfSegments: int = 3
    ReadoutsPerSegment: int = 10
    MissFooter: bool = False
    Dual: bool = False
    Truncated: bool = False
    NoHeader: bool = False


def _WriteSequence(*sargs, **kwargs):
    args = _WriteSequenceArgs(*sargs, **kwargs)
    res = UTestData.SequenceInfo()
    res.Segments = []
    res.Readouts = []
    frameID = args.Readout.frameID
    for i in range(args.NumberOfSegments):
        frameID, segmentPath, readouts = _WriteSegment(
            index=i, frameID=frameID, args=args)
        res.Readouts += readouts
        res.Segments.append(segmentPath)
    return res


def _WriteSegment(index,
                  frameID,
                  args: _WriteSequenceArgs):
    filepath = os.path.join(
        args.Basepath, _HermesFileName(args.Basename, index))
    readouts = []
    with HermesFileWriter(filepath, args.Dual and index == args.NumberOfSegments - 1) as f:
        if args.NoHeader == False:
            h = Header_pb2.Header()
            h.version.vmajor = 0
            h.version.vminor = 1
            dir(Header_pb2)
            h.type = Header_pb2.Header.Type.File
            h.width = args.Readout.width
            h.height = args.Readout.height
            if index > 0:
                h.previous = _HermesFileName(args.Basename, index-1)
            f.write(h)

        line = Header_pb2.FileLine()
        for i in range(args.ReadoutsPerSegment):
            ro = line.readout
            ro.Clear()
            ro.frameID = frameID
            ts = (frameID - args.Readout.frameID) * int(1e5)
            ro.timestamp = ts + 1
            ts_s = int(ts / 1e6)

            ts_ns = (ts - ts_s) * int(1e3) + args.Readout.time.nanos
            while ts_ns >= int(1e9):
                ts_s += 1
                ts_ns -= int(1e9)

            ro.time.seconds = args.Readout.time.seconds + ts_s
            ro.time.nanos = ts_ns
            ro.tags.extend(args.Readout.tags)

            f.write(line)
            ro.width = args.Readout.width
            ro.height = args.Readout.height

            readouts.append(ro)
            frameID += 1

        line.Clear()

        if (index < args.NumberOfSegments - 1 or (args.MissFooter == False and args.Truncated == False)):
            if (index < args.NumberOfSegments - 1):
                line.footer.next = _HermesFileName(args.Basename, index + 1)
            else:
                line.footer.next = ''
            f.write(line)

    return frameID, filepath, readouts


class UTestData(metaclass=Singleton):
    class SequenceInfo(object):
        def __init__(self):
            self.Segments = []
            self.Readouts = []

    def cleanUpFileSystem(self):
        shutil.rmtree(self.Basepath)

    def __init__(self):
        self.Basepath = tempfile.mkdtemp(prefix="py_fort_hermes_utestdata-")
        self.initSequences()

    def initSequences(self):
        ro = FrameReadout_pb2.FrameReadout()
        ro.time.FromJsonString("2019-11-02T23:02:01.123Z")
        ro.frameID = 12345
        ro.width = 1000
        ro.height = 1000
        t = ro.tags.add()
        t.ID = 123
        t.x = 500
        t.y = 500
        t.theta = math.pi / 4

        self.Normal = _WriteSequence(Basepath=self.Basepath,
                                     Basename="normal",
                                     Readout=ro)

        self.NoFooter = _WriteSequence(Basepath=self.Basepath,
                                       Basename="no-footer",
                                       Readout=ro,
                                       MissFooter=True)

        self.NoHeader = _WriteSequence(Basepath=self.Basepath,
                                       Basename="no-header",
                                       Readout=ro,
                                       NumberOfSegments=1,
                                       ReadoutsPerSegment=2,
                                       NoHeader=True)

        self.Truncated = _WriteSequence(Basepath=self.Basepath,
                                        Basename="truncated",
                                        Readout=ro,
                                        Truncated=True)

        self.TruncatedDual = _WriteSequence(Basepath=self.Basepath,
                                            Basename="truncated-dual",
                                            Readout=ro,
                                            Dual=True,
                                            Truncated=True)


if __name__ == '__main__':
    UTestData()
