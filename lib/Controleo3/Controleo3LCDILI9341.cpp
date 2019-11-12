// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//
// LCD controller for ILI9488 controller


// For general software development, LCD_DEBUG should be enabled so that
// range checking happens on all parameters ensuring that all drawing takes
// place on the screen.	For release, LCD_DEBUG can be disabled to improve
// drawing speed.
//#define LCD_DEBUG

#include "Controleo3LCDILI9341.h"
#include <Arduino.h>

#if TEENSY31 | TEENSY36
#include <ILI9341_t3.h>
#else 
#include <Adafruit_ILI9341.h>
#endif

#if TEENSY31 | TEENSY36
#define TFT_DC 10
#define TFT_CS 22
#define TFT_RST 23
ILI9341_t3 tft9341 = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);
#else
#if ESP32
#define TFT_DC 2
#define TFT_CS 5
#define TFT_RST 4
#else 
#define TFT_DC 10
#define TFT_CS 22
#define TFT_RST 23
#endif
Adafruit_ILI9341 tft9341 = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
#endif

#if ESP32 
#define SPI_FREQ 40000000
#else 
#define SPI_FREQ 8000000
#endif


// Constructor for the tft9341 display
Controleo3LCDILI9341::Controleo3LCDILI9341(void)
{
}

void Controleo3LCDILI9341::begin()
{
	tft9341.begin(SPI_FREQ);
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
void Controleo3LCDILI9341::fillScreen(uint16_t color) 
{
	tft9341.fillScreen(color);
}

// Draw a pixel at the specified location
void Controleo3LCDILI9341::drawPixel(int16_t x, int16_t y, uint16_t color) 
{
	tft9341.drawPixel(x, y, color);
}

// Draw part or all of a bitmap.	The bitmaps must be 16-bit color.
// This function can be called over and over again until the whole
// bitmap has been rendered to the screen.
void Controleo3LCDILI9341::drawBitmapFull(int16_t x, int16_t y, const uint16_t *pcolors, int16_t w, int16_t h)
{
	tft9341.drawRGBBitmap(x, y, pcolors, w, h);
}

// Start drawing a bitmap
void Controleo3LCDILI9341::startBitmap(int16_t x, int16_t y, int16_t w, int16_t h)
{
	tft9341.setAddrWindow(x, y, x + w - 1, y + h - 1);
}


// Draw part or all of a bitmap.	The bitmaps must be 16-bit color.
// This function can be called over and over again until the whole
// bitmap has been rendered to the screen.
void Controleo3LCDILI9341::drawBitmap(uint16_t *data, uint32_t len)
{
// #define write8DataBitmap(d)	*bitmapReg = (bitmapRegValue + d); LCD_WR_ACTIVE;
// 	while(len--) {
// 		tft9341.writePixels(data, len);
//				 data++;
//		 }
}


// End the drawing of the bitmap
void Controleo3LCDILI9341::endBitmap()
{
	// do nothing
}

uint8_t Controleo3LCDILI9341::readcommand8(uint8_t reg, uint8_t index)
{
	return tft9341.readcommand8(reg, index);
}

void Controleo3LCDILI9341::setRotation(uint8_t r) {
	tft9341.setRotation(r);
}