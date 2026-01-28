# Smart Hourglass - Hardware Integration Guide

## Hardware Specifications

### Microcontroller
- **Model**: Arduino Nano R3
- **Processor**: ATmega328P
- **Clock Speed**: 16 MHz
- **Flash Memory**: 32 KB
- **RAM**: 2 KB
- **EEPROM**: 1 KB
- **GPIO Pins**: 22 (14 digital + 8 analog)

### LED Display System
- **Count**: 2× 8×8 LED Matrix Modules
- **Driver IC**: Max7219
- **Total Display Resolution**: 16×8 pixels (stacked vertically)
- **Connection**: SPI Serial Interface
- **Pins Used**:
  - Data In (DIN): GPIO Pin 12 (PB4)
  - Clock (CLK): GPIO Pin 11 (PB3)
  - Load (CS): GPIO Pin 10 (PB2)

### Sensors
- **Accelerometer/Gyroscope**: MPU6050
- **Interface**: I2C (TWI)
- **Functionality**: 
  - Device orientation detection
  - Flip counter (vertical motion detection)
  - Tilt-based mode switching
- **I2C Pins**:
  - SDA: A4 (PC4)
  - SCL: A5 (PC5)

### Input
- **Button**: Single Push Button
- **Pin**: GPIO Pin 2 (PD2)
- **Function**: Mode selection with debounce

### Serial Communication
- **Protocol**: UART Serial
- **Baud Rate**: 9600 bps
- **Data Format**: 8 data bits, 1 stop bit, no parity
- **Connection**: USB via CH340G chip (on Arduino Nano board)
- **TX/RX Pins**: Hardware serial on PD0/PD1

## Web UI Connection

### Browser Compatibility
- Chrome 89+
- Edge 89+
- Opera 76+
- *(Firefox support pending)*

### Connection Flow
1. User clicks "🔌 Connect" button in UI
2. Browser opens port selection dialog
3. User selects Arduino Nano R3 COM port
4. Web Serial API establishes connection @9600 baud
5. Firmware sends handshake message
6. UI displays "🟢 Connected" status

### Serial Protocol

#### Message Format
```
Command: <TYPE>:<PARAM1>:<PARAM2>:<PARAM3>\n
Response: <STATUS>:<DATA>\n
```

#### Commands

| Command | Parameters | Function |
|---------|-----------|----------|
| `MODE:X` | X = 0,1,2,3 | Set mode (Clock/Hourglass/Dice/FlipCounter) |
| `TIME:HH:MM` | Hours, Minutes | Set clock time |
| `HOUR:HH:MM` | Hours, Minutes | Set hourglass duration |
| `BRIGHT:X` | 0-15 | Set LED brightness |
| `RESET` | - | Reset current mode |
| `STATUS` | - | Request status JSON |

#### Responses

| Response | Data | Meaning |
|----------|------|---------|
| `OK:mode,value` | Current state | Command successful |
| `ERR:message` | Error text | Invalid command |
| `STATUS:json` | Full state | Device status |
| `ORIENT:angle,label` | Orientation | Device tilt/orientation |

## LED Display Pinout Diagram

```
Arduino Nano R3
┌─────────────────────────┐
│ D10 (CS)   ─────────┐   │
│ D11 (CLK)  ─────┐   │   │
│ D12 (DIN)  ──┐  │   │   │
│             │  │   │   │
│            LED Display Chain
│          ┌──────────┐
│          │ Max7219  │
│          │ Matrix A │
│          └──────────┘
│              │
│          ┌──────────┐
│          │ Max7219  │
│          │ Matrix B │
│          └──────────┘
└─────────────────────────┘
```

## Firmware Architecture

### Files
- `main.ino` - Main sketch
- `config.h` - Pin definitions and constants
- `display/LedControl.h` - Max7219 driver
- `serial/SerialProtocol.h` - Protocol handler
- `MPU6050.h` - Sensor interface
- `modes/ClockMode.h` - Clock display logic
- `modes/HourglassMode.h` - Hourglass timer logic
- `modes/DiceMode.h` - Dice roller logic
- `modes/FlipCounterMode.h` - Flip counter logic

### Mode Descriptions

#### Clock Mode
- Displays time in HH:MM format
- Horizontal: Digital display on both matrices
- Vertical: Dot-based visualization

#### Hourglass Mode
- Countdown timer with visual progress
- Sand animation effect on LED matrices
- Settable duration (1 minute - 23:59)

#### Dice Mode
- Random 1-6 dice roll
- Dot pattern display
- Roll history tracking

#### Flip Counter Mode
- Counts vertical flips
- Requires MPU6050 for motion detection
- Reset button to clear counter

## Troubleshooting

### Connection Issues
1. Ensure USB cable is properly connected
2. Check if Arduino drivers are installed (CH340G)
3. Try different USB port
4. Restart browser if Web Serial API acts up

### LED Display Issues
1. Check SPI connections (D10, D11, D12)
2. Verify Max7219 brightness setting
3. Ensure power supply is stable
4. Check for loose connector pins

### Sensor Issues
1. Verify I2C connections (A4, A5)
2. Check MPU6050 address (0x68 default)
3. Ensure proper power supply (+5V, GND)
4. Test with sensor calibration sketch

## Testing Checklist

- [ ] USB connection established
- [ ] LED matrices display correctly
- [ ] Serial communication at 9600 baud
- [ ] Mode switching works (all 4 modes)
- [ ] Brightness control functional
- [ ] Orientation detection responds to tilt
- [ ] Flip counter detects motion
- [ ] Clock time can be set
- [ ] Hourglass timer counts down
- [ ] Dice rolls show random numbers

## Notes

- The firmware runs on a single thread with non-blocking delays
- LED refresh rate: ~60 Hz
- Serial read loop runs continuously when connected
- All mode timings are interrupt-safe
