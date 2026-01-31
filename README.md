# 🕐 HourGlass: Smart LED Display System

**A sophisticated multi-mode LED display system featuring clock, hourglass, dice, and flip counter modes with Web Serial UI control.**

---

## 📋 Table of Contents

1. [Project Overview](#project-overview)
2. [Features](#features)
3. [Hardware Requirements](#hardware-requirements)
4. [Quick Start](#quick-start)
5. [Installation](#installation)
6. [Usage Guide](#usage-guide)
7. [Web UI Setup](#web-ui-setup)
8. [Serial Commands API](#serial-commands-api)
9. [Configuration](#configuration)
10. [Hardware Details](#hardware-details)
11. [Troubleshooting](#troubleshooting)
12. [Development Notes](#development-notes)

---

## Project Overview

HourGlass is an Arduino-based project that combines multiple operational modes on dual 8×8 LED matrices controlled via MAX7219 LED drivers. The system features real-time mode switching, Web Serial API connectivity, and accelerometer-based orientation detection via MPU6050.

**Target Platform:** Arduino Nano R3 (ATmega328P, 2KB SRAM)

**Communication:** USB Serial @ 9600 baud (Web Serial API compatible)

---

## Features

### 🎯 Operational Modes

- **Clock Mode** - Real-time digital clock display (HH:MM format)
- **Hourglass Mode** - Animated sand falling animation with duration timer
- **Dice Mode** - Random number roller (1-6)
- **Flip Counter Mode** - Flip detection and counting (0-99)

### 🔧 Hardware Integration

- **Dual 8×8 LED Matrices** - 16×8 logical display (stacked vertically)
- **MAX7219 LED Drivers** - SPI-controlled brightness and multiplexing
- **MPU6050 IMU** - Accelerometer-based orientation and gesture detection
- **Push Button** - Hardware interrupt for mode cycling
- **Buzzer** - Audio feedback (piezo on Pin 13)

### 🌐 Connectivity

- **Web Serial API** - Browser-based control via Chrome/Edge/Opera 76+
- **JSON API** - Machine-readable status and sensor data
- **Command Protocol** - Text-based serial commands (SET_MODE, SET_TIME, etc.)

### ⚡ Optimization

- **SRAM Optimized** - Only 2KB available; uses fixed buffers and C strings
- **Non-blocking** - Responsive UI, no blocking delays
- **Power Efficient** - USB-powered (with brightness limits)

---

## Hardware Requirements

### Essential Components

| Component | Qty | Notes |
|-----------|-----|-------|
| Arduino Nano R3 | 1 | ATmega328P, 16MHz, 2KB SRAM |
| MAX7219 LED Driver | 2 | SPI-controlled multiplexer |
| 8×8 LED Matrix (Red) | 2 | Common cathode, ~300mA each at full brightness |
| MPU6050 Module | 1 | 6-axis IMU (I2C) |
| Push Button | 1 | Momentary, active-low |
| Piezo Buzzer | 1 | 5V logic-driven, ~80dB |
| 100µF Capacitor | 1 | Main power supply decoupling |
| 10µF Capacitors | 3 | Arduino and LED matrices (1 each) |
| 100nF Capacitors | 3 | I2C (1) and SPI matrices (1 each) |

### USB Power Limitation ⚠️

- **USB 2.0 provides:** 500mA maximum
- **System requires:** ~800mA at full LED brightness
- **Solution:** Reduce brightness (SET_BRIGHTNESS 0-8) when USB powered, OR use external 5V ≥800mA supply

---

## Quick Start

### 1. Hardware Assembly

**Wire the Arduino Nano R3 as follows:**

```
MAX7219 Display (SPI):
  DIN → Pin 5 (D1)
  CLK → Pin 4 (D2)
  CS/LOAD → Pin 10 (D3)  [Changed from Pin 0 to avoid serial RX conflict]

MPU6050 (I2C):
  SDA → Pin A4
  SCL → Pin A5
  VCC → 5V
  GND → GND

Controls:
  Button → Pin 2 (D4)
  Buzzer → Pin 13 (D7)

Power:
  GND → Common ground
  5V → VCC (USB or external supply)
```

**Capacitor Setup (Highly Recommended for Stability):**
- **100µF (Electrolytic):** Across main 5V/GND rails (near power source).
- **10µF (Electrolytic):** One across Arduino 5V/GND, and one across VCC/GND of **each** LED matrix.
- **100nF (Ceramic):** One across MPU-6050 VCC/GND, and one across VCC/GND of **each** LED matrix.
- *Note: Electrolytic capacitors must be oriented correctly (+ to 5V, - to GND).*

### 2. Arduino IDE Installation

1. **Install Arduino IDE** (v1.8.13 or later)
2. **Select Board:** Tools → Board → Arduino Nano
3. **Select Processor:** Tools → Processor → ATmega328P
4. **Select COM Port:** Tools → Port → (your COM port)
5. **Baud Rate:** Tools → Serial → 9600

### 3. Flash Firmware

1. Open `firmware/main/main.ino` in Arduino IDE
2. Verify code: Sketch → Verify
3. Upload: Sketch → Upload
4. Wait for "Done uploading" message

### 4. Test Connection

1. Open Serial Monitor: Tools → Serial Monitor
2. Set baud to 9600
3. Type command: `GET_STATUS`
4. Expected response: `{"mode":1}`

---

## Installation

### Option A: Arduino IDE (Desktop)

**Requirements:**
- Arduino IDE v1.8.13+
- USB cable (A to Mini-B)
- Drivers: CH340 or FT232 (automatic on most systems)

**Steps:**
1. Connect Arduino Nano R3 via USB
2. Wait for driver installation (~10 seconds)
3. Verify COM port appears in Device Manager
4. Open firmware/main/main.ino
5. Select Board: Arduino Nano, Processor: ATmega328P
6. Click Upload button
7. Watch for compilation and upload progress

### Option B: ArduinoDroid (Mobile)

**Requirements:**
- Android 5.0+
- USB OTG cable
- ArduinoDroid app (Google Play Store)

**Steps:**
1. Install ArduinoDroid on Android device
2. Connect Arduino via USB OTG cable
3. In ArduinoDroid: File → Open → select main.ino
4. Tap Verify (check icon)
5. Tap Upload (arrow icon)
6. Wait for "Upload successful" message

**Note:** Web UI requires a desktop/laptop browser; mobile can only use serial commands.

### Option C: Web UI (No Installation)

The Web UI automatically connects via Web Serial API:
1. Open `web-ui/index.html` in Chrome/Edge/Opera 76+
2. Click "🔌 Connect" button
3. Select Arduino COM port from dialog
4. Web UI displays live LED matrix and controls

---

## Usage Guide

### Mode Selection

**Method 1: Web UI**
- Click mode button (Clock, Hourglass, Dice, Flip Counter)
- Watch display update in real-time

**Method 2: Hardware Button**
- Short press: Cycle through modes
- Long press (2 sec): Trigger mode-specific action

**Method 3: Serial Command**
```
SET_MODE CLOCK
SET_MODE HOURGLASS
SET_MODE DICE
SET_MODE FLIPCOUNTER
```

### Clock Mode

**Display:** HH:MM (24-hour format)

**Set Time:**
```
SET_TIME 14 30
```
(Sets clock to 14:30)

**Manual Control:**
- Web UI: Click hour/minute spinners
- Arrow keys: ↑/↓ adjust displayed time

### Hourglass Mode

**Display:** Animated sand falling + timer

**Set Duration:**
```
SET_HG 0 30
```
(Sets 30-minute timer)

**Controls:**
- Button press: Start/pause sand animation
- Timer counts down visually on LED matrix
- Buzzer alarm when timer expires

**Reset Timer:**
```
RESET_HG
```

### Dice Mode

**Display:** Random number 1-6 on matrix

**Roll Dice:**
```
ROLL_DICE
```

**Controls:**
- Button press: Roll random number
- Web UI: Click "🎲 Roll" button
- Auto-roll on device flip (MPU6050 detection)

**Get Current Value:**
```
GET_STATUS
```

### Flip Counter Mode

**Display:** Count on dual matrices (tens/ones digits)

**Auto-Detection:**
- Device detects flip motions via MPU6050
- Counter increments automatically

**Manual Commands:**
```
RESET_FLIP         # Reset counter to 0
GET_FLIP_COUNT     # Query current count
```

**Display Limit:** Shows 0-99 (capped to prevent overflow)

---

## Web UI Setup

### Browser Compatibility

| Browser | Support | Min Version | Notes |
|---------|---------|-------------|-------|
| Chrome | ✅ Full | 89+ | Recommended; full Web Serial support |
| Edge | ✅ Full | 89+ | Based on Chromium; full support |
| Opera | ✅ Full | 76+ | Web Serial support added in v76 |
| Firefox | ❌ Not Supported | - | No Web Serial API |
| Safari | ❌ Not Supported | - | No Web Serial API |

### Setup Steps

1. **Open Web UI**
   - Local: `file:///path/to/web-ui/index.html`
   - OR use simple HTTP server: `python -m http.server 8000`

2. **Allow Permissions**
   - Browser will request Serial API permission
   - Click "Allow" when prompted

3. **Connect to Arduino**
   - Click "🔌 Connect" button
   - Browser opens port selection dialog
   - Select Arduino Nano COM port
   - Connection established (status shows "✅ Connected")

4. **Use Controls**
   - Mode buttons: Switch operational modes
   - LED matrix display: Shows live visualization
   - Sensor readout: Displays MPU6050 angle and flip detection
   - Command input: Send custom serial commands

### Web UI Features

**Live LED Matrix Display**
- Shows both 8×8 matrices in real-time
- Updates every 500ms via GET_DISPLAY command
- Color-coded: cyan for active LEDs

**Mode Controls**
- 4 quick-access buttons for modes
- Status indicator showing current mode
- Brightness slider (0-15)

**Sensor Panel**
- Real-time orientation angle (degrees)
- Flip detection status
- Acceleration magnitude

**Command Console**
- Send arbitrary serial commands
- View responses (JSON formatted)
- Command history

---

## Serial Commands API

### General Commands

#### `GET_STATUS`
Returns current mode status
```
→ GET_STATUS
← {"mode":1}
```

#### `GET_ORIENTATION`
Returns accelerometer angle
```
→ GET_ORIENTATION
← {"angle":45.23}
```

#### `GET_DISPLAY`
Returns both LED matrices
```
→ GET_DISPLAY
← {"matrixA":[[1,0,1,0,0,0,0,0],...], "matrixB":[[0,1,0,1,0,0,0,0],...]}
```

#### `SET_BRIGHTNESS <level>`
Set LED brightness (0-15)
```
→ SET_BRIGHTNESS 8
← OK
```

### Mode Control

#### `SET_MODE <mode>`
Switch operational mode
```
SET_MODE CLOCK          # Digital clock
SET_MODE HOURGLASS      # Sand animation timer
SET_MODE DICE           # Random number roller
SET_MODE FLIPCOUNTER    # Flip counter (alias: FLIP)
```

### Clock Mode Commands

#### `SET_TIME <hours> <minutes>`
Set the displayed time (24-hour format)
```
→ SET_TIME 14 30
← OK
```

### Hourglass Mode Commands

#### `SET_HG <hours> <minutes>`
Set hourglass duration (0-23 hours, 1-59 minutes)
```
→ SET_HG 0 30      # 30 minutes
← OK
```

#### `RESET_HG`
Reset timer to zero and stop animation
```
→ RESET_HG
← OK
```

### Dice Mode Commands

#### `ROLL_DICE`
Roll a random number (1-6)
```
→ ROLL_DICE
← {"diceValue":4}
```

### Flip Counter Mode Commands

#### `GET_FLIP_COUNT`
Query current flip count
```
→ GET_FLIP_COUNT
← {"count":42}
```

#### `RESET_FLIP`
Reset counter to zero
```
→ RESET_FLIP
← OK
```

### Error Responses

All errors follow this format:
```
ERR <description>
```

Examples:
```
ERR Invalid mode
ERR Invalid time range
ERR Missing time parameters
ERR Brightness must be 0-15
ERR Unknown command
```

---

## Configuration

### config.h

Edit `firmware/main/config.h` to customize hardware pins and parameters:

**Pin Definitions:**
```cpp
#define PIN_DATAIN 5       // MAX7219 Data In
#define PIN_CLK 4          // MAX7219 Clock
#define PIN_LOAD 10        // MAX7219 CS/LOAD (was Pin 0 - changed to avoid serial conflict)
#define PIN_BUTTON 2       // Push button
#define PIN_BUZZER 13      // Piezo buzzer
#define PIN_SDA 12         // I2C SDA (optional, ESP8266)
#define PIN_SCL 14         // I2C SCL (optional, ESP8266)
```

**Display Configuration:**
```cpp
#define NUM_MATRICES 2     // Number of LED matrices
#define MATRIX_A 1         // Address of first matrix
#define MATRIX_B 0         // Address of second matrix
#define DISPLAY_INTENSITY 8 // Initial brightness (0-15)
```

**Sensor Thresholds:**
```cpp
#define ACC_THRESHOLD_LOW 300      // Acceleration low threshold
#define ACC_THRESHOLD_HIGH 360     // Acceleration high threshold
#define SHAKE_THRESHOLD 0.5        // Shake detection sensitivity
#define FLIP_THRESHOLD 0.8         // Flip detection sensitivity
```

**Timing:**
```cpp
#define DELAY_FRAME 100            // Main loop delay (ms)
#define DEBOUNCE_DELAY 50          // Button debounce (ms)
#define UPDATE_INTERVAL 1000       // Status update interval (ms)
```

**Hourglass Animation:**
```cpp
#define HOURGLASS_PARTICLE_COUNT 60     // Sand particles
#define HOURGLASS_TONE_FREQ 440         // Buzzer frequency (Hz)
#define HOURGLASS_TONE_DURATION 10      // Buzz duration (ms)
#define HOURGLASS_ALARM_CYCLES 5        // Alarm beep count
```

### Local Configuration Override

Create `firmware/main/config_local.h` (git-ignored) to override WiFi credentials without modifying main config:

```cpp
#define CONFIG_LOCAL_INCLUDED
#define WIFI_SSID "YourNetwork"
#define WIFI_PASSWORD "YourPassword"
#define WIFI_AP_MODE false   // Set to true for AP mode
```

Place this BEFORE main config is included for proper override behavior.

---

## Hardware Details

### Pin Mapping Summary

| Pin | Function | Type | Component | Notes |
|-----|----------|------|-----------|-------|
| D0 | RXD | Input | USB Serial | Serial RX - **DO NOT use for other functions** |
| D1 | TXD | Output | USB Serial | Serial TX - **DO NOT use for other functions** |
| D2 | Button | Input | Push Button | Interrupt-capable |
| D3 | (PWM) | - | - | Available for future PWM use |
| D4 | CLK (SPI) | Output | MAX7219 | Clock line |
| D5 | DIN (SPI) | Output | MAX7219 | Data In line |
| D6 | (PWM) | - | - | Available for future PWM use |
| D7 | - | - | - | Reserved |
| D10 | CS/LOAD (SPI) | Output | MAX7219 | Chip Select (changed from D0) |
| D12 | SDA (I2C) | I/O | MPU6050 | Data line (optional) |
| D13 | Buzzer | Output | Piezo | Built-in LED + buzzer |
| D14 | SCL (I2C) | I/O | MPU6050 | Clock line (optional) |
| A0-A3 | ADC | Input | Analog Sensors | Available for future use |
| A4 | SDA (I2C) | I/O | MPU6050 | Standard I2C on Nano |
| A5 | SCL (I2C) | I/O | MPU6050 | Standard I2C on Nano |

### SPI Protocol (MAX7219)

**Timing:**
- Clock speed: 10 MHz (SPI mode 0)
- Data format: 16-bit address + data
- Load pulse width: ≥100ns

**Command Sequences:**
```
Brightness:  0x0A [level]
Scan limit:  0x0B [7 for 8 rows]
Decode mode: 0x09 [0 for raw LED control]
Test mode:   0x0F [0 to exit test]
```

### I2C Protocol (MPU6050)

**Address:** 0x68 (7-bit) / 0xD0-0xD1 (8-bit with R/W)

**Key Registers:**
- 0x3B-0x40: Accelerometer (X, Y, Z) ±2g or ±16g selectable
- 0x41-0x46: Temperature
- 0x43-0x48: Gyroscope (not used in current firmware)

**Data Format:**
- Accelerometer: 16-bit signed (MSB first)
- Range: ±2g default = ±16384 LSB/g

---

## Troubleshooting

### Upload Fails

**Symptom:** "Device not found" or upload timeout

**Solutions:**
1. Check USB cable is properly connected
2. Verify COM port in Device Manager
3. Install CH340/FT232 driver for your OS
4. Try different USB port on computer
5. Reset board: Press RST button on Nano

**Windows Troubleshooting:**
```
Device Manager → COM Ports
Right-click → Update driver → Search automatically
(Installs CH340 driver if needed)
```

### Serial Monitor Shows Garbage

**Symptom:** Garbled text instead of JSON responses

**Solutions:**
1. Check Serial Monitor baud rate: **must be 9600**
2. Click Tools → Serial → 9600 in Arduino IDE
3. Verify no other application is using the COM port
4. Close and reopen Serial Monitor

### LEDs Don't Light Up

**Symptom:** MAX7219 matrices remain dark

**Solutions:**
1. **Check wiring:** Verify DIN, CLK, CS/LOAD connections
2. **Test 5V supply:** Use multimeter to verify 5V at matrix VCC
3. **Check brightness:** Send `SET_BRIGHTNESS 15` command
4. **Test SPI:** Use this serial test command:
   ```
   GET_DISPLAY
   ```
   Should return 64×2 LED states (if working, not just errors)
5. **Replace MAX7219:** If SPI timing is correct, IC may be defective

### Button Doesn't Work

**Symptom:** Mode doesn't change on button press

**Solutions:**
1. Verify button wired to Pin 2
2. Check for adequate debounce time (default 50ms)
3. Test in serial: `SET_MODE CLOCK` (should work via command)
4. If commands work but button doesn't: check Button.cpp wiring logic
5. Use `GET_STATUS` to verify mode changes via other methods

### Accelerometer (MPU6050) Not Responding

**Symptom:** Orientation always 0°; flip detection doesn't work

**Solutions:**
1. Verify I2C wiring: SDA to A4, SCL to A5
2. Add 10kΩ pull-up resistors on SDA/SCL if not on MPU6050 board
3. Check for I2C address conflict (addr 0x68)
4. Power supply: MPU6050 requires clean 3.3V from Nano (not 5V!)
5. Test with I2C scanner: Search "Arduino I2C scanner" sketch

### Web UI Can't Connect

**Symptom:** "Connect" button doesn't show port selection

**Solutions:**
1. **Check browser:** Only Chrome/Edge/Opera 76+ support Web Serial
2. **Enable flag (if needed):** For older Chrome versions:
   - Go to chrome://flags
   - Search "web serial"
   - Enable "Experimental Web Platform features"
3. **Check USB:** Verify COM port appears in Device Manager
4. **Reload page:** Close and reopen web-ui/index.html
5. **Clear browser cache:** Ctrl+Shift+Delete → Clear all data

### Web UI Connects but Data Doesn't Update

**Symptom:** LED display shows empty grid; commands don't respond

**Solutions:**
1. Open browser Console (F12 → Console tab)
2. Look for error messages (usually permission or port issue)
3. Try sending `GET_STATUS` in Web UI command box
4. Check Arduino Serial Monitor works (confirms firmware is running)
5. If Serial Monitor works but Web UI doesn't: Web Serial API may have permissions issue
   - Try: Click "Connect" again → Select port with "Never ask again" unchecked

### Memory/Performance Issues

**Symptom:** Arduino acts sluggish, resets unexpectedly, or LEDs flicker

**Solutions:**
1. **Reduce update frequency:** Increase DELAY_FRAME in config.h (currently 100ms)
2. **Lower brightness:** Command `SET_BRIGHTNESS 6` reduces power draw
3. **Use external power supply:** USB 500mA limit insufficient for full brightness
4. **Reduce particle count:** Change HOURGLASS_PARTICLE_COUNT in config.h
5. **Check SRAM usage:** Compile and watch Arduino IDE output:
   ```
   Global variables use 1600 bytes of 2048 bytes dynamic memory
   ```
   If >90%, reduce features or buffer sizes

---

## Development Notes

### Project Structure

```
HourGlass/
├── README.md (this file - comprehensive documentation)
├── FIXES_APPLIED.md (detailed list of all bug fixes)
├── firmware/
│   └── main/
│       ├── main.ino (main loop and JSON functions)
│       ├── config.h (hardware and timing configuration)
│       ├── SerialProtocol.h/cpp (USB serial command parser)
│       ├── LedControl.h/cpp (MAX7219 matrix control)
│       ├── MPU6050.h/cpp (accelerometer interface)
│       ├── Button.h/cpp (debounced button input)
│       ├── Delay.h/cpp (non-blocking delay timer - overflow-safe)
│       ├── ClockMode.h/cpp (24-hour clock display)
│       ├── HourglassMode.h/cpp (sand animation and timer)
│       ├── DiceMode.h/cpp (random number roller)
│       ├── FlipCounterMode.h/cpp (flip counting with cap at 99)
│       └── README_ARDUINODROID.md (mobile setup guide)
├── web-ui/
│   ├── index.html (Web Serial UI)
│   ├── css/styles.css (glassmorphism styling)
│   └── js/
│       ├── app.js (main application logic)
│       ├── serial.js (Web Serial API wrapper)
│       ├── display.js (LED matrix visualization)
│       ├── api.js (command builder)
│       ├── modes.js (mode-specific UI logic)
│       └── led-display.js (matrix rendering with bounds checking)
└── docs/
    └── Various reference guides (architecture, hardware specs, etc.)
```

### Key Architectural Decisions

**Memory Optimization (SRAM ~2KB limit):**
- Fixed char[64] buffers instead of String class
- Static buffers for JSON output (getStatusJSON, etc.)
- Caller-provided buffers for matrixToJson() to prevent reentrancy
- F() macros for string literals (stored in Flash, not RAM)

**Non-blocking Design:**
- No delay() calls in main loop; uses custom Delay class
- Interrupt-safe button debouncing
- Async serial command processing

**Safe Reentrancy:**
- getDisplayJSON() calls matrixToJson() twice with separate buffers
- Each function has clear buffer ownership semantics
- Stack-allocated buffers for getTimeString() ensure freshness

**Overflow Safety:**
- Delay timer uses (millis() - startTime) for 32-bit unsigned subtraction
- Flip counter capped at 99 to prevent tens digit overflow
- Buffer bounds checking in JavaScript setPattern()

### Bug Fixes Applied

All critical issues have been resolved:

1. **Button press loss** - Fixed flag clearing in debounce logic
2. **Timer overflow** - Fixed Delay class to handle millis() wrapping
3. **Flip counter overflow** - Cap display at 99
4. **Config override** - Include order fixed for config_local.h
5. **String reentrancy** - Removed static buffers from getTimeString()
6. **Matrix buffer overwrite** - Fixed matrixToJson() nested call issue
7. **LED bounds checking** - Added validation in JavaScript patterns
8. **USB power warning** - Documented 500mA limit vs 800mA requirement
9. **PIN conflict** - Changed MAX7219 CS from Pin 0 to Pin 10
10. **Browser version** - Updated Opera requirement to v76+

See FIXES_APPLIED.md for detailed explanations.

### Future Enhancement Opportunities

**Firmware:**
- Bluetooth/WiFi connectivity (ESP8266 variant)
- EEPROM settings persistence
- Custom animation mode
- Temperature display integration
- Ambient light sensor for auto-brightness

**Web UI:**
- Animation editor for custom patterns
- Dark mode toggle
- Mobile touch support
- Settings persistence in localStorage
- Offline mode with cached patterns

**Hardware:**
- Battery power with boost converter
- Rotary encoder for direct control
- IR receiver for remote control
- Additional sensors (humidity, pressure)

### Testing Checklist

Before deployment:
- [ ] Compile without errors in Arduino IDE
- [ ] Upload to Arduino Nano R3
- [ ] Serial Monitor responds to GET_STATUS
- [ ] All 4 modes cycle and display correctly
- [ ] Web UI connects and shows live LED updates
- [ ] Button cycles modes (short press) and triggers actions (long press)
- [ ] MPU6050 detects orientation and flips
- [ ] Buzzer beeps on hourglass alarm
- [ ] Brightness adjustment works (SET_BRIGHTNESS command)
- [ ] USB power: LEDs work at reduced brightness
- [ ] External power: Full brightness operates without issues

---

## Support & Troubleshooting

**For compilation errors:**
1. Verify Arduino IDE version 1.8.13+
2. Check board selected: Arduino Nano
3. Check processor selected: ATmega328P
4. Ensure all .cpp/.h files are in same directory

**For runtime issues:**
1. Check Serial Monitor at 9600 baud
2. Send GET_STATUS command
3. Verify hardware connections with multimeter
4. Check SRAM usage in Arduino IDE compilation output

**For Web UI issues:**
1. Use Chrome/Edge/Opera 76+ (Firefox/Safari not supported)
2. Check browser console (F12) for error messages
3. Verify COM port is available in Device Manager
4. Clear browser cache and reload

---

## License & Credits

**Project:** HourGlass Smart LED Display System  
**Platform:** Arduino Nano R3 (ATmega328P)  
**License:** MIT (open source)

**Key Libraries:**
- LedControl (MAX7219 driver)
- Wire (I2C communication)
- Web Serial API (browser-based serial control)

**Version:** 1.0.0  
**Last Updated:** January 28, 2026

---

## Quick Reference

### Common Serial Commands
```
GET_STATUS                 → {"mode":0}
SET_MODE CLOCK            → OK
SET_TIME 14 30            → OK
SET_HG 0 30               → OK
RESET_HG                  → OK
ROLL_DICE                 → {"diceValue":4}
GET_FLIP_COUNT            → {"count":42}
RESET_FLIP                → OK
SET_BRIGHTNESS 8          → OK
GET_ORIENTATION           → {"angle":45.23}
GET_DISPLAY               → {"matrixA":[[...]], "matrixB":[[...]]}
```

### Mode Codes
- 0 = CLOCK
- 1 = HOURGLASS (default at startup)
- 2 = DICE
- 3 = FLIPCOUNTER

### Default Configuration
- Baud Rate: 9600
- Brightness: 8/15
- Button: Pin 2
- MAX7219 CS: Pin 10
- I2C: Default (A4=SDA, A5=SCL)