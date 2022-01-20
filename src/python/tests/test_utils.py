import unittest
import io
from py_fort_hermes import utils


class UtilsTestCase(unittest.TestCase):

    def setUp(self):
        self.data = {
            0: b'\x00',
            1: b'\x01',
            127: b'\x7f',
            128: b'\x80\x01',
            257: b'\x81\x02',
            300: b'\xac\x02',  # from google's protobuf documentation
        }

    def test_encoding_varuint32(self):
        for v, b in self.data.items():
            self.assertEqual(utils._encodeVaruint32(v), b)

    def test_decoding_varuint32(self):
        for v, b in self.data.items():
            self.assertEqual(utils._decodeVaruint32(io.BytesIO(b)), v)

        errors = {
            b'': "EOF",
            b'\x80': "EOF",
            b'\x80\x80\x80\x80\x80': "too large",
        }

        for b, expected in errors.items():
            with self.assertRaisesRegex(RuntimeError, expected):
                utils._decodeVaruint32(io.BytesIO(b))
