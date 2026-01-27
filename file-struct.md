# HourGlass Project File Structure

## Overview
Complete directory tree of the HourGlass project - an Arduino-based programmable hourglass with multiple modes (Clock, Hourglass, Dice, Flip Counter) controlled via USB serial and Web UI.

---

## Full Directory Tree

```
HourGlass/
│
├── .agent/                          # Agent configuration directory
│   └── skills/
│
├── .cursor/                         # Cursor IDE configuration
│   └── commands/
│
├── .git/                            # Git repository metadata
│
├── .gitignore                       # Git ignore rules
│
├── .shared/                         # Shared resources
│   └── ui-ux-pro-max/
│
├── ARCHITECTURE.md                  # System architecture documentation
├── README.md                        # Project overview and introduction
├── QUICK_START.md                   # Quick start guide for setup
├── PROJECT_SUMMARY.md               # Implementation status and features
├── FIRMWARE_UPDATES.md              # Firmware changelog and history
├── final-project.md                 # Final project documentation
│
├── package.json                     # Node.js project configuration
├── package-lock.json                # Dependency lock file
│
├── docs/                            # Documentation directory
│   ├── API_DOCUMENTATION.md         # Serial protocol API reference
│   └── SETUP_GUIDE.md               # Detailed setup instructions
│
├── firmware/                        # Arduino/ESP8266 firmware code
│   ├── main.ino                     # Main Arduino sketch entry point
│   ├── config.h                     # Configuration header (pins, WiFi, etc.)
│   │
│   ├── Button.h                     # Button class header
│   ├── Button.cpp                   # Button debounce implementation
│   │
│   ├── Delay.h                      # Non-blocking delay class header
│   ├── Delay.cpp                    # Non-blocking delay implementation
│   │
│   ├── ClockMode.h                  # Clock mode class header
│   ├── ClockMode.cpp                # Clock mode implementation
│   │
│   ├── HourglassMode.h              # Hourglass simulation class header
│   ├── HourglassMode.cpp            # Hourglass particle physics implementation
│   │
│   ├── DiceMode.h                   # Dice roller mode class header
│   ├── DiceMode.cpp                 # Dice mode implementation
│   │
│   ├── FlipCounterMode.h            # Flip counter mode class header
│   ├── FlipCounterMode.cpp          # Flip counter implementation
│   │
│   ├── MPU6050.h                    # IMU sensor class header
│   ├── MPU6050.cpp                  # IMU sensor implementation
│   │
│   ├── display/                     # LED display drivers
│   │   ├── LedControl.h             # MAX7219 LED matrix controller header
│   │   └── LedControl.cpp           # MAX7219 LED matrix implementation
│   │
│   ├── modes/                       # Reserved for mode plugins
│   ├── network/                     # Reserved for network code
│   ├── sensors/                     # Reserved for sensor drivers
│   └── serial/                      # Serial protocol handlers
│       ├── SerialProtocol.h         # Serial protocol header
│       └── SerialProtocol.cpp       # Serial protocol implementation
│
├── web-ui/                          # Web-based user interface
│   ├── index.html                   # Main HTML file
│   │
│   ├── css/                         # Stylesheets
│   │   └── styles.css               # Main stylesheet
│   │
│   └── js/                          # JavaScript modules
│       ├── app.js                   # Main application controller
│       ├── api.js                   # Device communication API
│       ├── serial.js                # Web Serial API wrapper
│       ├── display.js               # Display rendering module
│       └── modes.js                 # Mode-specific UI handlers
│
├── tests/                           # Test configuration
│   └── testsprite.config.json       # TestSprite test configuration
│
├── testsprite_tests/                # TestSprite generated test outputs
│   └── tmp/                         # Temporary test artifacts
│       ├── config.json              # Test configuration snapshot
│       ├── code_summary.json        # Code analysis summary
│       └── prd_files/               # Generated PRD files for testing
│           ├── API_DOCUMENTATION.md # API docs copy
│           └── SETUP_GUIDE.md       # Setup guide copy
│
└── node_modules/                    # NPM dependencies (generated)

```

---

## Directory Descriptions

### Root Level
- **Documentation Files**: Project overview, architecture, setup, and changelog
- **package.json**: Node.js configuration for local development server
- **.gitignore**: Excludes build artifacts and node_modules from version control

### `/firmware` - Arduino/ESP8266 Code
Core firmware components:
- **main.ino**: Entry point that initializes modes and main loop
- **config.h**: Centralized configuration (pins, thresholds, WiFi)
- **Mode Classes**: ClockMode, HourglassMode, DiceMode, FlipCounterMode
- **Hardware Drivers**: Button, MPU6050 (IMU), LedControl (LED matrices)
- **Utilities**: Delay (non-blocking), SerialProtocol

### `/web-ui` - Frontend Application
Web interface for device control:
- **index.html**: Single-page application shell
- **api.js**: Communication layer with device via Web Serial API
- **serial.js**: Web Serial API wrapper and connection management
- **modes.js**: UI logic for mode-specific controls
- **display.js**: Real-time display visualization
- **app.js**: Main application orchestration

### `/docs` - User Documentation
- **API_DOCUMENTATION.md**: Serial protocol commands and responses
- **SETUP_GUIDE.md**: Hardware assembly and software installation

### `/tests` - Test Configuration
- **testsprite.config.json**: Configuration for automated test generation

### `/testsprite_tests/tmp` - Test Artifacts
Generated test files and summaries from TestSprite testing framework

---

## Key File Dependencies

```
main.ino (entry point)
  ├── config.h (shared configuration)
  ├── Button.h
  ├── ClockMode.h
  ├── HourglassMode.h
  ├── DiceMode.h
  ├── FlipCounterMode.h
  ├── MPU6050.h
  ├── display/LedControl.h
  └── serial/SerialProtocol.h

web-ui/index.html
  └── js/app.js
      ├── api.js
      ├── serial.js
      ├── modes.js
      └── display.js

docs/API_DOCUMENTATION.md
  └── describes protocol handled by serial/SerialProtocol.cpp
```

---

## File Statistics

| Category | Count |
|----------|-------|
| Arduino Source Files (.ino/.cpp/.h) | 30+ |
| Web UI Files (HTML/CSS/JS) | 6 |
| Documentation Files (.md) | 8 |
| Configuration Files (.json) | 3 |
| Build/Dependency Files | 2 |

---

## Setup Instructions

1. **Firmware Development**:
   - Open `/firmware/main.ino` in Arduino IDE
   - Ensure `/firmware/config.h` is configured with correct pins
   - Upload to ESP8266/Arduino board

2. **Web UI Development**:
   - Install dependencies: `npm install`
   - Start local server: `npm run serve-node`
   - Access at `http://localhost:8080`

3. **Testing**:
   - Configure `/tests/testsprite.config.json`
   - Run test generation with TestSprite MCP tool

---

## Notes

- **Relative Imports**: Firmware uses relative paths; folder structure matters
- **ESP8266 Pins**: Defined in `firmware/config.h`; adjust for different boards
- **Web Serial API**: Requires HTTPS or localhost for browser security
- **Non-blocking Delay**: Use `NonBlockDelay` class instead of `delay()` in firmware
- **Node Modules**: Generated directory (~500MB); excluded from git

