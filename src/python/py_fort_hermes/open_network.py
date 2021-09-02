import py_fort_hermes as fh

import socket


class NetworkContext:
    def __init__(self, host, port=4002, blocking=True, timeout=2.0):
        self._buffer = bytearray()
        self._bytesRead = 0
        self._nextSize = 0
        self._s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.blocking = blocking
        try:
            host = socket.gethostbyname(host)
            self._s.setblocking(blocking)
            if blocking:
                self._s.settimeout(timeout)

            self._s.connect((host, port))
            header = fh.Header()
            try:
                self._readMessage(header)
            except Exception as e:
                raise fh.InternalError(fh.ErrorCode.FH_STREAM_NO_HEADER,
                                       "could not read header: %s" % e)
            fh.check.CheckNetworkHeader(header)
        except:
            self._s.close()
            raise

    def __enter__(self):
        return self

    def close(self):
        if self._s is not None:
            self._s.close()
        self._s = None

    def __next__(self):
        ro = fh.FrameReadout()
        try:
            self._readMessage(ro)
        except socket.timeout:
            if self.blocking == True:
                raise StopIteration
            else:
                raise
        return ro

    def __exit__(self, type, value, traceback):
        self.close()

    def _readMessage(self, message):
        if self._nextSize == 0:
            self._nextSize = self._readVaruint32()
            self._bytesRead = 0
        self._readAll(self._nextSize)
        message.ParseFromString(self._buffer[:self._bytesRead])
        self._nextSize = 0
        self._bytesRead = 0

    def _readReset(self):
        self._bytesRead = 0

    def _readAll(self, size):
        if self._bytesRead >= size:
            return

        if len(self._buffer) < size:
            self._buffer += bytearray(size - len(self._buffer))

        while self._bytesRead < size:
            b = self._s.recv_into(memoryview(self._buffer)[self._bytesRead:size],
                                  size - self._bytesRead)
            if b == 0 and self.blocking == True:
                raise socket.timeout
            self._bytesRead += b

    def _readVaruint32(self):
        self._readAll(1)
        while self._buffer[self._bytesRead - 1] & 0x80 != 0:
            self._readAll(self._bytesRead + 1)

        v = self._buffer[0] & 0x7f
        for i in range(1, self._bytesRead):
            v = v << 7
            v += self._buffer[i] & 0x7f
        return v


def OpenNetworkStream(host, port=4002, blocking=True, timeout=2.0):
    return NetworkContext(host, port, blocking, timeout)