#ifndef ReflowWizard_Touch_H
#define ReflowWizard_Touch_H

#include <Arduino.h>

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