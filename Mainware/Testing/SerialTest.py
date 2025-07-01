"""
Ensures that serial communication is properly occuring between pi and arduino by sending
known move command to said arduino.
"""

import serial
import time

# may need to change port (possibly /dev/ttyUSB0)
ser = serial.Serial("/dev/ttyUSB0", 9600, timeout=1)
time.sleep(2)

# attempt a drive foward
ser.write(b"rotate_in_place 800 -100\n")
time.sleep(10)
ser.write(b"drive_straight 800 -100\n")
