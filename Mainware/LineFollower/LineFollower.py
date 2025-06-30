import SerialCommunicator
import CVModule

# make neccesary classes
serialComs = SerialCommunicator()
cv = CVModule()


# simple single step in the line following loop (correct course or continue foward)
def line_follower_step():
    turnDegs = 10  # distance to turn by when correcting
    fowardDist = 100  # distance to go foward when line in box

    # if line out of box, correct, otherwise drive foward
    if cv.capture_and_process() == "left":
        serialComs.rotateInPlace(serialComs.min_speed(), -turnDegs)
    elif cv.capture_and_process() == "right":
        serialComs.rotateInPlace(serialComs.min_speed(), turnDegs)
    else:
        serialComs.drive_straight(serialComs.min_speed(), fowardDist)


# simple line follower loop runs for given number of steps
def line_follower_loop(numSteps):
    for i in range(numSteps):
        line_follower_step()
