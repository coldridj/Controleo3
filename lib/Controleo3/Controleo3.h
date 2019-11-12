// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//
// This library controls the functions of ControLeo3
//  - LCD display and backlight
//  - Touchscreen
//  - Buzzer
//  - Thermocouple

#ifndef CONTROLEO3_H_
#define CONTROLEO3_H_

#include "Arduino.h"

// Include all the code for the display, touch, thermocouple, flash and the SD card
#include "Controleo3MAX31856.h"
#include "Controleo3LCDILI9341.h"
//#include "Controleo3Touch.h"
#include "Controleo3Flash.h"
#include "Controleo3SD.h"

#define SD_DETECT_PIN   

#endif // CONTROLEO3_H_
