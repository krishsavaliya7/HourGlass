# Smart Hourglass & Clock System - Architecture Documentation

## 1. System Overview

The Smart Orientation-Based Hourglass & Clock System consists of three main components:

1. **Hardware Layer**: Arduino-based embedded device with sensors and displays
2. **Firmware Layer**: Embedded code running on Arduino/ESP8266/ESP32
3. **Host UI Layer**: Simple browser-based UI running on a PC or mobile device, connected **only via USB data cable (serial)** – no WiFi or Bluetooth

The device exposes a **serial command protocol** over USB. The host UI (on PC or mobile with OTG) uses this serial link to control modes, set time, and read status.

## 2. Hardware Architecture

### 2.1 Components
- **Microcontroller**: Arduino Nano / ESP8266 / ESP32 (used only as a USB-serial device in this project; WiFi/Bluetooth features are not required)
- **Display**: 2× MAX7219 8×8 LED Matrix (16×8 combined)
- **Sensors**: MPU-6050 (Gyroscope + Accelerometer)
- **Input**: Push Button
- **Power**: Li-ion Battery + Charging Module + Boost Converter

### 2.2 Pin Configuration
```
Example ESP8266/ESP32 Pin Mapping:
- GPIO 5 (D1)  → MAX7219 DATA IN
- GPIO 4 (D2)  → MAX7219 CLK
- GPIO 0 (D3)  → MAX7219 LOAD/CS
- GPIO 2 (D4)  → Push Button
- GPIO 14 (D5) → MPU-6050 SCL (I²C)
- GPIO 12 (D6) → MPU-6050 SDA (I²C)
- GPIO 13 (D7) → Buzzer (particle drop feedback & completion alarm)
```

## 3. Software Architecture

### 3.1 Firmware Structure
```
firmware/
├── main.ino              # Main program entry (state machine + serial protocol)
├── config.h              # Configuration constants
├── modes/
│   ├── ClockMode.h/cpp   # Clock mode implementation
│   ├── HourglassMode.h/cpp
│   ├── DiceMode.h/cpp
│   └── FlipCounterMode.h/cpp
├── sensors/
│   ├── MPU6050.h/cpp     # MPU-6050 sensor interface
│   └── Button.h/cpp      # Button input handler
├── display/
│   └── LedControl.h/cpp  # LED matrix control
└── serial/
    └── SerialProtocol.h/cpp    # Serial command parser
### 3.2 Host Web Interface Structure (USB Serial)
```
web-ui/
├── index.html            # Simple UI page (runs in Chrome/Edge on PC/mobile)
├── css/
│   └── styles.css        # Basic styling
├── js/
│   ├── app.js            # Main application logic
│   ├── serial.js         # Web Serial communication (USB cable)
│   ├── display.js        # LED matrix visualization (16×8 virtual grid)
│   └── modes.js          # Mode-specific UI logic
└── assets/
    └── icons/            # UI icons (optional)
```

The host UI uses the **Web Serial API** (supported in Chromium-based browsers) to open the Arduino’s virtual COM port over the USB data cable.

## 4. Communication Protocol (USB Serial)

All communication between the host UI and the device happens over the **USB serial port** (e.g. `COMx` on Windows, `/dev/ttyUSBx` on Linux, via OTG on Android).

### 4.1 Command Format (Host → Device)

Commands are sent as ASCII text lines terminated by `\n`. Example commands:

- **Set mode**
  ```
  SET_MODE clock
  SET_MODE hourglass
  SET_MODE dice
  SET_MODE flip
  ```

- **Set clock time**
  ```
  SET_TIME <hours> <minutes>
  Example: SET_TIME 14 30
  ```

- **Set hourglass duration**
  ```
  SET_HG <hours> <minutes>
  Example: SET_HG 0 5
  ```

- **Roll dice**
  ```
  ROLL_DICE
  ```

- **Get status**
  ```
  GET_STATUS
  ```

### 4.2 Response Format (Device → Host)

The device responds with one or more text lines. Two options are possible; the recommended one is a single JSON line per response:

```json
{"mode":"clock","time":"14:30","orientation":"horizontal","hourglassProgress":42,"diceValue":3,"flipCount":5}
```

Alternatively, a simple key=value format can be used:

```
MODE=clock
TIME=14:30
ORIENTATION=horizontal
HG_PROGRESS=42
DICE=3
FLIP=5
```

The host UI parses these responses and updates the display accordingly.

## 5. Operating Modes

### 5.1 Clock Mode
- **Horizontal**: Digital time display (HH:MM) across 16×8
- **Vertical**: Dot-based time (hours on left matrix, minutes on right)
- **Control**: `SET_MODE clock`, `SET_TIME h m`, `GET_STATUS`

### 5.2 Hourglass Mode
- Realistic particle physics animation (matches reference implementation)
- Diagonal slice-based particle processing with random direction
- Sand animation flowing top to bottom with proper collision detection
- Reversible on flip (gravity-based top/bottom detection)
- Configurable duration
- **Buzzer feedback**: Short beep on particle drop, alarm when complete
- **Control**: `SET_MODE hourglass`, `SET_HG h m`, `RESET_HG`, `GET_STATUS`

### 5.3 Dice Mode
- Random 1-6 on button press or shake
- Visual dice pattern on display
- **Control**: `SET_MODE dice`, `ROLL_DICE`, `GET_STATUS`

### 5.4 Flip Counter Mode
- Counts complete vertical flips
- Debounced flip detection
- **Control**: `SET_MODE flip`, `GET_STATUS`

## 6. Button Functionality

- **Short Press**: Mode-specific action (dice roll, etc.)
- **Long Press (2s)**: Cycle to next mode
- Mode cycle: Clock → Hourglass → Dice → Flip Counter → Clock

## 7. Host UI Features (USB Serial)

### 7.1 Dashboard
- Real-time LED matrix visualization
- Current mode indicator
- Orientation display
- Battery level indicator (optional / simulated)
- Mode selection buttons

### 7.2 Mode-Specific Controls
- **Clock**: Time setting interface (sends `SET_TIME`)
- **Hourglass**: Duration selector, reset (sends `SET_HG`)
- **Dice**: Roll button, history (sends `ROLL_DICE`)
- **Flip Counter**: Reset button (if implemented), count display (`GET_STATUS`)

### 7.3 Settings
- Serial port selection (choose Arduino COM port)
- Display brightness (if exposed via serial command)
- Button sensitivity (configured in firmware)
- Orientation calibration (optional)

## 8. Security Considerations

- USB serial link is **local-only** (no network exposure)
- Only trusted PCs/mobiles with physical access can control the device
- Firmware performs basic validation of incoming serial commands to avoid crashes

## 9. Error Handling

- Serial disconnection handling (host can detect port closed and prompt user)
- Sensor failure detection
- Display error recovery
- Graceful degradation if invalid commands are received

## 10. Testing Strategy

- Unit tests for mode logic
- Integration tests for serial protocol handling
- Hardware-in-the-loop tests
- TestSprite automated UI tests (against the local `web-ui/index.html` using a mock serial layer)


