import time
import socket
import re

def initializeCAN(network):
    """
    Initializes the CAN network, and returns the resulting socket.
    """
    # create a raw socket and bind it to the given CAN interface
    s = socket.socket(socket.AF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
    s.bind((network,))
    return s


def readspeed2():
    part = b""
    part2 = b""
    while True:
        data = canSocket.recv(1024)
        if (data[0], data[1]) == (100,4):
            if data[8] == 16:
                parts = str(part)
                m = re.search("speed x([0-9 ]+)x([0-9 ]+)x([0-9 ]+)x([0-9 ]+)x([0-9 ]+)x([0-9 ]+)", parts)
                if m:
                    print((time.time(),parts))
                part = b""
            part += data[9:]
        elif (data[0], data[1]) == (108,4):
            # Reading DistPub this way is not a good idea, since those
            # messages come often and slow down the other threads (or the
            # whole process?).
            # DistPub
            # note that non-ASCII will appear as text \x07 in 'parts'
            if data[8] == 16:
                if len(part2) > 18:
                    part2x = part2[19:]
                    part2s = part2x.decode('ascii')
                    l = part2[18]
                    part2s2 = part2s[0:l]

                    m = re.search("([0-9]+) ([0-9]+)", part2s2)
                    if m:
                        print((time.time(),part2s2))
                    part2 = b""
            part2 += data[9:]

canSocket = initializeCAN("can0")

readspeed2()
