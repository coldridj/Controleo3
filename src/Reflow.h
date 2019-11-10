#ifndef ReflowWizard_Reflow_H
#define ReflowWizard_Reflow_H

#include <Arduino.h>

void reflow(uint8_t profileNo);
void drawStopDoneButton(boolean isGraphDisplayed, boolean buttonIsStop);
void drawReflowAbortDialog();
void updateStatusMessage(uint16_t token, uint16_t timer, uint16_t temperature, boolean abortDialogIsOnScreen);
void showReflowError(uint16_t iconsX, char *line1, char *line2);
void displayReflowDuration(uint32_t seconds, boolean isGraphDisplayed);
uint16_t getBasePIDPower(double temperature, double increment, uint16_t *bias, uint16_t maxBias);
void drawGraphOutline(uint16_t graphMaxTemp, uint16_t graphMaxSeconds);


#endif