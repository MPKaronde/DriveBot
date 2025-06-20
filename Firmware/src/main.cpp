#include <Arduino.h>
#include <AccelStepper.h>

#define HALFSTEP 4

const int FRONT_BUMP_SWITCH = 8; // pin for foward bump switches
const int BAUD_RATE = 9600;      // serial baud rate

// TODO: Determine exact values
const int MOTOR_ACCEL = 1000000; // motor acceleraton rate
const int MAX_SPEED = 1200;      // (absolute value) max motor speed allowed
const int MIN_SPEED = 800;       // (absolute value) min motor speed allowed
const int TICKS_PER_CM = 50;     // Number of motor ticks per cm traveled in straight line

AccelStepper Left(HALFSTEP, 0, 1, 2, 3);
AccelStepper Right(HALFSTEP, 4, 5, 6, 7);

void setup()
{
    // Serial Setup
    Serial.begin(BAUD_RATE);

    // Switch Setup
    pinMode(FRONT_BUMP_SWITCH, INPUT_PULLUP);

    // Motor Setup
    Left.setAcceleration(MOTOR_ACCEL);
    Left.setMaxSpeed(MAX_SPEED);
    Right.setAcceleration(MOTOR_ACCEL);
    Right.setAcceleration(MAX_SPEED);
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

void loop()
{
}