"""
Plots data received from a tracking host in "real time"
"""
import argparse
import time
import py_fort_hermes
from matplotlib import pyplot as plt
import matplotlib
matplotlib.use('TkAgg')


parser = argparse.ArgumentParser(
    description='Python hermes network reader, close connection with Ctrl+C')
parser.add_argument('host', type=str, help='tracking host <hostname.local>')
args = parser.parse_args()


fig, ax = plt.subplots()
plt.ion()

x_lim = [0.0, 0.0]
y_lim = [0.0, 0.0]


try:
    with py_fort_hermes.network.connect(args.host) as s:
        # TODO: header.width/height fields seem to send zero
        while plt.fignum_exists(fig.number):
            msg = next(s)
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
        print("Closing connection after end of figure")
except KeyboardInterrupt:
    print("Closing connection after KeyboardInterrupt")
print("Connection closed")
