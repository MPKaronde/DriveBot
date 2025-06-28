"""
General program to communicate w/ Arduino for known movement commands
To be used as machine API for all Mainware code (other than testers/calibraions)

All movement commands return boolean
    True = movement completed
    False = Error (most likely either out of bounds, serial connection lost, machine movement failed)
"""

import serial
import time


class SerialCommunicator:

    # Establish connection, determine currently accepted speed limits
    def __init__(self, port_name="/dev/ttyACM0", baud_rate=9600):
        self.Ser = serial.Serial(port_name, baud_rate, timeout=1)
        time.sleep(2)

        # get min and max limits
        self.Ser.write("limits")
        limString = self.Ser.readline()
        self.MIN_SPEED, self.MAX_SPEED = map(int, limString.split())
