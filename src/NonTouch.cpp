#include <Arduino.h>
#include "ReflowWizard.h"
#include "Render.h"
#include "Temperature.h"
#include "NonTouch.h"
#include "Bitmaps.h"

// Display the temperature on the screen once per second
void displayTemperatureInHeader()
{
#ifdef THERMO_ENABLE
  float temperature = getCurrentTemperature();
  char *str = getTemperatureString(buffer100Bytes, temperature, true);

  // Display the temperature
  if (IS_MAX31856_ERROR(temperature)) {
    tft.fillRect(366, 11, 110, 19, WHITE);
    displayString(418, 11, FONT_9PT_BLACK_ON_WHITE_FIXED, str);
  }
  else {
    displayFixedWidthString(351, 11, str, 9, FONT_9PT_BLACK_ON_WHITE_FIXED);
  }
#endif
}