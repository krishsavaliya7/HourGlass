# Firmware Updates - Reference Code Integration

## ✅ Changes Made

### 1. Hourglass Animation - Matches Reference Code
- **Particle Physics**: Implemented exact particle movement algorithm from reference
  - Diagonal slice-based particle processing
  - Random direction selection for particles
  - Proper gravity-based top/bottom matrix detection
  - Particle drop mechanism with timing

- **Key Functions Implemented**:
  - `getDown()`, `getLeft()`, `getRight()` - Particle movement helpers
  - `canGoLeft()`, `canGoRight()`, `canGoDown()` - Collision detection
  - `moveParticle()` - Core particle physics matching reference
  - `updateMatrix()` - Diagonal slice processing with random direction
  - `dropParticle()` - Timed particle drops between matrices
  - `fill()` - Diagonal fill pattern matching reference

### 2. Buzzer Support Added
- **Buzzer Pin**: `PIN_BUZZER` (GPIO 13, configurable in `config.h`)
- **Features**:
  - Particle drop feedback: Short beep (440Hz, 10ms) when particle drops
  - Alarm: 5 beeps (440Hz, 200ms each, 1s apart) when hourglass completes
  - Initialized in `setup()` with pin mode configuration

### 3. USB Serial Communication
- **Serial Protocol**: New `SerialProtocol` class for USB communication
- **Commands Supported**:
  - `GET_STATUS` - Get device status (JSON)
  - `SET_MODE <mode>` - Change mode (clock/hourglass/dice/flipcounter)
  - `SET_TIME <hours> <minutes>` - Set clock time
  - `SET_HG <hours> <minutes>` - Set hourglass duration
  - `RESET_HG` - Reset hourglass
  - `ROLL_DICE` - Roll dice
  - `GET_FLIP_COUNT` - Get flip counter value
  - `RESET_FLIP` - Reset flip counter
  - `GET_ORIENTATION` - Get orientation data (JSON)
  - `GET_DISPLAY` - Get LED matrix state (JSON)
  - `SET_BRIGHTNESS <level>` - Set display brightness (0-15)

- **Response Format**:
  - Success: `OK` or JSON data
  - Error: `ERR <message>`

### 4. Removed WiFi Dependencies
- Removed ESP8266WebServer includes
- Removed WiFi initialization
- Removed WebServer class usage
- Now works with Arduino Nano, ESP8266, or ESP32 via USB Serial only

### 5. Configuration Updates
- **Serial Baud Rate**: Changed to 9600 (matching reference and Web Serial standard)
- **Initial Mode**: Changed to `MODE_HOURGLASS` (matching reference)
- **Buzzer Pin**: Configurable via `PIN_BUZZER` in `config.h`

## 📋 File Structure

```
firmware/
├── main.ino                    # Main program (USB Serial only)
├── config.h                    # Configuration
├── Delay.h/cpp                 # Non-blocking delay
├── display/
│   └── LedControl.h/cpp       # LED matrix control
├── sensors/
│   ├── MPU6050.h/cpp          # Orientation sensor
│   └── Button.h/cpp            # Button input
├── serial/
│   ├── SerialProtocol.h/cpp   # USB Serial command parser
└── modes/
    ├── ClockMode.h/cpp
    ├── HourglassMode.h/cpp    # Updated with reference animation
    ├── DiceMode.h/cpp
    └── FlipCounterMode.h/cpp
```

## 🎯 Hourglass Animation Details

### Particle Movement Algorithm
1. **Processing Order**: Diagonal slices (top-left to bottom-right)
2. **Direction**: Random per slice (left-to-right or right-to-left)
3. **Movement Priority**:
   - First: Try to move down (if both left and right are clear)
   - Second: Move left or right (whichever is available)
   - Third: Random choice if both available

### Gravity Detection
- Uses MPU-6050 angle to determine top/bottom matrix
- `gravity == 90` → MATRIX_A is top
- Otherwise → MATRIX_B is top

### Particle Drop Timing
- Drops particles based on duration: `hours * 60 + minutes` seconds
- Only drops when device is horizontal (gravity 0 or 180)
- Buzzer feedback on each drop

### Completion Detection
- When no particles move AND no drops occur AND top matrix is empty
- Triggers alarm (5 beeps)

## 🔧 Usage

### Upload Firmware
1. Open `firmware/main.ino` in Arduino IDE
2. Select appropriate board (Arduino Nano/ESP8266/ESP32)
3. Upload firmware
4. Open Serial Monitor at 9600 baud

### Test Commands
```
SET_MODE hourglass
SET_HG 0 1
GET_STATUS
```

### Web UI Integration
- Web UI uses Web Serial API to communicate
- Commands sent via `serial.js` module
- Responses parsed in `api.js` module

## ⚠️ Notes

1. **Pin Configuration**: 
   - Reference uses PIN_LOAD = 6, PIN_BUZZER = 14
   - Current config uses PIN_LOAD = 0, PIN_BUZZER = 13
   - Adjust in `config.h` if needed for your hardware

2. **Sensor Fallback**: 
   - If MPU-6050 fails, code falls back to analog pins (A1, A2)
   - This matches reference code behavior

3. **Buzzer**: 
   - Uses Arduino `tone()` function
   - Requires buzzer connected to `PIN_BUZZER`
   - Can be disabled by commenting out `tone()` calls

4. **Serial Communication**:
   - Commands must end with `\n` or `\r\n`
   - Responses sent as single line
   - JSON responses for status/orientation/display

## ✅ Testing Checklist

- [x] Hourglass animation matches reference particle physics
- [x] Buzzer support for particle drops
- [x] Buzzer alarm when hourglass completes
- [x] USB Serial command parser
- [x] All modes functional
- [x] Button input handling
- [x] Orientation detection
- [x] Web UI integration ready

---

## 📚 Documentation Status

All documentation has been updated to reflect the final build:
- ✅ USB Serial communication (no WiFi/Bluetooth)
- ✅ Buzzer support documented
- ✅ Reference hourglass animation documented
- ✅ Complete serial command protocol
- ✅ Hardware setup with buzzer pin
- ✅ Web UI integration guide

---

**Status**: ✅ Final build complete - Reference code integrated, buzzer support added, USB Serial ready, all documentation updated

