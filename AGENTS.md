# Smart Hourglass System - Agent Guide

This document provides essential information for AI coding agents working on the Smart Hourglass System project.

---

## Project Overview

The Smart Hourglass System is an Arduino-based embedded project that combines:
- **LED Hourglass Animation** - Visual sand-falling physics simulation on 8x8 LED matrices
- **Digital Clock Display** - Time display with orientation-aware rendering
- **Web UI Control Panel** - Browser-based control using Web Serial API

The device automatically detects its physical orientation using an MPU6050 gyroscope and can switch between modes via physical button or web interface.

**Hardware Target**: Arduino Nano R3 (ATmega328P)  
**Firmware Language**: C++ (Arduino Framework)  
**Web UI**: Vanilla HTML/CSS/JavaScript (ES6+)  
**License**: MIT (LedControl library)

---

## Project Structure

```
rishi-file/
├── Firmware Files (Arduino/C++)
│   ├── rishi-file.ino          # Main sketch - setup(), loop(), mode logic
│   ├── LedControl.h/.cpp       # MAX7219 LED driver library (MIT licensed)
│   ├── Delay.h/.cpp            # Non-blocking delay timer class
│   ├── DiceMode.h/.cpp         # Dice roller mode (standby module)
│   ├── config.h                # Compile-time configuration & pin definitions
│   └── utils.h                 # Utility functions (angle normalization)
│
├── Web UI (Browser Control Panel)
│   └── web-ui/
│       ├── index.html          # Main UI markup
│       ├── css/
│       │   ├── styles.css      # Glassmorphism UI styles
│       │   └── led-matrix.css  # LED display visualization styles
│       └── js/
│           ├── serial.js       # Web Serial API communication
│           ├── api.js          # Command API wrapper
│           ├── app.js          # Main application logic
│           ├── modes.js        # Mode switching handlers
│           ├── display.js      # Display update logic
│           └── led-display.js  # LED matrix visualization
│
└── Documentation
    ├── README.md               # User-facing project documentation
    ├── ARCHITECTURE_DIAGRAM.md # System architecture & data flow diagrams
    ├── COMMANDS.md             # Serial command reference
    └── package.json            # Node.js dependencies (http-server)
```

---

## Technology Stack

### Firmware
- **Platform**: Arduino Nano R3 (ATmega328P @ 16MHz)
- **Language**: C++ (Arduino Framework)
- **Libraries**: 
  - `Wire.h` (built-in) - I2C communication with MPU6050
  - `LedControl` (custom) - SPI communication with MAX7219
- **Baud Rate**: 9600 (USB Serial)

### Web UI
- **Frontend**: Vanilla HTML5, CSS3, JavaScript (ES6+)
- **Styling**: Glassmorphism design system with CSS custom properties
- **Browser APIs**: Web Serial API (Chrome 89+, Edge 89+)
- **Server**: Static file server (Python or Node.js http-server)

### Hardware Components
| Component | Interface | Arduino Pins |
|-----------|-----------|--------------|
| MAX7219 LED Matrix (x2) | SPI | D10 (CS), D11 (DIN), D13 (CLK) |
| MPU6050 Accelerometer/Gyro | I2C | A4 (SDA), A5 (SCL) |
| Push Button | GPIO | D2 (INPUT_PULLUP) |
| Piezo Buzzer | GPIO | D3 |

---

## Build and Test Commands

### Web UI Development
```bash
# Using Python (built-in)
npm run serve
# OR
python -m http.server 8080

# Using Node.js http-server
npm run serve-node
# OR
npx http-server -p 8080 -c-1
```

Access at: `http://localhost:8080/web-ui/`

### Firmware Upload
1. Open `rishi-file.ino` in Arduino IDE
2. Select Board: "Arduino Nano" (ATmega328P)
3. Select Processor: "ATmega328P" (or "ATmega328P (Old Bootloader)" for clones)
4. Select correct COM port
5. Click Upload

### Testing
```bash
# No automated test suite - manual testing via:
# 1. Web UI in browser (Chrome/Edge required for Web Serial)
# 2. Serial Monitor in Arduino IDE (9600 baud)
# 3. Physical hardware verification
```

---

## Code Organization

### Firmware Architecture

**Main Loop Cycle (80ms per iteration)**:
```
1. handleSerialCommands()  - Process Web UI commands
2. checkButton()           - Physical button handling
3. readAccelerometer()     - MPU6050 orientation detection
4. Update Display:
   - Hourglass mode: updateMatrix() + dropParticle()
   - Clock mode: displayClockMode()
5. sendStatusUpdate()      - Broadcast status to Web UI (every 500ms)
```

**Key Modules**:
- **rishi-file.ino**: Main program logic, mode switching, serial protocol
- **LedControl**: Low-level LED matrix control via SPI
- **Delay**: Non-blocking timer for particle animation timing
- **config.h**: Centralized pin definitions and compile-time constants

### Web UI Architecture

**Module Responsibilities**:
- **serial.js**: Web Serial API connection management, data streaming
- **api.js**: Command/response protocol, JSON parsing, request queue
- **app.js**: Application state, auto-refresh, UI coordination
- **modes.js**: Mode switching UI, mode-specific controls
- **display.js**: LED matrix visualization, orientation display

---

## Serial Communication Protocol

**Connection**: USB Serial @ 9600 baud, newline-terminated commands

### Commands (Web UI → Arduino)
```
SET_MODE clock|hourglass    # Switch operating mode
SET_TIME <hh> <mm>          # Set clock time (24h format)
RESET_HG                    # Reset hourglass particles
SET_BRIGHTNESS <0-15>       # LED intensity
GET_STATUS                  # Request status JSON
GET_ORIENTATION             # Request orientation JSON
GET_DISPLAY                 # Request LED matrix state
SET_TIMER <minutes>         # Configure hourglass duration
```

### Responses (Arduino → Web UI)
```
OK MODE <mode>              # Mode switch confirmation
OK TIME <hh>:<mm>           # Time set confirmation
OK BRIGHTNESS SET           # Brightness confirmation
ERR <message>               # Error response
{"mode":"...",...}          # JSON status data
STATUS:{"mode":"...",...}   # Auto-broadcast (every 500ms)
```

---

## Configuration

### config.h - Key Settings
```cpp
// Pin Definitions
#define PIN_DATAIN 11       // MAX7219 DIN
#define PIN_CLK 13          // MAX7219 CLK
#define PIN_LOAD 10         // MAX7219 CS
#define PIN_BUTTON 2        // Push button
#define PIN_BUZZER 3        // Piezo buzzer

// Timing
#define DELAY_FRAME 80      # Main loop delay (ms)
#define LONG_PRESS_TIME 1500 // Mode switch threshold (ms)
#define DEBOUNCE_TIME 50    // Button debounce (ms)

// Display
#define DISPLAY_INTENSITY 8 // Default brightness (0-15)
#define ROTATION_OFFSET 90  // Hardware mounting offset

// Hourglass
#define TOTAL_PARTICLES 60  // Number of sand particles
```

### Local Config Override
Create `config_local.h` (gitignored) for sensitive credentials:
```cpp
#ifndef CONFIG_LOCAL_INCLUDED
#define CONFIG_LOCAL_INCLUDED
#define WIFI_SSID "YourNetwork"
#define WIFI_PASSWORD "YourPassword"
#endif
```

---

## Code Style Guidelines

### C++ (Arduino)
- **Naming**:
  - Classes: `PascalCase` (e.g., `LedControl`, `NonBlockDelay`)
  - Functions: `camelCase` (e.g., `checkButton()`, `getOrientation()`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `LONG_PRESS_TIME`, `PIN_BUTTON`)
  - Variables: `camelCase` (e.g., `currentMode`, `buttonWasPressed`)
- **Comments**: Use `//` for single-line, `/* */` for multi-line file headers
- **Indentation**: 2 spaces (as seen in existing code)
- **Braces**: Opening brace on same line (K&R style)

### JavaScript
- **ES6+ Features**: Use `class`, `const`/`let`, arrow functions, async/await
- **Naming**: `camelCase` for variables/functions, `PascalCase` for classes
- **JSDoc**: Document functions with JSDoc comments
- **Quotes**: Single quotes for strings

### CSS
- **Naming**: Kebab-case for class names (e.g., `.glass-section`, `.mode-btn`)
- **Variables**: Define in `:root` for theming (see `css/styles.css`)
- **Glassmorphism**: Use `backdrop-filter: blur()`, rgba colors, subtle borders

---

## Testing Instructions

### Manual Testing Checklist

**Hardware**:
- [ ] Arduino Nano recognized on COM port
- [ ] LED matrices display test pattern on startup
- [ ] MPU6050 responds (check serial debug output)
- [ ] Button short press resets hourglass
- [ ] Button long press (1.5s) switches modes
- [ ] Buzzer sounds on mode switch

**Web UI**:
- [ ] Opens in Chrome/Edge without errors
- [ ] "Connect" button opens port selector
- [ ] Serial connection established at 9600 baud
- [ ] Mode buttons switch device modes
- [ ] LED matrix visualization matches hardware
- [ ] Brightness slider controls display intensity
- [ ] Status updates received every 500ms

**Serial Protocol**:
```bash
# Test commands via Serial Monitor (9600 baud)
SET_MODE clock
SET_MODE hourglass
SET_TIME 14 30
GET_STATUS
SET_BRIGHTNESS 8
RESET_HG
```

---

## Security Considerations

### Firmware
- **No sensitive data**: Do not hardcode WiFi passwords in `config.h`
- **Use config_local.h**: For any credentials (gitignored by default)
- **Input validation**: All serial commands validate parameter ranges
- **Buffer safety**: No dynamic memory allocation in main loop

### Web UI
- **Web Serial API**: Requires user interaction to select port (secure by design)
- **No network requests**: Web UI is entirely client-side
- **localStorage**: Only stores brightness preference (no sensitive data)

---

## Common Issues & Solutions

### Firmware Won't Upload
- Check processor setting: Try "ATmega328P (Old Bootloader)" for Nano clones
- Verify COM port is correct and not in use by Serial Monitor

### Web UI Won't Connect
- Must use Chrome 89+ or Edge 89+ (Web Serial API requirement)
- Ensure Arduino is programmed with 9600 baud serial
- Check USB cable supports data (not charge-only)

### LED Matrices Not Responding
- Verify SPI wiring: D10→CS, D11→DIN, D13→CLK
- Check power: MAX7219 needs 5V, can share with Arduino
- Test with simple LedControl example first

### Orientation Detection Off
- MPU6050 must be mounted with consistent orientation
- Check I2C wiring: A4→SDA, A5→SCL
- Add pull-up resistors (4.7kΩ) if needed for longer wires

---

## Adding New Features

### Adding a New Mode
1. Define mode constant in `rishi-file.ino`: `#define MODE_NEW 2`
2. Add mode handler functions (see `DiceMode.h/cpp` for template)
3. Update `handleSerialCommands()` to accept `SET_MODE new`
4. Add UI controls in `web-ui/index.html`
5. Add mode handler in `web-ui/js/modes.js`

### Adding a New Serial Command
1. Add command handler in `handleSerialCommands()` in `rishi-file.ino`
2. Send response with `Serial.println("OK ...")` or `Serial.println("ERR ...")`
3. Add API method in `web-ui/js/api.js`
4. Wire to UI in `web-ui/js/app.js` or `modes.js`

### Modifying LED Display
- Use `LedControl` methods: `setLed()`, `setRow()`, `clearDisplay()`
- Access matrices by address: `0` (bottom), `1` (top)
- Set rotation: `lc.setRotation(angle)` for orientation-aware rendering

---

## Documentation References

| File | Purpose |
|------|---------|
| `README.md` | User guide and feature overview |
| `ARCHITECTURE_DIAGRAM.md` | System architecture diagrams |
| `COMMANDS.md` | Serial protocol reference |
| `web-ui/README.md` | Web UI documentation index |
| `web-ui/HARDWARE_INTEGRATION.md` | Hardware specs and pinout |
| `web-ui/DESIGN_REFERENCE.md` | CSS customization guide |

---

**Last Updated**: 2026-02-06  
**Firmware Version**: 1.0.2-OPT  
**Web UI Version**: 1.0 Glassmorphism
