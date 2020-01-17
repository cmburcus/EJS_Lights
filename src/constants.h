#ifndef CONSTANTS_H
#define CONSTANTS_H

// Define pins
#define LED_FAULT 5
#define LED_READY 4
#define LED_SET 3
#define LED_GO 2

#define CE_PIN 7
#define CSN_PIN 8

// Define intervals
#define PRE_START_LED_DELAY 300
#define START_LED_DELAY 1000
#define SEQUENCE_DELAY 2000

// Comminucation constants
#define START_RACE_SIGNAL "START"
#define RESET_RACE_SIGNAL "RESET"
#define FAULT_SIGNAL "FAULT"

#endif
