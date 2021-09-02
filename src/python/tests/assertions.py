import unittest


class Assertions(unittest.TestCase):
    def assertReadoutEqual(self, a, b):
        self.assertEqual(a.frameID, b.frameID)
        self.assertEqual(a.timestamp, b.timestamp)
        self.assertEqual(a.producer_uuid, b.producer_uuid)
        self.assertEqual(a.quads, b.quads)
        self.assertEqual(a.width, b.width)
        self.assertEqual(a.height, b.height)
        self.assertEqual(a.time.seconds, b.time.seconds)
        self.assertEqual(a.time.nanos, b.time.nanos)
        self.assertEqual(len(a.tags), len(b.tags))
        for i in range(len(b.tags)):
            self.assertEqual(a.tags[i].ID, b.tags[i].ID)
            self.assertEqual(a.tags[i].x, b.tags[i].x)
            self.assertEqual(a.tags[i].y, b.tags[i].y)
            self.assertEqual(a.tags[i].theta, b.tags[i].theta)
