/*
Main firmware code for the SLAMBot to be run on Arduino Uno.
Commands to be sent and interacted with over serial w/ the rest of the system

NOTE: most of the methods attempt to move by a fixed number of mm and/or degrees. Although the speed can be trusted,
      the distance measurements are merely an ESTIMATE and NOT TO BE FULLY TRUSTED. Although the bot is run via steppers,
      variations such as voltage level, wheel compression, otherwise unknown drag, etc, can cause the system to drift from these
      measurements so other onboard sensors (distance sensors, camera) should be used with this & these distance should absolutely
      NOT be applied through Dead Reckoning
*/

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

bool SERIAL_RUNNING = true; // whether to run serial loop. Setting to false will require arduino reset to set true.

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
bool rotate_in_place(int speed, int degrees)
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

// Input: serial_command - a command read over serial in understandable format
// Behavior: run appropriate command
// Return: true if command success, false if command failure or command not recognized
bool run_serial(String serial_command)
{
    // Parse main command name
    int space1 = serial_command.indexOf(" ");
    String main_command = serial_command;
    if (space1 != -1)
    {
        main_command = serial_command.substring(0, space1);
    }

    // parse individual commands
    if (main_command == "drive_straight")
    {
        // cant be drive_straight
        if (space1 == -1)
        {
            return false;
        }
        int space2 = serial_command.indexOf(" ", space1);
        // cant be drive_straight
        if (space2 == -1)
        {
            return false;
        }
        int speed = serial_command.substring(space1, space2).toInt();
        int distance = serial_command.substring(space2).toInt();
        return drive_straight(speed, distance);
    }
    else if (main_command == "rotate_in_place")
    {
        // cant be rotate in place
        if (space1 == -1)
        {
            return false;
        }
        int space2 = serial_command.indexOf(" ", space1);
        // cant be rotate in place
        if (space2 == -1)
        {
            return false;
        }
        int speed = main_command.substring(space1, space2).toInt();
        int degrees = main_command.substring(space2).toInt();
        return rotate_in_place(speed, degrees);
    }
    else if (main_command == "end")
    {
        SERIAL_RUNNING = false;
    }
    // unknown command
    return false;
}

// Input: none
// Behavior: move away from source of bumps if needed
// Return: none
void respond_to_bumps()
{
    // if front bumped, back off
    if (digitalRead(FRONT_BUMP_SWITCH) == LOW)
    {
        drive_straight(MIN_SPEED, -100);
    }

    // whenever rear is bumped, go foward and away
    // TODO: apply when rear switch added
}

// Input: none
// Behavior: none
// Return: true if any switch has been bumped
bool check_bumps()
{
    return digitalRead(FRONT_BUMP_SWITCH) == LOW;
}

// Input: none
// Behavior: continues interpreting and communicating over serial / running machine as long as SERIAL_RUNNING is true
// Return: none
void serial_runner()
{
    int numCommands = 0;
    while (SERIAL_RUNNING)
    {
        // block while waiting for next command or bump switch
        while (Serial.available() == 0 || !check_bumps())
        {
        }

        // if bumped, respond to bumps
        if (check_bumps())
        {
            respond_to_bumps();
        }
        // otherwise command recieved
        else
        {
            numCommands++;
            String command = Serial.readString();
            bool commandExecuted = run_serial(command);
            String printBack = "command no :: " + (String)numCommands + "       execution state :: " + (String)commandExecuted;
            Serial.println(printBack);
        }
    }
    Serial.println("Session ended       # commands executed :: " + numCommands);
}

void loop()
{
    serial_runner();
}