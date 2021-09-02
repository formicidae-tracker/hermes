"""
Minimal example to read hermes messages from a host in python
"""
import argparse
import py_fort_hermes

parser = argparse.ArgumentParser(
    description='Python hermes network reader, close connection with Ctrl+C')
parser.add_argument('host', type=str, help='tracking host <hostname.local>')
args = parser.parse_args()

if __name__ == '__main__':
    try:
        with py_fort_hermes.network.connect(args.host) as s:
            for ro in s:
                print("FrameID: %d, tags: %d" % (ro.frameID, len(ro.tags)))
    except KeyboardInterrupt:
        print("Closing connection after KeyboardInterrupt.")
    print("Connection closed.")
