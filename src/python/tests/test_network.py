import tracemalloc
import unittest
import threading
from tests import utestdata, streamserver, assertions

import py_fort_hermes


class NetworkTestCase(assertions.Assertions):
    def setUpClass():
        tracemalloc.start()

    def setUp(self):
        self.server = streamserver.StreamServer()
        self.thread = threading.Thread(target=self.server.run)
        self.thread.start()

    def tearDown(self):
        self.server.stop()
        self.thread.join()

    def test_normal_reading(self):
        with py_fort_hermes.OpenNetworkStream(self.server.hostname, self.server.port) as s:
            for expected in self.server.readouts:
                self.assertReadoutEqual(next(s), expected)
                print(expected)
            print('ok')
            with self.assertRaises(StopIteration):
                next(s)
