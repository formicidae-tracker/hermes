"""
Minimal example to read hermes messages from a host in python
"""
import argparse
import socket
from google.protobuf.internal.decoder import _DecodeVarint32
import Header_pb2
import FrameReadout_pb2


def get_msg(sock):
    data_size = bytearray()
    for i in range(0, 100):
        data_size += sock.recv(1)
        if bytes([b'\x80'[0] & data_size[i]]) == b'\x00':
            break
    msg_len, new_pos = _DecodeVarint32(data_size, 0)
    buf = sock.recv(msg_len)
    return buf


parser = argparse.ArgumentParser(
    description='Python hermes network reader, close connection with Ctrl+C')
parser.add_argument('host', type=str, help='tracking host <hostname.local>')
args = parser.parse_args()

port = 4002

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    try:
        host = socket.gethostbyname(args.host)
        s.connect((host, port))
        print("Connected to {}".format(args.host))
        msg = get_msg(s)
        header = Header_pb2.Header()
        header.ParseFromString(msg)
        while True:
            msg = get_msg(s)
            ro = FrameReadout_pb2.FrameReadout()
            ro.ParseFromString(msg)
            print("frameID: {}, no tags: {}".format(ro.frameID, len(ro.tags)))
    except KeyboardInterrupt:
        s.shutdown(socket.SHUT_RDWR)
        s.close()
        print("Connection closed")
