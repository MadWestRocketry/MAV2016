#ifndef SCARA_H
#define SCARA_H

#include <Arduino.h> //not sure if we need this; we might for the built-in Arduino functions

class Scara {

    public:
        Scara(volatile bool *pFlag); // Constructor
        void setStates(int moCount, float floatDistances[], bool intMotors[]); // Constructor using internal states
        void enable();                       // Safely enable the stepper motors
        void disable();                      // Safely disable the stepper motors
        float linMotion(float distance_cm); // Move the vertical arm by a distance in cm
        float rotMotion(float distance_deg);   // Move the rotational arm by a distance in deg
        int runAll();                         // Run any internal motions
        bool homed();                         // Check if arm at home location


    private:
        // Hardware/other constants
        const float M = 1000000;      // The number "1 million"

        const int RELAY_PWR_PIN = 28; // OUTPUT, Relay "enable" pin

        const int ROT_ENA_PIN = 6;    // OUTPUT, Rotational motor "enable" pin
        const int ROT_DIR_PIN = 7;    // OUTPUT, Rotational motor "direction" pin
        const int ROT_PUL_PIN = 8;    // OUTPUT, Rotational motor "pulse" pin

        const int ROT_PLS_PIN = 30;   // INPUT, Rotational motor "plus" microswitch
        const int ROT_MIN_PIN = 31;   // INPUT, Rotational motor "minus" microswitch

        const int LIN_ENA_PIN = 10;   // OUTPUT, Linear motor "enable" pin
        const int LIN_DIR_PIN = 11;   // OUTPUT, Linear motor "direction" pin
        const int LIN_PUL_PIN = 12;   // OUTPUT, Linear motor "pulse" pin

        const int LIN_PLS_PIN = 33;   // INPUT, Linear motor "plus" microswitch
        const int LIN_MIN_PIN = 32;   // INPUT, Linear motor "minus" microswitch

        // ALL STEPPER MOTOR VALUES MUST BE LONG (32,767 steps vs 2.14 million)
        const long LIN_STEP_PER_CM = 5328;  // Linear motor steps per cm at 1/128 microsteps
        const long ROT_STEP_PER_DEG = 71;   // Rotational motor steps per degress at 1/128 microsteps
        // const long ROT_MAX_SPEED = 1800;    // Rotational Stepper motor maximum speed; steps per second
        const long ROT_MAX_SPEED = 1200;
        const long LIN_MAX_SPEED = 4800;    // Linear motor maximum speed; microsteps per second
        const long ROT_ACCEL = 150;         // Rotational motor acceleration rate; steps per second^2
        const long LIN_ACCEL = 720;         // Linear motor acceleration rate; steps per second^2
        int errorCode = 0;                  // Internal error code, 0 = nominal
        volatile bool *pauseFlag;            // Reference to globa pause flag

        int directionPins[2];
        int pulsePins[2];
        int switchPins[4];

        // Internal state trackers
        int motionCount;                    // Number of internal motions
        long internalDistances[16];         // Array of distances to target position (max 16)
        bool internalMotors[16];            // Array of motor integers; [0] = rotational, [1] = linear (max 16)

        // Private functions
        long runMotor(int pinIndex, long steps, long maxSpeed, long accel); // Run a stepper motor
        long pulseTrain(int pin, int switchIndex, long steps, long startSpeed, long accelRate); // Send a train of pulses
        void pulse(int pin, int period);    // Send a single pulse of a given period
        long intLinMotion(long steps);      // Move the vertical arm by a distance in steps
        long intRotMotion(long steps);      // Move the rotational arm by a distance in steps
};

#endif
