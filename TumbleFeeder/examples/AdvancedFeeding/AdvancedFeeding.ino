/*
  AdvancedFeeding Example
  
  Demonstrates advanced features:
  - FR5 schedule
  - Mode switching between FR and free feeding
  - Custom servo positions
  - Periodic shake food
  
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

// Mode control
bool frMode = true;  // true = FR mode, false = free feeding
unsigned long lastShake = 0;
const unsigned long shakeInterval = 180000;  // Shake every 3 minutes

void setup() {
  Serial.begin(9600);
  
  // Initialize with custom servo positions
  feeder.begin(150, 0);  // closed = 150, open = 0
  
  // Set FR5 schedule
  feeder.setFR(5);
  
  // Set 30 second open duration
  feeder.setOpenDuration(30);
  
  // Register callbacks
  feeder.setLogCallback(logEvent);
  feeder.setDisplayCallback(updateDisplay);
  
  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(A2), leftISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(A0), rightISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(A1), feedISR, FALLING);
  
  Serial.println("Advanced TumbleFeeder Example");
  Serial.println("FR5 Schedule Active");
  Serial.println("Commands:");
  Serial.println("  'm' - Toggle mode (FR/Free)");
  Serial.println("  'r' - Reset counters");
  Serial.println("  's' - Manual shake");
}

void loop() {
  // Check for serial commands
  if (Serial.available()) {
    char cmd = Serial.read();
    handleCommand(cmd);
  }
  
  // Check inputs based on current mode
  if (frMode) {
    feeder.checkInputs(feeder.getFR());  // Use FR schedule
  } else {
    feeder.freeInputs();  // Free feeding mode
  }
  
  // Periodic shake food
  if (millis() - lastShake > shakeInterval) {
    Serial.println("Auto-shake food");
    feeder.shakeFood();
    lastShake = millis();
  }
  
  delay(100);
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

// Handle serial commands
void handleCommand(char cmd) {
  switch(cmd) {
    case 'm':
    case 'M':
      frMode = !frMode;
      Serial.print("Mode: ");
      Serial.println(frMode ? "FR Schedule" : "Free Feeding");
      break;
      
    case 'r':
    case 'R':
      feeder.resetCounts();
      Serial.println("Counters reset");
      break;
      
    case 's':
    case 'S':
      Serial.println("Manual shake");
      feeder.shakeFood();
      break;
      
    default:
      Serial.println("Unknown command");
  }
}

// Callback: Log events
void logEvent() {
  unsigned long timestamp = millis();
  
  Serial.print("[");
  Serial.print(timestamp);
  Serial.print("] ");
  
  Serial.print("Left: ");
  Serial.print(feeder.getLeftPokeCount());
  Serial.print(" (");
  Serial.print(feeder.getLeftPokeDur());
  Serial.print("ms) | ");
  
  Serial.print("Right: ");
  Serial.print(feeder.getRightPokeCount());
  Serial.print(" (");
  Serial.print(feeder.getRightPokeDur());
  Serial.print("ms) | ");
  
  Serial.print("Feed: ");
  Serial.print(feeder.getFeederCount());
  Serial.print(" (");
  Serial.print(feeder.getLeftFeederDur());
  Serial.println("ms)");
}

// Callback: Update display
void updateDisplay() {
  // Add display update code here
}
