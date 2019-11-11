// Written by Peter Easton
// Released under the MIT license
// Build a reflow oven: https://whizoo.com

#include <Arduino.h>
#include "ReflowWizard.h"
#include "Outputs.h"
// #include "Prefs.h"
#include "Render.h"
// #include "Tones.h"
// #include "Temperature.h"
// #include "Servo.h"
// #include "Touch.h"
#include "Screens.h"

// #define SD_ENABLE // process the SD card
// #define OUTPUT_ENABLE // process outputs

void setup() {
	//Serial.begin(115200);
	delay(1000);
#ifdef OUTPUT_ENABLE
	// First priority - turn off the relays!
	initOutputs();
#endif

#ifdef SD_ENABLE
	// See if there is a SD card present
	pinMode(SD_DETECT_PIN, INPUT_PULLUP);
	if (digitalRead(SD_DETECT_PIN) == LOW) {
		// There is a SD card
		SD.begin();

		// Should the controller wait for new firmware to be loaded?	This is only
		// necessary if the existing firmware is really f%#*ed up
		if (SD.exists((char *) "iamafool.txt")) {
		// Wait here forever, until new firmware is loaded
		while (true)
			delay(1000);
		}

		// Should the controller be reset to factory settings?
		if (SD.exists((char *) "factory3.txt")) {
		flash.begin();
		getPrefs();
		factoryReset(false);
		}
	}
#endif



	// Get the splash screen up as quickly as possible
	tft.begin();
	tft.setRotation(2);
//	 flash.begin();

	
	// read diagnostics (optional but can help debug problems)
	uint8_t x = tft.readcommand8(ILI9341_RDMODE);
	Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDMADCTL);
	Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDPIXFMT);
	Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDIMGFMT);
	Serial.print("Image Format: 0x"); Serial.println(x, HEX);
	x = tft.readcommand8(ILI9341_RDSELFDIAG);
	Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
	

	// Display the initial splash screen
	// tft.pokeRegister(ILI9488_DISPLAYOFF);
	tft.fillScreen(ILI9341_RED);
	tft.fillScreen(WHITE);
	renderBitmap(BITMAP_COOLING_FAN3, 40, 0);
	//renderBitmap(BITMAP_WHIZOO, 84, 200);
	displayString(0, 92, FONT_9PT_BLACK_ON_WHITE, (char *) "Smart Oven Controller");
	displayString(210, 290, FONT_9PT_BLACK_ON_WHITE, (char *) CONTROLEO3_VERSION);
#ifdef NDEF
	// tft.pokeRegister(ILI9488_DISPLAYON);
	playTones(TUNE_STARTUP);
	

	// Get the prefs from external flash
	getPrefs();

	// Initialize the MAX31856's registers
	thermocouple.begin();
	initTemperature();

	// Initialize the timer used to control the servo and read the temperature
	initializeTimer();

	// Start the touchscreen
	touch.begin();
	
	// Move the servo to the closed position
	setServoPosition(prefs.servoClosedDegrees, 1000); 

	// Is there touchscreen calibration data?
	if (prefs.topLeftX != 0) {
	sendTouchCalibrationData();
	// Show the logos for a few seconds
	delay(3000);
	}
	else {
	// Touchscreen needs to be calibrated first
	CalibrateTouchscreen();
	}

	// Go to the first screen (this should never exit)


#endif
	if (areOutputsConfigured())
		showScreen(SCREEN_HOME);
	else	
		showScreen(SCREEN_SETTINGS);
	Serial.println("Test");
}


void loop()
{
	// This should never be called (showScreen never exits)
	//showScreen(SCREEN_HOME);
}



