#ifndef ReflowWizard_Outputs_H
#define  ReflowWizard_Outputs_H

#include <Arduino.h>

#if TEENSY31
extern volatile uint8_t *portAOut, *portAMode, *portBOut, *portBMode;
#else
extern volatile uint32_t *portAOut, *portAMode, *portBOut, *portBMode;
#endif

void initOutputs();
void setOutput(uint8_t outputNumber, boolean state);
boolean getOutput(uint8_t outputNumber);
void setOvenOutputs(boolean elementsOn, boolean convectionFanOn, boolean coolingFanOn);
void turnConvectionFanOn(boolean on);
void turnCoolingFanOn(boolean on);
boolean areOutputsConfigured();
uint8_t numOutputsConfigured();

#endif