// Class to handle movement of the SCARA robotic arm. This class controls the motion of two stepper motors,
// which move in the vertical and horizontal directions. It also monitors for the depression of microswitches
// corresponding to the current travel direction.
//
// Currently, all constants are stored internally in the class. If we start to hit memory issues we can
// change these to preprocessor directives, but I want to hold off on that unless absolutely necessary.

#include "Scara.h"
#include "math.h"

// Constructor: Set all relevant pins to "output" and provide default values
Scara::Scara(volatile bool *pFlag) { // Pass in the global pause flag by reference for internal use
    // Build pin index arrays: Rotational motor is pin index "0," linear motor is pin index "1"
    directionPins[0] = ROT_DIR_PIN; directionPins[1] = LIN_DIR_PIN;
    pulsePins[0] = ROT_PUL_PIN; pulsePins[1] = LIN_PUL_PIN;
    switchPins[0] = ROT_PLS_PIN; switchPins[1] = ROT_MIN_PIN;
    switchPins[2] = LIN_PLS_PIN; switchPins[3] = LIN_MIN_PIN;

    // Set all rotational and linear pins to output
    pinMode(RELAY_PWR_PIN, OUTPUT);
    pinMode(ROT_DIR_PIN, OUTPUT); 
    pinMode(ROT_PUL_PIN, OUTPUT);
    pinMode(ROT_ENA_PIN, OUTPUT);
    pinMode(ROT_PLS_PIN, INPUT_PULLUP);
    pinMode(ROT_MIN_PIN, INPUT_PULLUP);
    
    pinMode(LIN_DIR_PIN, OUTPUT);
    pinMode(LIN_PUL_PIN, OUTPUT);
    pinMode(LIN_ENA_PIN, OUTPUT);
    pinMode(LIN_PLS_PIN, INPUT_PULLUP);
    pinMode(LIN_MIN_PIN, INPUT_PULLUP);
    
    // Write all output pins to "low"
    digitalWrite(ROT_DIR_PIN, LOW);
    digitalWrite(LIN_DIR_PIN, LOW);
    digitalWrite(ROT_PUL_PIN, LOW);
    digitalWrite(LIN_PUL_PIN, LOW);
    
    // Write "enable" pins HIGH to disable the motors
    // (Might be causing motor hiccups)
    digitalWrite(ROT_ENA_PIN, HIGH);
    digitalWrite(LIN_ENA_PIN, HIGH);

    // Write relay power pin to HIGH (to disable the relays/motors)
    // (Might be causing motor hiccups)
    digitalWrite(RELAY_PWR_PIN, HIGH);

    pauseFlag = pFlag; // Set global pause flag reference
}

void Scara::enable() {
  // Safely enable the motors for motion
  // Ensure the "enable" pins are HIGH
  digitalWrite(LIN_ENA_PIN, HIGH);
  digitalWrite(ROT_ENA_PIN, HIGH);

  // Close the stepper power relay
  digitalWrite(RELAY_PWR_PIN, LOW);

  // Wait a second
  delay(1000);

  // Set "enable" pins LOW to enable the motors
  digitalWrite(LIN_ENA_PIN, LOW);
  digitalWrite(ROT_ENA_PIN, LOW);
}

void Scara::disable() {
  // Safely disable the motors
  // Write "enable" pin HIGH to disable the motors
  digitalWrite(LIN_ENA_PIN, HIGH);
  digitalWrite(ROT_ENA_PIN, HIGH);

  // wait a second
  delay(1000);

  // Open the stepper power relay
  digitalWrite(RELAY_PWR_PIN, HIGH);
}

// Set internal states for automated routine
void Scara::setStates(int moCount, float floatDistances[], bool intMotors[]) {

    // Set up the internal state distances
    motionCount = moCount;
    for(int i = 0; i < motionCount; i++){ // For each motion
      // Set the boolean motor values
      internalMotors[i] = intMotors[i];
      
      // Set the number of steps for each motion
      if (internalMotors[i] == 0) { // Rotational motion
        float factor = (float)(ROT_STEP_PER_DEG);
        internalDistances[i] = (long)(factor*floatDistances[i]);
      }
      else { // Linear motion
        float factor = (float)(LIN_STEP_PER_CM);
        internalDistances[i] = (long)(factor*floatDistances[i]);
      }
    }
    return;
}

bool Scara::homed() {
  // Check if the arm is at its "home" state - full outboard, full down
  return ((!digitalRead(LIN_MIN_PIN) && !digitalRead(ROT_PLS_PIN)));
}

long Scara::pulseTrain(int pin, int switchIndex, long steps, long startSpeed, long accelRate) {
  long counter = 0;
  float period;
  float currentSpeed = float(startSpeed);
  while(counter < steps){
    // Check for any limit switches;
    if (!digitalRead(switchPins[switchIndex])) { // Not sure about logic here
        errorCode = switchIndex + 1; // Set internal error code
        return counter;
    }
    if (*pauseFlag) { // If pause flag is flipped
      
      errorCode = -1;
      return counter;
    }
    period = M/currentSpeed; // Current period in microseconds
    pulse(pin, (int)(period)); // I think this is slowing us down...
    currentSpeed += accelRate*period/M;
    counter++;
  }
  
  return counter; // Need to make sure this returns the accurate number
}


void Scara::pulse(int pin, int period) {
  period = (int)(period*0.5);
  digitalWrite(pin, HIGH);
  delayMicroseconds(period);
  digitalWrite(pin, LOW);
  delayMicroseconds(period);
}


long Scara::runMotor(int pinIndex, long steps, long maxSpeed, long accel) {
    if (steps == 0) {return 0;} // Exit immediately if 0 steps required
    if ((pinIndex != 0) && (pinIndex != 1)) { return 0;} // Exit immediately for bad pin index

    float accelSteps = ((float)(maxSpeed)*(float)(maxSpeed))/((float)(accel)*2.0);
    float decelSteps;
    float constSteps;
    int dir = (steps > 0) - (steps < 0); // Get sign of the motion; no standard "sign" function in C++
    steps = abs(steps); // Change "steps" to absolute value

    // Handle microswitch indexing
    // Serial.print("dir = "); Serial.println(dir);
    int switchIndex;
    if (dir == 1) { // Moving in positive direction
        switchIndex = 2*pinIndex;
    }
    else { // Otherwise, moving in the negative direction
        switchIndex = 2*pinIndex + 1;
    }

    // Serial.print("pinIndex = "); Serial.println(pinIndex);
   // Serial.print("switchIndex = "); Serial.println(switchIndex);

    // Determine whether motion profile is trapezoidal or triangular
    if(accelSteps*2 >= steps){ // Triangular profile
      constSteps = 0;
      accelSteps = steps/2;
      decelSteps = steps/2;
      maxSpeed = (int) sqrt(2.0*(float)(accel)*(accelSteps));
    }
    else // Trapezoidal profile
    {
      decelSteps = accelSteps;
      constSteps = steps - (accelSteps + decelSteps);
    }

    // Re-cast the step counts as long integers for use below
    accelSteps = (long) accelSteps;
    decelSteps = (long) decelSteps;
    constSteps = (long) constSteps;

    // Set direction
    if(dir > 0){
        //Serial.println("Writing Voltage High");
        digitalWrite(directionPins[pinIndex], HIGH);
    }
    else
    {
        //Serial.println("Writing Voltage Low");
        digitalWrite(directionPins[pinIndex], LOW);
    }

    // Perform motion
    // Acceleration
    long stepCount = pulseTrain(pulsePins[pinIndex], switchIndex,
            accelSteps, sqrt(accel), accel);
    steps -= stepCount;
    if (stepCount != accelSteps) { // If this happens, something is wrong
        return dir*steps; // Signed number of steps remaining
    }
    // Constant Speed
    stepCount = pulseTrain(pulsePins[pinIndex], switchIndex,
            constSteps, maxSpeed, 0);
    steps -= stepCount;
    if (stepCount != constSteps) { // If this happens, something is wrong
        return dir*steps; // Signed number of steps remaining
    }
    // Deceleration
    stepCount = pulseTrain(pulsePins[pinIndex], switchIndex,
            decelSteps, maxSpeed, -accel);
    steps -= stepCount;

    return (long)(dir)*steps; // Signed number of steps remaining
}


long Scara::intRotMotion(long steps) {
    // Send instructions to the "runMotor" function; get back number of steps actually completed and subtract from "target steps."
    return runMotor(0, steps, ROT_MAX_SPEED, ROT_ACCEL);
}


long Scara::intLinMotion(long steps) {
    // Send instructions to the "runMotor" function; get back number of steps actually completed and subtract from "target steps."
    return runMotor(1, steps, LIN_MAX_SPEED, LIN_ACCEL);
}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/
float Scara::rotMotion(float distance_deg) {
    // Convert "distance_deg" to a number of steps, set as "target steps"
    long rotTarget = (long) (distance_deg*ROT_STEP_PER_DEG);

    // Send instructions to the "runMotor" function
    long stepsComplete = runMotor(0, rotTarget,
           ROT_MAX_SPEED, ROT_ACCEL);
      // Get back number of steps actually completed and subtract from "target steps."

    // Return distance remaining from movement
    return (float)(stepsComplete)/ROT_STEP_PER_DEG;
}

float Scara::linMotion(float distance_cm) {
    // Convert "distance_cm" to a number of steps, set as "target steps"
    long linTarget = (long) (distance_cm*LIN_STEP_PER_CM);

    // Send instructions to the "runMotor" function
    long stepsComplete = runMotor(1, linTarget,
           LIN_MAX_SPEED, LIN_ACCEL);
      // Get back number of steps actually completed
    return (float)(stepsComplete)/LIN_STEP_PER_CM;
}

int Scara::runAll() {
  errorCode = 0;
  for(int i = 0; i < motionCount; i++) { // Loop through every internal motion
    if (internalDistances[i] == 0) {continue;} // kick out if no distance left

    if (internalMotors[i] == 0) { // Rotational motion
      internalDistances[i] = intRotMotion(internalDistances[i]);
    }
    else {
      internalDistances[i] = intLinMotion(internalDistances[i]);
    }
    if (errorCode != 0) {break;}
  }

  return errorCode;
}
