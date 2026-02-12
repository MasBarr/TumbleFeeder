/*
  BasicFeeding Example
  
  Demonstrates basic usage of the TumbleFeeder library.
  Sets up a simple FR1 schedule with capacitive touch inputs.
  
  Hardware:
  - Servo on pin 10
  - LED on pin 13
  - Left touch input on A2
  - Right touch input on A0
  - Feeder touch input on A1
*/

#include <TumbleFeeder.h>

// Create feeder object
TumbleFeeder feeder;

// Interrupt flags
volatile bool leftFlag = false;
volatile bool rightFlag = false;
volatile bool feedFlag = false;

void setup() {
  Serial.begin(9600);
  
  // Initialize feeder with servo positions
  // closedPosition = 150, openPosition = 0
  feeder.begin(150, 0);
  
  // Set FR schedule
  feeder.setFR(1);  // FR1
  
  // Set how long feeder stays open (seconds)
  feeder.setOpenDuration(60);
  
  // Register callbacks
  feeder.setLogCallback(logEvent);
  feeder.setDisplayCallback(updateDisplay);
  
  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(A2), leftISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(A0), rightISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(A1), feedISR, FALLING);
  
  Serial.println("TumbleFeeder Ready!");
  Serial.println("FR1 Schedule Active");
}

void loop() {
  // Check inputs every loop
  // Pass open duration for this check (can override default)
  feeder.checkInputs(60);
  
  delay(100);  // Small delay to prevent overwhelming the system
}

// Interrupt Service Routines
void leftISR() {
  feeder.handleLeftTouch();
}

void rightISR() {
  feeder.handleRightTouch();
}

void feedISR() {
  feeder.handleFeederTouch();
}

// Callback: Called whenever an event needs to be logged
void logEvent() {
  Serial.print("Left: ");
  Serial.print(feeder.getLeftPokeCount());
  Serial.print(" | Right: ");
  Serial.print(feeder.getRightPokeCount());
  Serial.print(" | Feeder: ");
  Serial.println(feeder.getFeederCount());
}

// Callback: Called whenever display should update
void updateDisplay() {
  // Add your display update code here
  // This is called whenever counts change
}
