#ifndef ReflowWizard_Learn_H
#define ReflowWizard_Learn_H

#include "ReflowWizard.h"

// Learning defines
#define LEARNING_NOT_DONE             0   // Learning has not yet been done
#define LEARNING_DONE                 1   // Learning has been done
#define LEARNING_BYPASSED             2   // Learning has been manually bypassed

void learn();
void DisplayLearningTime(uint16_t duration, float temperature, int duty, int integral);
void drawLearningAbortDialog();
void displaySecondsLeft(uint32_t overallSeconds, uint32_t phaseSeconds);
void drawPerformanceBar(boolean redraw, uint8_t percentage);
uint8_t ovenScore();
void showLearnedNumbers();


#endif