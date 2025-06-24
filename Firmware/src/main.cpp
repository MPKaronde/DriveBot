#include <Arduino.h>
#include <AccelStepper.h>

#define HALFSTEP 4

const int FRONT_BUMP_SWITCH = 13; // pin for foward bump switches
const int BAUD_RATE = 9600;       // serial baud rate
const int MOTOR_ACCEL = 1000;     // motor acceleraton rate
const int MAX_SPEED = 1000;       // (absolute value) max motor speed allowed
const int MIN_SPEED = 400;        // (absolute value) min motor speed allowed
const int TICKS_PER_CM = 35;      // Number of motor ticks per cm traveled in straight line NOTE: Actual calculated value = 35.0877193
const int TRACK_WIDTH = 148;      // mm distance between ground contact of the drive wheels
const int WHEEL_DIAMATER = 57;    // mm diameter of the wheels

AccelStepper Right(HALFSTEP, 7, 6, 5, 4);
AccelStepper Left(HALFSTEP, 8, 9, 10, 11);

void setup()
{
    // Serial Setup
    Serial.begin(BAUD_RATE);

    // Switch Setup
    pinMode(FRONT_BUMP_SWITCH, INPUT_PULLUP);

    // Motor Setup
    Left.setAcceleration(MOTOR_ACCEL);
    Left.setMaxSpeed(MAX_SPEED);
    Left.setSpeed(MAX_SPEED);
    Right.setAcceleration(MOTOR_ACCEL);
    Right.setMaxSpeed(MAX_SPEED);
    Right.setSpeed(MAX_SPEED);
}

// Input: percentage of range between min & max speed
// Returns: appropriate ticks per second for motors to attain given percent
int get_speed_percentage(double percent)
{
    /*
    FORMULA:
    1. find distance between lowest and highest speeds
    2. find how far between the two the percentage is
    3. offset by minspeed
    */
    return ((double)(MAX_SPEED - MIN_SPEED) * percent) + MIN_SPEED;
}

// Input: number cm desired to traverse (double w/ precison up to 4 decimals)
// Output: number motor ticks required
int get_steps_from_cm(double cm)
{
    return (int)(cm * (double)TICKS_PER_CM);
}

// Input: Speed (motor speed) & Distance (ticks) desired to drive in straight line
// Behavior: Robot drives at given speed for given distance in straight line
// Return: True if valid values, False if invalid values or error deteced
bool drive_straight(int speed, int distance)
{
    // speed out of range
    if (speed < MIN_SPEED || speed > MAX_SPEED)
    {
        return false;
    }

    // set desired speed and distance into motors
    Left.setMaxSpeed(speed);
    Right.setMaxSpeed(speed);
    Left.moveTo(distance);
    Right.moveTo(distance);

    // run motors as needed
    while (Left.distanceToGo() != 0 || Right.distanceToGo() != 0)
    {
        Left.run();
        Right.run();
    }

    return true;
}

// Input: Degrees to rotate by Speed (motor speed) to rotate in
// Behavior: Rotate in place by given amount at given speed (runs both motors opposite directions for required time at given speed)
// Return: True if valid values, False if invalid values or error deteced
bool rotate_in_place(int degrees, int speed)
{
    // speed out of range
    if (speed < MIN_SPEED || speed > MAX_SPEED)
    {
        return false;
    }

    // set motor speed
    Left.setMaxSpeed(speed);
    Right.setMaxSpeed(speed);

    /*
    Calculate Ticks::
    Formulas:
    Wheel Rotations = (TrackWidth * Degrees) / (360 * Wheel Diameter)
    Ticks per rotation = 200
    Therefore,
    Ticks = (TrackWidth * Degrees) / (360 * 200 * Wheel Diameter)
    */
    int ticks = (TRACK_WIDTH * degrees) / (72000 * WHEEL_DIAMATER);

    // Determine if going left or right
    // TODO: determine if this is accurate
    if (degrees > 0)
    {
        Left.moveTo(ticks);
        Right.moveTo(-ticks);
    }
    else
    {
        Left.moveTo(-ticks);
        Right.moveTo(ticks);
    }

    while (Left.distanceToGo() != 0 || Right.distanceToGo() != 0)
    {
        Left.run();
        Right.run();
    }

    return true;
}

void loop()
{
    int distance = 1000;

    while (1 == 1)
    {
        Left.moveTo(distance);
        Right.moveTo(distance);

        while (Left.distanceToGo() != 0)
        {
            Left.run();
            Right.run();
        }

        distance = -distance;
    }
}