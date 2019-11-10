#ifndef ReflowWizard_Help_H
#define ReflowWizard_Help_H

#include <Arduino.h>

void showHelp(uint8_t screen);
void drawHelpBorder(uint16_t width, uint16_t height);
void eraseHelpScreen(uint16_t width, uint16_t height);
void displayHelpLine(char *text);

#endif