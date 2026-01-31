# Setup Guide

## Prerequisites

### Hardware
- Arduino Nano, ESP8266, ESP32, or similar development board with USB serial
- 2× MAX7219 8×8 LED Matrix displays
- MPU-6050 Gyroscope/Accelerometer module
- Push button
- **Buzzer** (piezo buzzer or active buzzer module)
- Li-ion battery + charging module + boost converter
- Jumper wires
- Breadboard (optional)

### Software
- Arduino IDE 1.8+ or PlatformIO
- Required Arduino libraries (see Installation section)

## Installation

### Step 1: Install Arduino IDE

1. Download Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)
2. Install and launch Arduino IDE

### Step 2: Install Board Support (if needed)

- **Arduino Nano / ATmega328-based boards**: use the built-in **Arduino AVR Boards**
- **ESP8266 / ESP32** (optional): you can also use these, but WiFi/Bluetooth is **not required** for this project. Follow their standard board installation guides if you choose them.

### Step 3: Install Required Libraries

Go to Sketch → Include Library → Manage Libraries and install:

1. **MPU6050** (by Electronic Cats or similar)

### Step 4: Hardware Connections

#### Example ESP8266 Pin Mapping:
```
ESP8266    Component
--------   ---------
GPIO 5     MAX7219 DATA IN
GPIO 4     MAX7219 CLK
GPIO 0     MAX7219 LOAD/CS
GPIO 2     Push Button
GPIO 12    MPU-6050 SDA (I²C)
GPIO 14    MPU-6050 SCL (I²C)
GPIO 13    Buzzer (particle drop feedback & completion alarm)
3.3V       Power for sensors/displays
GND        Common ground
```

#### MAX7219 Connections:
- Connect both MAX7219 modules in daisy chain
- VCC → 5V (via boost converter)
- GND → GND
- DIN → GPIO 5
- CLK → GPIO 4
- CS → GPIO 0

#### MPU-6050 Connections:
- VCC → 3.3V
- GND → GND
- SDA → GPIO 12
- SCL → GPIO 14

### Step 5: Configure Firmware

1. Open `firmware/main.ino` in Arduino IDE
2. Edit `firmware/config.h`:
   ```cpp
   // Set your pin mappings and thresholds
   #define PIN_DATAIN 5
   #define PIN_CLK    4
   // etc.
   ```
3. Adjust pin mappings if needed (match your actual wiring)
4. Set display intensity (0–15) and sensor thresholds as desired

### Step 6: Upload Firmware

1. Select board: Tools → Board → ESP8266/ESP32 → Your Board Model
2. Select port: Tools → Port → COMx (Windows) or /dev/ttyUSBx (Linux/Mac)
3. Click Upload button
4. Wait for upload to complete

### Step 7: Start the Host Web UI (USB Serial)

1. From the project root, start a static web server (for example):
   ```bash
   npm install
   npm run serve-node
   # or
   python -m http.server 8080
   ```
2. Open a Chromium-based browser and navigate to:  
   `http://localhost:8080/web-ui/index.html`
3. Click the **Connect** button in the UI and select your device’s serial port.
4. If prompted, grant permission for the website to access the serial port.

### Step 8: Verify Operation

1. **Check Serial Monitor** for initialization messages (optional)
2. **Test Host UI**:
   - Verify connection status shows as connected
   - Test mode switching (Clock, Hourglass, Dice, Flip Counter)
   - Observe virtual LED matrix and orientation indicators

3. **Test Physical Controls**:
   - Short press button (mode-specific action)
   - Long press button (cycle mode)
   - Tilt device (orientation change)
   - Shake device (dice roll trigger)

## Troubleshooting

### Device Not Found
- **Check USB connection**: Ensure you are using a **data** cable, not a charge-only cable
- **Check serial port**: Confirm the device appears as a COM/USB device in your OS
- **Browser permissions**: Make sure the browser has permission to use Web Serial and the selected port

### Display Not Working
- **Check connections**: Verify SPI connections (DATA, CLK, CS)
- **Check power**: Ensure 5V power supply adequate
- **Check MAX7219**: Test with LED test pattern
- **Check intensity**: Increase brightness in settings

### Sensor Not Responding
- **Check I²C connections**: Verify SDA/SCL connections
- **Check power**: Ensure 3.3V power to MPU-6050
- **Check pull-up resistors**: I²C may need 4.7kΩ pull-ups
- **Check address**: Verify MPU-6050 I²C address (default 0x68)

### Web UI Not Loading
- **Check local server**: Ensure the static server (npm/http-server or python) is running
- **Check URL**: Confirm you opened `http://localhost:8080/web-ui/index.html` (or the correct port)
- **Check JavaScript console**: Open browser DevTools for errors
- **Try different browser**: Use a recent version of Chrome/Edge with Web Serial support

### Compilation Errors
- **Check library versions**: Ensure compatible library versions
- **Check board selection**: Verify correct ESP8266/ESP32 board
- **Check includes**: Ensure all header files in correct locations
- **Check Arduino IDE version**: Update to latest version

## Advanced Configuration

### Custom Pin Mapping
Edit `firmware/config.h`:
```cpp
#define PIN_DATAIN 5
#define PIN_CLK 4
// ... etc
```

### Sensor Calibration
Some sensors may require calibration. Adjust thresholds in `config.h`:
```cpp
#define ACC_THRESHOLD_LOW 300
#define ACC_THRESHOLD_HIGH 360
```

## Next Steps

- Read [ARCHITECTURE.md](../ARCHITECTURE.md) for system design
- Read [API_DOCUMENTATION.md](API_DOCUMENTATION.md) for API details
- Customize modes and behaviors
- Add additional features

## Support

For issues:
1. Check Serial Monitor output
2. Review error messages
3. Check hardware connections
4. Review documentation
5. Open GitHub issue with details

