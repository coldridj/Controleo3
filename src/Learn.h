#ifndef ReflowWizard_Learn_H
#define ReflowWizard_Learn_H

#include "ReflowWizard.h"

void learn();
void DisplayLearningTime(uint16_t duration, float temperature, int duty, int integral);
void drawLearningAbortDialog();
void displaySecondsLeft(uint32_t overallSeconds, uint32_t phaseSeconds);
void drawPerformanceBar(boolean redraw, uint8_t percentage);
uint8_t ovenScore();
void showLearnedNumbers();


#endif