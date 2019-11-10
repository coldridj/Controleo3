#ifndef ReflowWizard_Servo_H
#define ReflowWizard_Servo_H

#include <Arduino.h>

void initializeTimer();
void TC3_Handler();
void setServoPosition(uint8_t servoDegrees, uint16_t timeToTake);
uint16_t degreesToTimerCounter(uint8_t servoDegrees);

#endif