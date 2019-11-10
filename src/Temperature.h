#ifndef ReflowWizard_Temperature_H
#define ReflowWizard_Temperature_H

#include <Arduino.h>

void initTemperature();
void takeCurrentThermocoupleReading();
float getCurrentTemperature();
char *getTemperatureString(char *str, float temperature, boolean displayInCelsius);

#endif