# Smart Hourglass Firmware v1.0.0

Professional-grade firmware for a smart hourglass device with multiple modes, motion sensing, and USB serial control.

## Features

- **Multiple Operating Modes**
  - 🕐 Clock Mode - Digital time display
  - ⏳ Hourglass Mode - Animated particle timer
  - 🎲 Dice Mode - Roll virtual dice
  - 🔄 Flip Counter - Count device flips

- **Motion Sensing**
  - MPU-6050 accelerometer/gyroscope support
  - Analog sensor fallback (Arduino Nano compatible)
  - Orientation detection (horizontal/vertical)
  - Shake and flip detection

- **USB Serial Control**
  - Full remote control via serial commands
  - JSON status reporting
  - Real-time display state query

- **Production Ready**
  - Memory-safe string handling
  - Overflow protection
  - Robust error handling
  - Configurable debug output

## Hardware Requirements

### Microcontroller
- Arduino Nano, Uno, or compatible
- ESP8266 (NodeMCU, D1 Mini)
- ESP32

### Components
- 2× MAX7219 8x8 LED Matrix displays
- MPU-6050 accelerometer/gyroscope (optional with analog fallback)
- Push button
- Buzzer
- Appropriate power supply

### Pin Configuration

Default pin assignments (customizable in `config.h`):

```
Display (MAX7219):
- DATA IN: Pin 5 (D1 on ESP8266)
- CLK:     Pin 4 (D2 on ESP8266)
- LOAD:    Pin 0 (D3 on ESP8266)

MPU-6050 (I2C):
- SDA:     Pin 12 (D6 on ESP8266)
- SCL:     Pin 14 (D5 on ESP8266)

Controls:
- Button:  Pin 2 (D4 on ESP8266)
- Buzzer:  Pin 13 (D7 on ESP8266)

Analog Fallback (if MPU-6050 unavailable):
- AccelX:  A1
- AccelY:  A2
- AccelZ:  A3
```

## Installation

### Arduino IDE

1. **Install Required Libraries**
   ```
   - Wire (built-in)
   - None! All code is self-contained
   ```

2. **Board Setup**
   - Select your board: `Tools > Board > [Your Board]`
   - Select correct COM port: `Tools > Port`
   - For ESP8266/ESP32, install board support first

3. **Upload**
   - Open `main.ino`
   - Click Upload

### PlatformIO

```ini
[env:nano]
platform = atmelavr
board = nanoatmega328
framework = arduino

[env:esp8266]
platform = espressif8266
board = d1_mini
framework = arduino
```

## Configuration

All settings in `config.h`:

### Pin Assignments
```cpp
#define PIN_DATAIN 5      // MAX7219 DATA
#define PIN_CLK 4         // MAX7219 CLK
#define PIN_LOAD 0        // MAX7219 LOAD
#define PIN_BUTTON 2      // Push button
#define PIN_BUZZER 13     // Buzzer
#define PIN_SDA 12        // I2C SDA
#define PIN_SCL 14        // I2C SCL
```

### Display Settings
```cpp
#define DISPLAY_INTENSITY 8     // 0-15, brightness
#define ROTATION_OFFSET 90      // Display rotation
```

### Timing
```cpp
#define DELAY_FRAME 100              // Main loop delay (ms)
#define BUTTON_LONG_PRESS_MS 2000    // Long press duration
#define DEBOUNCE_DELAY 50            // Button debounce
```

### Debug Output
```cpp
#define DEBUG_OUTPUT 1    // Set to 0 to disable serial debug
#define SERIAL_BAUD 9600  // Serial communication speed
```

### Hourglass Settings
```cpp
#define HOURGLASS_PARTICLE_COUNT 60  // Particle count
#define HOURGLASS_TONE_FREQ 440      // Buzzer frequency (Hz)
#define HOURGLASS_ALARM_CYCLES 5     // Alarm beep cycles
```

## Usage

### Button Controls

**Short Press:**
- Clock Mode: No action
- Hourglass Mode: Reset timer
- Dice Mode: Roll dice
- Flip Counter: No action

**Long Press (2 seconds):**
- Cycle to next mode

### Serial Commands

Connect at 9600 baud. Commands are case-insensitive.

#### Mode Control
```
SET_MODE CLOCK          - Switch to clock mode
SET_MODE HOURGLASS      - Switch to hourglass mode
SET_MODE DICE           - Switch to dice mode
SET_MODE FLIPCOUNTER    - Switch to flip counter mode
```

#### Clock Mode
```
SET_TIME 14 30          - Set time to 14:30 (24-hour format)
```

#### Hourglass Mode
```
SET_HG 0 5              - Set 5 minute timer
SET_HG 1 30             - Set 1 hour 30 minute timer
RESET_HG                - Reset hourglass
```

#### Dice Mode
```
ROLL_DICE               - Roll the dice
Response: {"diceValue":4}
```

#### Flip Counter Mode
```
GET_FLIP_COUNT          - Get current count
RESET_FLIP              - Reset counter
```

#### Status & Info
```
GET_STATUS              - Get current mode and state
Response: {"mode":"hourglass","orientation":"horizontal","hourglassProgress":45}

GET_ORIENTATION         - Get sensor data
Response: {"angle":90,"x":0.05,"y":0.02,"z":0.98}

GET_DISPLAY             - Get raw LED matrix state
Response: {"matrixA":[[...]],"matrixB":[[...]]}

SET_BRIGHTNESS 10       - Set display brightness (0-15)
```

### Response Format

**Success:**
```
OK
```

**Error:**
```
ERR [error message]
```

**JSON Data:**
```json
{"mode":"hourglass","orientation":"horizontal","hourglassProgress":67}
```

## Code Architecture

### Class Structure

```
main.ino
├── LedControl          - LED matrix driver
├── MPU6050            - Motion sensor interface
├── Button             - Debounced button handler
├── SerialProtocol     - Command parser
├── NonBlockDelay      - Non-blocking timers
└── Modes
    ├── ClockMode      - Digital clock display
    ├── HourglassMode  - Particle animation timer
    ├── DiceMode       - Dice roller
    └── FlipCounterMode - Flip counter
```

### Key Improvements (v1.0.0)

- ✅ Memory-safe string handling
- ✅ Fixed button long-press bug
- ✅ Const correctness throughout
- ✅ Eliminated magic numbers
- ✅ Integer overflow protection
- ✅ Improved I2C error handling
- ✅ NaN protection in calculations
- ✅ Serial buffer overflow detection
- ✅ Static variable race condition fixes

See `CHANGELOG.md` for detailed improvements.

## Troubleshooting

### MPU-6050 Not Detected
```
Symptom: "MPU-6050 initialization failed! Using fallback."
Solution: 
1. Check I2C connections (SDA/SCL)
2. Verify MPU-6050 address (default 0x68)
3. System will use analog fallback automatically
```

### Display Not Working
```
Symptom: LEDs stay off or show random patterns
Solution:
1. Check MAX7219 connections (DIN, CLK, CS)
2. Verify power supply (5V, sufficient current)
3. Check NUM_MATRICES and matrix addresses
```

### Button Not Responding
```
Symptom: Button presses not detected
Solution:
1. Verify button pin configuration
2. Check INPUT_PULLUP is working
3. Adjust DEBOUNCE_DELAY if needed
```

### Serial Communication Issues
```
Symptom: Commands not recognized
Solution:
1. Verify baud rate (9600)
2. Use uppercase commands
3. Check line endings (CR or LF)
4. Commands are case-insensitive but traditionally uppercase
```

### Memory Issues
```
Symptom: Crashes after running a while
Solution:
1. Set DEBUG_OUTPUT to 0 to save RAM
2. Reduce HOURGLASS_PARTICLE_COUNT
3. Monitor free RAM during operation
```

## Development

### Building from Source

```bash
# Clone repository
git clone [repository-url]
cd firmware

# Open in Arduino IDE
arduino main.ino

# Or use PlatformIO
pio run -e nano
pio run -e esp8266
```

### Adding New Modes

1. Create mode class inheriting pattern from existing modes
2. Implement: `init()`, `enter()`, `exit()`, `update()`
3. Add mode constant to `config.h`
4. Register in `main.ino` setup and loop

### Testing

Run through test scenarios in `CHANGELOG.md`:
- Overflow test (99 hour hourglass)
- Button spam test
- I2C recovery test
- Serial flood test
- 24-hour memory leak test

## Performance

- **Loop Frequency:** 10 Hz (configurable via DELAY_FRAME)
- **Memory Footprint:** ~2KB RAM, ~20KB Flash (Arduino Nano)
- **Startup Time:** ~1 second
- **I2C Polling:** Every loop iteration
- **Button Response:** 50ms debounce delay

## Safety & Reliability

### Compile-Time Checks
- Display intensity validation
- Frame delay bounds checking
- Pin conflict detection

### Runtime Protection
- I2C failure recovery
- NaN detection in calculations
- Integer overflow prevention
- Serial buffer overflow protection
- Watchdog support (can be added)

## License

[Add your license here]

## Credits

- Firmware optimizations based on embedded systems best practices
- Motion physics inspired by classic hourglass designs
- Serial protocol follows standard IoT patterns

## Version History

- **v1.0.0** (2026-01-28)
  - Production-ready release
  - All code review improvements applied
  - Comprehensive error handling
  - Full documentation

## Support

For issues, questions, or contributions:
- File issues on GitHub
- Read the CHANGELOG.md for recent changes
- Check config.h for customization options

---

**Happy Building! 🎉**
