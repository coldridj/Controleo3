#ifndef ReflowWizard_Bake_H
#define ReflowWizard_Bake_H

#include <Arduino.h>

// Baking Defines
#define BAKE_DOOR_OPEN                 0
#define BAKE_DOOR_OPEN_CLOSE_COOL      1
#define BAKE_DOOR_LEAVE_CLOSED         2
#define BAKE_DOOR_LAST_OPTION          2

#define BAKE_MAX_DURATION              127  // 127 = 168 hours (see getBakeSeconds)
#define BAKE_TEMPERATURE_STEP          5    // Step between temperature settings
#define BAKE_MIN_TEMPERATURE           40   // Minimum temperature for baking
#define BAKE_MAX_TEMPERATURE           200  // Maximum temperature for baking

#define BAKING_PHASE_HEATUP            0    // Heat up the oven rapidly to just under the desired temperature
#define BAKING_PHASE_BAKE              1    // The main baking phase. Just keep the oven temperature constant
#define BAKING_PHASE_START_COOLING     2    // Start the cooling process
#define BAKING_PHASE_COOLING           3    // Wait till the oven has cooled down to 50°C
#define BAKING_PHASE_DONE              4    // Baking is done.  Stay on this screen
#define BAKING_PHASE_ABORT             5    // Baking was aborted

extern const char *bakeDoorDescription[];
extern const char *bakeUseCoolingFan[];
extern const char *bakePhaseStr[];
extern const uint8_t bakePhaseStrPosition[];

void bake();
void DumpDataToUSB(uint16_t duration, float temperature, int duty, int integral);
void displayBakePhase(uint8_t phase, boolean abortDialogIsOnScreen);
void drawBakingAbortDialog();
void displayBakeTemperatureAndDuration(boolean displayCelsius);
void displayBakeSecondsLeft(uint32_t seconds);
uint32_t getBakeSeconds(uint16_t duration);

#endif