# ⏳ Smart LED Hourglass with Arduino

A multi-functional LED hourglass device built with Arduino Nano, featuring 4 operating modes, Web UI control, and real-time sensor integration.

![Version](https://img.shields.io/badge/version-1.0.2-blue.svg)
![Arduino](https://img.shields.io/badge/Arduino-Nano-green.svg)
![License](https://img.shields.io/badge/license-MIT-orange.svg)

## 📋 Table of Contents
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Pinout](#pinout)
- [Installation](#installation)
- [Operating Modes](#operating-modes)
- [Serial Commands](#serial-commands)
- [Web UI](#web-ui)
- [API Reference](#api-reference)
- [Troubleshooting](#troubleshooting)

## ✨ Features

### 4 Operating Modes
1. **⏳ Hourglass Mode** - Animated sand timer with configurable duration
2. **🕐 Clock Mode** - Digital clock with dot patterns and auto-rotation
3. **🎲 Dice Mode** - Electronic dice roller with shake-to-roll
4. **🔄 Flip Counter** - Counts device flips (0° ↔ 180°)

### Key Features
- **Auto-rotation**: Display adjusts based on device orientation (MPU6050)
- **Web UI Control**: Browser-based control panel via USB Serial
- **Hardware Controls**: Physical button for mode switching and actions
- **Live LED Visualization**: Real-time Web UI mirror of LED matrices
- **Shake Detection**: Roll dice by shaking the device
- **Flip Detection**: Count complete 180° flips

## 🔧 Hardware Requirements

| Component | Specification | Quantity |
|-----------|--------------|----------|
| Microcontroller | Arduino Nano R3 | 1 |
| LED Matrix | MAX7219 8x8 LED Matrix | 2 |
| Gyroscope | MPU6050 (GY-521) | 1 |
| Push Button | Active-Low | 1 |
| Buzzer | Piezo Buzzer 5V | 1 |
| Resistors | 10kΩ (for button pull-up) | 1 |
| Wires | Dupont/Jumper wires | As needed |

### Wiring Diagram

```
Arduino Nano Pin    Connected To
────────────────    ────────────
D11 (MOSI)  ───────► MAX7219 DIN (both matrices)
D13 (SCK)   ───────► MAX7219 CLK (both matrices)
D10 (SS)    ───────► MAX7219 CS  (both matrices)
D2          ───────► Push Button ──► GND
D3          ───────► Piezo Buzzer ──► GND
A4 (SDA)    ───────► MPU6050 SDA
A5 (SCL)    ───────► MPU6050 SCL
5V          ───────► MAX7219 VCC, MPU6050 VCC
GND         ───────► All GND connections
```

## 📍 Pinout

| Pin | Function | Description |
|-----|----------|-------------|
| D2 | Button Input | Mode switch / Action (Active-Low) |
| D3 | Buzzer Output | Audio feedback (1000-1500Hz) |
| D10 | MAX7219 CS | Chip Select for LED matrices |
| D11 | MAX7219 DIN | Data In (SPI MOSI) |
| D13 | MAX7219 CLK | Clock (SPI SCK) |
| A4 | I2C SDA | MPU6050 Data |
| A5 | I2C SCL | MPU6050 Clock |

## 🚀 Installation

### 1. Arduino Setup
1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install required libraries:
   - Wire (built-in)
   - SPI (built-in)
3. Open `rishi-file.ino` in Arduino IDE
4. Select Board: **Arduino Nano**
5. Select Processor: **ATmega328P (Old Bootloader)**
6. Select correct COM port
7. Click **Upload**

### 2. Web UI Setup
1. Open `web-ui/index.html` in a modern browser (Chrome/Edge recommended)
2. Connect Arduino via USB
3. Click **Connect** button in Web UI
4. Select the Arduino device from the serial port list

## 🎮 Operating Modes

### Mode 1: ⏳ Hourglass (Default)
Animated sand timer with falling particles.

| Control | Action |
|---------|--------|
| **Short Press** | Reset hourglass |
| **Long Press (1.5s)** | Switch to Clock mode |
| **Web UI** | Set duration (1-60 minutes) |
| **Alarm** | Beeps 3x when timer completes |

**Display**: Particles flow from top to bottom matrix based on gravity.

### Mode 2: 🕐 Clock
Digital clock with automatic orientation adjustment.

| Control | Action |
|---------|--------|
| **Long Press (1.5s)** | Switch to Dice mode |
| **Rotate Device** | Display rotates to stay upright |
| **Web UI** | Set time (HH:MM) |

**Orientations**:
- **0°**: Dots pattern (Hours top, Minutes bottom)
- **90°**: Digital digits horizontal
- **180°**: Dots pattern (reversed)
- **270°**: Digital digits (rotated)

### Mode 3: 🎲 Dice Mode
Electronic dice with dual-dice display.

| Control | Action |
|---------|--------|
| **Short Press** | Roll single dice (same on both matrices) |
| **Shake Device** | Roll two dice (different values) |
| **Long Press (1.5s)** | Switch to Flip Counter mode |
| **Web UI** | Click "Roll Dice" button |

**Display**:
- Matrix A: Shows Dice A value
- Matrix B: Shows Dice B value
- 3×3 dot pattern for dice faces

### Mode 4: 🔄 Flip Counter
Counts complete 180° flips of the device.

| Control | Action |
|---------|--------|
| **Flip 0° → 180°** | Count +1 |
| **Flip 180° → 0°** | Count +1 |
| **Short Press** | Reset counter to 0 |
| **Long Press (1.5s)** | Switch to Hourglass mode |

**Display**:
- **0° (Upright)**: Count shown on **top** LED matrix
- **180° (Upside Down)**: Count shown on **bottom** LED matrix

## 📡 Serial Commands

### Mode Control
```
SET_MODE hourglass      → OK MODE HOURGLASS
SET_MODE clock          → OK MODE CLOCK
SET_MODE dice           → OK MODE DICE
SET_MODE flipcounter    → OK MODE FLIPCOUNTER
```

### Clock Commands
```
SET_TIME 14 30          → OK TIME 14:30
```

### Hourglass Commands
```
SET_HG 0 5              → OK TIMER SET TO 5 MIN
SET_HG 1 0              → OK TIMER SET TO 60 MIN
RESET_HG                → OK HOURGLASS RESET
```

### Dice Commands
```
ROLL_DICE               → {"value":7,"diceValue":3,"diceA":3,"diceB":4}
```

### Flip Counter Commands
```
RESET_FLIP              → OK FLIP COUNTER RESET
GET_FLIP_COUNT          → {"count":5}
```

### Status Commands
```
GET_STATUS              → {"mode":"hourglass","time":"11:31","orientation":0,...}
GET_ORIENTATION         → {"angle":0}
GET_DISPLAY             → {"matrixA":"...","matrixB":"..."}
GET_ALL                 → Full status + display + orientation
SET_BRIGHTNESS 8        → OK BRIGHTNESS SET (0-15)
```

## 🌐 Web UI

### Features
- **Live LED Visualization**: Real-time mirror of hardware LED matrices
- **Mode Switching**: Click mode buttons to switch modes
- **Device Control**: Set clock time, hourglass duration, roll dice
- **Auto-Refresh**: 10 FPS real-time updates (100ms interval)
- **Orientation Display**: Visual indicator of device angle

### Browser Requirements
- Chrome 89+ or Edge 89+ (Web Serial API support)
- USB connection to Arduino

### Usage
1. Open `web-ui/index.html`
2. Click **Connect** button
3. Select Arduino from device list
4. Control device via UI buttons

## 📊 API Reference

### Response Formats

#### Dice Roll Response
```json
{
  "value": 7,
  "diceValue": 3,
  "diceA": 3,
  "diceB": 4
}
```

#### Status Response
```json
{
  "mode": "dice",
  "time": "11:31",
  "orientation": 0,
  "diceValue": 3,
  "diceA": 3,
  "diceB": 4
}
```

#### Hourglass Status
```json
{
  "mode": "hourglass",
  "time": "11:31",
  "orientation": 0,
  "hourglassProgress": 45,
  "particlesRemaining": 33
}
```

#### Flip Counter Status
```json
{
  "mode": "flipcounter",
  "time": "11:31",
  "orientation": 0,
  "flipCount": 12
}
```

#### Display Data (16-char hex string per matrix)
```json
{
  "matrixA": "3C7EE7C3C3E77E3C",
  "matrixB": "0018180000000000"
}
```

## 🧭 Orientation Angles

| Angle | Position | Description |
|-------|----------|-------------|
| 0° | Upright | Vertical, USB port down |
| 90° | Left Side | Horizontal, lying on left side |
| 180° | Upside Down | Vertical, inverted |
| 270° | Right Side | Horizontal, lying on right side |

## 🐛 Troubleshooting

### Compilation Errors
- **Error: 'displayClockMode' not declared**: Check forward declaration exists
- **Library not found**: Install Wire library via Library Manager

### Connection Issues
- **Web UI not connecting**: Use Chrome/Edge (Firefox/Safari don't support Web Serial)
- **Wrong COM port**: Check Device Manager for Arduino port
- **Upload failed**: Try "Old Bootloader" option for Nano

### Display Issues
- **LEDs not lighting**: Check MAX7219 wiring (VCC, GND, DIN, CLK, CS)
- **Wrong rotation**: Verify MPU6050 is oriented correctly
- **Flickering**: Normal in some modes; clock mode is now static

### Sensor Issues
- **No orientation change**: Check MPU6050 SDA/SCL connections
- **False flip detection**: Increase `FLIP_DEBOUNCE_MS` value
- **Shake not working**: Verify `ay` axis is being read

## 📁 File Structure

```
rishi-file/
├── rishi-file.ino          # Main firmware
├── config.h                # Configuration settings
├── utils.h                 # Utility functions
├── DiceMode.h              # Dice mode class header
├── DiceMode.cpp            # Dice mode implementation
├── LedControl.h            # LED matrix library
├── LedControl.cpp          # LED matrix implementation
├── Delay.h                 # Non-blocking delay
├── Delay.cpp               # Delay implementation
└── web-ui/
    ├── index.html          # Web interface
    ├── css/
    │   └── styles.css      # UI styles
    └── js/
        ├── app.js          # Main app logic
        ├── api.js          # Serial API
        ├── display.js      # LED visualization
        ├── modes.js        # Mode handlers
        └── serial.js       # Web Serial connection
```

## 📜 License

This project is open source. Feel free to modify and distribute.

## 🙏 Acknowledgments

- LedControl library by Eberhard Fahle
- Web Serial API by Google Chrome Labs
- MPU6050 sensor community

---

**Made with ❤️ for the Arduino community**

For issues or feature requests, please open a GitHub issue.
