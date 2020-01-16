#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>

#include <constants.h>

/**
 * Initializes the Arduino pins
 */
void initializePins() {
  pinMode(LED_FAULT, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(LED_SET, OUTPUT);
  pinMode(LED_GO, OUTPUT);
}

/**
 * This function is to be used when the system
 * boots up. We will toggle through all the LEDs
 * as a way to ensure they work. We will use
 * delay() as we don't want the code to reach
 * the loop until this is completed
 */
void initializeSystem() {
  delay(SEQUENCE_DELAY);
  digitalWrite(LED_GO, HIGH);
  delay(300);
  digitalWrite(LED_GO, LOW);
  digitalWrite(LED_SET, HIGH);
  delay(300);
  digitalWrite(LED_SET, LOW);
  digitalWrite(LED_READY, HIGH);
  delay(300);
  digitalWrite(LED_READY, LOW);
  digitalWrite(LED_FAULT, HIGH);
  delay(300);
  digitalWrite(LED_FAULT, LOW);
  delay(SEQUENCE_DELAY);
  digitalWrite(LED_FAULT, HIGH);
}

/**
 * Turns all LEDs off
 */
void turnOffAllLed() {
  digitalWrite(LED_FAULT, LOW);
  digitalWrite(LED_READY, LOW);
  digitalWrite(LED_SET, LOW);
  digitalWrite(LED_GO, LOW);
}

/**
 * Returns true if timeValue is between the startTime (inclusive) and
 * the endTime
 */
bool isMillisBetween(long timeValue, long startTime, long endTime) {
  return timeValue >= startTime && timeValue < endTime;
}

#endif
