import py_fort_hermes as fh

import socket


class Context:
    """
    A context manager to open network connection to a live leto

    Example:
        import py_fort_hermes as fh

        with fg.network.connect(host) as s:
             for ro in s:
                 # do something
                 pass
    """

    __slots__ = [
        "_buffer",
        "_bytesRead",
        "_nextSize",
        "_s",
        "blocking",
        "_ro",
    ]

    def __init__(self, host, port=4002, blocking=True, timeout=2.0, allocateNewMessages=False):
        """Initializes a connection to leto
        Args:
            host (str): a host to connect to
            port (int): the port to connect to
            blocking (bool): use a blocking connection
            timeout (float): timeout in second to use for waiting a message
            allocateNewMessages (bool): if True, each call to
                __next__() will return a newly allocated FrameReadout,
                with some performance onverhead. Otherwise the same
                object will be reused to avoid memory allocation.
        Raises:
            various error with socket.connect

        """
        self._buffer = bytearray()
        self._bytesRead = 0
        self._nextSize = 0
        self._s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.blocking = blocking
        if allocateNewMessages is True:
            self._ro = None
        else:
            self._ro = fh.FrameReadout()

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
        """
        Gets the next FrameReadout in the stream

        Returns:
            FrameReadout: the next readout

        Raises:
            StopIteration: if no data was received after self.timeout
            socket.timeout: if some incomplete data have been received
                in self.timeout
            OSError: with EWOULDBLOCK or EAGAIN if non-blocking and no
                message where fully ready
        """
        ro = self._ro
        if ro is None:
            ro = fh.FrameReadout()
        self._readMessage(ro)
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
                raise StopIteration
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


def connect(host, port=4002, blocking=True, timeout=2.0, allocateNewMessages=False):
    """
    Connects to a leto host.
    Args:
        host (str): the host to connect to
        port (int): the port to connect to
        blocking (bool): use a blocking connection
        timeout (float): sets a timeout for IO
        allocateNewMessages (bool): if True, each call to
            next() will return a newly allocated FrameReadout,
            with some performance onverhead. Otherwise the same
            object will be reused to avoid memory allocation.
    Returns:
        Context: an iterable context manager for the connection

    Example:
        with py_fort_hermes.network.connect(host) as c:
            for ro in c:
                pass
    """
    return Context(host, port, blocking, timeout)
