/*
  TumbleFeeder.cpp - Library for servo-based pellet feeder control
*/

#include "Arduino.h"
#include "TumbleFeeder.h"

// Constructor
TumbleFeeder::TumbleFeeder() {
  leftPokeCount = 0;
  rightPokeCount = 0;
  FeederCount = 0;
  leftPokeDur = 0;
  rightPokeDur = 0;
  leftFeederDur = 0;
  
  _leftTouch = false;
  _rightTouch = false;
  _feedTouch = false;
  
  _FR = 1;
  _openDuration = 60;
  
  _logCallback = nullptr;
  _displayCallback = nullptr;
}

// Initialize the feeder
void TumbleFeeder::begin(int closedPosition, int openPosition) {
  _closedPos = closedPosition;
  _openPos = openPosition;
  
  // Configure pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(LEFT_TOUCH_PIN, INPUT_PULLUP);
  pinMode(RIGHT_TOUCH_PIN, INPUT_PULLUP);
  pinMode(FEEDER_TOUCH_PIN, INPUT_PULLUP);
  
  digitalWrite(LED_PIN, LOW);
  
  // Don't attach servo yet - only attach when moving
}

// Configuration methods
void TumbleFeeder::setFR(int fr) {
  _FR = (fr == 0) ? 1 : fr;  // Ensure FR is never 0
}

void TumbleFeeder::setOpenDuration(unsigned long seconds) {
  _openDuration = seconds;
}

void TumbleFeeder::setPositions(int openPos, int closedPos) {
  _openPos = openPos;
  _closedPos = closedPos;
}

// Callback registration
void TumbleFeeder::setLogCallback(void (*callback)()) {
  _logCallback = callback;
}

void TumbleFeeder::setDisplayCallback(void (*callback)()) {
  _displayCallback = callback;
}

// Touch flag setters (called from ISRs)
void TumbleFeeder::handleLeftTouch() {
  _leftTouch = true;
}

void TumbleFeeder::handleRightTouch() {
  _rightTouch = true;
}

void TumbleFeeder::handleFeederTouch() {
  _feedTouch = true;
}

// Servo control methods
void TumbleFeeder::feederOpen() {
  digitalWrite(LED_PIN, HIGH);
  _servo.attach(SERVO_PIN);
  
  for (int pos = _closedPos; pos >= _openPos; pos -= 1) {
    _servo.write(pos);
    delay(50);
  }
  
  _servo.detach();
  digitalWrite(LED_PIN, LOW);
}

void TumbleFeeder::feederClose() {
  digitalWrite(LED_PIN, HIGH);
  _servo.attach(SERVO_PIN);
  
  for (int pos = _openPos; pos <= _closedPos; pos += 1) {
    _servo.write(pos);
    delay(50);
  }
  
  _servo.detach();
  digitalWrite(LED_PIN, LOW);
}

void TumbleFeeder::shake() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(50);
    digitalWrite(LED_PIN, LOW);
    delay(50);
  }
}

void TumbleFeeder::shakeFood() {
  feederClose();
  feederOpen();
  _callLogCallback();
}

// Helper method to read touch duration
void TumbleFeeder::_readTouchPin(int pin, unsigned long &startTime, int &count, int &duration) {
  startTime = millis();
  while (digitalRead(pin) == LOW) {
    delay(1);
  }
  duration = millis() - startTime;
  count++;
}

// Check left poke with FR schedule
void TumbleFeeder::checkLeft(unsigned long openDuration) {
  if (_leftTouch) {
    unsigned long startTime;
    _readTouchPin(LEFT_TOUCH_PIN, startTime, leftPokeCount, leftPokeDur);
    
    _callDisplayCallback();
    _callLogCallback();
    
    _leftTouch = false;
    leftPokeDur = 0;
    
    // Check if FR condition is met
    if (leftPokeCount % _FR == 0) {
      feederOpen();
      unsigned long feederStart = millis();
      
      // Keep feeder open for specified duration
      while (millis() - feederStart < (openDuration * 1000)) {
        checkFeeder();  // Check for feeder touches during open period
        // Note: Display updates during open period should be handled by main sketch
      }
      
      feederClose();
    }
  }
}

// Check right poke (no reward)
void TumbleFeeder::checkRight() {
  if (_rightTouch) {
    unsigned long startTime;
    _readTouchPin(RIGHT_TOUCH_PIN, startTime, rightPokeCount, rightPokeDur);
    
    _callDisplayCallback();
    _callLogCallback();
    
    _rightTouch = false;
    rightPokeDur = 0;
  }
}

// Check feeder access
void TumbleFeeder::checkFeeder() {
  if (_feedTouch) {
    unsigned long startTime;
    _readTouchPin(FEEDER_TOUCH_PIN, startTime, FeederCount, leftFeederDur);
    
    _callDisplayCallback();
    _callLogCallback();
    
    _feedTouch = false;
    leftFeederDur = 0;
  }
}

// Check all inputs (FR mode)
void TumbleFeeder::checkInputs(unsigned long openDuration) {
  checkRight();
  checkLeft(openDuration);
  checkFeeder();
}

// Free feeding mode (no FR requirement)
void TumbleFeeder::freeInputs() {
  checkRight();
  checkFeeder();
  
  // Left poke in free mode - no feeder opening
  if (_leftTouch) {
    unsigned long startTime;
    _readTouchPin(LEFT_TOUCH_PIN, startTime, leftPokeCount, leftPokeDur);
    
    _callDisplayCallback();
    _callLogCallback();
    
    _leftTouch = false;
    leftPokeDur = 0;
  }
}

// Getters
int TumbleFeeder::getLeftPokeCount() {
  return leftPokeCount;
}

int TumbleFeeder::getRightPokeCount() {
  return rightPokeCount;
}

int TumbleFeeder::getFeederCount() {
  return FeederCount;
}

int TumbleFeeder::getLeftPokeDur() {
  return leftPokeDur;
}

int TumbleFeeder::getRightPokeDur() {
  return rightPokeDur;
}

int TumbleFeeder::getLeftFeederDur() {
  return leftFeederDur;
}

int TumbleFeeder::getFR() {
  return _FR;
}

// Reset all counters
void TumbleFeeder::resetCounts() {
  leftPokeCount = 0;
  rightPokeCount = 0;
  FeederCount = 0;
  leftPokeDur = 0;
  rightPokeDur = 0;
  leftFeederDur = 0;
}

// Helper methods for callbacks
void TumbleFeeder::_callLogCallback() {
  if (_logCallback != nullptr) {
    _logCallback();
  }
}

void TumbleFeeder::_callDisplayCallback() {
  if (_displayCallback != nullptr) {
    _displayCallback();
  }
}
