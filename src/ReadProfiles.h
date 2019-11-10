#ifndef ReflowWizard_ReadProfiles_H
#define ReflowWizard_ReadProfiles_H

#include "ReflowWizard.h"

void ReadProfilesFromSDCard();
void processDirectory(File dir);
void processFile(File file);
void initTokenPtrs();
uint8_t hasTokenBeenFound(char c);
boolean getStringFromFile(File file, char *strBuffer, uint8_t maxLength);
boolean getNumberFromFile(File file, uint16_t *num);
char *tokenToText(char *str, uint8_t token, uint16_t *numbers);
void initProfileWriteToFlash(uint16_t startingBlock);
void saveTokenAndNumbersToFlash(uint8_t token, uint16_t *numbers, uint8_t numOfNumbers);
void saveTokenAndStringToFlash(uint16_t token, char *str);
boolean writeTokenBufferToFlash(boolean endOfProfile);
uint16_t getNextTokenFromFlash(char *str, uint16_t *num);
void dumpProfile(uint8_t profileNo);
boolean profileExists(char *profileName);
void deleteProfileByName(char *profileName);
uint16_t getFreeProfileBlock();
void sortProfiles(void);
void deleteProfile(uint8_t num);
uint8_t getNumberOfProfiles();

#endif