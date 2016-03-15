// Class to handle AGSE outputs: Light tower/horn and LCD screen.
//
// Currently, all constants are stored internally in the class. If we start to hit memory issues we can
// change these to preprocessor directives, but I want to hold off on that unless absolutely necessary.

#include "Output.h"
#include <LiquidCrystal.h>

// Constructor: Set all relevant pins to "output" and provide default values
Output::Output(){
    pinMode(GRN_PIN, OUTPUT);
    pinMode(YLW_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(HRN_PIN, OUTPUT);
    pinMode(LED_MEGA_ONBOARD, OUTPUT);

    // relays are energized when outputs of Arduino are LOW
    // LED of relay PCBA will turn on when pins are pulled LOW
    digitalWrite(GRN_PIN, HIGH);
    digitalWrite(RED_PIN, HIGH);
    digitalWrite(YLW_PIN, HIGH);
    digitalWrite(HRN_PIN, HIGH);

    // initialize the library with the numbers of the interface pins
    // (note that the below function takes care of pinmode for all relevant pins for you)
    lcd = new LiquidCrystal(LCD_RS,LCD_EN,LCD_D4,LCD_D5,LCD_D6,LCD_D7); // OUTPUT, LCD 4-bit data

    // LCD setup
    lcd -> begin(LCD_columns, LCD_rows); // declares the size of the LCD we're using (columns by rows)
    // printMessage("", "");


}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

// When you write a "member" function of a class you must preface it with "Classname::memberFunction()" as
// you see here
void Output::redOn() {
    digitalWrite(RED_PIN, LOW);
}

void Output::redOff() {
    digitalWrite(RED_PIN, HIGH);
}

void Output::redToggle() {
    if (digitalRead(RED_PIN)) { // If light is OFF
      redOn();
    }
    else {
      redOff();
    }

}

void Output::greenOn() {
    digitalWrite(GRN_PIN, LOW);
}

void Output::greenOff() {
    digitalWrite(GRN_PIN, HIGH);
}

void Output::greenToggle() {
    if (digitalRead(GRN_PIN)) { // If light is OFF
      greenOn();
    }
    else {
      greenOff();
    }
}

void Output::yellowOn() {
    digitalWrite(YLW_PIN, LOW);
}

void Output::yellowOff() {
    digitalWrite(YLW_PIN, HIGH);
}

void Output::yellowToggle() {
    if (digitalRead(YLW_PIN)) { // If light is OFF
      yellowOn();
    }
    else {
      yellowOff();
    }
}
void Output::ledOn() {
    digitalWrite(LED_MEGA_ONBOARD, HIGH);
}

void Output::ledOff() {
    digitalWrite(LED_MEGA_ONBOARD, LOW);
}

void Output::ledToggle(){
if (!digitalRead(LED_MEGA_ONBOARD)) { // If light is OFF
     ledOn();
    }
    else {
     ledOff();
    }
}

void Output::setLight(int light){
  // Turn off the currently blinking light, in case it's on
  if (currentLight != 0) {
    digitalWrite(currentLight, HIGH);
  }
  switch (light) {
    case (0): {currentLight = 0; break;}       // None
    case (1): {currentLight = RED_PIN; break;} // Red
    case (2): {currentLight = YLW_PIN; break;} // Yellow
    case (3): {currentLight = GRN_PIN; break;} // Green
    default: {break;}
    }
  }
  

void Output::toggle() { // Toggle current light setting
  if (currentLight == 0) {return;}
  digitalWrite(currentLight, !digitalRead(currentLight));
}

void Output::randomLight()  {
  int number=random(6);
  while(number==0){
    greenOn();
    delay(1000);
    number=random(6);
    delay(1000);
  }
  while(number==1){
    greenOff();
    delay(1000);
    number=random(6);
    delay(1000);
  }
  while(number==2){
    yellowOn();
    delay(1000);
    number=random(6);
    delay(1000);
  }
  while(number==3){
    yellowOff();
    delay(1000);
    number=random(6);
    delay(1000);
  }
  while(number==4){
    redOn();
    delay(1000);
    number=random(6);
    delay(1000);
  }
  while(number==5){
    redOff();
    delay(1000);
    number=random(6);
    delay(1000);
  }
}

void Output::hornBlast(int duration) {
    digitalWrite(HRN_PIN, LOW);
    delay(duration);
    digitalWrite(HRN_PIN, HIGH);
}

void Output::printTop(const char *message) {  
  lcd -> setCursor(0, 0);
  lcd -> print("                "); // Print 16 char to clear line
  lcd -> setCursor(0, 0);
  lcd -> print(message);
}  

void Output::printBottom(const char *message) {  
  lcd -> setCursor(0, 1);
  lcd -> print("                "); // Print 16 char to clear line
  lcd -> setCursor(0, 1);
  lcd -> print(message);
}

void Output::printMessage(const char *topMessage, const char *bottomMessage){
  lcd -> clear();
  lcd -> setCursor(0, 0);
  lcd -> print(topMessage);
  lcd -> setCursor(0, 1);
  lcd -> print(bottomMessage);
}


