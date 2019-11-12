#ifndef ReflowWizard_Outputs_H
#define  ReflowWizard_Outputs_H

#include <Arduino.h>

#if TEENSY31 | TEENSY36
#define PORT_A_OUTPUT 5
#define PORT_B_OUTPUT 6
extern volatile uint8_t *portAOut, *portAMode, *portBOut, *portBMode;
#else
#if ESP32
#define PORT_A_OUTPUT 5
#define PORT_B_OUTPUT 6
#else
#define PORT_A_OUTPUT 5
#define PORT_B_OUTPUT A2
extern volatile uint32_t *portAOut, *portAMode, *portBOut, *portBMode;
#endif
#endif

// Outputs
#define NUMBER_OF_OUTPUTS 6

// Types of outputs
#define TYPE_UNUSED 0
#define TYPE_WHOLE_OVEN 0 // Used to store whole oven values (learned mode)
#define TYPE_BOTTOM_ELEMENT 1
#define TYPE_TOP_ELEMENT 2
#define TYPE_BOOST_ELEMENT 3
#define TYPE_CONVECTION_FAN 4
#define TYPE_COOLING_FAN 5
#define NO_OF_TYPES 6
#define isHeatingElement(x) (x == TYPE_TOP_ELEMENT || x == TYPE_BOTTOM_ELEMENT || x == TYPE_BOOST_ELEMENT)

// To be used with setOvenOutputs()
#define ELEMENTS_OFF 0
#define LEAVE_ELEMENTS_AS_IS 1
#define CONVECTION_FAN_OFF 0
#define CONVECTION_FAN_ON 1
#define COOLING_FAN_OFF 0
#define COOLING_FAN_ON 1

extern const char *outputDescription[];
extern const char *longOutputDescription[];

void initOutputs();
void setOutput(uint8_t outputNumber, boolean state);
boolean getOutput(uint8_t outputNumber);
void setOvenOutputs(boolean elementsOn, boolean convectionFanOn, boolean coolingFanOn);
void turnConvectionFanOn(boolean on);
void turnCoolingFanOn(boolean on);
boolean areOutputsConfigured();
uint8_t numOutputsConfigured();

#endif