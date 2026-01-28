# Smart Hourglass Firmware - ArduinoDroid Edition

## 📱 Optimized for ArduinoDroid Mobile App

This version has a **flat folder structure** that works perfectly with ArduinoDroid!

---

## Quick Start Guide for ArduinoDroid

### Step 1: Extract Files
- Extract this ZIP file on your Android phone
- You'll get a folder called `firmware_mobile` with all files in one place

### Step 2: Open in ArduinoDroid
1. Open **ArduinoDroid** app
2. Tap **"Sketch"** → **"Open"**
3. Navigate to `firmware_mobile` folder
4. Select **`main.ino`**
5. When asked about additional files, tap **"Yes"** or **"Open All"**

### Step 3: Configure Board
1. Tap the **board icon** (top toolbar)
2. Select **"Arduino Nano"**
3. Select **"ATmega328P"** as processor
4. Select **"Old Bootloader"** if new bootloader doesn't work

### Step 4: Upload
1. Connect Arduino Nano via **OTG cable**
2. Tap the **Upload button** (arrow icon)
3. Wait for compilation and upload

### Step 5: Test (Without Hardware)
1. Tap **"Serial Monitor"** icon
2. Set baud rate to **9600**
3. Type: `GET_STATUS`
4. You should see JSON response!

---

## ✅ What's Different from Desktop Version?

- **Flat folder structure** - All files in one folder (no subdirectories)
- **Same functionality** - Works exactly the same
- **ArduinoDroid compatible** - No "file not found" errors

---

## 🔧 Testing Without Hardware

You can upload this code to Arduino Nano **without any external components** connected!

**What happens:**
- ✅ Code compiles and uploads successfully
- ✅ Serial communication works
- ⚠️ LED displays won't show anything (not connected)
- ⚠️ MPU6050 will use analog fallback (not connected)

**Serial Monitor will show:**
```
=== Smart Hourglass System ===
Firmware Version: 1.0.0
Initializing display...
Display initialized
Initializing MPU-6050...
MPU-6050 initialization failed! Using fallback.
System initialized successfully!
Ready for commands via USB Serial
```

---

## 📋 Serial Commands to Test

Try these in Serial Monitor (9600 baud):

```
GET_STATUS              → Get current mode and state
SET_MODE CLOCK          → Switch to clock mode
SET_MODE HOURGLASS      → Switch to hourglass mode
SET_MODE DICE           → Switch to dice mode
ROLL_DICE               → Roll the dice (returns random 1-6)
GET_ORIENTATION         → Get sensor data
SET_BRIGHTNESS 10       → Set LED brightness (0-15)
```

---

## 🛠️ Hardware Requirements (When You're Ready)

### Components:
- Arduino Nano R3
- 2× MAX7219 8x8 LED Matrix displays
- MPU-6050 sensor (optional)
- Push button
- Buzzer
- Breadboard and wires

### Default Pin Configuration:
```
MAX7219 Display:
- DIN:    Pin 5
- CLK:    Pin 4
- CS/LOAD: Pin 10 (NOTE: Avoid Pin 0 on Nano - conflicts with hardware serial RX)

MPU-6050 (I2C):
- SDA:    A4 (Nano I2C)
- SCL:    A5 (Nano I2C)

Controls:
- Button: Pin 2
- Buzzer: Pin 13

Analog Fallback (if no MPU-6050):
- X-Axis: A1
- Y-Axis: A2
- Z-Axis: A3
```

**⚠️ Important:** The CS/LOAD pin (MAX7219 chip select) must NOT be Pin 0 on Arduino Nano, as Pin 0 is the hardware serial RX and conflicts with USB communication. Always use an alternate pin like Pin 10. If using Pin 0, you may experience upload failures or Serial Monitor issues.


**Important - Arduino Nano Configuration:**
For Arduino Nano users, the following pins in `config.h` should be adjusted:
- **I2C Communication**: Nano automatically uses A4 (SDA) and A5 (SCL) for I2C - the SDA/SCL defines in config.h do not affect hardware I2C on Nano
- **Pin Definitions**: Verify all pin definitions (PIN_DATAIN, PIN_CLK, PIN_LOAD, PIN_BUTTON, PIN_BUZZER) match your hardware wiring before uploading
- **No ESP8266-specific features** are needed for Nano (WiFi, AP mode are skipped)

If using ESP8266, pins are pre-configured. For Nano or other boards, edit `config.h` line ~20-30 to match your connections.

---

## 🔍 Troubleshooting

### "Sketch uses too much memory"
- Normal! Arduino Nano has limited RAM
- Code will still work, just be careful with long runtimes
- Set `DEBUG_OUTPUT 0` in config.h to save ~500 bytes

### "Upload Failed" or "Port Not Found"
1. Check OTG cable connection
2. Try different USB cable
3. Enable OTG support in phone settings
4. Grant USB permissions to ArduinoDroid

### "Board Not Detected"
1. Select **"Old Bootloader"** in board options
2. Try manual port selection in ArduinoDroid settings

### Code Compiles but Serial Monitor Shows Nothing
1. Verify baud rate is **9600**
2. Try unplugging and replugging USB
3. Close and reopen Serial Monitor

---

## 📱 Files Included

All in one folder for ArduinoDroid compatibility:

**Main Files:**
- `main.ino` - Main program (open this first!)
- `config.h` - Configuration settings

**Mode Classes:**
- `ClockMode.cpp/.h` - Digital clock
- `HourglassMode.cpp/.h` - Timer with animation
- `DiceMode.cpp/.h` - Dice roller
- `FlipCounterMode.cpp/.h` - Flip counter

**Hardware Drivers:**
- `LedControl.cpp/.h` - LED matrix driver
- `MPU6050.cpp/.h` - Motion sensor
- `Button.cpp/.h` - Button handler
- `SerialProtocol.cpp/.h` - Command parser
- `Delay.cpp/.h` - Non-blocking delays

**Documentation:**
- `README.md` - Full documentation
- `CHANGELOG.md` - List of improvements

---

## ⚙️ Customization

Edit `config.h` to change:

```cpp
// Display brightness (0-15)
#define DISPLAY_INTENSITY 8

// Debug messages (1=on, 0=off)
#define DEBUG_OUTPUT 1

// Hourglass settings
#define HOURGLASS_PARTICLE_COUNT 60
#define HOURGLASS_TONE_FREQ 440

// Button timing
#define BUTTON_LONG_PRESS_MS 2000
```

---

## 🎯 Pro Tips for ArduinoDroid

1. **Save often** - ArduinoDroid can crash on complex projects
2. **Use Serial Monitor** - Great for debugging without hardware
3. **Test incrementally** - Upload after small changes
4. **Enable auto-save** - In ArduinoDroid settings
5. **Keep backup** - Copy working version to another folder

---

## ✨ What's New in This Version?

All improvements from code review applied:
- ✅ Memory-safe string handling
- ✅ Button long-press bug fixed
- ✅ Integer overflow protection
- ✅ Better error handling
- ✅ All magic numbers replaced with constants
- ✅ Production-ready code

See `CHANGELOG.md` for complete details!

---

## 📞 Need Help?

Check the troubleshooting section above or:
- Read `README.md` for full documentation
- Check `config.h` for pin assignments
- Enable `DEBUG_OUTPUT` to see diagnostic messages

---

**Happy Coding! 🚀📱**

This firmware is ready to compile and upload on your Android phone!
