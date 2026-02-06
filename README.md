# Dual Mode LED Hourglass + Digital Clock

A creative Arduino project that combines an LED hourglass animation with a digital clock display using accelerometer-based orientation detection. **Now with Web UI Control Panel!**

---

## 📋 Quick Links

- **🎮 [Web UI Synchronization Guide](./WEB_UI_SYNC_GUIDE.md)** - How to control from browser
- **🏗️ [System Architecture](./ARCHITECTURE_DIAGRAM.md)** - Visual diagrams and data flow
- **📁 [Web UI Folder](./web-ui/)** - Control panel files

---

## 📋 Project Overview

This project implements two interactive modes:
1. **Hourglass Mode** - Visual sand-falling animation that responds to device orientation
2. **Clock Mode** - Digital time display on LED matrices

The device automatically detects its physical orientation using an MPU6050 gyroscope and switches between modes via a push button or web UI.

---

## 🛠️ Hardware Components

| Component | Details |
|-----------|---------|
| **Microcontroller** | Arduino Nano R3 |
| **LED Matrices** | 2x MAX7219 (8x8 Red LED matrices) |
| **Accelerometer/Gyro** | MPU6050 (GY-521) |
| **Input Control** | Push Button (Active-Low on D2) |
| **Audio Feedback** | Piezo Buzzer 5V (D3) |

---

## 🔌 Pin Configuration

| Arduino Pin | Component | Purpose |
|------------|-----------|---------|
| D2 | Push Button | Mode switching & reset |
| D3 | Piezo Buzzer | Audio feedback |
| D10 | MAX7219 CS | Chip Select |
| D11 | MAX7219 DIN | Data Input |
| D13 | MAX7219 CLK | Clock Signal |
| A4 | MPU6050 SDA | I2C Data |
| A5 | MPU6050 SCL | I2C Clock |

---

## 📁 File Structure

### **rishi-file.ino** (Main Program)
- Main sketch with setup() and loop() functions
- Implements hourglass physics simulation
- Clock display logic
- Button debouncing and mode switching
- MPU6050 orientation detection
- Particle animation rendering

### **LedControl.h / LedControl.cpp**
- Library for controlling MAX7219 LED drivers
- Handles SPI communication with matrices
- Provides functions for:
  - Clearing displays
  - Setting LED intensity
  - Drawing characters/digits
  - Managing display refresh

### **Delay.h / Delay.cpp**
- Non-blocking delay timer class
- Allows periodic actions without blocking the main loop
- Used for particle drop timing and animation frame delays
- Methods:
  - `Delay(unsigned long t)` - Set delay duration
  - `Timeout()` - Check if delay has elapsed
  - `Time()` - Get current timeout value

---

## ⚙️ Key Features

### 1. **Hourglass Mode**
- Displays 60 falling "sand particles" on LED matrices
- Particles fall based on device orientation
- Responds to left/right rotation (roll) of the device
- Sand falls when device is tilted appropriately
- Animation updates at 80ms intervals

### 2. **Clock Mode**
- Shows digital time (HH:MM format) across both matrices
- Large 8x8 pixel digit rendering
- Soft timer updates synchronized with actual seconds

### 3. **Orientation Detection (MPU6050)**
- Reads accelerometer data via I2C
- Calculates roll angle (left/right tilt)
- 4 orientation states:
  - **0°** - Normal (sand falls down)
  - **90°** - On side
  - **180°** - Upside down (sand falls up)
  - **270°** - Other side

### 4. **Button Control**
- **Short Press** - Reset hourglass (beep sound)
- **Long Press (1.5s)** - Switch between modes (dual beep tone)
- Debouncing: 50ms

---

## 🚀 How It Works

```
┌─────────────────────────────────────────┐
│         MAIN LOOP                       │
├─────────────────────────────────────────┤
│ 1. Check Button Input                   │
│    ├─ Detect short/long presses         │
│    └─ Mode switching logic              │
│                                         │
│ 2. Read Accelerometer (MPU6050)         │
│    ├─ Get X, Y, Z acceleration         │
│    └─ Calculate device orientation      │
│                                         │
│ 3. Update Display Based on Mode         │
│    ├─ Hourglass: Simulate falling sand  │
│    └─ Clock: Show time                  │
│                                         │
│ 4. Render to LED Matrices               │
│    └─ SPI communication to MAX7219      │
└─────────────────────────────────────────┘
```

---

## 📊 Settings & Constants

```cpp
LONG_PRESS_TIME      = 1500ms   // Mode switch threshold
DEBOUNCE_TIME        = 50ms     // Button debounce
DELAY_FRAME          = 80ms     // Animation frame rate
PARTICLE_DROP_DELAY  = 1000ms   // Particle fall interval
TOTAL_PARTICLES      = 60       // Number of sand particles
ROTATION_OFFSET      = 90°      // Orientation reference
```

---

## 💡 Usage

1. **Power up the device** - Automatically starts in Hourglass mode
2. **Tilt the device** - Watch sand particles fall based on orientation
3. **Short press button** - Reset hourglass animation
4. **Long press button** - Switch to Clock mode (or back to Hourglass)
5. **In Clock mode** - View current time displayed on LED matrices

---

## 🔧 Development Notes

- **Non-blocking operation** - Uses `NonBlockDelay` class to avoid blocking delays
- **I2C Communication** - MPU6050 controlled via Wire library on pins A4/A5
- **SPI Protocol** - MAX7219 matrices use standard SPI communication
- **Custom Fonts** - 10-digit lookup table for large LED display rendering
- **Gravity-based Physics** - Simple particle simulation in hourglass mode

---

## 📝 License

The LedControl library is licensed under the MIT License (see LedControl.cpp header).

---

## 🎯 Customization Ideas

- Adjust `TOTAL_PARTICLES` for more/fewer sand particles
- Modify `DELAY_FRAME` for faster/slower animations
- Change `LONG_PRESS_TIME` for different button sensitivity
- Customize digit patterns in the `digit[10][8]` array
- Add more modes (timer, animation, etc.)

