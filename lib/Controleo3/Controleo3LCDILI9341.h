// Written by Peter Easton
// Released under CC BY-NC-SA 3.0 license
// Build a reflow oven: http://whizoo.com
//
// LCD controller for ILI9488 controller

#ifndef CONTROLEO3LCDILI9341_H_
#define CONTROLEO3LCDILI9341_H_

#include "Arduino.h"
#include "bits.h"
#include <Adafruit_ILI9341.h>

#define LCD_WIDTH  		240
#define LCD_HEIGHT 		320
#define LCD_MAX_X		239
#define LCD_MAX_Y		319

// Some common colors
#define BLACK                   0x0000      //   0,   0,   0
#define NAVY                    0x000F      //   0,   0, 128
#define DARK_GREEN              0x03E0      //   0, 128,   0
#define DARK_CYAN               0x03EF      //   0, 128, 128
#define MAROON                  0x7800      // 128,   0,   0
#define PURPLE                  0x780F      // 128,   0, 128
#define OLIVE                   0x7BE0      // 128, 128,   0
#define LIGHT_GREY              0xC618      // 192, 192, 192
#define DARK_GREY               0x7BEF      // 128, 128, 128
#define BLUE                    0x001F      //   0,   0, 255
#define GREEN                   0x07E0      //   0, 255,   0
#define CYAN                    0x07FF      //   0, 255, 255
#define RED                     0xF800      // 255,   0,   0
#define MAGENTA                 0xF81F      // 255,   0, 255
#define YELLOW                  0xFFE0      // 255, 255,   0
#define WHITE                   0xFFFF      // 255, 255, 255
#define ORANGE                  0xFD20      // 255, 165,   0
#define GREEN_YELLOW            0xAFE5      // 173, 255,  47
#define PINK                    0xF81F      // 255, 192, 203

extern Adafruit_ILI9341 tft9341;

class Controleo3LCDILI9341
{
	public:
		Controleo3LCDILI9341(void);

		void begin();
		// void reset(uint8_t type);
		void drawPixel(int16_t x, int16_t y, uint16_t color);
		void drawFastHLine(int16_t x0, int16_t y0, int16_t w, uint16_t color);
		void drawFastVLine(int16_t x0, int16_t y0, int16_t h, uint16_t color);
		void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
		void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c);
		void fillScreen(uint16_t color);

		void drawBitmapFull(int16_t x, int16_t y, uint16_t *pcolors, int16_t w, int16_t h);

		void startBitmap(int16_t x, int16_t y, int16_t w, int16_t h);
  		void endBitmap();
  		void drawBitmap(uint16_t *data, uint32_t len);

		uint8_t readcommand8(uint8_t reg, uint8_t index=0);
		void setRotation(uint8_t r);
};


#endif // CONTROLEO3LCD_H_
