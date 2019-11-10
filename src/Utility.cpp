// Written by Peter Easton
// Released under the MIT license
// Build a reflow oven: https://whizoo.com

#include <Arduino.h>
#include "ReflowWizard.h"
#include "Utility.h"
#include "Outputs.h"
#include "Render.h"
#include "Prefs.h"
#include "Tones.h"

// Convert the duration into a readable string
void secondsToEnglishString(char *str, uint32_t seconds)
{
  boolean hasHours = false;
  // Take care of hours
  if (seconds >= 3600) {
    sprintf(str, "%ld hour%c", seconds/3600, seconds>=7200? 's':0);
    seconds = seconds % 3600;
    hasHours = true;
    str += strlen(str);
  }

  // Take care of minutes
  seconds = seconds / 60;
  if (seconds) {
    if (hasHours)
      strcpy(str++, " ");
    sprintf(str, "%ld minutes", seconds);
  }
}


// Display seconds in the format hhh:mm:ss
char *secondsInClockFormat(char *str, uint32_t seconds)
{
  uint16_t hours = seconds/3600;
  uint16_t minutes = (seconds % 3600) / 60;
  seconds = seconds % 60;
  if (hours)
    sprintf(str, "%d:%02d:%02ld", hours, minutes, seconds);
  else
    sprintf(str, "%d:%02ld", minutes,seconds);
  return str;
}


// Animate the heating element and fan icons, based on their current state
// This function is called 50 times per second
void animateIcons(uint16_t x)
{
  static uint16_t lastBitmap[NUMBER_OF_OUTPUTS];
  static uint8_t animateFan = 0;
  static uint8_t animationPhase = 0;
  static uint32_t lastUpdate = 0;
  uint16_t bitmap;

  // If it has been a while since the last update then redraw all the icons
  // This can happen if the user finishes a bake, then starts another one.
  uint32_t now = millis();
  if (now - lastUpdate > 100) {
    for (int8_t i=0; i < NUMBER_OF_OUTPUTS; i++)
      lastBitmap[i] = 0;
    lastUpdate = now;
  }
  
  for (int8_t i=0; i < NUMBER_OF_OUTPUTS; i++) {
    // Don't display anything if the output is unused
    if (prefs.outputType[i] == TYPE_UNUSED)
      continue;
    // Handle heating elements first
    if (isHeatingElement(prefs.outputType[i])) {
      // Is the element on?
      if (getOutput(i) == HIGH)
        bitmap = BITMAP_ELEMENT_ON;
      else
        bitmap = BITMAP_ELEMENT_OFF;
    }
    else {
      // Output must be a fan
      if (prefs.outputType[i] == TYPE_CONVECTION_FAN)
        bitmap = BITMAP_CONVECTION_FAN1;
      else
        bitmap = BITMAP_COOLING_FAN1;

      // Should the fan be animated?
      if (getOutput(i) == HIGH)
        bitmap += animationPhase;
    }

    // Draw the bitmap
    if (bitmap != lastBitmap[i]) {
      renderBitmap(bitmap, x, 4);
      lastBitmap[i] = bitmap;
    }
    x +=40;
  }
  // Go to the next animation image the next time this function is called
  if (++animateFan == 3) {
    animationPhase = (animationPhase + 1) % 3;
    animateFan = 0;
  }
}


// Take a screenshot.  This is a "blocking" call - so nothing else gets processed
// while this is happening
void takeScreenshot() 
{
  char buf[320 * 3];
  // Initialize the SD card
  if (!SD.begin()) {
    SerialUSB.println("Card failed, or not present");
    return;
  }
  
  // Open the file for writing
  sprintf(buf, "C3_%05d.bmp", prefs.screenshotNumber);
  File dataFile = SD.open(buf, FILE_WRITE);
  if (!dataFile) {
    SerialUSB.println("Can't open " + String(buf));
    return;
  }
  SerialUSB.println("Writing screenshot to " + String(buf));
  
  // Write the bitmap header
  memcpy_P(buf, bmpHeader, 54);
  dataFile.seek(0);
  dataFile.write(buf, 54);

  // Start the screen read
  tft.startReadBitmap(0, 0, 480, 320);

  // Keeping reading from the screen and writing to the SD card
  for (uint16_t i=0; i< 480; i++) {
    tft.readBitmap24bit((uint8_t *) buf, 320);
    dataFile.write(buf, 320 * 3);
    // Beep every 1/12 of the operation to let the user know something is happening
    if (i % 40 == 0)
      playTones(TUNE_SCREENSHOT_BUSY);
  }
  tft.endReadBitmap();
  dataFile.close();

  // Increase the file number for the next screenshot
  prefs.screenshotNumber = (prefs.screenshotNumber + 1) % 10000;
  savePrefs();
  playTones(TUNE_SCREENSHOT_DONE);
  SerialUSB.println("Screenshot written!");
}


extern "C" char *sbrk(int i);
uint32_t getFreeRAM() {
  char stack_dummy = 0;
  return &stack_dummy - sbrk(0);
}


// Free memory is checked every time there is a screen transition
// There should be NO leaks!  The microcontroller has 32K of RAM, and
// around 4K is used by glabal variables and strings.  The software
// (being written in "C" for the most part) is extremely frugal with
// memory and even during relow there should still be more than 20K
// remaining.  Take that Windows 10!
void checkFreeMemory()
{
  static uint32_t freeRAMOnBoot = getFreeRAM();

  // Check the amount of free RAM (Allow 500 bytes because some drivers like SD card consume RAM when they load)
  if (getFreeRAM() > freeRAMOnBoot - 500)
    return;

  // There has been a memory leak!
  sprintf(buffer100Bytes, "Free RAM was %ld and is now %ld", freeRAMOnBoot, getFreeRAM());
  SerialUSB.println(buffer100Bytes);
//  drawThickRectangle(80, 70, 320, 135, 10, RED);
//  displayString(114, 100, FONT_12PT_BLACK_ON_WHITE, (char *) "Memory Leak!!!");
//  sprintf(buffer100Bytes, "%d bytes lost", freeRAMOnBoot - getFreeRAM());
//  displayString(164, 150, FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
  freeRAMOnBoot = getFreeRAM();
//  delay(1000);
//  tft.fillScreen(WHITE);
}
