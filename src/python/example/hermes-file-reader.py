"""
Minimal example to read hermes file sequences
"""

import py_fort_hermes
import argparse

parser = argparse.ArgumentParser(
    description='Python hermes file reader')
parser.add_argument('file', type=str, help='file to read')
args = parser.parse_args()


if __name__ == '__main__':
    with py_fort_hermes.OpenFile(args.file) as f:
        for readout in f:
            print(readout)
