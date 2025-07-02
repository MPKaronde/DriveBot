"""
Code to test serial communication module.
Most importantly, is delay amount enough between command sends and does confirmation reading work
"""

import sys
import os

# Always resolve path relative to the script location
current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir))  # project dir
if parent_dir not in sys.path:
    sys.path.insert(0, parent_dir)

from Mainware import SerialCommunicator

serCom = SerialCommunicator()

# make sure min and max speed works right
print("minSpeed: " + str(serCom.min_speed()) + " maxSpeed: " + str(serCom.max_speed()))

# attempt a drive followed by a turn to test for:
#   driving works
#   turning works
#   successive commands work
serCom.drive_straight(serCom.min_speed(), 100)
serCom.rotate_in_place(serCom.min_speed(), 90)
