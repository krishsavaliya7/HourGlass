# Two-Mode System Architecture

## 📋 System Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    WEB CONTROL PANEL                         │
│  (Chrome/Edge Browser - Web Serial API)                      │
└──────────────────────────┬──────────────────────────────────┘
                           │ USB Serial (9600 baud)
                           ↓
┌─────────────────────────────────────────────────────────────┐
│              ARDUINO NANO R3 (ATmega328P)                     │
├─────────────────────────────────────────────────────────────┤
│                                                               │
│  ┌──────────────────────────────────────────────────────┐   │
│  │          Serial Command Handler                      │   │
│  ├──────────────────────────────────────────────────────┤   │
│  │  SET_MODE clock/hourglass                            │   │
│  │  SET_TIME hh mm                                      │   │
│  │  RESET_HG                                            │   │
│  │  GET_STATUS / GET_ORIENTATION                        │   │
│  │  SET_BRIGHTNESS 0-15                                 │   │
│  └──────────────────────────────────────────────────────┘   │
│           ↓                                 ↓                │
│    ┌─────────────────────┐        ┌──────────────────────┐  │
│    │   CLOCK MODE        │        │  HOURGLASS MODE      │  │
│    ├─────────────────────┤        ├──────────────────────┤  │
│    │ • Display time      │        │ • Sand animation     │  │
│    │ • Update every 1s   │        │ • Update every 80ms  │  │
│    │ • Shows HH:MM       │        │ • Gravity physics    │  │
│    │ • Orientation-aware │        │ • 60 particles       │  │
│    │ • 4 display modes   │        │ • Responds to tilt   │  │
│    └─────────────────────┘        └──────────────────────┘  │
│           ↓                                 ↓                │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  MAX7219 LED Driver (SPI)                           │    │
│  └──────────┬────────────────────────────┬─────────────┘    │
│             │ CS (D10)  CLK (D13)        │ DIN (D11)        │
│      ┌──────┴──────────────────┴────────┐                   │
│      │  8×8 LED Matrix Stack             │                   │
│      ├───────────────────────────────────┤                   │
│      │   Matrix A (Top)                  │                   │
│      │   ┌─────────────────────────────┐ │                   │
│      │   │ 8×8 LED Display (Max7219)   │ │                   │
│      │   └─────────────────────────────┘ │                   │
│      │   Matrix B (Bottom)               │                   │
│      │   ┌─────────────────────────────┐ │                   │
│      │   │ 8×8 LED Display (Max7219)   │ │                   │
│      │   └─────────────────────────────┘ │                   │
│      └───────────────────────────────────┘                   │
│                                                               │
│  ┌────────────────────────────────────────────────────────┐  │
│  │  MPU6050 (I2C - A4/A5)                                 │  │
│  │  └─ Reads accelerometer (ax, ay, az)                   │  │
│  │  └─ Calculates roll angle (0°/90°/180°/270°)          │  │
│  │  └─ Updates orientation every 80ms                     │  │
│  └────────────────────────────────────────────────────────┘  │
│                                                               │
│  ┌────────────────────────────────────────────────────────┐  │
│  │  GPIO                                                   │  │
│  │  ├─ Button (D2) - Physical mode switch                │  │
│  │  └─ Buzzer (D3) - Audio feedback                      │  │
│  └────────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

---

## 🔄 Mode Switching Flow

```
START
  ↓
┌─────────────────────────────────────────────┐
│  Button Press < 1.5s?  (Physical)           │
│  OR Click Mode Button? (Web UI)             │
└────────┬──────────────────────────┬─────────┘
         │                          │
        YES                        NO
         ↓                          ↓
    ┌─────────────┐           Continue
    │ Check which │           in current
    │ mode active │           mode
    └──────┬──────┘
           ↓
    ┌──────────────────────┐
    │ Current Mode = ?     │
    └──────┬──────┬────────┘
           │      │
      Clock        Hourglass
        │            │
        ↓            ↓
    Switch to    Switch to
    Hourglass    Clock
        │            │
        ↓            ↓
    ┌─────────────────────────────┐
    │ 1. Clear displays           │
    │ 2. Play dual beep sound     │
    │ 3. Initialize mode          │
    │ 4. Send "OK" response       │
    └─────────────────────────────┘
        ↓
    MODE ACTIVE
```

---

## 🕐 Clock Mode - Display Orientation

### Orientation 0° (Upright)
```
Device Position:  ┌────────┐
                  │        │
                  │ DEVICE │
                  │        │
                  └────────┘

Display:  ┌──────────────────┐
          │  12:30           │  Digital HH:MM
          │  ┌────┬────┐    │  
          │  │ 12 │ 30 │    │  Full size digits
          │  └────┴────┘    │
          └──────────────────┘
```

### Orientation 90° (Right Side)
```
Device Position:  ┌─────┐
                  │ D   │
                  │ E   │
                  │ V   │
                  │ I   │
                  │ C   │
                  │ E   │
                  └─────┘

Display:  ┌─────────────┐
          │12   │  30   │  Matrix A = Hours
          │12   │  30   │  Matrix B = Minutes
          │12   │  30   │
          │12   │  30   │
          └─────────────┘
```

### Orientation 180° (Upside Down)
```
Device Position:  ┌────────┐
                  │        │
                  │ DEVICE │
                  │ (flipped)
                  └────────┘

Display:  ┌──────────────────┐
          │ 03:21 (inverted) │  Flipped display
          │                  │
          └──────────────────┘
```

### Orientation 270° (Left Side)
```
Device Position:  ┌─────┐
                  │   D │
                  │   E │
                  │   V │
                  │   I │
                  │   C │
                  │   E │
                  └─────┘

Display:  ┌─────────────┐
          │ 30   │  12  │  Matrix A = Minutes
          │ 30   │  12  │  Matrix B = Hours
          │ 30   │  12  │
          │ 30   │  12  │
          └─────────────┘
```

---

## ⏳ Hourglass Mode - Physics Simulation

### Sand Particle Behavior
```
Frame 1: Gravity = Down     Frame 2: Gravity = Down
  ┌───────────────┐           ┌───────────────┐
  │ █ (sand top)  │           │ █             │
  │               │           │ █  (falling)  │
  │               │           │               │
  │               │           │ █ █ (rested)  │
  │               │           │ █ █ █         │
  └───────────────┘           └───────────────┘
```

### Gravity Directions
```
Roll 0° & 180°:        Roll 90° & 270°:
Gravity = DOWN         Gravity = SIDEWAYS

┌─────────────┐        ┌─────────────┐
│ ┌─────────┐ │        │  ┌───────┐  │
│ │█ █ █ █  │ │        │  │█ █ █  ┃  │
│ │█ █ █ █  │ │        │  │█ █ █  ┃  │
│ │         │ │        │  │       ┃  │
│ │█ █ █ █  │ │        │  │█ █ █  ┃  │
│ └─────────┘ │        │  └───────┘  │
└─────────────┘        └─────────────┘
```

### Physics Rules
```
Particle Movement Priority:
1. Can go DOWN?         → Go Down
2. Can go LEFT only?    → Go Left
3. Can go RIGHT only?   → Go Right
4. Can go LEFT or RIGHT? → Random direction
5. Blocked everywhere?  → Stay still

Collision Detection:
- Check 8 adjacent cells
- Particle slides around obstacles
- Settles in low points
```

---

## 📊 Web UI to Arduino Communication

### Event Timeline
```
T=0ms:    User clicks "⏳ Hourglass" button
          ↓
T=10ms:   Web UI validates mode
          ↓
T=20ms:   Web UI sends: "SET_MODE hourglass\n"
          ↓
T=50ms:   Arduino receives command
          ↓
T=60ms:   Arduino clears matrices
          ↓
T=70ms:   Arduino fills top chamber (60 particles)
          ↓
T=80ms:   Arduino plays sound (1000Hz, 100ms)
          ↓
T=200ms:  Arduino plays sound (1500Hz, 100ms)
          ↓
T=220ms:  Arduino sends: "OK MODE HOURGLASS\n"
          ↓
T=250ms:  Web UI receives response
          ↓
T=260ms:  Web UI updates mode badge to "Hourglass"
          ↓
T=270ms:  Hourglass animation starts
```

---

## 🎛️ Command Processing

```
Arduino Main Loop (80ms per cycle):
┌──────────────────────────────────┐
│ 80ms Delay (DELAY_FRAME)         │
│         ↓                        │
│ handleSerialCommands()           │
│ └─ Check if data available       │
│ └─ Parse command                 │
│ └─ Execute & respond             │
│         ↓                        │
│ checkButton()                    │
│ └─ Physical button handling      │
│         ↓                        │
│ Read Accelerometer               │
│ └─ Get orientation               │
│         ↓                        │
│ Update Display:                  │
│ ├─ IF Clock:  displayClockMode() │
│ └─ IF Hourglass: updateMatrix()  │
│         ↓                        │
│ Loop back (80ms later)           │
└──────────────────────────────────┘
```

---

## 📱 Web UI State Machine

```
START
  ↓
┌─────────────────────────────────────┐
│ App Initialization                  │
│ ├─ Load serial connection           │
│ ├─ Load LED display                 │
│ ├─ Load mode manager                │
│ └─ Show test pattern                │
└──────────┬──────────────────────────┘
           ↓
    ┌─────────────────┐
    │  DISCONNECTED   │
    │                 │
    │ Click "Connect" │
    └────────┬────────┘
             ↓
    ┌─────────────────┐
    │  CONNECTING...  │
    │                 │
    │  Wait for port  │
    └────────┬────────┘
             ↓
    ┌─────────────────────────┐
    │  CONNECTED              │
    │                         │
    │  • Mode buttons active  │
    │  • Controls enabled     │
    │  • Status badge on      │
    │  • Auto-refresh runs    │
    └────────┬────────────────┘
             ↓
    ┌─────────────────────────┐
    │  MODE SELECTED          │
    │                         │
    │  • Clock or Hourglass   │
    │  • Mode-specific UI     │
    │  • Commands sent        │
    │  • Display updates      │
    └─────────────────────────┘
```

---

## 🔌 Pin Configuration Summary

| Function | Arduino Pin | Max7219/Sensor | Purpose |
|----------|------------|-----------------|---------|
| **SPI Data** | D11 | Max7219 DIN | LED data input |
| **SPI Clock** | D13 | Max7219 CLK | LED clock signal |
| **SPI Chip Select** | D10 | Max7219 CS | LED chip select |
| **I2C Data** | A4 | MPU6050 SDA | Accelerometer communication |
| **I2C Clock** | A5 | MPU6050 SCL | Accelerometer clock |
| **Button** | D2 | - | Physical mode switch |
| **Buzzer** | D3 | - | Audio feedback |
| **Serial TX** | D1 | USB | Web UI communication |
| **Serial RX** | D0 | USB | Web UI communication |

---

## ✅ Checklist for Synchronization

- [x] Arduino sketch handles serial commands
- [x] Web UI sends only Clock/Hourglass commands  
- [x] Mode switching works from both web UI and physical button
- [x] Clock mode responds to orientation changes
- [x] Hourglass mode shows sand animation
- [x] Status updates display correctly
- [x] Brightness control works
- [x] Documentation complete

---

**Status:** ✅ **FULLY SYNCHRONIZED**  
**Date:** February 5, 2026  
**Version:** 1.0

