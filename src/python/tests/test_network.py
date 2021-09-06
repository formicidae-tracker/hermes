import tracemalloc
import unittest
import threading
import time

from tests import utestdata, streamserver, assertions

import py_fort_hermes


class NetworkTestCase(assertions.Assertions):

    def setUpClass():
        tracemalloc.start()

    def setUp(self):
        self.server = streamserver.StreamServer(port=4002)
        self.thread = threading.Thread(target=self.server.run)
        self.thread.start()
        self.server.waitReady()

    def tearDown(self):
        self.server.stop()
        self.thread.join()

    def test_normal_reading(self):
        with py_fort_hermes.network.connect(host=self.server.hostname,
                                            port=self.server.port,
                                            timeout=0.1) as s:
            for expected in self.server.readouts:
                self.assertReadoutEqual(next(s), expected)
            with self.assertRaises(StopIteration):
                next(s)

    def test_no_header_detection(self):
        self.server.writeHeader = False
        with self.assertRaises(py_fort_hermes.InternalError):
            py_fort_hermes.network.connect(host=self.server.hostname,
                                           port=self.server.port,
                                           timeout=0.1)

    def test_is_also_an_iterable(self):
        with py_fort_hermes.network.connect(host=self.server.hostname,
                                            port=self.server.port,
                                            timeout=0.1) as s:
            for ro in s:
                pass
