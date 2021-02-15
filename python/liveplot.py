"""
Plots data received from a tracking host in "real time"
"""
import argparse
import time
import socket
from google.protobuf.internal.decoder import _DecodeVarint32
import Header_pb2
import FrameReadout_pb2
from matplotlib import pyplot as plt
import matplotlib
matplotlib.use('TkAgg')


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

fig, ax = plt.subplots()
plt.ion()

x_lim = [0.0, 0.0]
y_lim = [0.0, 0.0]

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    try:
        host = socket.gethostbyname(args.host)
        s.connect((host, port))
        print("Connected to {}".format(args.host))
        msg = get_msg(s)
        header = Header_pb2.Header()
        header.ParseFromString(msg)
        # TODO: header.width/height fields seem to send zero
        while plt.fignum_exists(fig.number):
            msg = get_msg(s)
            ro = FrameReadout_pb2.FrameReadout()
            ro.ParseFromString(msg)
            x = [t.x for t in ro.tags]
            y = [t.y for t in ro.tags]
            ax.clear()
            ax.plot(x, y, 'b.')
            tmp = ax.get_xlim()
            x_lim[0] = tmp[0] if tmp[0] < x_lim[0] else x_lim[0]
            x_lim[1] = tmp[1] if tmp[1] > x_lim[1] else x_lim[1]
            ax.set_xlim(x_lim)
            tmp = ax.get_ylim()
            y_lim[0] = tmp[0] if tmp[0] < y_lim[0] else y_lim[0]
            y_lim[1] = tmp[1] if tmp[1] > y_lim[1] else y_lim[1]
            ax.set_ylim(y_lim)
            ax.set_title("host: {} time: {}".format(args.host, time.strftime(
                '%Y-%m-%d %H:%M:%S', time.localtime(ro.time.seconds))))
            plt.draw()
            plt.pause(0.001)
        s.shutdown(socket.SHUT_RDWR)
        s.close()
        print("Connectoin closed")
    except KeyboardInterrupt:
        s.shutdown(socket.SHUT_RDWR)
        s.close()
        print("Connectoin closed")
