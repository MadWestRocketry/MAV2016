#ifndef INPUT_H
#define INPUT_H

#include <Arduino.h> //not sure if we need this; we might for the built-in Arduino functions

class Input {

    public:
        Input(); // Constructor

        // dummy operations
        int Home();

        int Start();

        int Pause();


    private:
        // ADD ALL REQUIRED PINS AS CONST INT
        const int BUT_BLU_PIN = 0;       // OUTPUT, red stack light
        const int BUT_GRN_PIN = 18;       // OUTPUT, yellow stack light
        const int BUT_YEL_PIN = 19;       // OUTPUT, green stack light
      
};
#endif
