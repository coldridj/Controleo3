#ifndef ReflowWizard_Render_H
#define ReflowWizard_Render_H

#include <Arduino.h>

uint16_t renderBitmap(uint16_t bitmapNumber, uint16_t x, uint16_t y);
uint16_t renderBitmapFromExternalFlash(uint16_t bitmapNumber, uint16_t x, uint16_t y);
uint16_t renderBitmapFromMicrocontrollerFlash(uint16_t bitmapNumber, uint16_t x, uint16_t y);
uint16_t displayString(uint16_t x, uint16_t y, uint8_t font, char *str);
uint16_t displayCharacter(uint8_t font, uint16_t x, uint16_t y, uint8_t c);
int16_t getYOffsetForCharacter(uint8_t font, uint8_t c);
boolean isSupportedCharacter(uint8_t font, uint8_t c);
uint16_t preCharacterSpace(uint8_t font, char c);
uint16_t postCharacterSpace(uint8_t font, char c);
void displayFixedWidthString(uint16_t x, uint16_t y, char *str, uint8_t maxChars, uint8_t font);
void drawButton(uint16_t x, uint16_t y, uint16_t width, uint16_t textWidth, boolean useLargeFont, char *text);
void drawButtonOutline(uint16_t x, uint16_t y, uint16_t width);


#endif