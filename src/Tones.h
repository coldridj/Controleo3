#ifndef ReflowWizard_Tones_H
#define ReflowWizard_Tones_H

#include <Arduino.h>

// Tunes
#define TUNE_STARTUP                   0
#define TUNE_BUTTON_PRESSED            1
#define TUNE_INVALID_PRESS             2
#define TUNE_REFLOW_DONE               3
#define TUNE_REMOVE_BOARDS             4
#define TUNE_SCREENSHOT_BUSY           5
#define TUNE_SCREENSHOT_DONE           6
#define TUNE_REFLOW_BEEP               7
#define MAX_TUNES                      8

#define BUZZER_PIN 35

void playTones(int tune);
void setupTonePin(uint8_t pin);

#if ESP32

#define LEDC_CHANNEL 0

void tone(uint8_t pin, uint16_t tone, uint16_t duration);
void noTone(uint8_t pin);

#endif
#endif