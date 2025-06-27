import serial
import time

# may need to change port (possibly /dev/ttyUSB0)
ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)

# attempt a drive foward
ser.write(b'drive_straight 800 100\n')
