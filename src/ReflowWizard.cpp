#include <Arduino.h>
#include "ReflowWizard.h"

char buffer100Bytes[100];
uint8_t flashBuffer256Bytes[256];     // Read/write from flash.  This is the size of a flash block

Sd2Card card;
SdVolume volume;
SdFile root;

Controleo3LCD tft;
Controleo3Touch  touch;
Controleo3Flash  flash;
Controleo3MAX31856 thermocouple;

const char *outputDescription[NO_OF_TYPES] = {"Unused", "Bottom Element", "Top Element", "Boost Element", "Convection Fan","Cooling Fan"};
const char *longOutputDescription[NO_OF_TYPES] = {
  "",
  "Controls the bottom heating element.",
  "Controls the top heating element.",
  "Controls the boost heating element.",
  "On at start, off once cooling is done.",
  "Turns on to cool the oven."
};

const char *bakeDoorDescription[BAKE_DOOR_LAST_OPTION+1] = {
  "Open oven door after bake.",
  "Open after bake, close when cool.",
  "Leave oven door closed."
};

const char *bakeUseCoolingFan[2] = {
  "Use cooling fan once baking is done.",
  "Don't use the cooling fan."
};

const char *bakePhaseStr[] = {"Pre-heat", "Baking", "Cooling", "Baking has finished", "Baking has finished", ""};
const uint8_t bakePhaseStrPosition[] = {190, 202, 198, 128, 128, 0};



Controleo3Prefs prefs;

