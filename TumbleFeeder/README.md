# TumbleFeeder Library

Arduino library for controlling a servo-based pellet feeder with capacitive touch inputs, designed for behavioral neuroscience experiments.

## Features

- **Servo Gate Control**: Smooth open/close movements with configurable positions
- **Three Touch Inputs**: Left poke, right poke, and feeder access detection
- **FR Schedules**: Fixed-ratio reinforcement schedules (FR1, FR5, etc.)
- **Touch Duration Tracking**: Automatic measurement of poke durations
- **Event Callbacks**: Hooks for data logging and display updates
- **Multiple Modes**: FR-based and free-feeding modes
- **Low Power**: Servo only powers during movement

## Hardware Requirements

**Hardcoded Pin Configuration:**
- Pin 10: Servo motor
- Pin 13: LED indicator
- Pin A2: Left touch input (capacitive)
- Pin A0: Right touch input (capacitive)
- Pin A1: Feeder access touch input (capacitive)

**External Components:**
- Servo motor (standard hobby servo)
- Capacitive touch sensors (3x)
- LED (optional, for status indication)

## Installation

### Arduino IDE
1. Download this repository as ZIP
2. In Arduino IDE: Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP file
4. Restart Arduino IDE

### Manual Installation
1. Copy the `TumbleFeeder` folder to your Arduino `libraries` folder
2. Restart Arduino IDE

## Quick Start

```cpp
#include <TumbleFeeder.h>

TumbleFeeder feeder;

void setup() {
  // Initialize with servo positions (closed=150, open=0)
  feeder.begin(150, 0);
  
  // Set FR1 schedule
  feeder.setFR(1);
  
  // Set 60 second open duration
  feeder.setOpenDuration(60);
  
  // Attach interrupts for touch inputs
  attachInterrupt(digitalPinToInterrupt(A2), leftISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(A0), rightISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(A1), feedISR, FALLING);
}

void loop() {
  feeder.checkInputs(60);  // Check all inputs
  delay(100);
}

// Interrupt handlers
void leftISR() { feeder.handleLeftTouch(); }
void rightISR() { feeder.handleRightTouch(); }
void feedISR() { feeder.handleFeederTouch(); }
```

## API Reference

### Initialization

#### `begin(int closedPosition, int openPosition)`
Initialize the feeder with servo positions.
- `closedPosition`: Servo angle when gate is closed (default: 150)
- `openPosition`: Servo angle when gate is open (default: 0)

### Configuration

#### `setFR(int fr)`
Set the fixed-ratio schedule. Feeder opens every `fr` left pokes.
- `fr`: Fixed ratio value (1 for FR1, 5 for FR5, etc.)

#### `setOpenDuration(unsigned long seconds)`
Set how long the feeder gate stays open after reward delivery.
- `seconds`: Duration in seconds

#### `setPositions(int openPos, int closedPos)`
Change servo positions after initialization.

### Callbacks

#### `setLogCallback(void (*callback)())`
Register a function to be called when events should be logged.

#### `setDisplayCallback(void (*callback)())`
Register a function to be called when display should update.

Example:
```cpp
void logEvent() {
  Serial.println("Event logged!");
}

void setup() {
  feeder.setLogCallback(logEvent);
}
```

### Feeder Control

#### `feederOpen()`
Manually open the feeder gate.

#### `feederClose()`
Manually close the feeder gate.

#### `shakeFood()`
Close then open gate to shake/settle food. Triggers log callback.

#### `shake()`
Flash LED 10 times (visual indicator).

### Input Handling

#### `checkInputs(unsigned long openDuration)`
Check all three inputs (left, right, feeder) in FR mode.
Opens feeder when FR requirement met.

#### `freeInputs()`
Check inputs in free-feeding mode (no FR requirement, gate doesn't open).

#### `checkLeft(unsigned long openDuration)`
Check only left poke input.

#### `checkRight()`
Check only right poke input.

#### `checkFeeder()`
Check only feeder access input.

### ISR Handlers

Call these from your interrupt service routines:

#### `handleLeftTouch()`
Set internal flag for left poke detection.

#### `handleRightTouch()`
Set internal flag for right poke detection.

#### `handleFeederTouch()`
Set internal flag for feeder access detection.

### Data Access

#### `getLeftPokeCount()`
Returns total left pokes.

#### `getRightPokeCount()`
Returns total right pokes.

#### `getFeederCount()`
Returns total feeder accesses.

#### `getLeftPokeDur()`
Returns duration of last left poke (milliseconds).

#### `getRightPokeDur()`
Returns duration of last right poke (milliseconds).

#### `getLeftFeederDur()`
Returns duration of last feeder access (milliseconds).

#### `getFR()`
Returns current FR value.

#### `resetCounts()`
Reset all counters to zero.

### Public Variables

Direct access to counters (for backward compatibility):
- `leftPokeCount`
- `rightPokeCount`
- `FeederCount`
- `leftPokeDur`
- `rightPokeDur`
- `leftFeederDur`

## Examples

### Basic FR1 Schedule
See `examples/BasicFeeding/BasicFeeding.ino`

### Advanced Features
See `examples/AdvancedFeeding/AdvancedFeeding.ino`
- FR5 schedule
- Mode switching
- Periodic shake food
- Serial commands

## Integration with Existing Code

To convert existing CastleCode sketches:

1. **Replace global variables** with feeder object:
```cpp
// Old:
int leftPokeCount = 0;

// New:
TumbleFeeder feeder;
// Access via: feeder.leftPokeCount or feeder.getLeftPokeCount()
```

2. **Replace function calls**:
```cpp
// Old:
feeder_open(openpos);

// New:
feeder.feederOpen();
```

3. **Update ISRs**:
```cpp
// Old:
void left_interrupt() {
  left_touch = 1;
}

// New:
void left_interrupt() {
  feeder.handleLeftTouch();
}
```

4. **Register callbacks**:
```cpp
void setup() {
  feeder.setLogCallback(logData);
  feeder.setDisplayCallback(update_display);
}
```

## License

MIT License - free to use and modify for research purposes.

## Author

Mason Barrett, Kravitz Lab - Behavioral Neuroscience Research

## Contributing

Issues and pull requests welcome on GitHub.
