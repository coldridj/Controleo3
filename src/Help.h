#ifndef ReflowWizard_Help_H
#define ReflowWizard_Help_H

#include <Arduino.h>

// Help
#define HELP_OUTPUTS_NOT_CONFIGURED    80
#define HELP_LEARNING_NOT_DONE         81
#define HELP_NO_SD_CARD                82
#define HELP_BAD_FORMAT                83
#define HELP_CANT_WRITE_TO_SD_CARD     84

void showHelp(uint8_t screen);
void drawHelpBorder(uint16_t width, uint16_t height);
void eraseHelpScreen(uint16_t width, uint16_t height);
void displayHelpLine(char *text);

#endif