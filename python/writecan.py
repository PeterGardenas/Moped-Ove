# - Add CAN output of the received data. See https://libbits.wordpress.com/2012/05/22/socketcan-support-in-python/ or http://elinux.org/Python_Can

import socket
import struct
import time

import re

def initializeCAN(network):
    """
    Initializes the CAN network, and returns the resulting socket.
    """
    # create a raw socket and bind it to the given CAN interface
    s = socket.socket(socket.AF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
    s.bind((network,))
    return s

def writeToCAN(canSocket, canFrameID):
    data = struct.pack("=hhBBH", 11, 12, 13, 14, 15)
    frame = struct.pack("=IB3x8s", canFrameID, 8, data)
    try:
        canSocket.send(frame)
    except socket.error:
        print("Error writing vehicle position to CAN")
    
canNetwork = "can0"
canFrameID = 1025
canSocket = initializeCAN(canNetwork)

cnt = 0

part = b""

while True:
    cnt += 1
    #writeToCAN(canSocket, canFrameID)
    #time.sleep(0.1)
    data = canSocket.recv(1024)
    if (data[0], data[1]) == (100,4):
        if data[8] == 16:
            #print(part)
            parts = str(part)
            m = re.search("rear speed x([0-9 ]+)x([0-9 ]+)", parts)
            if m:
                sp = int(m.group(1))
                odo = int(m.group(2))
                print("%d %d" % (sp, odo))
            part = b""
        part += data[9:]
#        print(data)
#        print("%d %d" % (data[8], data[9]))
