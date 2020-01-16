/**
 * This piece of code will control ONE set of lights in a
 * flyball EJS system. To use a set of lights, simply use
 * this code with another setup.
 * 
 * A flick switch is used to determine if a setup is used
 * on the LEFT or RIGHT lane.
 * 
 * Communication to this setup is done wirelessly.
 */

#include <Arduino.h>

#include <constants.h>
#include <helpers.h>
#include <race.h>
#include <radio.h>

void setup() {
  Serial.begin(9600);
  
  initializePins();
  turnOffAllLed();
  initializeSystem();
  initializeRadio();
}

void loop() {
  handleRadioSignals();
  
  // Handle pre-race and race light sequence
  if (isPreRaceSequenceStarted || isRaceSequenceStarted) {
    handlePreRaceSequence();
  }
  
  // Handle race actual start
  if (isRaceStarted) {
    handleRace();
  }
}
