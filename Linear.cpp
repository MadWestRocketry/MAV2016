// Class to handle linear motor operations.
//
// Currently, all constants are stored internally in the class. If we start to hit memory issues we can
// change these to preprocessor directives, but I want to hold off on that unless absolutely necessary.

#include "Linear.h"

// Constructor: Set all relevant pins to "output" and provide default values
Linear::Linear(volatile bool *pFlag){
    pauseFlag = pFlag; // This will refer to the global pause flag variable
    pinMode(M_DOR_PIN,INPUT_PULLUP);
    pinMode(M_ERC_OUT_PIN,INPUT_PULLUP);
    pinMode(M_ERC_IN_PIN,INPUT_PULLUP);
    pinMode(M_IGN_OUT_PIN,INPUT_PULLUP);
    pinMode(M_IGN_IN_PIN,INPUT_PULLUP);

    // these are the six outputs used to control the linear motors
    // these drive relays that enable the motors to run forward or reverse
    pinMode(DOR_MIN_PIN,OUTPUT);
    pinMode(DOR_PLS_PIN,OUTPUT);
    pinMode(ERC_MIN_PIN,OUTPUT);
    pinMode(ERC_PLS_PIN,OUTPUT);
    pinMode(IGN_MIN_PIN,OUTPUT);
    pinMode(IGN_PLS_PIN,OUTPUT);

    // this sets the initial state of the digital outputs so that the relays are off at startup
    // Karnaugh map:
    // MIN  PLS
    // 1    1   both relays are de-energized, relay PCB LEDs both OFF, normally open
    // 0    0   both relays are energized, relay PCB LEDs both ON, normally closed
    // 1    0   +12V to black wire on linear stage, GND to red wire
    // 0    1   +12V to red wire on linear stage, GND to black wire
    digitalWrite(DOR_MIN_PIN,HIGH);
    digitalWrite(DOR_PLS_PIN,HIGH);
    digitalWrite(ERC_MIN_PIN,HIGH);
    digitalWrite(ERC_PLS_PIN,HIGH);
    digitalWrite(IGN_MIN_PIN,HIGH);
    digitalWrite(IGN_PLS_PIN,HIGH);
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

// When you write a "member" function of a class you must preface it with "Classname::memberFunction()" as
// you see here
void Linear::reset(){
    Serial.println("Relay conditions reset");
    digitalWrite(DOR_MIN_PIN,HIGH);
    digitalWrite(DOR_PLS_PIN,HIGH);
    digitalWrite(ERC_MIN_PIN,HIGH);
    digitalWrite(ERC_PLS_PIN,HIGH);
    digitalWrite(IGN_MIN_PIN,HIGH);
    digitalWrite(IGN_PLS_PIN,HIGH);
}
int Linear::doorExtend() {
    
    digitalWrite(DOR_PLS_PIN,LOW);
    bool flag = true;
    long timer = millis();
    while(flag){
      if(*pauseFlag){break;}
      if((millis() - timer) > DOR_TIME) {break;}
    }
    digitalWrite(DOR_PLS_PIN,HIGH);
    delay(100);
    return 0;
}

int Linear::doorRetract() {
    if (!digitalRead(M_DOR_PIN) || (*pauseFlag)) {return 0;}
    digitalWrite(DOR_MIN_PIN,LOW);
    bool flag = true;
    long timer = millis();
    while(flag){
      if(!digitalRead(M_DOR_PIN)) {break;}
      if(*pauseFlag) {break;}
      if((millis() - timer) > DOR_TIME) {break;}
    }
    digitalWrite(DOR_MIN_PIN,HIGH);
    delay(100);
    return 0;
}

int Linear::erectExtend() {
    if (!digitalRead(M_ERC_OUT_PIN) || (*pauseFlag)) {return 0;}
    digitalWrite(ERC_PLS_PIN,LOW);
    bool flag = true;
    long timer = millis();
    Serial.println (*pauseFlag);
    while(flag){
      if(!digitalRead(M_ERC_OUT_PIN)) {break;}
      if(*pauseFlag){break;}
      if((millis() - timer) > ERC_TIME) {break;}
    }
    digitalWrite(ERC_PLS_PIN,HIGH);
    delay(1000);
    return 0;
}

int Linear::erectRetract() {
    
    if (!digitalRead(M_ERC_IN_PIN) || (*pauseFlag)) {return 0;}
    digitalWrite(ERC_MIN_PIN,LOW);
    bool flag = true;
    long timer = millis();
    while(flag){
      if(!digitalRead(M_ERC_IN_PIN)) {break;}
      if(*pauseFlag) {break;}
      if((millis() - timer) > ERC_TIME) {break;}
    }
    digitalWrite(ERC_MIN_PIN,HIGH);
    delay(1000);
    return 0;
}

int Linear::igniteExtend() {
    if (!digitalRead(M_IGN_OUT_PIN) || (*pauseFlag)) {return 0;}
    digitalWrite(IGN_PLS_PIN,LOW);
    bool flag = true;
    long timer = millis();
    while(flag){
      if(!digitalRead(M_IGN_OUT_PIN)){break;}
      if(*pauseFlag){break;}
      if((millis() - timer) > IGN_TIME) {break;}
    }
    digitalWrite(IGN_PLS_PIN,HIGH);
    delay(100);
    return 0;
}

int Linear::igniteRetract() {
    if (!digitalRead(M_IGN_IN_PIN) || (*pauseFlag)) {return 0;}
    digitalWrite(IGN_MIN_PIN,LOW);
    bool flag = true;
    long timer = millis();
    while(flag){
      if(!digitalRead(M_IGN_IN_PIN)) {break;}
      if(*pauseFlag) {break;}
      if((millis() - timer) > IGN_TIME) {break;}
    }
    digitalWrite(IGN_MIN_PIN,HIGH);
    delay(100);
    return 0;
}
