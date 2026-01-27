# Quick Start Guide

## 🚀 Get Started in 5 Minutes

### Step 1: Test Web UI Locally

```bash
# Install Node.js dependencies (optional)
npm install

# Start local web server
npm run serve-node
# OR
python -m http.server 8080

# Open browser
http://localhost:8080/web-ui/index.html
```

### Step 2: Prepare Hardware

1. **Connect Components** (see `docs/SETUP_GUIDE.md` for details):
   - Arduino Nano / ESP8266 / ESP32 (or similar board with USB serial)
   - 2× MAX7219 LED matrices
   - MPU-6050 sensor
   - Push button
   - **Buzzer** (for audio feedback)
   - Power supply

2. **Pin Connections**:
   ```
   MAX7219 DATA → GPIO 5
   MAX7219 CLK  → GPIO 4
   MAX7219 CS   → GPIO 0
   Button       → GPIO 2
   MPU-6050 SDA → GPIO 12
   MPU-6050 SCL → GPIO 14
   Buzzer       → GPIO 13
   ```

### Step 3: Upload Firmware

1. **Open Arduino IDE**
2. **Install Libraries**:
   - MPU6050 library
3. **Open** `firmware/main.ino`
4. **Configure pins/thresholds** in `firmware/config.h` (no WiFi needed)
5. **Select Board**: Tools → Board → your board (e.g. Arduino Nano, NodeMCU 1.0)
6. **Select Port**: Tools → Port → the COM/USB port for your board
7. **Upload** firmware

### Step 4: Connect via USB Serial and Open Web UI

1. Ensure the board is connected with a **USB data** cable
2. Open `http://localhost:8080/web-ui/index.html` (from Step 1)
3. Click **Connect** in the UI and choose your device’s serial port
4. If prompted, allow the browser to access the serial port

### Step 5: Test Modes

- **Clock Mode**: Set time, tilt device to see dot display
- **Hourglass Mode**: Set duration, watch sand flow
- **Dice Mode**: Press button or shake to roll
- **Flip Counter**: Flip device vertically to count

## 🧪 Run Tests with TestSprite

```bash
# 1. Start web server
npm run serve-node

# 2. Use TestSprite MCP tool
# Bootstrap TestSprite with:
# - Type: frontend
# - Port: 8080
# - Path: <PROJECT_PATH>  (or ./path/to/HourGlass)
# - Scope: codebase

# 3. Generate and execute tests
# Tests defined in tests/testsprite.config.json
```

## 📚 Documentation

- **README.md** - Project overview
- **ARCHITECTURE.md** - System design
- **docs/SETUP_GUIDE.md** - Detailed setup
- **docs/API_DOCUMENTATION.md** - API reference
- **work-design.md** - AI image generation & build workflow
- **PROJECT_SUMMARY.md** - Complete feature list

## ⚡ Quick Commands

```bash
# Serve web UI
npm run serve-node

# View serial protocol docs
cat docs/API_DOCUMENTATION.md

# Check firmware config
cat firmware/config.h
```

## 🐛 Troubleshooting

**Device not found?**
- Check that the board appears as a serial device (COMx, /dev/ttyUSBx, etc.)
- Make sure you are using a **data-capable** USB cable
- Ensure the correct port is selected in the browser

**Display not working?**
- Check SPI connections
- Verify power supply
- Increase brightness

**Web UI not loading?**
- Verify the local server is running (`npm run serve-node` or `python -m http.server 8080`)
- Check the browser console for errors
- Ensure you opened the correct URL (`http://localhost:8080/web-ui/index.html`)

## ✅ What's Included

✅ Complete web UI (HTML/CSS/JS)
✅ Full firmware (Arduino/ESP8266)
✅ Serial command protocol implementation
✅ All 4 operating modes
✅ Comprehensive documentation
✅ TestSprite configuration
✅ Error handling
✅ Responsive design

---

**Ready to build!** 🎉

