import unittest
import py_fort_hermes
import os

from tests import utestdata, assertions


class FileTestCase(assertions.Assertions):
    def test_normal_reading(self):
        info = utestdata.UTestData().Normal
        with py_fort_hermes.file.open(info.Segments[0]) as f:
            for expected in info.Readouts:
                self.assertReadoutEqual(next(f), expected)
            with self.assertRaises(StopIteration):
                next(f)
            with self.assertRaises(StopIteration):
                next(f)

    def test_normal_partial_reading(self):
        info = utestdata.UTestData().Normal
        with py_fort_hermes.file.open(info.Segments[0], followFile=False) as f:
            for i in range(info.ReadoutsPerSegment):
                expected = info.Readouts[i]
                self.assertReadoutEqual(next(f), expected)
            with self.assertRaises(StopIteration):
                next(f)

    def test_truncated_file_reading(self):
        info = utestdata.UTestData().Truncated
        classic = 0
        dual = 0
        with py_fort_hermes.file.open(info.Segments[0]) as f, self.assertRaises(py_fort_hermes.UnexpectedEndOfFileSequence) as cm:
            for i in range(len(info.Readouts)):
                self.assertReadoutEqual(next(f), info.Readouts[i])
                classic = i + 1

        self.assertEqual(cm.exception.segmentPath,
                         info.Segments[len(info.Segments)-1])

        info = utestdata.UTestData().TruncatedDual
        with py_fort_hermes.file.open(info.Segments[0]) as f, self.assertRaises(py_fort_hermes.UnexpectedEndOfFileSequence) as cm:
            for i in range(len(info.Readouts)):
                self.assertReadoutEqual(next(f), info.Readouts[i])
                dual = i + 1

        self.assertGreater(dual, classic)

    def test_reports_no_footer_file(self):
        info = utestdata.UTestData().NoFooter
        with py_fort_hermes.file.open(info.Segments[0]) as f:
            for expected in info.Readouts:
                self.assertReadoutEqual(next(f), expected)
            with self.assertRaises(py_fort_hermes.UnexpectedEndOfFileSequence) as cm:
                next(f)
            self.assertEqual(cm.exception.segmentPath,
                             info.Segments[len(info.Segments)-1])

    def test_reports_file_without_header(self):
        with self.assertRaises(py_fort_hermes.InternalError):
            py_fort_hermes.file.open(
                utestdata.UTestData().NoHeader.Segments[0])

    def test_report_unexisting_file(self):
        with self.assertRaises(FileNotFoundError):
            py_fort_hermes.file.open(os.path.join(
                utestdata.UTestData().Basepath, "oops"))

    def test_is_also_an_iterable(self):
        info = utestdata.UTestData().Normal
        with py_fort_hermes.file.open(info.Segments[0]) as f:
            for ro in f:
                pass
