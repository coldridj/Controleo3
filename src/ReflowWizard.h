// Written by Peter Easton
// Released under the MIT license
// Build a reflow oven: https://whizoo.com

#ifndef ReflowWizard_H
#define ReflowWizard_H

#include <Arduino.h>
#include <Controleo3.h> // Hardware-specific library

#define CONTROLEO3_VERSION             "1.5"

// Global temporary buffers (used everywhere)
extern char buffer100Bytes[100];
extern uint8_t flashBuffer256Bytes[256];     // Read/write from flash.  This is the size of a flash block

#ifdef SD_ENABLE
extern Sd2Card card;
extern SdVolume volume;
extern SdFile root;
#endif

extern Controleo3LCDILI9341 tft;
extern Controleo3Flash  flash;

// Having the top element on too much can damage the insulation, or the IR can heat up dark components
// too much.  The boost element is typically not installed in the ideal location, nor designed for
// continuous use.  Restrict the amount of power given to these elements
#define MAX_TOP_DUTY_CYCLE             80  
#define MAX_BOOST_DUTY_CYCLE           60

#endif