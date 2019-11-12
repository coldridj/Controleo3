#ifdef THERMO_ENABLE

#ifndef ReflowWizard_Temperature_H
#define ReflowWizard_Temperature_H

#include <Arduino.h>

extern Controleo3MAX31856 thermocouple;

void initTemperature();
void takeCurrentThermocoupleReading();
float getCurrentTemperature();
char *getTemperatureString(char *str, float temperature, boolean displayInCelsius);

#endif
#endif