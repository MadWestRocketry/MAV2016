#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h> //not sure if we need this; we might for the built-in Arduino functions
#include <LiquidCrystal.h>

class Output {

    public:
        Output(); // Constructor

        // Light operations
        void redOn();      // Turn on red light
        void redOff();     // Turn off red light
        void redToggle();  // Toggle red light

        void yellowOn();    // ...
        void yellowOff();
        void yellowToggle();

        void greenOn();   
        void greenOff();
        void greenToggle();

       
        void ledOn(); 
        void ledOff();
        void ledToggle();

        void setLight(int light);
        void toggle();

        void randomLight();

        // Horn works a little differently
        void hornBlast(int duration); // Provide a length of time for the horn to sound in milliseconds

        // LCD Operations
        void printTop(const char *message);
        void printBottom(const char *message);
        void printMessage(const char *topMessage, const char *bottomMessage);
        


    private:
        // ADD ALL REQUIRED PINS AS CONST INT
        const int RED_PIN = 14;       // OUTPUT, red stack light
        const int YLW_PIN = 15;       // OUTPUT, yellow stack light
        const int GRN_PIN = 16;       // OUTPUT, green stack light
        const int HRN_PIN = 17;       // OUTPUT, alarm horn
        // ALSO ADD ANY OTHER REQUIRED FUNCTIONS AS PRIVATE FUNCTIONS

        const int LCD_RS = 44;         // OUTPUT, LCD reset
        const int LCD_EN = 45;         // OUTPUT, LCD enable
        const int LCD_D4 = 46;         // OUTPUT, LCD 4-bit data
        const int LCD_D5 = 47;         // OUTPUT, LCD 4-bit data
        const int LCD_D6 = 48;         // OUTPUT, LCD 4-bit data
        const int LCD_D7 = 49;         // OUTPUT, LCD 4-bit data

        const int LCD_columns = 16; // display number of characters wide
        const int LCD_rows = 2; // display number of rows

        const int LED_MEGA_ONBOARD = 13; // OUTPUT, green LED on ATMega 2560 PCBA main board

        int currentLight = 0; // Current light for blinking

        LiquidCrystal *lcd;

};
#endif
