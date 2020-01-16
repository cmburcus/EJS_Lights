#ifndef RACE_H
#define RACE_H

#include <Arduino.h>

#include <constants.h>
#include <helpers.h>

// Race specific
bool isPreRaceSequenceStarted = false;
bool isRaceSequenceStarted = false;
bool isRaceStarted = false;
unsigned long raceSequenceStartTime = 0;
unsigned long raceStartTime = 0;
int faults = 0;

// Race pre-start sequence timing
const long preStartStepOneTime = SEQUENCE_DELAY + PRE_START_LED_DELAY;
const long preStartStepTwoTime = preStartStepOneTime + PRE_START_LED_DELAY;
const long preStartStepThreeTime = preStartStepTwoTime + PRE_START_LED_DELAY;
const long preStartStepFourTime = preStartStepThreeTime + PRE_START_LED_DELAY;
const long preStartCompletedTime = preStartStepFourTime + PRE_START_LED_DELAY;

// Race start sequence timing
const long startStepOneTime = preStartCompletedTime + START_LED_DELAY;
const long startStepTwoTime = startStepOneTime + START_LED_DELAY;
const long startStepThreeTime = startStepTwoTime + START_LED_DELAY;
const long startStepFourTime = startStepThreeTime + START_LED_DELAY;

/**
 *  Sets the state of the lights and variables for race start
 */
void startRace() {
  // If race already started, nothing to do
  if (isPreRaceSequenceStarted || isRaceSequenceStarted || isRaceStarted) {
    return;
  }

  // Set race start conditions
  isPreRaceSequenceStarted = true;
  raceSequenceStartTime = millis();
  digitalWrite(LED_FAULT, LOW);
}

/**
 * Begins the race start sequence
 */
void handlePreRaceSequence() {
  long timePassed = millis() - raceSequenceStartTime;

  // If pre-start sequence completed, move on to the start sequence
  if (timePassed >= preStartCompletedTime) {
    isPreRaceSequenceStarted = false;
    isRaceSequenceStarted = true;
  }

  // We want to finish the start sequence with the GREEN 
  if (timePassed >= startStepFourTime) {
    isRaceSequenceStarted = false;
    isRaceStarted = true;
  }

  if (isPreRaceSequenceStarted) {
    digitalWrite(LED_GO, isMillisBetween(timePassed, preStartStepOneTime, preStartStepTwoTime) ? HIGH : LOW);
    digitalWrite(LED_SET, isMillisBetween(timePassed, preStartStepTwoTime, preStartStepThreeTime) ? HIGH : LOW);
    digitalWrite(LED_READY, isMillisBetween(timePassed, preStartStepThreeTime, preStartStepFourTime) ? HIGH : LOW);
    digitalWrite(LED_FAULT, isMillisBetween(timePassed, preStartStepFourTime, preStartCompletedTime) ? HIGH : LOW);
  }

  if (isRaceSequenceStarted) {
    // If a fault has been commited, ignore red LED
    if (faults == 0) {
      digitalWrite(LED_FAULT, isMillisBetween(timePassed, startStepOneTime, startStepTwoTime) ? HIGH : LOW);
    }
    
    digitalWrite(LED_READY, isMillisBetween(timePassed, startStepTwoTime, startStepThreeTime) ? HIGH : LOW);
    digitalWrite(LED_SET, isMillisBetween(timePassed, startStepThreeTime, startStepFourTime) ? HIGH : LOW);
    
    // Green LED will be triggered by the start race
    // Yellow LED will be turned off at the same time
  }
}

/**
 * Actual race start. Sets the start time and
 * turns on green LED
 */
void handleRace() {
  if (digitalRead(LED_GO) == LOW) {
    digitalWrite(LED_SET, LOW);
    digitalWrite(LED_GO, HIGH);
    raceStartTime = millis();
  }
}

/**
 * Handles the event of a fault
 */
void handleFault() {
  // If race is not about to start, nothing to do
  if (!isRaceSequenceStarted && !isRaceStarted) {
    return;
  }

  faults++;

  if (digitalRead(LED_FAULT) == LOW) {
    digitalWrite(LED_FAULT, HIGH);
  }
}

/**
 * Resets the state of the lights to standby
 */
void resetRace() {
  // If the race is not started, nothing to do
  if (!isPreRaceSequenceStarted && !isRaceSequenceStarted && !isRaceStarted) {
    return;
  }

  // Reset conditions
  isPreRaceSequenceStarted = false;
  isRaceSequenceStarted = false;
  isRaceStarted = false;
  faults = 0;
  raceStartTime = 0;
  raceSequenceStartTime = 0;
  turnOffAllLed();
  delay(1000);
  digitalWrite(LED_FAULT, HIGH);
}

#endif
