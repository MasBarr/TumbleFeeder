/*
  TumbleFeeder.h - Library for servo-based pellet feeder control
  Designed for behavioral neuroscience experiments with capacitive touch inputs
  
  Hardware configuration (hardcoded):
  - Servo: Pin 10
  - LED indicator: Pin 13
  - Left touch input: A2
  - Right touch input: A0
  - Feeder touch input: A1
*/

#ifndef TumbleFeeder_h
#define TumbleFeeder_h

#include "Arduino.h"
#include <Servo.h>

class TumbleFeeder {
  public:
    // Constructor
    TumbleFeeder();
    
    // Initialization
    void begin(int closedPosition = 150, int openPosition = 0);
    
    // Configuration
    void setFR(int fr);
    void setOpenDuration(unsigned long seconds);
    void setPositions(int openPos, int closedPos);
    
    // Callback registration
    void setLogCallback(void (*callback)());
    void setDisplayCallback(void (*callback)());
    
    // Core feeder control
    void feederOpen();
    void feederClose();
    void shakeFood();
    void shake();
    
    // Touch handling (call these from main loop)
    void checkLeft(unsigned long openDuration);
    void checkRight();
    void checkFeeder();
    void checkInputs(unsigned long openDuration);  // Checks all three
    void freeInputs();  // Free feeding mode (no FR requirement)
    
    // Touch flag setters (call these from ISRs)
    void handleLeftTouch();
    void handleRightTouch();
    void handleFeederTouch();
    
    // Getters for counters and durations
    int getLeftPokeCount();
    int getRightPokeCount();
    int getFeederCount();
    int getLeftPokeDur();
    int getRightPokeDur();
    int getLeftFeederDur();
    int getFR();
    
    // Reset counters
    void resetCounts();
    
    // Public variables (for compatibility with existing code)
    int leftPokeCount;
    int rightPokeCount;
    int FeederCount;
    int leftPokeDur;
    int rightPokeDur;
    int leftFeederDur;
    
  private:
    // Hardware pins (hardcoded)
    static const int SERVO_PIN = 10;
    static const int LED_PIN = 13;
    static const int LEFT_TOUCH_PIN = A2;
    static const int RIGHT_TOUCH_PIN = A0;
    static const int FEEDER_TOUCH_PIN = A1;
    
    // Servo object
    Servo _servo;
    
    // Servo positions
    int _closedPos;
    int _openPos;
    
    // Fixed ratio schedule
    int _FR;
    
    // Touch flags
    volatile bool _leftTouch;
    volatile bool _rightTouch;
    volatile bool _feedTouch;
    
    // Open duration (seconds)
    unsigned long _openDuration;
    
    // Callbacks
    void (*_logCallback)();
    void (*_displayCallback)();
    
    // Helper methods
    void _callLogCallback();
    void _callDisplayCallback();
    void _readTouchPin(int pin, unsigned long &startTime, int &count, int &duration);
};

#endif
