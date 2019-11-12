#ifndef ReflowWizard_Reflow_H
#define ReflowWizard_Reflow_H

#include <Arduino.h>

// Reflow defines
#define REFLOW_PHASE_NEXT_COMMAND      0  // Get the next command (token) in the profile
#define REFLOW_WAITING_FOR_TIME        1  // Waiting for a set time to pass
#define REFLOW_WAITING_UNTIL_ABOVE     2  // Waiting until the oven temperature rises above a certain temperature
#define REFLOW_WAITING_UNTIL_BELOW     3  // Waiting until the oven temperature drops below a certain temperature
#define REFLOW_WAITING_FOR_TAP         4  // Waiting for the user to tap the screen
#define REFLOW_MAINTAIN_TEMP           5  // Hold a specific temperature for a certain duration 
#define REFLOW_PID                     6  // Use PID to get to the specified temperature
#define REFLOW_ALL_DONE                7  // All done, waiting for user to tap screen
#define REFLOW_ABORT                   8  // Abort (or done)

void reflow(uint8_t profileNo);
void drawStopDoneButton(boolean isGraphDisplayed, boolean buttonIsStop);
void drawReflowAbortDialog();
void updateStatusMessage(uint16_t token, uint16_t timer, uint16_t temperature, boolean abortDialogIsOnScreen);
void showReflowError(uint16_t iconsX, char *line1, char *line2);
void displayReflowDuration(uint32_t seconds, boolean isGraphDisplayed);
uint16_t getBasePIDPower(double temperature, double increment, uint16_t *bias, uint16_t maxBias);
void drawGraphOutline(uint16_t graphMaxTemp, uint16_t graphMaxSeconds);

#endif