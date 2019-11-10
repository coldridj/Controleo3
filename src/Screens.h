#ifndef ReflowWizard_Screens_H
#define ReflowWizard_Screens_H

#include <Arduino.h>

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
