#include <Arduino.h>
#include "ReflowWizard.h"

char buffer100Bytes[100];
uint8_t flashBuffer256Bytes[256];		 // Read/write from flash.	This is the size of a flash block

Controleo3LCDILI9341 tft = Controleo3LCDILI9341();
#ifdef TOUCH_ENABLE
Controleo3Touch	touch;
#endif

Controleo3Flash	flash;


