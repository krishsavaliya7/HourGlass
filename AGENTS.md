# Smart Hourglass - AI Agent Documentation

## Project Overview

Smart Hourglass is an Arduino-based embedded system featuring a dual-mode LED hourglass and digital clock. The project consists of:

- **Firmware**: Arduino C++ code running on Arduino Nano R3
- **Web UI**: Browser-based control panel using Web Serial API for hardware communication

### Hardware Components
- Arduino Nano R3 (ATmega328P @ 16MHz)
- 2× MAX7219 LED Matrix (8×8, daisy-chained for 16×8 total display)
- MPU6050 Gyroscope/Accelerometer (GY-521 module, I2C address 0x68)
- Push Button (Active-Low on D2)
- Piezo Buzzer (5V on D3)

### Operating Modes
1. **Hourglass Mode**: Particle simulation with sand-like falling animation, configurable timer (1-60 minutes)
2. **Clock Mode**: Digital clock display with orientation-aware layout
3. **Dice Mode**: Random dice roller (1-6) with dot pattern display
4. **Flip Counter Mode**: Counts vertical flips using MPU6050 orientation detection

---

## Project Structure

```
rishi-file/
├── rishi-file.ino          # Main Arduino sketch (entry point)
├── config.h                # Pin definitions, constants, configuration
├── utils.h                 # Utility functions (angle normalization)
├── Delay.h/cpp             # Non-blocking delay utility class
├── LedControl.h/cpp        # MAX7219 LED matrix driver library
├── DiceMode.h/cpp          # Dice mode implementation
├── web-ui/                 # Web-based control interface
│   ├── index.html          # Main UI page
│   ├── css/
│   │   └── styles.css      # Glassmorphism design stylesheet
│   └── js/
│       ├── serial.js       # Web Serial API communication
│       ├── api.js          # Command API wrapper
│       ├── display.js      # LED matrix visualization
│       ├── modes.js        # Mode selection handlers
│       └── app.js          # Main application logic
└── .github/                # GitHub prompts and skills (opsx framework)
```

---

## Technology Stack

### Firmware (Arduino)
- **Language**: C/C++ (Arduino framework)
- **Target**: Arduino Nano R3 (ATmega328P)
- **Clock Speed**: 16 MHz
- **Libraries**: 
  - Wire.h (I2C for MPU6050)
  - Standard Arduino SPI (bit-banged in LedControl)
- **Memory**: 32KB Flash, 2KB SRAM

### Web UI
- **Frontend**: Vanilla HTML5, CSS3, JavaScript (ES6+ classes)
- **Design**: Glassmorphism (frosted glass effect with backdrop-filter)
- **Communication**: Web Serial API (Chrome/Edge 89+)
- **No Build Process**: Static files served directly or opened locally

---

## Build and Flash Instructions

### Arduino Firmware
1. Open `rishi-file.ino` in Arduino IDE
2. Select Board: "Arduino Nano" 
3. Select Processor: "ATmega328P" (or "ATmega328P (Old Bootloader)" if upload fails)
4. Select appropriate COM port
5. Click Upload (Ctrl+U)

### Web UI
No build required. Open `web-ui/index.html` directly in Chrome/Edge:
```bash
# Windows
start web-ui/index.html

# Or simply double-click the file
```

---

## Serial Communication Protocol

### Connection Parameters
- Baud Rate: 9600
- Data Bits: 8
- Stop Bits: 1
- Parity: None

### Command Format
Commands are sent as ASCII text terminated with newline (`\n`):
```
<COMMAND> [PARAM1] [PARAM2]\n
```

### Available Commands

| Command | Parameters | Response | Description |
|---------|-----------|----------|-------------|
| `SET_MODE` | `clock` / `hourglass` / `dice` | `OK MODE <mode>` | Switch operating mode |
| `SET_TIME` | `<hours> <minutes>` (0-23, 0-59) | `OK TIME <time>` | Set clock time |
| `SET_HG` | `<hours> <minutes>` | `OK TIMER SET` | Set hourglass duration |
| `SET_BRIGHTNESS` | `<0-15>` | `OK BRIGHTNESS SET` | LED intensity |
| `RESET_HG` | - | `OK HOURGLASS RESET` | Reset hourglass timer |
| `ROLL_DICE` | - | `{"value":N}` | Roll dice |
| `GET_STATUS` | - | JSON status object | Get current state |
| `GET_ORIENTATION` | - | `{"angle":N}` | Get device tilt |
| `GET_DISPLAY` | - | `{"matrixA":"...","matrixB":"..."}` | LED state as hex |
| `GET_ALL` | - | Combined JSON | All status + display + orientation |

### JSON Response Format
```json
{
  "mode": "hourglass",
  "time": "12:30",
  "orientation": 90,
  "hourglassProgress": 45,
  "particlesRemaining": 33
}
```

---

## Pin Configuration

| Pin | Function | Connected To |
|-----|----------|--------------|
| D2 | INPUT_PULLUP | Push Button (active LOW) |
| D3 | OUTPUT | Piezo Buzzer |
| D10 | OUTPUT | MAX7219 CS (LOAD) |
| D11 | OUTPUT | MAX7219 DIN (DATA) |
| D13 | OUTPUT | MAX7219 CLK (CLOCK) |
| A4 (SDA) | I2C Data | MPU6050 SDA |
| A5 (SCL) | I2C Clock | MPU6050 SCL |

**Note**: The `config.h` and `rishi-file.ino` have slightly different pin definitions. The actual wiring uses pins from `rishi-file.ino` (DIN=11, CS=10, CLK=13).

---

## Key Code Patterns

### Non-Blocking Timing
The firmware uses `NonBlockDelay` class instead of `delay()`:
```cpp
NonBlockDelay dropTimer;
dropTimer.Delay(1000);  // Set timeout
if (dropTimer.Timeout()) {  // Check if expired
    // Handle timeout
}
```

### LED Matrix Coordinate System
- `setXY(addr, x, y, state)` - Logical coordinates (0,0 = bottom-left)
- `setRawXY(addr, x, y, state)` - Physical matrix coordinates
- `setLed(addr, row, col, state)` - Row/column addressing
- Rotation support: 0°, 90°, 180°, 270° via `setRotation()`

### Orientation Detection
MPU6050 accelerometer readings converted to orientation:
- 0° = Upright (portrait)
- 90° = Right side down
- 180° = Upside down
- 270° = Left side down

Requires 6 consecutive stable readings before changing orientation (debouncing).

---

## Web UI Architecture

### Class Structure
- `SerialConnection` - Manages Web Serial API, port selection, read loop
- `API` - Wraps serial commands with Promise-based interface
- `Display` - Visualizes LED matrices and orientation indicator
- `ModeManager` - Handles mode switching and mode-specific controls
- `App` - Main application controller, event coordination

### Browser Compatibility
- ✅ Chrome 89+
- ✅ Edge 89+
- ✅ Opera 76+
- ❌ Firefox (no Web Serial API support)
- ⚠️ Safari 15+ (partial)

### Local Storage
- Brightness preference: `localStorage.getItem('brightness')`

---

## Code Style Guidelines

### C++ (Arduino)
- Use `#define` for constants in UPPER_CASE
- Pin definitions in `config.h`
- Class names in PascalCase (`LedControl`, `NonBlockDelay`)
- Methods in camelCase
- Member variables use `this->` prefix in class implementations
- Comments use `//` for inline, `/* */` for headers

### JavaScript
- ES6 class syntax
- camelCase for methods and variables
- Event-driven architecture with callbacks
- Async/await for serial operations
- No external dependencies (vanilla JS)

### CSS
- CSS variables in `:root` for theming
- Glassmorphism effects: `backdrop-filter: blur(10px)`
- Mobile-first responsive design with `@media` queries
- BEM-like naming for components

---

## Testing Instructions

### Hardware Tests
1. **LED Test**: Check both matrices light up on startup
2. **Button Test**: Short press resets hourglass, long press (1.5s) changes mode
3. **Orientation Test**: Tilt device, display should rotate accordingly
4. **Serial Test**: Connect via Arduino Serial Monitor at 9600 baud, send `GET_STATUS`

### Web UI Tests
1. Open `index.html` in Chrome/Edge
2. Click "Connect" and select Arduino COM port
3. Verify status changes to "Connected"
4. Test mode switching buttons
5. Test brightness slider
6. Verify LED matrix visualization matches hardware

---

## Security Considerations

### Firmware
- No authentication on serial commands
- WiFi credentials (if ESP8266 variant) should go in `config_local.h` (gitignored)
- Serial protocol is plaintext - not encrypted

### Web UI
- Web Serial API requires user interaction to select port
- No sensitive data stored in localStorage
- Served as static files - no server-side processing

---

## Development Notes

### Memory Constraints
- ATmega328P has only 2KB SRAM
- LedControl stores 64 bytes per matrix for LED state
- String operations should be minimized in `loop()`
- `DEBUG_OUTPUT` disabled in config.h to save ~200 bytes RAM

### Timing
- Main loop delay: 80ms (`DELAY_FRAME`)
- Orientation check: every loop iteration with 6-sample debouncing
- Clock update: every 60 seconds
- Auto-refresh in Web UI: 500ms polling

### Known Limitations
- Hourglass timer accuracy depends on `millis()` which drifts slightly
- No RTC (Real-Time Clock) - time resets on power cycle
- Clock mode time must be set manually via serial or Web UI
- Web Serial API requires HTTPS for production deployment (except localhost)

---

## Firmware Version
- Current: `1.0.2-OPT` (defined in `config.h`)
- Build date: `__DATE__` macro

---

## Resources

### Hardware Datasheets
- [Arduino Nano R3](https://docs.arduino.cc/hardware/nano)
- [MAX7219 LED Driver](https://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf)
- [MPU6050 Sensor](https://invensense.tdk.com/products/motion-tracking/6-axis/mpu-6050/)

### Web APIs
- [Web Serial API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Serial_API)
- [CSS Backdrop Filter](https://developer.mozilla.org/en-US/docs/Web/CSS/backdrop-filter)

### Project Documentation
See `web-ui/README.md` for detailed UI documentation and `web-ui/HARDWARE_INTEGRATION.md` for hardware specs.
