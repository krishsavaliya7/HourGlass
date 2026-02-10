# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

A smart LED hourglass device built with Arduino Nano featuring 4 operating modes, Web UI control, and MPU6050 orientation sensing. The system consists of:
- **Firmware**: Arduino C++ code for hardware control
- **Web UI**: Browser-based control panel using Web Serial API

## Hardware Architecture

| Component | Specification |
|-----------|--------------|
| Microcontroller | Arduino Nano R3 |
| LED Matrix | 2x MAX7219 8x8 LED Matrix (daisy-chained) |
| Gyroscope | MPU6050 (GY-521) on I2C (A4/A5) |
| Button | Active-Low on D2 |
| Buzzer | 5V on D3 |

**Pin Mapping**:
- D11: MAX7219 DIN (MOSI)
- D13: MAX7219 CLK (SCK)
- D10: MAX7219 CS
- D2: Button (Active-Low, internal pull-up)
- D3: Buzzer
- A4: MPU6050 SDA
- A5: MPU6050 SCL

## Firmware Development

### File Structure

```
Firmware/rishi-file/
├── rishi-file.ino    # Main firmware with loop(), setup(), mode handlers
├── config.h          # Pin definitions, thresholds, timing constants
├── utils.h           # Utility functions (normalizeAngle)
├── LedControl.h/.cpp # MAX7219 LED matrix driver with rotation support
├── DiceMode.h/.cpp   # Dice mode class with shake detection
└── Delay.h/.cpp      # Non-blocking delay utility
```

### Key Mode Constants

```cpp
#define MODE_HOURGLASS 0
#define MODE_CLOCK 1
#define MODE_DICE 2
#define MODE_FLIPCOUNTER 3
```

### Arduino Upload Commands

Open `Firmware/rishi-file/rishi-file.ino` in Arduino IDE:
1. Board: Arduino Nano
2. Processor: ATmega328P (Old Bootloader)
3. Select correct COM port
4. Upload

### Serial Command Protocol (Firmware)

The firmware responds to simple text commands over USB Serial at 9600 baud:

**Mode Control**:
```
SET_MODE hourglass      → OK MODE HOURGLASS
SET_MODE clock          → OK MODE CLOCK
SET_MODE dice           → OK MODE DICE
SET_MODE flipcounter    → OK MODE FLIPCOUNTER
```

**Clock**:
```
SET_TIME <hours> <minutes>  → OK TIME HH:MM
```

**Hourglass**:
```
SET_HG <hours> <minutes>    → OK TIMER SET TO X MIN
RESET_HG                     → OK HOURGLASS RESET
```

**Dice**:
```
ROLL_DICE                    → {"value":7,"diceValue":3,"diceA":3,"diceB":4}
```

**Flip Counter**:
```
RESET_FLIP                   → OK FLIP COUNTER RESET
GET_FLIP_COUNT               → {"count":5}
```

**Status**:
```
GET_STATUS                   → {"mode":"hourglass","time":"11:31","orientation":0,...}
GET_ORIENTATION              → {"angle":0}
GET_DISPLAY                  → {"matrixA":"3C7EE7C3C3E77E3C","matrixB":"0018180000000000"}
GET_ALL                      → Combined status + display + orientation
SET_BRIGHTNESS <0-15>        → OK BRIGHTNESS SET
```

### Important Firmware Patterns

**Orientation Handling**: MPU6050 returns angles normalized to 0°, 90°, 180°, 270°. Gravity affects hourglass particle flow direction.

**Display Rotation**: `LedControl.setRotation()` handles virtual matrix rotation. Combined with `ROTATION_OFFSET` (90°), displays stay upright when device rotates.

**LED Matrix Indexing**:
- `lc.setLed(addr, row, col, state)` - Standard LED matrix access
- `lc.setXY(addr, x, y, state)` - With rotation transform applied
- `lc.getRawXY(addr, x, y)` - Raw access without rotation

**Particle System**: Hourglass uses `TOTAL_PARTICLES` (60) with `updateMatrix()` moving particles diagonally through matrix coordinate space.

**Mode Entry/Exit**: Always call `diceMode.enter()` and `diceMode.exit()` when entering/leaving dice mode to handle shake detection.

## Web UI Development

### File Structure

```
web-ui/
├── index.html        # Main UI
├── css/styles.css    # Glassmorphism styling
└── js/
    ├── app.js        # Main application class (connect, auto-refresh)
    ├── serial.js     # Web Serial API wrapper (SerialConnection class)
    ├── api.js        # Serial command API with priority queue
    ├── display.js    # LED matrix visualization (Display class)
    └── modes.js      # Mode-specific controls (ModeManager class)
```

### Running the Web UI

```bash
# Python (installed on most systems)
cd web-ui
python -m http.server 8080

# Or Node.js
npx http-server -p 8080 -c-1
```

Then open `http://localhost:8080` in Chrome or Edge (required for Web Serial API).

### JavaScript Architecture

**SerialConnection** (`serial.js`):
- Wraps Web Serial API
- Baud rate: 9600
- Line-buffered reads (splits on `\n`)
- Emits data via `onData()` callback

**API** (`api.js`):
- Command queue with priority (SET/RESET/ROLL are high priority)
- 3-second timeout for responses
- JSON parsing for GET_* commands
- Dispatches `statusUpdate` event on JSON responses

**Display** (`display.js`):
- Renders 8x8 LED matrices as HTML grids
- Orientation smoothing with 5-sample history
- Snaps to cardinal angles (0°, 90°, 180°, 270°)
- Caches matrix state to minimize DOM updates

**ModeManager** (`modes.js`):
- Switches modes via UI buttons
- Syncs with hardware mode changes
- Handles mode-specific controls

**App** (`app.js`):
- Auto-refresh at 100ms intervals (10 FPS)
- Data change detection to skip redundant updates
- Connection status management

### Web UI Serial Communication

The Web UI sends commands using `api.sendCommand()` and receives responses via the `statusUpdate` event:

```javascript
// Send command
await api.setMode('dice');

// Listen for hardware status changes
window.addEventListener('statusUpdate', (e) => {
    const data = e.detail; // {mode: 'dice', diceValue: 3, ...}
});
```

## Display Data Format

Matrix data is transmitted as 16-character hex strings (8 bytes, 2 hex chars each):

```json
{
  "matrixA": "3C7EE7C3C3E77E3C",  // 8 bytes = 64 LEDs
  "matrixB": "0018180000000000"
}
```

Each byte represents one row (8 columns). Bit 7 = col 0, Bit 0 = col 7.

## Mode-Specific Behavior

### Hourglass (MODE_HOURGLASS)
- Particles flow from `getTopMatrix()` to `getBottomMatrix()` based on gravity
- Top matrix changes based on `currentGravity` (0° vs 90°)
- 3-beep alarm when `countParticles(top) == 0`

### Clock (MODE_CLOCK)
- Static display (updates only when time/orientation changes to prevent flicker)
- 0°/180°: Dot patterns for hours (top) / minutes (bottom)
- 90°/270°: Two-digit numbers on both matrices
- Time increments every 60 seconds

### Dice (MODE_DICE)
- Shake detection on Y-axis accelerometer
- Button press: Single dice (both matrices show same value)
- Shake: Two dice (different values on each matrix)
- Uses `DiceMode` class with animation

### Flip Counter (MODE_FLIPCOUNTER)
- Counts 180° flips between 0° and 180° orientations
- 500ms debounce (`FLIP_DEBOUNCE_MS`)
- Display shows count on top matrix at 0°, bottom at 180°

## Key Constants (config.h)

| Constant | Value | Purpose |
|----------|-------|---------|
| `ROTATION_OFFSET` | 90 | Aligns virtual rotation with physical |
| `DELAY_FRAME` | 80ms | Main loop timing |
| `LONG_PRESS_TIME` | 1500ms | Mode switch button press |
| `FLIP_DEBOUNCE_MS` | 500 | Minimum time between flips |
| `TOTAL_PARTICLES` | 60 | Hourglass particle count |
| `MPU6050_ADDR` | 0x68 | I2C gyroscope address |
| `SERIAL_BAUD` | 9600 | Serial baud rate |

## Common Tasks

### Add a new serial command to firmware

1. Add command parsing in `handleSerialCommands()` in `rishi-file.ino`
2. Send response via `Serial.println()` (plain text) or JSON
3. For JSON, follow existing patterns: `{"key":value}`

### Add Web UI control

1. Add button/element to `index.html`
2. Add handler in `ModeManager` (`initEventListeners()`)
3. Call API method (or add new one in `api.js`)
4. Add firmware command if needed

### Modify display patterns

- Clock digits: `digit[10][8]` and `smallDigit[10][3]` arrays in `rishi-file.ino`
- Dice patterns: `DiceMode::drawDicePattern()` in `DiceMode.cpp`
- Custom patterns: Use `lc.setLed(addr, row, col, true/false)`