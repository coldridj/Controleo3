#ifndef ReflowWizard_Prefs_H
#define ReflowWizard_Prefs_H

#include <Arduino.h>
#include "Outputs.h"
#include "Profiles.h"

// Preferences (this can be 4Kb maximum).  Ideally keep it at 1228 bytes
struct Controleo3Prefs {
  uint32_t  sequenceNumber;                   // Prefs are rotated between 4 blocks in flash, each 4K in size
  uint16_t  versionNumber;                    // Version number of these prefs
  uint16_t  screenshotNumber;                 // Next file number of screenshot
  uint16_t  topLeftX;                         // Touchscreen calibration points
  uint16_t  topRightX;
  uint16_t  bottomLeftX;
  uint16_t  bottomRightX;
  uint16_t  topLeftY;
  uint16_t  bottomLeftY;
  uint16_t  topRightY;
  uint16_t  bottomRightY;
  uint8_t   outputType[NUMBER_OF_OUTPUTS];    // The type of output for each of the 6 outputs 
  uint8_t   lineVoltageFrequency;             // Used for MAX31856 filtering
  uint8_t   servoOpenDegrees;                 // Servo door open 
  uint8_t   servoClosedDegrees;               // Servo door closed
  uint8_t   learningComplete;                 // Have the learning runs been completed?
  uint8_t   learnedPower;                     // The power (duty cycle) of all elements to keep the oven at 120C
  uint8_t   spare1[3];                        // Spare (was power of top, bottom and boost)
  uint16_t  learnedInertia;                   // The time need to rasie the temperature from 120C to 150C
  uint16_t  spare2[3];                        // Spare (was inertia of top, bottom and boost)
  uint16_t  learnedInsulation;                // The insulation value of the oven  
  uint16_t  bakeTemperature;                  // Bake temperature in Celsius
  uint16_t  bakeDuration;                     // Bake duration (see getBakeSeconds( ))
  uint8_t   openDoorAfterBake;                // What to do with the oven door
  uint8_t   bakeUseCoolingFan;                // Use the cooling fan to help cool the oven
  uint16_t  numReflows;                       // Total number of reflows
  uint16_t  numBakes;                         // Total number of bakes
  uint16_t  numProfiles;                      // The number of saved reflow profiles
  uint8_t   selectedProfile;                  // The reflow profile that was used last
  profiles profile[MAX_PROFILES];             // Array of profile information
  uint16_t  lastUsedProfileBlock;             // The last block used to store a profile.  Keep cycling them to reduce flash wear
  uint8_t   logToSDCard;                      // Write reflow data to the SD card
  uint16_t  logNumber;                        // Log file sequential number

  uint8_t   spare[96];                        // Spare bytes that are initialized to zero.  Aids future expansion
};

extern Controleo3Prefs prefs;

void getPrefs();
void savePrefs();
void checkIfPrefsShouldBeWrittenToFlash();
void writePrefsToFlash();
void factoryReset(boolean saveTouchCalibrationData);

#endif