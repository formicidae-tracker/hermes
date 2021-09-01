import unittest
import os
from tests import utestdata


if __name__ == '__main__':
    loader = unittest.TestLoader()
    tests = loader.discover('.')
    testRunner = unittest.runner.TextTestRunner(verbosity=2)
    print("Created unittest data in %s" % utestdata.UTestData().Basepath)
    try:
        testRunner.run(tests)
    finally:
        if os.environ.get("PRESERVE_UTESTDATA") is None:
            print("Removing unittest data in %s" % utestdata.UTestData().Basepath)
            utestdata.UTestData().cleanUpFileSystem()
        else:
            print("unittest data is located in %s" % utestdata.UTestData().Basepath)
