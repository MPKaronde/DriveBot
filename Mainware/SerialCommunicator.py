"""
General program to communicate w/ Arduino for known movement commands
To be used as machine API for all Mainware code (other than testers/calibraions)

All movement commands return boolean
    True = movement completed
    False = Error (most likely either out of bounds, serial connection lost, machine movement failed)

As in the arduino, all speeds are in motor speed, all distances are mm, and all angles are degrees
    Note that these are all estimates as the arduino itself does not attempt to handle error correction
"""

import serial
import time


class SerialCommunicator:

    # Establish connection, determine currently accepted speed limits
    def __init__(self, port_name="/dev/ttyUSB0", baud_rate=9600):
        self.Ser = serial.Serial(port_name, baud_rate, timeout=1)
        time.sleep(2)

        # get min and max limits
        self.Ser.write(b"limits\n")
        limString = self.Ser.readline().decode()
        print("hello " + limString)
        self.MIN_SPEED, self.MAX_SPEED = map(int, limString.split())

    # waits until execution confirmation recieved
    def wait_for_machine(self):
        curString = self.Ser.readline().decode()
        while not "command no" in curString:
            curString = self.Ser.readline().decode()

    # reads returned execution state string
    # returns true if machine confirmed execution, false otherwise
    def check_execution(self):
        if "true" in self.Ser.readline() or "True" in self.Ser.readline():
            return True
        return False

    # to be run at the end of every command
    # waits for arduino to be ready & confirms command completion
    def end_execution(self):
        self.wait_for_machine()
        return self.check_execution()

    # returns string with speed limits
    # NOTE: self's MIN_SPEED and MAX_SPEED were set in __init__ so no point resetting them here (they dont change)
    def limits(self):
        return str(self.MIN_SPEED) + " " + str(self.MAX_SPEED)

    # Drive forward by given amount at given speed if speed is valid
    # NOTE: speed check is also done on arduino, but serial communication is time expensive so easier to do it here too
    def drive_straight(self, speed, distance):
        # check speed limits
        if speed < self.MIN_SPEED or speed > self.MAX_SPEED:
            return False

        commandString = "drive_straight " + str(speed) + " " + str(distance) + "\n"
        self.Ser.write(commandString.encode())
        return self.end_execution()

    # rotate by given amount if speed is valid
    # NOTE: speed check is also done on arduino, but serial communication is time expensive so easier to do it here too
    # positive degs = right, negative degs = left
    def rotate_in_place(self, speed, degrees):
        # check speed limits
        if speed < self.MIN_SPEED or speed > self.MAX_SPEED:
            return False

        commandString = "rotate_in_place " + str(speed) + " " + str(degrees) + "\n"
        self.Ser.write(commandString.encode())
        return self.end_execution()

    # return min speed
    def min_speed(self):
        return self.MIN_SPEED

    # return max speed
    def max_speed(self):
        return self.MAX_SPEED

    # ends the serial communication session
    # Arduino needs to restart in order to restart session after this executes
    def end(self):
        self.Ser.write("end\n".encode())
        self.Ser.close()
