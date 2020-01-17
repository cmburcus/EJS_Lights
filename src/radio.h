#ifndef RADIO_H
#define RADIO_H

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include <constants.h>
#include <race.h>

// Define communication
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "P_EJS"; // Primary EJS

/**
 * Initializes the radio to be able to listen
 */
void initializeRadio() {
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

void handleRadioSignals() {
  char text[32] = "";

  if (radio.available()) {
    radio.read(&text, sizeof(text));
  }

  // Handle reset signal
  if (strcmp(text, RESET_RACE_SIGNAL) == 0) {
    resetRace();
  }
  
  // Handle start race signal
  if (strcmp(text, START_RACE_SIGNAL) == 0) {
    startRace();
  }

  // Handle fault
  if (strcmp(text, FAULT_SIGNAL) == 0) {
    handleFault();
  }
}

#endif