#include <Arduino.h>
#include <AccelStepper.h>

#define HALFSTEP 4

const int FRONT_BUMP_SWITCH = 13; // pin for foward bump switches
const int BAUD_RATE = 9600;       // serial baud rate
const int MOTOR_ACCEL = 1000;     // motor acceleraton rate
const int MAX_SPEED = 1000;       // (absolute value) max motor speed allowed
const int MIN_SPEED = 400;        // (absolute value) min motor speed allowed
const double TICKS_PER_CM = 12;   // Number of motor ticks per cm traveled in straight line NOTE: Actual calculated value = 11.16876794
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
int get_speed_from_percentage(double percent)
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
    return (int)(cm * TICKS_PER_CM);
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
    Ticks = (TrackWidth * Degrees * 200) / (360 * Wheel Diameter)
    */
    int ticks = (((double)TRACK_WIDTH * (double)degrees * 200.0)) / ((double)(360.0 * (double)WHEEL_DIAMATER));

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

    // run the motors
    while (Left.distanceToGo() != 0 || Right.distanceToGo() != 0)
    {
        Left.run();
        Right.run();
    }

    return true;
}

// Input: how far to go in cm on each straight and what % between min and max speed to go
// Behavior: robot drives in a square with given length and speed
// Return: none
void test_drive_loop(int distance_cm, double speed_percentage)
{
    int speed = get_speed_from_percentage(speed_percentage);
    drive_straight(speed, distance_cm);
    rotate_in_place(90, speed / 2);
    drive_straight(speed, distance_cm);
    rotate_in_place(90, speed / 2);
    drive_straight(speed, distance_cm);
    rotate_in_place(90, speed / 2);
    drive_straight(speed, distance_cm);
}

void loop()
{
    bool go = true;
    if (go)
    {
        // drive_straight(MIN_SPEED, 100);
        rotate_in_place(90, MIN_SPEED);
        go = false;
    }
}