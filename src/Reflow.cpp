// Written by Peter Easton
// Released under the MIT license
// Build a reflow oven: https://whizoo.com

#include <Arduino.h>
#include "ReflowWizard.h"
#include "Reflow.h"
#include "Outputs.h"
#include "Help.h"
#include "Prefs.h"
#include "ReadProfiles.h"
#include "Screens.h"
#include "Touch.h"
#include "Tones.h"
#include "Bake.h"
#include "Temperature.h"
#include "Servo.h"
#include "Render.h"
#include "Utility.h"

#define BUTTON_DONE      0
#define BUTTON_STOP      1

// Graph location and size
#define GRAPH_TOP      145
#define GRAPH_LEFT      45
#define GRAPH_HEIGHT   150 
#define GRAPH_WIDTH    300

#define CLOSE_LOG_FILE   if (logFileOpen) { logFile.close();  logFileOpen = false; }

// Perform a reflow
// Stay in this function until the reflow is done or canceled
void reflow(uint8_t profileNo)
{
  uint32_t reflowTimer = 0, countdownTimer = 0, plotSeconds = 0, secondsFromStart = 0, lastLoopTime = millis();
  uint8_t counter = 0;
  uint8_t reflowPhase = REFLOW_PHASE_NEXT_COMMAND;
  double currentTemperature = 0, pidTemperatureDelta = 0, pidTemperature = 0;
  uint8_t elementDutyCounter[NUMBER_OF_OUTPUTS];
  boolean isOneSecondInterval = false, displayGraph = false;
  uint16_t iconsX, i, token = NOT_A_TOKEN, numbers[4], maxDuty[4], currentDuty[4], bias[4];
  boolean isPID = false, incrementTimer = true;
  boolean abortDialogIsOnScreen = false, logFileOpen = false;
  uint16_t maxTemperatureDeviation = 20, maxTemperature = 260, desiredTemperature = 0, Kd, maxBias;
  int16_t pidPower;
  float pidPreviousError = 0, pidIntegral = 0, pidDerivative, thisError;
  uint16_t graphMaxTemp = 0, graphMaxSeconds = 0;
  File logFile;
  
  // Verify the outputs are configured
  if (areOutputsConfigured() == false) {
    showHelp(HELP_OUTPUTS_NOT_CONFIGURED);
    return;
  }

  // Make sure learning has completed
  if (prefs.learningComplete == LEARNING_NOT_DONE) {
    showHelp(HELP_LEARNING_NOT_DONE);
    return;
  }

  // Is SD card logging of time/temperature enabled?
  if (prefs.logToSDCard) {
    // Is the SD card inserted?
    if (digitalRead(A0) == HIGH) {
      showHelp(HELP_NO_SD_CARD);
      return;
    }

    // Open the log file on the SD card
    // Try initializing twice.  Necessary if good card follows bad one
    if (!SD.begin() && !SD.begin()) {
      showHelp(HELP_BAD_FORMAT);
      return;
    }
    SerialUSB.println("SD Card initialized");

    // Open the log file
    sprintf(buffer100Bytes, "Log%05d.csv", prefs.logNumber);
    logFile = SD.open(buffer100Bytes, FILE_WRITE);
    if (!logFile) {
      SerialUSB.println("Unable to open logging file " + String(buffer100Bytes));
      showHelp(HELP_CANT_WRITE_TO_SD_CARD);
      return;
    }

    // Log file has been successfully opened.  Write the name of the profile to the log file
    logFileOpen = true;
    SerialUSB.println("Opened logging file " + String(buffer100Bytes));
    logFile.println(prefs.profile[profileNo].name);

    // Increment the file number (we don't care about wrap-around from 65536 to 0)
    prefs.logNumber++;
    savePrefs();
  }

  SerialUSB.println("Running profile: " + String(prefs.profile[profileNo].name));
  SerialUSB.println("Power=" + String(prefs.learnedPower) + "  Inertia=" + String(prefs.learnedInertia) + "  Insulation=" + String(prefs.learnedInsulation));

  // Calculate the centered position of the heating and fan icons (icons are 32x32)
  iconsX = 240 - (numOutputsConfigured() * 20) + 4;  // (2*20) - 32 = 8.  8/2 = 4

  // Stagger the element start cycle to avoid abrupt changes in current draw
  // Simple method: there are 6 outputs but the first ones are likely the heating elements
  for (i=0; i< NUMBER_OF_OUTPUTS; i++)
    elementDutyCounter[i] = (65 * i) % 100;

  // Default the maximum duty cycles for the elements.  These values can be overwritten by the profile file
  maxDuty[TYPE_BOTTOM_ELEMENT] = 100;
  maxDuty[TYPE_TOP_ELEMENT] = 75;
  maxDuty[TYPE_BOOST_ELEMENT] = 60;

  // Default the current duty cycles for the elements.
  currentDuty[TYPE_BOTTOM_ELEMENT] = 0;
  currentDuty[TYPE_TOP_ELEMENT] = 0;
  currentDuty[TYPE_BOOST_ELEMENT] = 0;

  // Default the bias for the elements.  These values can be overwritten by the profile file
  bias[TYPE_BOTTOM_ELEMENT] = 100;
  bias[TYPE_TOP_ELEMENT] = 80;
  bias[TYPE_BOOST_ELEMENT] = 50;
  maxBias = 100;

  // Set up the flash reads to start with the first block of this profile
  if (getNextTokenFromFlash(0, &prefs.profile[profileNo].startBlock) == TOKEN_END_OF_PROFILE) {
    CLOSE_LOG_FILE;
    return;
  }

  // Default the title to the old "Reflow" (the title can be overwritten in the profile)
  eraseHeader();
  displayHeader((char *) "Reflow", false);

  // Ug, hate goto's!  But this saves a lot of extraneous code.
userChangedMindAboutAborting:

  // Erase the bottom part of the screen
   tft.fillRect(0, 100, 480, 230, WHITE);

  // Setup the STOP/DONE tap targets on this screen
  drawStopDoneButton(displayGraph, BUTTON_STOP);

  // Display the graph, if user chose to display it
  if (displayGraph)
    drawGraphOutline(graphMaxTemp, graphMaxSeconds);

  // Toggle the baking temperature between C/F if the user taps in the top-right corner
  setTouchTemperatureUnitChangeCallback(0);

  // Display the status (if waiting)
  updateStatusMessage(token, countdownTimer, desiredTemperature, abortDialogIsOnScreen);
  
  // Debounce any taps that took us to this screen
  debounce();

  // Keep looping until reflow is done
  while (1) {
    // Waiting for the user to tap the screen?
    if (reflowPhase == REFLOW_WAITING_FOR_TAP && touch.isPressed()) {
      updateStatusMessage(NOT_A_TOKEN, 0, 0, abortDialogIsOnScreen);
      playTones(TUNE_BUTTON_PRESSED);
      quickDebounce();
      reflowPhase = REFLOW_PHASE_NEXT_COMMAND;
    }
    
    // Has there been a touch?
    switch (getTap(CHECK_FOR_TAP_THEN_EXIT)) {
      case 0: 
        // If reflow is done (or user taps "stop" in Abort dialog) then clean up and return to the main menu
        if (reflowPhase >= REFLOW_ALL_DONE || abortDialogIsOnScreen) {
          reflowPhase = REFLOW_ABORT;
          // Make sure we exit this screen as soon as possible
          lastLoopTime = millis() - 20;
          counter = 40;
        }
        else {
          // User tapped to abort the reflow
          drawReflowAbortDialog();
          abortDialogIsOnScreen = true;
        }
        break;

      default:
        // The user didn't tap the screen, but if the Abort dialog is up and the phase makes
        // it irrelevant then automatically dismiss it now
        // You never know, maybe the cat tapped "Stop" ...
        if (!abortDialogIsOnScreen || reflowPhase < REFLOW_ALL_DONE)
          break;
        // Intentional fall-through (simulate user tapped Cancel) ...

      case 1:
        // This is the cancel button of the Abort dialog.  User wants to continue
        // Erase the Abort dialog
        abortDialogIsOnScreen = false;
        counter = 0;
        // Redraw the screen under the dialog
        goto userChangedMindAboutAborting;
    }
    
    // Execute this loop every 20ms (50 times per second)
    if (millis() - lastLoopTime < 20) {
      delay(1);
      continue;
    }
    lastLoopTime += 20;

    // Try not to update everything in the same 20ms time slice
    // Update the temperature
    if (counter == 1)
      displayTemperatureInHeader();
    // Dump data to the debugging port
    if (counter == 5 && reflowPhase != REFLOW_ALL_DONE)
      DumpDataToUSB(reflowTimer, currentTemperature, 0, 0);
    // Update the reflow timer
    if (counter == 10 && !abortDialogIsOnScreen)
      displayReflowDuration(reflowTimer, displayGraph);
    // Log data to the SD card
    if (counter == 20 && logFileOpen) {
      sprintf(buffer100Bytes, "%ld,%d.%02d", secondsFromStart, (uint16_t) currentTemperature, (uint16_t) ((currentTemperature - (uint16_t) currentTemperature) * 100));
      logFile.println(buffer100Bytes);
      // Flush the buffer (write to SD card) frequenty to prevent stutters when writing big blocks of data
      logFile.flush();
    }

    // Determine if this is on a 1-second interval
    isOneSecondInterval = false;
    if (++counter >= 50) {
      counter = 0;
      isOneSecondInterval = true;
      if (countdownTimer)
        countdownTimer--;
      if (incrementTimer && reflowPhase < REFLOW_ALL_DONE)
        reflowTimer++;
      plotSeconds++;
      secondsFromStart++;
    }
    
    // Read the current temperature
    currentTemperature = getCurrentTemperature();
    if (IS_MAX31856_ERROR(currentTemperature)) {
      switch ((int) currentTemperature) {
        case FAULT_OPEN:
          strcpy(buffer100Bytes, "Fault open (disconnected)");
          break;
        case FAULT_VOLTAGE:
          strcpy(buffer100Bytes, "Over/under voltage (wrong type?)");
          break;
        case NO_MAX31856: // Should never happen unless MAX31856 is broken
          strcpy(buffer100Bytes, "MAX31856 error");
          break;
      }
    
      // Abort the reflow
      SerialUSB.println("Thermocouple error:" + String(buffer100Bytes));
      SerialUSB.println("Profile aborted because of thermocouple error!");
      showReflowError(iconsX, (char *) "Thermocouple error:", buffer100Bytes);
      reflowPhase = REFLOW_ABORT;
    }

    // Was the maximum temperature exceeded?
    if (currentTemperature > maxTemperature && reflowPhase != REFLOW_PHASE_NEXT_COMMAND) {
      // Open the oven door to cool things off
      setServoPosition(prefs.servoOpenDegrees, 3000);

      // Abort the reflow
      SerialUSB.println("Profile aborted because of maximum temperature exceeded!");
      sprintf(buffer100Bytes, "Maximum temperature of %d~C", maxTemperature);
      showReflowError(iconsX, buffer100Bytes, (char *) "was exceeded.");
      reflowPhase = REFLOW_ABORT;      
    }

    switch (reflowPhase) {
      case REFLOW_PHASE_NEXT_COMMAND:
        // Get the next token from flash, and act on it
        token = getNextTokenFromFlash(buffer100Bytes, numbers);

        if (token != TOKEN_DISPLAY && token != TOKEN_TITLE)
          SerialUSB.println(tokenToText(buffer100Bytes, token, numbers));

        switch (token) {
          case TOKEN_DISPLAY:
            // Erase the text that was there and display the text from the profile
            tft.fillRect(20, LINE(1), 459, 24, WHITE);
            displayString(20, LINE(1), FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
            break;

          case TOKEN_TITLE:
            // Erase the text that was there and display the text from the profile
            eraseHeader();
            displayHeader(buffer100Bytes, false);
            break;

          case TOKEN_MAX_DUTY:
            // Overwrite the default max duty cycles of the elements (max is 100%)
            maxDuty[TYPE_BOTTOM_ELEMENT] = numbers[0] < 100? numbers[0]: 100;
            maxDuty[TYPE_TOP_ELEMENT] = numbers[1] < 100? numbers[1]: 100;
            maxDuty[TYPE_BOOST_ELEMENT] = numbers[2] < 100? numbers[2]: 100;
            break;

          case TOKEN_ELEMENT_DUTY_CYCLES:
            // Force a specific duty cycle on the elements.  This turns off PID
            currentDuty[TYPE_BOTTOM_ELEMENT] = numbers[0] < 100? numbers[0]: 100;
            currentDuty[TYPE_TOP_ELEMENT] = numbers[1] < 100? numbers[1]: 100;
            currentDuty[TYPE_BOOST_ELEMENT] = numbers[2] < 100? numbers[2]: 100;
            // Make sure the maximum duty cycles hasn't been exceeded
            for (i = TYPE_BOTTOM_ELEMENT; i <= TYPE_BOOST_ELEMENT; i++) {
              if (currentDuty[i] > maxDuty[i])
                currentDuty[i] = maxDuty[i];
            }
            // Turn PID temperature control off.  The user wants these specific values to be used
            isPID = false;
            break;

          case TOKEN_BIAS:
            // Specify a bottom/top/boost temperature bias
            // They cannot all be zero
            if ((numbers[0] + numbers[1] + numbers[2]) == 0)
              break;
            bias[TYPE_BOTTOM_ELEMENT] = numbers[0];
            bias[TYPE_TOP_ELEMENT] = numbers[1];
            bias[TYPE_BOOST_ELEMENT] = numbers[2];
            // Figure out the high number for bias.  For example, 90/60/30 could also be expressed as 3/2/1
            maxBias = 0;
            for (i = TYPE_BOTTOM_ELEMENT; i <= TYPE_BOOST_ELEMENT; i++) {
              if (bias[i] > maxBias)
                maxBias = bias[i];
            }
            break;

          case TOKEN_DEVIATION:
            // Specify the delta between the target temperature and current temperature which if exceeded will
            // cause abort.  This is only used when running PID.
            maxTemperatureDeviation = constrain(numbers[0], 1, 100);
            break;

          case TOKEN_MAX_TEMPERATURE:
            // Specify the maximum temperature the oven isn't allowed to exceed
            maxTemperature = numbers[0];
            break;

          case TOKEN_INITIALIZE_TIMER:
            // Update the on-screen timer and logging timer with this new value
            reflowTimer = numbers[0];
            incrementTimer = true;
            break;

          case TOKEN_START_TIMER:
            incrementTimer = true;
            break;
            
          case TOKEN_STOP_TIMER:
            incrementTimer = false;
            break;
          
          case TOKEN_OVEN_DOOR_OPEN:
            // Open the oven door over X seconds
            numbers[0] = numbers[0] < 30? numbers[0]: 30;
            setServoPosition(prefs.servoOpenDegrees, numbers[0] * 1000);
            break;
            
          case TOKEN_OVEN_DOOR_CLOSE:
            // Close the oven door over X seconds
            numbers[0] = numbers[0] < 30? numbers[0]: 30;
            setServoPosition(prefs.servoClosedDegrees, numbers[0] * 1000);
            break;

          case TOKEN_OVEN_DOOR_PERCENT:
            // Open the oven door a certain percentage
            numbers[0] = numbers[0] < 100? numbers[0]: 100;
            numbers[1] = numbers[1] < 30? numbers[1]: 30;
            setServoPosition(map(numbers[0], 0, 100, prefs.servoClosedDegrees, prefs.servoOpenDegrees), numbers[1] * 1000);
            break;

          case TOKEN_WAIT_FOR_SECONDS:
            // Keep the oven in this state for a number of seconds
            // PID shouldn't be on now. TOKEN_ELEMENT_DUTY_CYCLES should've been specified
            if (isPID) {
              SerialUSB.println("ERROR: Must specify \"element duty cycle\" before \"wait for\"!");
              isPID = false;
              // Assume elements should be off
              currentDuty[TYPE_BOTTOM_ELEMENT] = 0;
              currentDuty[TYPE_TOP_ELEMENT] = 0;
              currentDuty[TYPE_BOOST_ELEMENT] = 0;
            }
            countdownTimer = numbers[0];
            reflowPhase = REFLOW_WAITING_FOR_TIME;
            updateStatusMessage(token, countdownTimer, 0, abortDialogIsOnScreen);
            break;

          case TOKEN_WAIT_UNTIL_ABOVE_C:
            // Wait until the oven temperature is above a certain temperature
            // PID shouldn't be on now. TOKEN_ELEMENT_DUTY_CYCLES should've been specified
            if (isPID) {
              SerialUSB.println("ERROR: Must specify \"element duty cycle\" before \"wait until above\"!");
              isPID = false;
              // Assume elements should be off
              currentDuty[TYPE_BOTTOM_ELEMENT] = 0;
              currentDuty[TYPE_TOP_ELEMENT] = 0;
              currentDuty[TYPE_BOOST_ELEMENT] = 0;
            }
            desiredTemperature = numbers[0];
            if (desiredTemperature >= maxTemperature) {
              // This is a problem because the reflow will abort as soon as this temperature is reached
              SerialUSB.println("ERROR: wait-until-temp higher than maximum temperature!");
              desiredTemperature = maxTemperature;
            }
            reflowPhase = REFLOW_WAITING_UNTIL_ABOVE;            
            updateStatusMessage(token, 0, desiredTemperature, abortDialogIsOnScreen);
            break;
            
          case TOKEN_WAIT_UNTIL_BELOW_C:
            // PID shouldn't be on now. TOKEN_ELEMENT_DUTY_CYCLES should've been specified
            if (isPID) {
              SerialUSB.println("ERROR: Must specify \"element duty cycle\" before \"wait until below\"!");
              isPID = false;
              // Assume elements should be off
              currentDuty[TYPE_BOTTOM_ELEMENT] = 0;
              currentDuty[TYPE_TOP_ELEMENT] = 0;
              currentDuty[TYPE_BOOST_ELEMENT] = 0;
            }
            // Wait until the oven temperature is above a certain temperature
            desiredTemperature = numbers[0];
            if (desiredTemperature < 25) {
              // This is a problem because the temperature is below room temperature
              SerialUSB.println("ERROR: wait-until-temp lower than room temperature!");
              desiredTemperature = 25;
            }
            reflowPhase = REFLOW_WAITING_UNTIL_BELOW;            
            updateStatusMessage(token, 0, desiredTemperature, abortDialogIsOnScreen);
            break;

          case TOKEN_MAINTAIN_TEMP:
            // Save the parameters
            pidTemperature = numbers[0];
            countdownTimer = numbers[1] > 0? numbers[1] : 1;
            updateStatusMessage(token, countdownTimer, pidTemperature, abortDialogIsOnScreen);
            desiredTemperature = pidTemperature;

            reflowPhase = REFLOW_MAINTAIN_TEMP;
            // The temperature control is now done using PID
            isPID = true;
            pidTemperatureDelta = 0;
            // Initialize the PID variables
            pidPreviousError = 0;
            pidIntegral = 0;
            break;

          case TOKEN_SHOW_GRAPH:
            // Graph the reflow
            displayGraph = true;
            graphMaxTemp = numbers[0] > 100? numbers[0] : 100;
            graphMaxSeconds = numbers[1] > 100? numbers[1] : 100;
            // Don't start plotting until the "start plotting" command
            plotSeconds = graphMaxSeconds + 1;
            // Draw the graph UI
            goto userChangedMindAboutAborting;
            break;

          case TOKEN_GRAPH_DIVIDER:
            if (displayGraph) {
              // Calculate where the line must be
              uint16_t ypos = GRAPH_TOP + GRAPH_HEIGHT - (GRAPH_HEIGHT * ((float) numbers[0]/(float) graphMaxTemp));
              ypos = constrain(ypos, GRAPH_TOP, GRAPH_TOP + GRAPH_HEIGHT);
              // Draw the line
              tft.drawFastHLine(GRAPH_LEFT+2, ypos, GRAPH_WIDTH-2, GREEN);
            }
            break;

          case TOKEN_START_PLOTTING:
            // Start plotting time / temperature
            plotSeconds = numbers[0];
            SerialUSB.println("Starting to plot");
            break;
            
          case TOKEN_CONVECTION_FAN_ON:
            // Turn on the convection fan
            turnConvectionFanOn(true);
            break;
            
          case TOKEN_CONVECTION_FAN_OFF:
            // Turn off the convection fan
            turnConvectionFanOn(false);
            break;
            
          case TOKEN_COOLING_FAN_ON:
            // Turn on the cooling fan
            turnCoolingFanOn(true);
            break;
            
          case TOKEN_COOLING_FAN_OFF:
            // Turn off the cooling fan
            turnCoolingFanOn(false);
            break;

          case TOKEN_PLAY_DONE_TUNE:
            // Play a tune to let the user know reflow is done
            playTones(TUNE_REFLOW_DONE);
            break;
 
          case TOKEN_PLAY_BEEP:
            // Play a tune to let the user know reflow is done
            playTones(TUNE_REFLOW_BEEP);
            break;
 
          case TOKEN_TEMPERATURE_TARGET:
            // Save the parameters
            desiredTemperature = numbers[0];
            countdownTimer = numbers[1] > 0? numbers[1] : 1;
            updateStatusMessage(token, countdownTimer, desiredTemperature, abortDialogIsOnScreen);
            // The temperature control is now done using PID
            isPID = true;
            // Calculate a straight line between the current temperature and the desired end temperature
            pidTemperatureDelta = (desiredTemperature - currentTemperature) / countdownTimer;
            // Start the PID temperature at the current temperature
            pidTemperature = currentTemperature;
            // Initialize the PID variables
            pidPreviousError = 0;
            pidIntegral = 0;
            reflowPhase = REFLOW_PID;
            break;

          case TOKEN_TAP_SCREEN:
            updateStatusMessage(NOT_A_TOKEN, 0, 0, abortDialogIsOnScreen);
            updateStatusMessage(token, 0, 0, abortDialogIsOnScreen);
            reflowPhase = REFLOW_WAITING_FOR_TAP;
            break;

          case TOKEN_END_OF_PROFILE:
            // The end of the profile has been reached.  Reflow is done
            // Change the STOP button to DONE
            drawStopDoneButton(displayGraph, BUTTON_DONE);
            reflowPhase = REFLOW_ALL_DONE;
            // One more reflow completed!
            prefs.numReflows++;
            savePrefs();
        }

        // If the reflow timer isn't ticking, execute the next command quickly
        if (!reflowTimer)
          lastLoopTime = millis() - 20;
        break;
        
      case REFLOW_WAITING_FOR_TIME:
        // Make changes every second
        if (!isOneSecondInterval)
          break;

        // Update the time left
        updateStatusMessage(token, countdownTimer, 0, abortDialogIsOnScreen);
        // We were waiting for a certain period of time.  Have we waited long enough?
        if (countdownTimer == 0) {
          SerialUSB.println("Finished waiting");
          // Erase the status
          updateStatusMessage(NOT_A_TOKEN, 0, 0, abortDialogIsOnScreen);
          // Get the next command
          reflowPhase = REFLOW_PHASE_NEXT_COMMAND;
        }
        break;
        
      case REFLOW_WAITING_UNTIL_ABOVE:
        // Make changes every second
        if (!isOneSecondInterval)
          break;

        // We were waiting for the oven temperature to rise above a certain point
        if (currentTemperature >= desiredTemperature) {
          SerialUSB.println("Heated to desired temperature");
          // Erase the status
          updateStatusMessage(NOT_A_TOKEN, 0, 0, abortDialogIsOnScreen);
          // Get the next command
          reflowPhase = REFLOW_PHASE_NEXT_COMMAND;
        }
        break;

      case REFLOW_WAITING_UNTIL_BELOW:
        // Make changes every second
        if (!isOneSecondInterval)
          break;

        // We were waiting for the oven temperature to drop below a certain point
        if (currentTemperature <= desiredTemperature) {
          SerialUSB.println("Cooled to desired temperature");
          // Erase the status
          updateStatusMessage(NOT_A_TOKEN, 0, 0, abortDialogIsOnScreen);
          // Get the next command
          reflowPhase = REFLOW_PHASE_NEXT_COMMAND;
        }
        break;

      case REFLOW_MAINTAIN_TEMP:
        // Make changes every second
        if (!isOneSecondInterval)
          break;

        // We were waiting for a certain period of time.  Have we waited long enough?
        if (countdownTimer == 0) {
          SerialUSB.println("Finished maintaining temperature");
          // Erase the status
          updateStatusMessage(NOT_A_TOKEN, 0, 0, abortDialogIsOnScreen);
          // Get the next command
          reflowPhase = REFLOW_PHASE_NEXT_COMMAND;
          break;
        }

        // Is the oven over the desired temperature?
        if (currentTemperature >= desiredTemperature) {
          // Turn all the elements off
          currentDuty[TYPE_BOTTOM_ELEMENT] = 0;
          currentDuty[TYPE_TOP_ELEMENT] = 0;
          currentDuty[TYPE_BOOST_ELEMENT] = 0;
          // Update the countdown timer
          updateStatusMessage(token, countdownTimer, desiredTemperature, abortDialogIsOnScreen);
          // Reset the PID variables
          pidIntegral = 0;
          pidPreviousError = 0;
          break;
        }
        // Intentional fall-through if under temperature ...

      case REFLOW_PID:
        // Make changes every second
        if (!isOneSecondInterval)
          break;

        // Has the desired temperature been reached?  Go to the next phase then
        // The PID phase terminates when the temperature is reached, not when the
        // timer reaches zero.
        if (currentTemperature > desiredTemperature) {
          // Erase the status
          updateStatusMessage(NOT_A_TOKEN, 0, 0, abortDialogIsOnScreen);
          // Get the next command
          reflowPhase = REFLOW_PHASE_NEXT_COMMAND;
          break;
        }
        
        // Calculate what the expected temperature should be at this point
        pidTemperature += pidTemperatureDelta;
      
        // Abort if deviated too far from the required temperature
        if (reflowPhase == REFLOW_PID && abs(pidTemperature - currentTemperature) > maxTemperatureDeviation && pidTemperature < desiredTemperature) {
          // Open the oven door
          setServoPosition(prefs.servoOpenDegrees, 3000);
          SerialUSB.println("ERROR: temperature delta exceeds maximum allowed!");
          sprintf(buffer100Bytes, "Exceeded max deviation of %d~C.", maxTemperatureDeviation);
          sprintf(buffer100Bytes+50, "Target = %d~C, actual = %d~C", (int) pidTemperature, (int) currentTemperature);
          showReflowError(iconsX, buffer100Bytes, buffer100Bytes+50);
          reflowPhase = REFLOW_ALL_DONE;
          break;
        }

        // Assume a certain power level, based on the current temperature and the rate-of-rise
        // This should be fairly accurate, and is based on the learned values
        pidPower = getBasePIDPower(pidTemperature, pidTemperatureDelta, bias, maxBias);
        
        // Do the PID calculation now.  The base power will be adjusted a bit based on this result
        // This is the standard PID formula, using a 1-second interval
        thisError = pidTemperature - currentTemperature;
        pidIntegral = pidIntegral + thisError;
        pidDerivative = thisError - pidPreviousError;
        pidPreviousError = thisError;
        
        // The black magic of PID tuning!
        // Compared to most other closed-loop systems, reflow ovens are slow to respond to input, so the derivative term is the
        // most important one.  The other terms are assigned lower weights.
        // Kp = 2.  This says that if the temperature is 5 degrees too low, the power should be increased by 10%.  This doesn't
        //   sound like much (and it isn't) but heating elements are slow to heat up and cool down, so this is reasonable.
        // Ki = 0.01. This is a very small number.  It basically says that if we're under-temperature for a very long time then
        //   increase the power to the elements a tiny amount.  Having this any higher will create oscillations.
        // Kd is based on the learned inertia value and for the typical reflow oven it should be around 35.  Some resistive
        //   elements take a very long time to heat up and cool down so this will be a much higher value.
        Kd = map(constrain(prefs.learnedInertia, 30, 100), 30, 100, 30, 75);
        // Dump these values out over USB for debugging
        SerialUSB.println("T="+String(currentTemperature)+" P="+String(pidTemperature)+" D="+String(pidTemperatureDelta)+" E="+String(thisError)+" I="+String(pidIntegral)+" D="+String(pidDerivative)+" Kd="+String(Kd));

        // If we're over-temperature, it is best to slow things down even more since taking a bit longer in a phase is better than taking less time
        if (thisError < 0)
          thisError = 4 * thisError + 0.01 * pidIntegral + Kd * pidDerivative;
        else
          thisError = 2 * thisError + 0.01 * pidIntegral + Kd * pidDerivative;

        // The base power we calculated first should be close to the required power, but allow the PID value to adjust
        // this up or down a bit.  The effect PID has on the outcome is deliberately limited because moving between zero
        // (elements off) and 100 (full power) will create hot and cold spots.  PID can move the power by 60%; 30% down or up.
        thisError = constrain(thisError, -30, 30);
        
        // Add the base power and the PID delta
        SerialUSB.println("Power was " + String(pidPower) + " and is now " + String(pidPower + thisError));
        pidPower += (thisError);

        // Make sure the resulting power is reasonable
        pidPower = constrain(pidPower, 0, 100);

        // Determine the duty cycle of each element based on the top/bottom/boost bias
        currentDuty[TYPE_TOP_ELEMENT] = pidPower * bias[TYPE_TOP_ELEMENT] / maxBias;
        currentDuty[TYPE_BOTTOM_ELEMENT] = pidPower * bias[TYPE_BOTTOM_ELEMENT] / maxBias;
        currentDuty[TYPE_BOOST_ELEMENT] = pidPower * bias[TYPE_BOOST_ELEMENT] / maxBias;

        // Make sure none of the max duty cycles are exceeded
        for (i = TYPE_BOTTOM_ELEMENT; i <= TYPE_BOOST_ELEMENT; i++) {
          if (currentDuty[i] > maxDuty[i])
            currentDuty[i] = maxDuty[i];
        }

        // Update the countdown timer
        updateStatusMessage(token, countdownTimer, desiredTemperature, abortDialogIsOnScreen);
        break;

      case REFLOW_ALL_DONE:
        // Nothing to do here.  Just waiting for user to tap the screen
        CLOSE_LOG_FILE;
        break;
        
      case REFLOW_ABORT:
        // User either tapped "Done" at the end of the reflow, or the user tapped abort
        setOvenOutputs(ELEMENTS_OFF, CONVECTION_FAN_OFF, COOLING_FAN_OFF);
        // Close the oven door
        setServoPosition(prefs.servoClosedDegrees, 1000);
        // Stop logging
        CLOSE_LOG_FILE;
        // All done!
        return;
    }
 
    // Turn the outputs on or off based on the duty cycle
    for (i=0; i< NUMBER_OF_OUTPUTS; i++) {
      switch (prefs.outputType[i]) {
        case TYPE_TOP_ELEMENT:
          // Turn the output on at 0, and off at the duty cycle value
          if (elementDutyCounter[i] == 0 && currentDuty[TYPE_TOP_ELEMENT] > 0)
              setOutput(i, HIGH);
          // Turn it off at the right duty cycle
          if (elementDutyCounter[i] >= currentDuty[TYPE_TOP_ELEMENT])
            setOutput(i, LOW);
          break;
        case TYPE_BOTTOM_ELEMENT:
          // Turn the output on at 0, and off at the duty cycle value
          if (elementDutyCounter[i] == 0 && currentDuty[TYPE_BOTTOM_ELEMENT] > 0)
              setOutput(i, HIGH);
          // Turn it off at the right duty cycle
          if (elementDutyCounter[i] >= currentDuty[TYPE_BOTTOM_ELEMENT])
            setOutput(i, LOW);
          break;
          
        case TYPE_BOOST_ELEMENT: 
          // Turn the output on at 0, and off at the duty cycle value
          if (elementDutyCounter[i] == 0 && currentDuty[TYPE_BOOST_ELEMENT] > 0)
              setOutput(i, HIGH);
          // Turn it off at the right duty cycle
          if (elementDutyCounter[i] >= currentDuty[TYPE_BOOST_ELEMENT])
            setOutput(i, LOW);
          break;
        }
      
        // Increment the duty counter
        elementDutyCounter[i] = (elementDutyCounter[i] + 1) % 100;
    }

    // Add data to the graph plot
     if (isOneSecondInterval  && !abortDialogIsOnScreen && displayGraph) {
       // Does this data point need to be plotted?
       if (plotSeconds > 0 && plotSeconds < graphMaxSeconds) {
         // Calculate the x and y positions of this point
         uint16_t xpos = GRAPH_LEFT + (((float) plotSeconds)/((float) graphMaxSeconds)) * GRAPH_WIDTH;
         uint16_t ypos = GRAPH_TOP + GRAPH_HEIGHT - (GRAPH_HEIGHT * ((float) currentTemperature/(float) graphMaxTemp));
         // Allow the temperature to go over the top of the graph, just a bit
         ypos = constrain(ypos, GRAPH_TOP - 6, GRAPH_TOP + GRAPH_HEIGHT - 1);
         xpos = constrain(xpos, GRAPH_LEFT + 1, GRAPH_LEFT + GRAPH_WIDTH - 1);
         tft.fillRect(xpos - 1, ypos - 1, 3, 3, RED);
       }
     }

     animateIcons(iconsX);
  } // end of big while loop  
}


// Draw the STOP/DONE button on the screen
void drawStopDoneButton(boolean isGraphDisplayed, boolean buttonIsStop)
{
  clearTouchTargets();

  if (isGraphDisplayed) {
      // Draw the button
      tft.fillRect(368, 247, 94, 26, WHITE);
      drawButton(352, 230, 126, buttonIsStop? 87: 93, BUTTON_LARGE_FONT, buttonIsStop? (char *) "STOP" : (char *) "DONE");
      // Define the tap target (as large as possible)
      defineTouchArea(320, 200, 160, 120);
  }
  else {
      // Draw the button
      tft.fillRect(194, 247, 94, 26, WHITE);
      drawButton(110, 230, 260, buttonIsStop? 87: 93, BUTTON_LARGE_FONT, buttonIsStop? (char *) "STOP" : (char *) "DONE");
      // Define the tap target (as large as possible)
      defineTouchArea(20, 150, 440, 170);
  }
}

// Draw the abort dialog on the screen.  The user needs to confirm that they want to exit reflow
void drawReflowAbortDialog()
{
  drawThickRectangle(0, 100, 480, 220, 10, RED);
  tft.fillRect(10, 110, 460, 200, WHITE);
  displayString(126, 116, FONT_12PT_BLACK_ON_WHITE, (char *) "Stop Running");
  displayString(54, 157, FONT_9PT_BLACK_ON_WHITE, (char *) "Are you sure you want to stop?");
  clearTouchTargets();
  drawTouchButton(60, 232, 160, 74, BUTTON_LARGE_FONT, (char *) "Stop");
  drawTouchButton(260, 232, 160, 105, BUTTON_LARGE_FONT, (char *) "Cancel");
}


void updateStatusMessage(uint16_t token, uint16_t timer, uint16_t temperature, boolean abortDialogIsOnScreen)
{
  uint16_t strLength;
  static uint8_t numberLength = 0;

  // Don't do anything if the abort dialog is on the screen
  if (abortDialogIsOnScreen)
    return;

  // Erase the area where the status message is displayed
  if (token == NOT_A_TOKEN) {
    tft.fillRect(20, LINE(2), 459, 24, WHITE);
    numberLength = 0;
  }

  switch (token) {
    case TOKEN_WAIT_FOR_SECONDS:
      strLength = displayString(20, LINE(2), FONT_9PT_BLACK_ON_WHITE, (char *) "Waiting... ");
      sprintf(buffer100Bytes, "%d", timer);
      if (numberLength == 0)
        numberLength = strlen(buffer100Bytes);
      displayFixedWidthString(20+strLength, LINE(2), buffer100Bytes, numberLength, FONT_9PT_BLACK_ON_WHITE_FIXED);
      break;

    case TOKEN_WAIT_UNTIL_ABOVE_C:
      sprintf(buffer100Bytes, "Continue when oven is above %d~C", temperature);
      displayString(20, LINE(2), FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
      break;
      
    case TOKEN_WAIT_UNTIL_BELOW_C:
      sprintf(buffer100Bytes, "Continue when oven is below %d~C", temperature);
      displayString(20, LINE(2), FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
      break;

    case TOKEN_TEMPERATURE_TARGET:
      sprintf(buffer100Bytes, "Ramping oven to %d~C ... ", temperature);
      strLength = displayString(20, LINE(2), FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
      sprintf(buffer100Bytes, "%d", timer);
      if (numberLength == 0)
        numberLength = strlen(buffer100Bytes);
      displayFixedWidthString(20+strLength, LINE(2), buffer100Bytes, numberLength, FONT_9PT_BLACK_ON_WHITE_FIXED);
      break;

    case TOKEN_MAINTAIN_TEMP:
      sprintf(buffer100Bytes, "Holding temperature at %d~C ... ", temperature);
      strLength = displayString(20, LINE(2), FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
      sprintf(buffer100Bytes, "%d", timer);
      if (numberLength == 0)
        numberLength = strlen(buffer100Bytes);
      displayFixedWidthString(20+strLength, LINE(2), buffer100Bytes, numberLength, FONT_9PT_BLACK_ON_WHITE_FIXED);
      break;

    case TOKEN_TAP_SCREEN:
      displayString(20, LINE(2), FONT_9PT_BLACK_ON_WHITE, (char *) "Tap the screen to continue ...");
      break;
  }
}


// Show an error dialog when an error forces reflow to be aborted
void showReflowError(uint16_t iconsX, char *line1, char *line2)
{
  uint32_t lastLoopTime = millis(), updateTemperatureInHeader = millis();;
  // Show the error on the screen
  drawThickRectangle(0, 90, 480, 230, 15, RED);
  tft.fillRect(15, 105, 450, 200, WHITE);
  displayString(196, 110, FONT_12PT_BLACK_ON_WHITE, (char *) "Error");
  displayString(40, 150, FONT_9PT_BLACK_ON_WHITE, line1);
  displayString(40, 180, FONT_9PT_BLACK_ON_WHITE, line2);
  drawStopDoneButton(false, BUTTON_DONE);

  // Turn everything off except the fans
  setOvenOutputs(ELEMENTS_OFF, CONVECTION_FAN_ON, COOLING_FAN_ON);
  // Wait for the user to tap the screen
  do {
    // Animate the fan icons
    if (millis() - lastLoopTime >= 20) {
      animateIcons(iconsX); 
      lastLoopTime += 20;
    }
    // Update the temperature in the header
    if (millis() - updateTemperatureInHeader >= 1000) {
      displayTemperatureInHeader(); 
      updateTemperatureInHeader += 1000;
    }
  } while (getTap(CHECK_FOR_TAP_THEN_EXIT) == -1);
}


// Display the reflow timer
void displayReflowDuration(uint32_t seconds, boolean isGraphDisplayed)
{
  static uint16_t oldWidth = 1, timerX = 381;
  uint16_t newWidth;

  if (isGraphDisplayed) {
    newWidth = displayString(timerX, 170, FONT_12PT_BLACK_ON_WHITE_FIXED, secondsInClockFormat(buffer100Bytes, seconds));
    // Keep the timer display centered
    if (newWidth != oldWidth) {
      // The width has changed (one more character on the display).  Erase what was there
      tft.fillRect(timerX, 170, newWidth > oldWidth? newWidth : oldWidth, 25, WHITE);
      // Redraw the timer
      oldWidth = newWidth;
      timerX = 415 - (newWidth >> 1);
      // If timer can't be centered then right-justify it
      if (newWidth > 120)
        timerX = 475 - newWidth;
      displayString(timerX, 170, FONT_12PT_BLACK_ON_WHITE_FIXED, buffer100Bytes);
    }
  }
  else {
    // Center the timer for the "no graph" initialization case
    if (timerX == 381)
      timerX = 178;
    newWidth = displayString(timerX, 160, FONT_22PT_BLACK_ON_WHITE_FIXED, secondsInClockFormat(buffer100Bytes, seconds));
    // Keep the timer display centered
    if (newWidth != oldWidth) {
      // The width has changed (one more character on the display).  Erase what was there
      tft.fillRect(timerX, 160, newWidth > oldWidth? newWidth : oldWidth, 48, WHITE);
      // Redraw the timer
      oldWidth = newWidth;
      timerX = 240 - (newWidth >> 1);
      displayString(timerX, 160, FONT_22PT_BLACK_ON_WHITE_FIXED, buffer100Bytes);
    }
  }
}


// Calculate the expected power level based on the desired temperature and desired rate-of-rise
uint16_t getBasePIDPower(double temperature, double increment, uint16_t *bias, uint16_t maxBias)
{
  uint16_t basePower, insulationPower, risePower, totalBasePower;
  float biasFactor;
  
  temperature = constrain(temperature, 29, 250);
  // First, figure out the power required to maintain this temperature
  // Start by extrapolating the power using all elements at 120C
  // 120C = 100%, 150C = 125%, 200C = 166%
  basePower = temperature * 0.83 * prefs.learnedPower / 100;
  // Adjust this number slightly based on the expected losses through the insulation. Heat losses will be higher at high temperatures
  insulationPower = map(prefs.learnedInsulation, 0, 300, map(temperature, 0, 400, 0, 20), 0);

  // Adjust by the desired rate-of-rise
  risePower = increment * basePower * 2;

  // Adjust power by the bias, since some elements may receive less than the calculated power
  // Hope that the user hasn't made the boost element receive the most power - that isn't good.
  // Example, if top/bottom bias was 50/100 and power was 40%, then power should increase to 53.33%
  // as shown below
  // No bias:
  //   top: bias @ 100% * power @ 40% ==> 40%
  //   bottom: bias @ 100% * power @ 40% ==> 40%
  //   average power: 40% + 40% / 2 ==> 40%
  // 50/100 top/bottom bias:
  //   top: bias @50% * power @ 53.33% ==> 26.66%
  //   bottom: bias @100% * power @ 53.33% ==> 53.33%
  //   average power: 26.66% + 53.33% / 2 ==> 40%
  //
  // Actual calculation:
  //   Bias factor = 2 / (Btop/Bmax + Bbottom/Bmax) = 2 * Bmax / (Btop + Bbottom)
  biasFactor = (float) 2 * maxBias / (bias[TYPE_BOTTOM_ELEMENT] + bias[TYPE_TOP_ELEMENT]);

  totalBasePower = basePower + insulationPower + risePower;
  SerialUSB.println("Base PID power at "+String(temperature)+"C:  B="+String(basePower)+" I="+String(insulationPower)+" R="+String(risePower)+" Total="+String(totalBasePower)+" bias="+String(biasFactor));

  // Put it all together
  totalBasePower = totalBasePower * biasFactor;
  return totalBasePower < 100? totalBasePower : 100;
}


// Draw the outline of the graph, including axis, labels and dividers
void drawGraphOutline(uint16_t graphMaxTemp, uint16_t graphMaxSeconds)
{
  // Y-axis (temperature)
  tft.drawFastVLine(GRAPH_LEFT, GRAPH_TOP, GRAPH_HEIGHT, BLACK);
  tft.drawFastVLine(GRAPH_LEFT+1, GRAPH_TOP, GRAPH_HEIGHT, BLACK);
  tft.drawFastVLine(GRAPH_LEFT + GRAPH_WIDTH / 2, GRAPH_TOP, GRAPH_HEIGHT, BLUE);
  tft.drawFastVLine(GRAPH_LEFT + GRAPH_WIDTH, GRAPH_TOP, GRAPH_HEIGHT, BLUE);
  sprintf(buffer100Bytes, "%d", graphMaxTemp);
  displayString(0, GRAPH_TOP - 5, FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
  displayString(11, GRAPH_TOP -12 + GRAPH_HEIGHT / 2, FONT_9PT_BLACK_ON_WHITE, (char *) "~C");
  displayString(28, GRAPH_TOP + GRAPH_HEIGHT - 7, FONT_9PT_BLACK_ON_WHITE, (char *) "0");

  // X-axis (time)
  tft.drawFastHLine(GRAPH_LEFT, GRAPH_TOP, GRAPH_WIDTH, BLUE);
  tft.drawFastHLine(GRAPH_LEFT, GRAPH_TOP + GRAPH_HEIGHT / 2, GRAPH_WIDTH, BLUE);
  tft.drawFastHLine(GRAPH_LEFT, GRAPH_TOP + GRAPH_HEIGHT, GRAPH_WIDTH + 1, BLACK);
  tft.drawFastHLine(GRAPH_LEFT, GRAPH_TOP + GRAPH_HEIGHT + 1, GRAPH_WIDTH + 1, BLACK);
  displayString(GRAPH_LEFT - 40 + GRAPH_WIDTH / 2, 300, FONT_9PT_BLACK_ON_WHITE, (char *) "seconds");
  sprintf(buffer100Bytes, "%d", graphMaxSeconds);
  displayString(GRAPH_LEFT - 40 + GRAPH_WIDTH, 300, FONT_9PT_BLACK_ON_WHITE, buffer100Bytes);
}

