import select
import socket
import os
import threading

from tests import utestdata
from py_fort_hermes import utils, Header_pb2


class StreamServer:
    def __init__(self,
                 port=12345,
                 readouts=utestdata.UTestData().Normal.Readouts,
                 writeHeader=True):
        self.hostname = "localhost"
        self.port = 0
        self._port = port
        self.readouts = readouts
        self.writeHeader = writeHeader
        self._s = None
        self._rc, self._wc = os.pipe()
        self._ready = threading.Condition()

    def stop(self):
        os.write(self._wc, b'!')

    def waitReady(self):
        with self._ready:
            while self.port == 0:
                self._ready.wait()

    def run(self):
        for port in range(self._port, self._port+20):
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.setblocking(False)
                try:
                    s.bind((self.hostname, port))
                except:
                    continue
                s.listen(1)
                with self._ready:
                    self.port = port
                    self._ready.notify()
                rfds, _, _ = select.select([s.fileno(), self._rc], [], [])
                if self._rc in rfds:
                    s.close()
                    return
                conn, addr = s.accept()
                try:
                    with conn:
                        if self.writeHeader == True:
                            h = Header_pb2.Header()
                            h.version.vmajor = 0
                            h.version.vminor = 1
                            h.type = Header_pb2.Header.Type.Network
                            self._sendMessage(conn, h)
                        for ro in self.readouts:
                            self._sendMessage(conn, ro)
                except:
                    pass

    def _sendMessage(self, conn, message):
        rfds, _, _ = select.select([self._rc], [conn], [])
        if self._rc in rfds:
            return
        size = utils._encodeVaruint32(message.ByteSize())
        buf = size + message.SerializeToString()
        conn.sendall(buf)
