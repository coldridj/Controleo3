#ifndef ReflowWizard_Bitmaps_H
#define ReflowWizard_Bitmaps_H

// Written by Peter Easton
// Released under the MIT license
// Build a reflow oven: https://whizoo.com
//

#include <Arduino.h>

// Fonts
#define FONT_9PT_BLACK_ON_WHITE		0
#define FONT_12PT_BLACK_ON_WHITE	   1
#define FONT_9PT_BLACK_ON_WHITE_FIXED  2
#define FONT_12PT_BLACK_ON_WHITE_FIXED 3
#define FONT_22PT_BLACK_ON_WHITE_FIXED 4

#define IS_FONT_FIXED_WIDTH(x)		 (x == FONT_9PT_BLACK_ON_WHITE_FIXED || x == FONT_12PT_BLACK_ON_WHITE_FIXED || x == FONT_22PT_BLACK_ON_WHITE_FIXED)

#define FONT_FIRST_9PT_BW			  0
#define FONT_FIRST_12PT_BW			 (FONT_FIRST_9PT_BW + 94)
#define FONT_FIRST_9PT_BW_FIXED		(FONT_FIRST_12PT_BW + 94)
#define FONT_FIRST_12PT_BW_FIXED	   (FONT_FIRST_9PT_BW_FIXED + 11)
#define FONT_FIRST_22PT_BW_FIXED	   (FONT_FIRST_12PT_BW_FIXED + 12)

#define FONT_IMAGES					(FONT_FIRST_22PT_BW_FIXED + 16)
#define BITMAP_LEFT_ARROW			  (FONT_IMAGES)
#define BITMAP_RIGHT_ARROW			 (FONT_IMAGES + 1)
#define BITMAP_HOME					(FONT_IMAGES + 2)
#define BITMAP_HELP					(FONT_IMAGES + 3)
#define BITMAP_LEFT_BUTTON_BORDER	  (FONT_IMAGES + 4)
#define BITMAP_RIGHT_BUTTON_BORDER	 (FONT_IMAGES + 5)
#define BITMAP_SETTINGS				(FONT_IMAGES + 6)
#define BITMAP_ELEMENT_OFF			 (FONT_IMAGES + 7)
#define BITMAP_ELEMENT_ON			  (FONT_IMAGES + 8)
#define BITMAP_CONVECTION_FAN1		 (FONT_IMAGES + 9)
#define BITMAP_CONVECTION_FAN2		 (FONT_IMAGES + 10)
#define BITMAP_CONVECTION_FAN3		 (FONT_IMAGES + 11)
#define BITMAP_COOLING_FAN1			(FONT_IMAGES + 12)
#define BITMAP_COOLING_FAN2			(FONT_IMAGES + 13)
#define BITMAP_COOLING_FAN3			(FONT_IMAGES + 14)
#define BITMAP_CONTROLEO3			  (FONT_IMAGES + 15)
#define BITMAP_WHIZOO				  (FONT_IMAGES + 16)
#define BITMAP_CONTROLEO3_SMALL		(FONT_IMAGES + 17)
#define BITMAP_WHIZOO_SMALL			(FONT_IMAGES + 18)
#define BITMAP_TRASH				   (FONT_IMAGES + 19)
#define BITMAP_EXIT					(FONT_IMAGES + 20)
#define BITMAP_UP_SMALL_ARROW		  (FONT_IMAGES + 21)
#define BITMAP_DOWN_SMALL_ARROW		(FONT_IMAGES + 22)
#define BITMAP_UP_ARROW				(FONT_IMAGES + 23)
#define BITMAP_DOWN_ARROW			  (FONT_IMAGES + 24)
#define BITMAP_HELP_ICON			   (FONT_IMAGES + 25)
#define BITMAP_DECREASE_ARROW		  (FONT_IMAGES + 26)
#define BITMAP_INCREASE_ARROW		  (FONT_IMAGES + 27)
#define BITMAP_SMILEY_GOOD			 (FONT_IMAGES + 28)
#define BITMAP_SMILEY_NEUTRAL		  (FONT_IMAGES + 29)
#define BITMAP_SMILEY_BAD			  (FONT_IMAGES + 30)
#define BITMAP_LAST_ONE				BITMAP_SMILEY_BAD

struct Bitmap {
    const uint16_t width, height;
    const uint16_t* data;

    Bitmap(const uint16_t *bitmap) :
        width ((*bitmap) >> 8),
        height (*bitmap & 0xff),
        data (++bitmap)
    {

    }
};

extern Bitmap flashBitmaps[];

#endif