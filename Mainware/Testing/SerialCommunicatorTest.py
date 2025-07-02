"""
Code to test serial communication module.
Most importantly, is delay amount enough between command sends and does confirmation reading work
"""

import SerialCommunicator

serCom = SerialCommunicator()

# make sure min and max speed works right
print("minSpeed: " + str(serCom.min_speed()) + " maxSpeed: " + str(serCom.max_speed()))

# attempt a drive followed by a turn to test for:
#   driving works
#   turning works
#   successive commands work
serCom.drive_straight(serCom.min_speed(), 100)
serCom.rotate_in_place(serCom.min_speed(), 90)
