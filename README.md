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
[Project Structure](#-project-structure) •
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

2️⃣ Flash Firmware
Option A — Arduino IDE

    Open firmware/main.ino

    Install MPU6050 library
    (Sketch → Include Library → Manage Libraries)

    Select correct Board and COM Port

    Click Upload

Option B — PlatformIO

pio run -t upload

3️⃣ Hardware Assembly

    Follow pin mapping in ARCHITECTURE.md

    Verify I²C (SDA/SCL) and SPI connections

    Ensure correct battery polarity

    Test LED matrices before final assembly

4️⃣ Start the Web UI
Option A — Open Directly

Open:

web-ui/index.html

in Chrome or Edge
Option B — Local Server (Recommended)

cd web-ui
python -m http.server 8000

Open:

http://localhost:8000

5️⃣ Connect & Use

    Plug in the device

    Click Connect in Web UI

    Select the serial port

    Control modes, time, and animations 🎉

📁 Project Structure

HourGlass/
├── firmware/                 # Arduino firmware
│   ├── main.ino              # Main program & state machine
│   ├── config.h              # Pin mapping & configuration
│   ├── modes/                # Operating modes
│   │   ├── ClockMode.h/.cpp
│   │   ├── HourglassMode.h/.cpp
│   │   ├── DiceMode.h/.cpp
│   │   └── FlipCounterMode.h/.cpp
│   ├── sensors/              # Hardware drivers
│   │   ├── MPU6050.h/.cpp
│   │   └── Button.h/.cpp
│   ├── display/              # LED matrix control
│   └── serial/               # USB serial protocol
├── web-ui/                   # Browser UI
│   ├── index.html
│   ├── css/
│   └── js/
├── docs/                     # Documentation
│   ├── ARCHITECTURE.md
│   ├── API_DOCUMENTATION.md
│   └── SETUP_GUIDE.md
├── tests/
├── README.md
└── LICENSE

🔌 Serial Protocol

Baud Rate: 9600
Commands (Host → Device)

SET_MODE:<0-3>        # 0=Clock, 1=Hourglass, 2=Dice, 3=Counter
SET_TIME:<hh:mm:ss>
SET_HG:<seconds>
ROLL_DICE
GET_STATUS

Responses (Device → Host)

STATUS:<mode>,<time>,<orientation>,<battery>
PARTICLE_DROP
ALARM

🎨 UI Features

    16×8 Virtual LED Matrix Preview

    Live Orientation Indicator

    Mode-specific control panels

    Connection & battery status

    Fully responsive layout

🔧 Customization

Edit firmware/config.h:

#define LED_BRIGHTNESS 8     // Range: 0–15
#define GRAVITY 0.3
#define PARTICLE_COUNT 128
#define FRICTION 0.99

Change serial speed:

Serial.begin(9600);

🐛 Troubleshooting
Issue	Solution
Device not detected	Use USB data cable
Web Serial missing	Use Chrome / Edge
No particle motion	Check MPU-6050 wiring
No sound	Verify buzzer GPIO
UI lag	Reduce refresh rate
🤝 Contributing

    Fork the repository

    Create a feature branch
    git checkout -b feature/new-feature

    Commit changes

    Push branch

    Open a Pull Request

Contribution ideas

    New display modes

    Bluetooth / Wi-Fi support

    Mobile-first UI

    3D-printed enclosure

👨‍💻 Author Team

    Krish Savaliya

    Rishi Rami

    Mit Parikh

📜 License

This project is licensed under the MIT License.
See the LICENSE file for details.
<div align="center">

Made with ❤️ for embedded systems enthusiasts
</div> ```