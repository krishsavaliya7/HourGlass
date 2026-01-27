# 🕐 HourGlass  
### Smart Orientation-Based Hourglass & Clock System

<div align="center">

![Status](https://img.shields.io/badge/Status-Active-green)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue)
![License](https://img.shields.io/badge/License-MIT-brightgreen)
![Version](https://img.shields.io/badge/Version-1.0-brightblue)

**A multi-mode embedded time display system controlled via USB Serial and a modern browser UI.**  
No Wi-Fi. No Bluetooth. Just plug & play.

[Features](#-features) •
[Hardware](#-hardware-requirements) •
[Quick Start](#-quick-start) •
[Documentation](#-documentation) •
[Contributing](#-contributing)

</div>

---

## ✨ Features

- 🕐 **Clock Mode** — Digital + dot-matrix time display  
- ⏳ **Hourglass Mode** — Physics-based sand animation using gravity  
- 🎲 **Dice Mode** — Motion-triggered random number generator  
- 🔢 **Flip Counter** — Counts device orientation changes  
- 🔌 **USB Serial Control** — Works on PC & mobile (OTG supported)  
- 🔊 **Audio Feedback** — Buzzer for particle drops & alarms  
- 📱 **Responsive Web UI** — Desktop & mobile friendly  
- 🎯 **Web Serial API** — No drivers or native apps required  

---

## 🛠️ Hardware Requirements

### Microcontroller
- Arduino Nano / ESP8266 / ESP32 (USB or UART supported)

### Display & Sensors
- **2× MAX7219 8×8 LED Matrix** (16×8 combined)
- **MPU-6050** Accelerometer + Gyroscope
- **Push Button** (mode control)
- **Buzzer** (5V)

### Power
- 3.7V Li-ion Battery  
- TP4056 Charging Module  
- MT3608 Boost Converter (5V)  
- USB data cable (+ OTG adapter for mobile)

---

## 💻 Software Requirements

- **Arduino IDE** 1.8+ or **PlatformIO**
- **Arduino Libraries**
  - `Wire`
  - `MPU6050`
- **Browser**
  - Chrome / Edge (Web Serial API supported)
- **Optional**
  - Node.js 14+ (for local UI server)

---

## 🚀 Quick Start

### 1️⃣ Clone Repository
```bash
git clone https://github.com/yourusername/HourGlass.git
cd HourGlass
