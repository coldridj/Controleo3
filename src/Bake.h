#ifndef ReflowWizard_Bake_H
#define ReflowWizard_Bake_H

#include <Arduino.h>

void bake();
void DumpDataToUSB(uint16_t duration, float temperature, int duty, int integral);
void displayBakePhase(uint8_t phase, boolean abortDialogIsOnScreen);
void drawBakingAbortDialog();
void displayBakeTemperatureAndDuration(boolean displayCelsius);
void displayBakeSecondsLeft(uint32_t seconds);
uint32_t getBakeSeconds(uint16_t duration);

#endif