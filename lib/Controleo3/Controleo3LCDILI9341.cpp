// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//
// LCD controller for ILI9488 controller


// For general software development, LCD_DEBUG should be enabled so that
// range checking happens on all parameters ensuring that all drawing takes
// place on the screen.  For release, LCD_DEBUG can be disabled to improve
// drawing speed.
//#define LCD_DEBUG

#include "Controleo3LCDILI9341.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

#define LCD_WIDTH  		240
#define LCD_HEIGHT 		320
#define LCD_MAX_X		239
#define LCD_MAX_Y		319

// For the Adafruit shield, these are the default.
#define tft9341_DC 9
#define tft9341_CS 10
#define tft9341_MOSI 11
#define tft9341_CLK 13
#define tft9341_RST 8
#define tft9341_MISO 12

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
// Adafruit_ILI9341 tft9341 = Adafruit_ILI9341(tft9341_CS, tft9341_DC);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft9341 = Adafruit_ILI9341(tft9341_CS, tft9341_DC, tft9341_MOSI, tft9341_CLK, tft9341_RST, tft9341_MISO);

// Constructor for the tft9341 display
Controleo3LCDILI9341::Controleo3LCDILI9341(void)
{

}


void Controleo3LCDILI9341::begin()
{
  	tft9341.begin();

	// Wait for things to settle before returning
    delay(15);
}

// Draw a horizontal line
void Controleo3LCDILI9341::drawFastHLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
	tft9341.drawFastHLine(x, y, length, color);
}


// Draw a vertical line
void Controleo3LCDILI9341::drawFastVLine(int16_t x, int16_t y, int16_t length, uint16_t color)
{
	tft9341.drawFastVLine(x, y, length, color);
}


// Draw a rectangle with the given color
void Controleo3LCDILI9341::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	tft9341.drawRect(x, y, w, h, color);
}


// Fill the rectangle with the given color
void Controleo3LCDILI9341::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t fillcolor)
{
	tft9341.fillRect(x, y, w, h, fillcolor);
}


// Fill the screen with the given color
void Controleo3LCDILI9341::fillScreen(uint16_t color) {
	tft9341.fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}


// Draw a pixel at the specified location
void Controleo3LCDILI9341::drawPixel(int16_t x, int16_t y, uint16_t color) {
	tft9341.drawPixel(x, y, color);
}

// Draw part or all of a bitmap.  The bitmaps must be 16-bit color.
// This function can be called over and over again until the whole
// bitmap has been rendered to the screen.
void Controleo3LCDILI9341::drawBitmap(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h)
{
	tft9341.drawRGBBitmap(x, y, pcolors, w, h);
}

// Start drawing a bitmap
void Controleo3LCDILI9341::startBitmap(int16_t x, int16_t y, int16_t w, int16_t h)
{
    tft9341.setAddrWindow(x, y, x + w - 1, y + h - 1);
}


// Draw part or all of a bitmap.  The bitmaps must be 16-bit color.
// This function can be called over and over again until the whole
// bitmap has been rendered to the screen.
void Controleo3LCDILI9341::drawBitmap(uint16_t *data, uint32_t len)
{
#define write8DataBitmap(d)  *bitmapReg = (bitmapRegValue + d); LCD_WR_ACTIVE;
	while(len--) {
		tft9341.writePixels(data, len);
        data++;
    }
}


// End the drawing of the bitmap
void Controleo3LCDILI9341::endBitmap()
{
    // do nothing
}