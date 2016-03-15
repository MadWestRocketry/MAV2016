#ifndef LINEAR_H
#define LINEAR_H

#include <Arduino.h> //not sure if we need this; we might for the built-in Arduino functions

class Linear {

    public:
        Linear(volatile bool *pFlag); // Constructor

        // Motor operations. Each of these will extend or retract the relevant motor, until:
        // (A) The relevant microswitch is triggered
        // (B) The "pause" button is pushed, sending the AGSE to its "pause" state. This condition is monitored
        // with the internal boolean value "pauseFlag."
        // Case (A) relates to a succesful function completion, return a "0." If Case (B) causes the pause, return a "-1."
        
        void reset();         // resets all relay position
        int doorExtend();     // extend; closes the payload door
        int doorRetract();    // retract
        int erectExtend();    // extend; lowers the launch rail
        int erectRetract();   // retract; raises the launch rail
        int igniteExtend();   // extend; inserts ignitor into rocket
        int igniteRetract();  // retract; removes ignitor from rocket


    private:
        // ADD ALL REQUIRED PINS AS CONST INTS
        // Notation:
        // "MIN" is black wire, "PLS" is the red wire
        // "DOR" = door open/close cylinder
        // "IGN" = igniter insert/retract cylinder
        // "ERC" = erector raise/lower cylinder

        volatile bool *pauseFlag;      // This connects to the global pause state of the AGSE.
        const int M_DOR_PIN = 36;     // INPUT, door retracted microswitch
        const int DOR_MIN_PIN = 23;   // OUTPUT, door closer cylinder minus (black)
        const int DOR_PLS_PIN = 22;   // OUTPUT, door closer cylinder plus (red)

        const int M_IGN_OUT_PIN = 4;  // INPUT, ignitor retracted microswitch
        const int M_IGN_IN_PIN = 3;   // INPUT, ignitor inserted microswitch
        const int IGN_MIN_PIN = 26;   // OUTPUT, ignitor cylinder minus (black)
        const int IGN_PLS_PIN = 27;   // OUTPUT, ignitor cylinder plus (red)

        const int M_ERC_OUT_PIN = 21; // INPUT, erector raised microswitch (near the pivot)
        const int M_ERC_IN_PIN = 20;  // INPUT, erector lowered microswitch (near the payload compartment/front of AGSE)
        const int ERC_MIN_PIN = 24;   // OUTPUT, erector cylinder minus (black)
        const int ERC_PLS_PIN = 25;   // OUTPUT, erector cylinder plus (red)

        // Hardcoded timer values
        const unsigned long DOR_TIME = 7000;
        const unsigned long IGN_TIME = 10000;
        const unsigned long ERC_TIME = 10000;


        // ALSO ADD ANY OTHER REQUIRED FUNCTIONS AS PRIVATE FUNCTIONS

};
#endif
