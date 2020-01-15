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
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define communication
RF24 radio(7, 8); // CSN, CE
const byte address[6] = "P_EJS"; // Primary EJS
#define RACE_START_SIGNAL 8
#define RACE_FAULT_SIGNAL 9

// Define LEDs
#define LED_FAULT 5
#define LED_READY 4
#define LED_SET 3
#define LED_GO 2

// Define intervals
#define PRE_START_LED_DELAY 300
#define START_LED_DELAY 1000
#define SEQUENCE_DELAY 2000

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

// Comminucation constants
#define START_RACE_SIGNAL "START"
#define RESET_RACE_SIGNAL "RESET"
#define FAULT_SIGNAL "FAULT"

// Declare functions
void initializeSystem();
void resetRace();
void startRace();
void handlePreRaceSequence();
void handleRace();
void handleFault();
bool isMillisBetween(long, long, long);
void turnOffAllLed();

void setup() {
  Serial.begin(9600);
  
  // Define pin modes
  pinMode(RACE_START_SIGNAL, INPUT);
  pinMode(LED_FAULT, OUTPUT);
  pinMode(LED_READY, OUTPUT);
  pinMode(LED_SET, OUTPUT);
  pinMode(LED_GO, OUTPUT);

  turnOffAllLed();
  initializeSystem();

  // Begin listening
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  char text[32] = "";
  
  // Read communication
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

  // Handle pre-race and race light sequence
  if (isPreRaceSequenceStarted || isRaceSequenceStarted) {
    handlePreRaceSequence();
  }
  
  // Handle race actual start
  if (isRaceStarted) {
    handleRace();
  }
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
  turnOffAllLed();
  delay(1000);
  digitalWrite(LED_FAULT, HIGH);
}

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
    digitalWrite(LED_FAULT, isMillisBetween(timePassed, startStepOneTime, startStepTwoTime) ? HIGH : LOW);
    digitalWrite(LED_READY, isMillisBetween(timePassed, startStepTwoTime, startStepThreeTime) ? HIGH : LOW);
    digitalWrite(LED_SET, isMillisBetween(timePassed, startStepThreeTime, startStepFourTime) ? HIGH : LOW);
    
    // Green LED will be triggered by the start race
    // Yellow LED will be turned off at the same time
  }
}

void handleRace() {
  if (digitalRead(LED_GO) == LOW) {
    digitalWrite(LED_SET, LOW);
    digitalWrite(LED_GO, HIGH);
  }
}

void handleFault() {
  // If race is not about to start, nothing to do
  if (!isRaceSequenceStarted || !isRaceStarted) {
    return;
  }

  faults++;

  if (digitalRead(LED_FAULT) == LOW) {
    digitalWrite(LED_FAULT, HIGH);
  }
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