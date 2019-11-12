#ifndef ReflowWizard_Screens_H
#define ReflowWizard_Screens_H

#include <Arduino.h>

// Screens
#define SCREEN_HOME                    0
#define SCREEN_SETTINGS                1
#define SCREEN_TEST                    2
#define SCREEN_SETUP_OUTPUTS           3
#define SCREEN_SERVO_OPEN              4
#define SCREEN_SERVO_CLOSE             5
#define SCREEN_LINE_FREQUENCY          6
#define SCREEN_RESET                   7
#define SCREEN_ABOUT                   8
#define SCREEN_PID_TUNING              9
#define SCREEN_BAKE                    10
#define SCREEN_EDIT_BAKE1              11
#define SCREEN_EDIT_BAKE2              12
#define SCREEN_REFLOW                  13
#define SCREEN_CHOOSE_PROFILE          14
#define SCREEN_LEARNING                15
#define SCREEN_RESULTS                 16

// Height of the button in pixels
#define BUTTON_HEIGHT                  61

#define BUTTON_SMALL_FONT              0
#define BUTTON_LARGE_FONT              1

#define LINE(x)           ((x *30) + 50)

// When displaying edit arrow on the screen
#define ONE_SETTING                    0
#define ONE_SETTING_WITH_TEXT          1
#define ONE_SETTING_TEXT_BUTTON        2
#define TWO_SETTINGS                   3

#define NUM_POWER_SETTINGS						 6
#define NUM_INERTIA_SETTINGS					 9
#define NUM_INSULATION_SETTINGS				4


void setTouchTemperatureUnitChangeCallback(void (*f) (boolean));
void showScreen(uint8_t screen);
void drawTouchButton(uint16_t x, uint16_t y, uint16_t width, uint16_t textWidth, boolean useLargeFont, char *text);
void drawNavigationButtons(boolean addRightArrow, boolean largeTargets);
void eraseHeader();
void displayHeader(char *text, boolean isSetting);
void drawThickRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t thickness, uint16_t color);
void drawIncreaseDecreaseTapTargets(uint8_t targetType);
void drawTestOutputIcon(uint8_t type, boolean outputIsOn);
void testOutputIconAnimator();
uint8_t mapValue(uint16_t value, uint16_t map[]);

#endif
