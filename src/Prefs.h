#ifndef ReflowWizard_Prefs_H
#define ReflowWizard_Prefs_H

#include <Arduino.h>

void getPrefs();
void savePrefs();
void checkIfPrefsShouldBeWrittenToFlash();
void writePrefsToFlash();
void factoryReset(boolean saveTouchCalibrationData);

#endif