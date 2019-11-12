#ifndef ReflowWizard_ReadProfiles_H
#define ReflowWizard_ReadProfiles_H

#include "ReflowWizard.h"

// Profiles
#define MAX_PROFILE_NAME_LENGTH        31
#define MAX_PROFILES                   28     // Defined by flash space

#define MAX_PROFILE_DISPLAY_STR        30     // Maximum length of "display" string in profile file
#define MAX_PROFILE_TITLE_STR          20     // Maximum length of "title" string in profile file

struct profiles {
  char name[MAX_PROFILE_NAME_LENGTH+1];       // Name of the profile
  uint16_t peakTemperature;                   // The peak temperature of the profile
  uint16_t noOfTokens;                        // Number of tokens (instructions) in the profile
  uint16_t startBlock;                        // Flash block where this profile is stored
};

#define FIRST_PROFILE_BLOCK            64     // First flash block available to store a profile
#define PROFILE_SIZE_IN_BLOCKS         16     // Each profile can take 4K (16 x 256 byte blocks)
#define LAST_PROFILE_BLOCK             (FIRST_PROFILE_BLOCK + (MAX_PROFILES * PROFILE_SIZE_IN_BLOCKS) - 1)

// Tokens used for profile file
#define NOT_A_TOKEN                   0   // Used to indicate end of profile (no more tokens)
#define TOKEN_NAME                    1   // The name of the profile (max 31 characters)
#define TOKEN_COMMENT1                2   // Comment 1 = #
#define TOKEN_COMMENT2                3   // Comment 2 = //
#define TOKEN_DEVIATION               4   // The allowed temperature deviation before the reflow aborts
#define TOKEN_MAX_TEMPERATURE         5   // If this temperature is ever exceeded then the reflow will be aborted and the door opened
#define TOKEN_INITIALIZE_TIMER        6   // Initialize the reflow timer for logging so that comparisons can be made to datasheets
#define TOKEN_START_TIMER             7   // Initialize the reflow timer for logging so that comparisons can be made to datasheets
#define TOKEN_STOP_TIMER              8   // Initialize the reflow timer for logging so that comparisons can be made to datasheets
#define TOKEN_MAX_DUTY                9   // The highest allowed duty cycle of the elements
#define TOKEN_DISPLAY                10   // Display a message to the screen (progress message)
#define TOKEN_OVEN_DOOR_OPEN         11   // Open the oven door, over a duration in seconds
#define TOKEN_OVEN_DOOR_CLOSE        12   // Close the oven door, over a duration in seconds
#define TOKEN_BIAS                   13   // The bottom/top/boost bias (weighting) for the elements
#define TOKEN_CONVECTION_FAN_ON      14   // Turn the convection fan on
#define TOKEN_CONVECTION_FAN_OFF     15   // Turn the convection fan off
#define TOKEN_COOLING_FAN_ON         16   // Turn the cooling fan on
#define TOKEN_COOLING_FAN_OFF        17   // Turn the cooling fan off
#define TOKEN_TEMPERATURE_TARGET     18   // The PID temperature target, and the time to get there
#define TOKEN_ELEMENT_DUTY_CYCLES    19   // Element duty cycles can be forced (typically followed by WAIT)
#define TOKEN_WAIT_FOR_SECONDS       20   // Wait for the specified seconds, or until the specified temperature is reached
#define TOKEN_WAIT_UNTIL_ABOVE_C     21   // Wait for the specified seconds, or until the specified temperature is reached
#define TOKEN_WAIT_UNTIL_BELOW_C     22   // Wait for the specified seconds, or until the specified temperature is reached
#define TOKEN_PLAY_DONE_TUNE         23   // Play a tune to let the user things are done
#define TOKEN_PLAY_BEEP              24   // Play a beep
#define TOKEN_OVEN_DOOR_PERCENT      25   // Open the oven door a certain percentage
#define TOKEN_MAINTAIN_TEMP          26   // Maintain a specific temperature for a certain duration
#define TOKEN_TAP_SCREEN             27   // Wait for the screen to be tapped
#define TOKEN_SHOW_GRAPH             28   // Display a graph as the reflow progresses
#define TOKEN_GRAPH_DIVIDER          29   // Draw a temperature line on the graph
#define TOKEN_START_PLOTTING         30   // Start plotting the graph
#define TOKEN_TITLE                  31   // The title that is displayed when the profile is running

#define NUM_TOKENS                   32   // Number of tokens to look for in the profile file on the SD card
#define TOKEN_NEXT_FLASH_BLOCK     0xFE   // Profile continues in next flash block 
#define TOKEN_END_OF_PROFILE       0xFF   // Safety measure.  Flash is initialized to 0xFF, so this token means end-of-profile 

#define MAX_TOKEN_LENGTH           (MAX_PROFILE_DISPLAY_STR + 5)

#ifdef SD_ENABLE
void ReadProfilesFromSDCard();
void processDirectory(File dir);
void processFile(File file);
void initTokenPtrs();
uint8_t hasTokenBeenFound(char c);
boolean getStringFromFile(File file, char *strBuffer, uint8_t maxLength);
boolean getNumberFromFile(File file, uint16_t *num);
#endif
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