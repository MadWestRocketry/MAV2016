/*******************************************************************************
 * PRE-PROCESSOR DIRECTIVES
 ******************************************************************************/
#include "Scara.h"
#include "Output.h"
#include "Input.h"
#include "Linear.h"
#include <LiquidCrystal.h>
// # define DEBUG_FLAG

/*******************************************************************************
 * STATE CONTROL BOOLEANS
 ******************************************************************************/
volatile bool pauseFlag = false; // ALL low-level actions must check for the pause flag at reasonable intervals
volatile bool goFlag = false;
volatile bool homeFlag = false;
bool homeStartupFlag = false;

/*******************************************************************************
 * OPERATION CONSTANTS
 ******************************************************************************/
 const int GO_PIN = 18;
 const int PAUSE_PIN = 19;
 const int HOME_PIN = 2;
 float SCARA_DIST[] = {5, -23, -4.5, 19.5, -180, -8.6, 8.6, 90}; // SCARA Motions
 bool SCARA_MOTOR[] = {1, 0, 1, 1, 0, 1, 1, 0}; // Scara motors: 0 for rotational, 1 for linear
 int SCARA_COUNT = 8; // Number of automated motions
 
/*******************************************************************************
 * INSTANCE OBJECTS
 ******************************************************************************/
Scara scara(&pauseFlag);       // Initialize the motor controller
Output out;                    // Initialize the output controller
Linear lin(&pauseFlag);        // Linear motor controller
Input in;                      // Input button controller

/*******************************************************************************
 * SETUP
 ******************************************************************************/
void setup(){
  #ifdef DEBUG_FLAG
  Serial.begin(9600);
  #endif
  // initialize timer1 - see https://arduino-info.wikispaces.com/Timers-Arduino
  timerInit(); // Initialize timer interrupt
  
  // Interrupt initialization - Mega, Mega2560, MegaADK has  2, 3, 18, 19, 20, 21 available for interrupts
  // 18 is "GO" and 19 is "pause" and 2 is "home"
  pinMode(HOME_PIN, INPUT_PULLUP);
  pinMode(GO_PIN, INPUT_PULLUP);
  pinMode(PAUSE_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HOME_PIN), setHome, FALLING);
  attachInterrupt(digitalPinToInterrupt(GO_PIN), setGo, FALLING);
  attachInterrupt(digitalPinToInterrupt(PAUSE_PIN), setPause, FALLING);

  
  // Setup SCARA arm for automated run - payload load sequence
  scara.setStates(SCARA_COUNT, SCARA_DIST, SCARA_MOTOR); // enable motors
  scara.enable();

  systemChecks(); // Perform important system checks
  out.printTop("SYSTEMS NOMINAL");
  out.printBottom("WAITING FOR GO");
  pause(); // Ensure all flags reset before continuing

}

/*******************************************************************************
 * PROGRAM LOOP
 ******************************************************************************/
void loop(){
  #ifdef DEBUG_FLAG
  Serial.println(scara.homed());
  delay(1000);
  #endif
  // This statement executes if the green button is pressed AND the home procedure
  // has been completed
  if (goFlag) {
    pauseFlag = false;
    execute();
    pause();
  }

  // This statement executes if the blue button is pressed.
  if (homeFlag) {
    pauseFlag = false;
    // Can add test functions here if desired and comment out "goHome"
    goHome();
    pause();
  }
}                                                                                                                                                                                                                                                                                                                                                                 

/*******************************************************************************
 * GAME DAY FUNCTIONS
 ******************************************************************************/

void execute(){
  // Perform actions necessary to win the $25,000 prize.
  
  out.setLight(2); // "setLight(2)" means blink yellow light
  out.printBottom("RUNNING");
  
  // (1) Go to a home position. Use boolean "homePositionFlag" to determine 
  // whether this step is finished or not.
  if (!homeStartupFlag) { // If we didn't run this already
    scara.linMotion(5);  // Traverse upwards 15 cm
    scara.rotMotion(270); // Rotate all the way outboard
    scara.linMotion(-30); // Bottom out the arm
    // Check if the system was paused
    if (pauseFlag) {return;} // Return if so
    homeStartupFlag = true;
  }
  
  // (2) Perform payload load sequence using the internal system states.
  out.printTop("LOADING PAYLOAD");
  int errState = scara.runAll();
  
  if (errState == -1) { // Go to "pause" state
    return;
  }

  if (errState > 0) { // Microswitch hit
    error(errState); // Go to error function (holds forever)
  }
  
  // (3) Perform linear actuator sequence. If "pause" gets hit for any of these, 
  // we'll get a "pause cascade" that will
  // end up running to the the next pause check. We use these pause checks just to 
  // keep the LCD messages lined up correctly.
  out.printTop("SECURING PAYLOAD");
  lin.doorExtend();
  lin.doorRetract();
  if (pauseFlag) {return;}
  out.printTop("ERECTING RAIL");
  lin.erectRetract();
  if (pauseFlag) {return;}
  out.printTop("IGNITION INSERT");
  lin.igniteExtend();
  
  if (pauseFlag) {return;}
  complete(); // Go to "complete" state
}

 
void goHome(){
  // Turn off yellow light
  out.yellowOff();
  // Flash red light
  out.setLight(1); // setLight(1) means blink red light
  
  out.printBottom("RUNNING");

  // (1) Clear SCARA arm from the area
  out.printTop("HOMING SCARA");
  if (!scara.homed()) { // If arm not at "home" location
    scara.linMotion(15); // Move up
    scara.rotMotion(360); // Move out
    scara.linMotion(-30); // Bottom out
  }
  
  // (2) Return linear actuators to home positions
  out.printTop("HOMING DOOR");
  lin.doorRetract(); if (pauseFlag) {return;} // Return from pause at each step
  out.printTop("HOMING IGNITOR");
  lin.igniteRetract(); if (pauseFlag) {return;} 
  out.printTop("HOMING ERECTOR");
  lin.erectExtend(); if (pauseFlag) {return;}

  // (3) Reset the SCARA arm for another run if desired
  scara.setStates(SCARA_COUNT, SCARA_DIST, SCARA_MOTOR); // enable motors
}

void systemChecks() {
  int delayTime = 1000;
  // Check reactor
  out.greenOff(); out.printTop("REACTOR..."); out.redOn(); delay(delayTime);
  out.redOff(); out.printBottom("ONLINE"); out.greenOn(); delay(delayTime);

  // Check sensors
  out.greenOff(); out.printTop("SENSORS..."); out.redOn(); delay(delayTime);
  out.redOff(); out.printBottom("ONLINE"); out.greenOn(); delay(delayTime);

  // Check sensors
  out.greenOff(); out.printTop("IGNITION..."); out.redOn(); delay(delayTime);
  out.redOff(); out.printBottom("ONLINE"); out.greenOn(); delay(delayTime);
  out.greenOff();
}

 /*******************************************************************************
 * STATE FLOW FUNCTIONS
 ******************************************************************************/
 void pause(){
  // Arrive from startup routine, "home" function, or pause button HW interrupt
  // Solid amber light
  out.setLight(0); // setLight(0) means flash no light
  out.yellowOn();

  // Reset all state flow flags
  pauseFlag = false;
  goFlag = false;
  homeFlag = false;

  out.printBottom("PAUSED");
 }
 

void error(int errorCode){
  // Arrive due to motion error in main loop or home routine
  // Solid red light
  // Display appropriate error code
  // Wait for "home" command
  out.setLight(0);
  out.redOn();
  out.printBottom("ERROR");
  
  while (true) {}
  
}

void complete() {
  // Solid green light
  // Sound horn
  // Wait for "home" command
  homeFlag = false;
  out.setLight(0);
  out.greenOn();
  out.printTop("PROCESS COMPLETE");
  out.printBottom("WAITING FOR HOME");
  while (!homeFlag) {
    continue;
  }

  out.greenOff();
}

/*******************************************************************************
 * SYSTEM TEST FUNCTIONS
 ******************************************************************************/

void scaraTest() {
  // Fully test the SCARA arm. Hand on the E-stop, please.
  // Move using the manual arm controls.
  scara.linMotion(30); // Full up
  scara.linMotion(-270); // Full inboard
  scara.linMotion(180); // Most of way inb
  scara.linMotion(90); // All way inboard
  scara.linMotion(-30); // Full down
  
  // Move using the automated SCARA system
  int errState = scara.runAll();
  if (errState == -1) { // Go to "pause" state
    pause();
    return;
  }

  if (errState > 0) { // Microswitch hit
    error(errState);
  }
  
}

void outputTest() {
  //Fully test the stack tower.
  // Test LCD
  
  out.printMessage("printMessage Top", "printMessage Bot"); delay(1000);
  out.printTop("printTop"); delay(1000);
  out.printBottom("printBottom"); delay(1000);

  // Test lights
  out.yellowOn(); delay(1000);
  out.redOn(); delay(1000);
  out.greenOn(); delay(1000);

  out.printTop("All lights off");
  out.ledOff(); out.yellowOff(); out.redOff(); out.greenOff();
  delay(1000);
  
  out.printTop("TEST COMPLETE");
}

void linearTest() {
  // Test the linear actuators
  out.yellowOff();
  out.setLight(1); // setLight(1) means blink red light
  //Fully test the linear actuators. SAFE SETUP NECESSARY. Hands on E-stop
  //lin.doorExtend(); // 7 seconds
  //lin.doorRetract();  // 10 seconds. Unless meet microswitch
  lin.erectRetract();  // 10 seconds. Unless meet microswitch
  out.printTop("EXTENDED");
  //lin.igniteExtend(); // 10 seconds. Unless meet microswitch
  //lin.igniteRetract(); // 10 seconds. Unless meet microswitch
  lin.erectExtend();  // 10 seconds. Unless meet microswitch
  out.printTop("RETRACTED");
}


/*******************************************************************************
 * INTERRUPT HANDLING
 ******************************************************************************/
 
// TIMER INITIALIZATION
void timerInit() {
  // This is magic. Don't touch it.
    noInterrupts();           // disable all interrupts
    TCCR1A = 0;               // set entire TCCR1A register to 0
    TCCR1B = 0;               // same for TCCR1B 
    TCNT1  = 0;               // Initialize counter to 0
  
    OCR1A = 62500;            // compare match register 16MHz/256/1Hz
    TCCR1B |= (1 << WGM12);   // CTC mode
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
    interrupts();             // enable all interrupts
}

// INTERRUPT SERVICE ROUTINES 
void setPause() { // set a global "pause" flag to "True"
  Serial.println("Pause Interrupt");
  pauseFlag = true;
}

void setGo() { // Set global "go" flag to "true"
  goFlag = true;
}

void setHome() { // Set global "go" flag to "true"
  homeFlag = true;
}

//This odd function blinks our light
ISR(TIMER1_COMPA_vect) {       // timer compare interrupt service routine
 out.ledToggle();
 out.toggle();
}











