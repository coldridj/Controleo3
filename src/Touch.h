#ifdef TOUCH_ENABLE

#ifndef ReflowWizard_Touch_H
#define ReflowWizard_Touch_H

#include <Arduino.h>

#ifdef TOUCH_ENABLE
extern Controleo3Touch  touch;
#endif

// Should the temperature be displayed while waiting for a tap?
#define DONT_SHOW_TEMPERATURE          0
#define SHOW_TEMPERATURE_IN_HEADER     1
#define CHECK_FOR_TAP_THEN_EXIT        2

void CalibrateTouchscreen();
void drawCrosshairs(uint16_t x, uint16_t y, boolean draw);
void debounce();
void quickDebounce();
void calibrationDebounce();
void sendTouchCalibrationData();
void clearTouchTargets();
void setTouchIntervalCallback(void (*f) (), uint16_t interval);
void setTouchTemperatureUnitChangeCallback(void (*f) (boolean displayInCelsius));
void defineTouchArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
int8_t getTap(uint8_t mode);
void touchCallback();
void displayTemperatureInHeader();


#endif
#else 

#include "NonTouch.h"

#endif