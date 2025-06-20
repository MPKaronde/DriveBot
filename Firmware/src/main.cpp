#include <Arduino.h>
#include <AccelStepper.h>

#define HALFSTEP 4

const int FRONT_BUMP_SWITCH = 8;
const int BAUD_RATE = 9600;
const int MOTOR_ACCEL = 1000000;
const int MAX_SPEED = 1200;

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

void loop()
{
}