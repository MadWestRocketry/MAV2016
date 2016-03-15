// Class to handle AGSE inputs: blue, green, yellow buttons
//
// Note that blue and green are standard inputs
// Yellow will be tied to a interrupt routine
// 

// MODIFICATION

#include "Input.h"

// Constructor: Set all relevant pins to "input"
Input::Input(){
    pinMode(BUT_BLU_PIN, INPUT);
    pinMode(BUT_GRN_PIN, INPUT);
    pinMode(BUT_YEL_PIN, INPUT);

}

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

// When you write a "member" function of a class you must preface it with "Classname::memberFunction()" as
// you see here
int Input::Home() {
    return digitalRead(BUT_BLU_PIN);
}

int Input::Start() {
    return digitalRead(BUT_GRN_PIN);
}

int Input::Pause() {
// 
    return digitalRead(BUT_YEL_PIN);
// *** >>> This code block should ultimately contain the interrupt routine for PAUSE functionality
//
}

