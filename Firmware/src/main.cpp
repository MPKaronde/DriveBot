#include <Arduino.h>
#include <AccelStepper.h>

#define HALFSTEP 4

const int FRONT_BUMP_SWITCH = 13; // pin for foward bump switches
const int BAUD_RATE = 9600;       // serial baud rate
const int MOTOR_ACCEL = 1000;     // motor acceleraton rate
const int MAX_SPEED = 1000;       // (absolute value) max motor speed allowed
const int MIN_SPEED = 400;        // (absolute value) min motor speed allowed
const int TICKS_PER_CM = 35;      // Number of motor ticks per cm traveled in straight line NOTE: Actual calculated value = 35.0877193

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