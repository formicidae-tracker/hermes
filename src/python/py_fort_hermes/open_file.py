import py_fort_hermes as fh
from py_fort_hermes import utils, check

import os
import gzip
import inspect


class FileContext:
    __slots__ = ['width', 'height', 'path', 'followFile', 'filestream', 'line']

    def __init__(self, filepath, followFile=True):
        self.followFile = followFile
        self.width = 0
        self.height = 0
        self.path = ''
        self.line = fh.Header_pb2.FileLine()
        self._openFile(filepath)

    def __enter__(self):
        return self

    def close(self):
        if self.filestream is not None:
            self.filestream.close()
        self.filestream = None

    def __next__(self):
        if self.filestream is None:
            raise StopIteration
        self.line.Clear()
        try:
            self._readMessage(self.line)
        except Exception as e:
            raise fh.UnexpectedEndOfFileSequence(segmentPath=self.path,
                                                 what="cannot decode line: %s" % e)
        if self.line.HasField('readout'):
            ro = fh.FrameReadout()
            ro.CopyFrom(self.line.readout)
            ro.width = self.width
            ro.height = self.height
            return ro
        elif not self.line.HasField('footer'):
            raise fh.UnexpectedEndOfFileSequence(segmentPath=self.path,
                                                 what="got an empty line")

        if self.line.footer.next == '' or self.followFile == False:
            self.close()
            raise StopIteration

        newPath = os.path.join(os.path.dirname(self.path),
                               self.line.footer.next)
        self._openFile(newPath)
        return self.__next__()

    def __exit__(self, type, value, traceback):
        self.close()

    def _openFile(self, filepath):
        try:
            self.filestream = open(filepath + "unc", "rb")
        except:
            self.filestream = gzip.open(filepath)

        h = fh.Header()
        try:
            self._readMessage(h)
        except Exception as e:
            raise fh.InternalError(ErrorCode.FH_STREAM_NO_HEADER,
                                   "cannot parse header: %s" % e.message)

        check.CheckFileHeader(h)
        self.width = h.width
        self.height = h.height
        self.path = filepath

    def _readMessage(self, message):
        size = utils._decodeVaruint32(self.filestream)
        message.ParseFromString(self.filestream.read(size))


def OpenFile(filepath, followFile=True):
    return FileContext(filepath, followFile)
