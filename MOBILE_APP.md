# Mobile App Integration - Bluetooth Control via ESP32

Control your Smart Hourglass & Clock System from your **mobile device** using **Bluetooth** (BLE - Bluetooth Low Energy) with an **ESP32** microcontroller.

## Overview

This guide enables you to:
- Control the hourglass remotely from **Android** or **iOS** devices
- Use the same web UI with Bluetooth instead of USB serial
- Stream real-time sensor data and LED display updates
- Works with mobile browsers supporting **Web Bluetooth API**

## Hardware Requirements

### Microcontroller
- **ESP32** (instead of Arduino Nano/Uno)
- USB micro cable for initial flashing
- Li-ion battery + charging module for wireless operation

### Display & Sensors
- 2× MAX7219 8×8 LED Matrix
- MPU-6050 Gyroscope/Accelerometer
- Push Button
- Buzzer (optional, for audio feedback)

### ESP32 Pin Configuration (Bluetooth-enabled)

```
MAX7219 (SPI):
- GPIO 23 → MAX7219 DATA IN (MOSI)
- GPIO 18 → MAX7219 CLK (SCK)
- GPIO 5  → MAX7219 LOAD/CS

Sensors (I²C):
- GPIO 21 → MPU-6050 SDA
- GPIO 22 → MPU-6050 SCL
- GPIO 2  → Push Button

Output:
- GPIO 13 → Buzzer (PWM)

Power:
- 3.3V/GND from battery through charging module
```

## Software Requirements

### Mobile Device
- **Android 6.0+** (Chrome, Edge, Opera with Web Bluetooth API)
- **iOS 13+** (requires native app or limited Bluetooth support)
- Bluetooth 4.0+ (BLE)

### Development Tools
- Arduino IDE 1.8.13+ or PlatformIO
- ESP32 board support package
- Bluetooth libraries: `BluetoothSerial` or `NimBLE-Arduino`

### Required Libraries
```
Arduino IDE → Sketch → Include Library → Manage Libraries:
- MPU6050 (by Electronic Cats)
- NimBLE-Arduino (fast, low-power Bluetooth)
- Wire (built-in I²C)
```

## Installation & Setup

### Step 1: Prepare Hardware

1. **Connect Components** according to pin mapping above
2. **Install ESP32 board support** in Arduino IDE:
   - Go to File → Preferences
   - Add URL: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Tools → Board → Boards Manager → Search "ESP32" → Install

### Step 2: Configure & Flash Firmware

#### Option A: Using Modified Firmware (Recommended)

1. **Update `firmware/config.h`** for ESP32:

```cpp
// ESP32 Pin Mapping
#define PIN_DATAIN 23      // SPI MOSI
#define PIN_CLK 18         // SPI SCK
#define PIN_LOAD 5         // SPI CS
#define PIN_BUTTON 2
#define PIN_BUZZER 13

// Bluetooth Configuration
#define BLE_DEVICE_NAME "HourGlass-ESP32"
#define BLE_SERVICE_UUID "12345678-1234-5678-1234-567812345678"
#define BLE_CHAR_RX_UUID "87654321-4321-8765-4321-876543218765"
#define BLE_CHAR_TX_UUID "11223344-5566-7788-99aa-bbccddeeff00"

// Feature Flags
#define ENABLE_BLE 1
#define USE_NIMBKE 1       // NimBLE for lower power
```

2. **Create `firmware/bluetooth/BLEManager.h`**:

```cpp
#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <NimBLEDevice.h>
#include <NimBLEServer.h>
#include <NimBLEUtils.h>

class BLEManager {
private:
    NimBLEServer* pServer;
    NimBLECharacteristic* pTxCharacteristic;
    NimBLECharacteristic* pRxCharacteristic;
    
public:
    BLEManager();
    void begin(const char* deviceName);
    void sendData(const String& data);
    String receiveData();
    bool isConnected();
    void update();
};

#endif
```

3. **Create `firmware/bluetooth/BLEManager.cpp`**:

```cpp
#include "BLEManager.h"

BLEManager::BLEManager() : pServer(nullptr), pTxCharacteristic(nullptr), pRxCharacteristic(nullptr) {}

void BLEManager::begin(const char* deviceName) {
    NimBLEDevice::init(deviceName);
    pServer = NimBLEDevice::createServer();
    
    NimBLEService* pService = pServer->createService(BLE_SERVICE_UUID);
    
    pTxCharacteristic = pService->createCharacteristic(
        BLE_CHAR_TX_UUID,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::READ
    );
    
    pRxCharacteristic = pService->createCharacteristic(
        BLE_CHAR_RX_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    
    pService->start();
    
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->start();
}

void BLEManager::sendData(const String& data) {
    if (pTxCharacteristic && isConnected()) {
        pTxCharacteristic->setValue(data.c_str());
        pTxCharacteristic->notify();
    }
}

String BLEManager::receiveData() {
    if (pRxCharacteristic && pRxCharacteristic->getValue().length() > 0) {
        String data = String((char*)pRxCharacteristic->getValue().c_str());
        pRxCharacteristic->setValue("");
        return data;
    }
    return "";
}

bool BLEManager::isConnected() {
    return pServer->getConnectedCount() > 0;
}

void BLEManager::update() {
    // BLE handled by background task in NimBLE
}
```

4. **Modify `firmware/main.ino`** to use Bluetooth:

```cpp
#include <Arduino.h>
#include "bluetooth/BLEManager.h"
#include "config.h"
// ... other includes

BLEManager bleManager;

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("Initializing Bluetooth...");
    bleManager.begin(BLE_DEVICE_NAME);
    
    // Initialize other components
    lc.init();
    mpu.init();
    button.init();
    
    Serial.println("Ready for Bluetooth connections!");
}

void loop() {
    // Check for incoming Bluetooth commands
    String command = bleManager.receiveData();
    if (command.length() > 0) {
        Serial.println("BLE Command: " + command);
        serialProtocol.parseCommand(command);
    }
    
    // Send status updates every 100ms
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 100) {
        String status = getStatusString();
        bleManager.sendData(status);
        lastUpdate = millis();
    }
    
    // Regular mode updates
    updateCurrentMode();
    delay(10);
}

String getStatusString() {
    String status = "STATUS|";
    status += "mode:" + String(currentMode) + "|";
    status += "battery:" + String(getBatteryLevel()) + "|";
    status += "matrix:" + getMatrixState();
    return status;
}
```

### Step 3: Flash to ESP32

1. **Connect ESP32** to computer via USB
2. **Select board**: Tools → Board → ESP32 Dev Module
3. **Select port**: Tools → Port → COM#
4. **Upload**: Sketch → Upload (or Ctrl+U)

Monitor output:
```
Tools → Serial Monitor (set to 115200 baud)
```

You should see:
```
Initializing Bluetooth...
Ready for Bluetooth connections!
```

### Step 4: Configure Web UI for Bluetooth

#### Update `web-ui/js/serial.js` for BLE support:

```javascript
// Bluetooth/BLE Serial Handler
class BLESerialAdapter {
    constructor() {
        this.device = null;
        this.gattServer = null;
        this.characteristic = null;
        this.isConnected = false;
        this.BLE_SERVICE_UUID = "12345678-1234-5678-1234-567812345678";
        this.BLE_CHAR_RX_UUID = "87654321-4321-8765-4321-876543218765";
        this.BLE_CHAR_TX_UUID = "11223344-5566-7788-99aa-bbccddeeff00";
    }

    async connect() {
        try {
            console.log("Requesting Bluetooth device...");
            
            this.device = await navigator.bluetooth.requestDevice({
                filters: [{ services: [this.BLE_SERVICE_UUID] }]
            });

            console.log("Device selected:", this.device.name);

            this.gattServer = await this.device.gatt.connect();
            console.log("GATT connected");

            const service = await this.gattServer.getPrimaryService(this.BLE_SERVICE_UUID);
            this.characteristicTx = await service.getCharacteristic(this.BLE_CHAR_TX_UUID);
            this.characteristicRx = await service.getCharacteristic(this.BLE_CHAR_RX_UUID);

            // Listen for notifications
            await this.characteristicTx.startNotifications();
            this.characteristicTx.addEventListener('characteristicvaluechanged', (e) => {
                this.onDataReceived(e);
            });

            this.isConnected = true;
            console.log("Connected via Bluetooth!");
            return true;
        } catch (error) {
            console.error("Bluetooth connection error:", error);
            return false;
        }
    }

    async sendCommand(command) {
        if (!this.characteristicRx) {
            console.warn("Not connected to device");
            return false;
        }

        try {
            const encoder = new TextEncoder();
            const data = encoder.encode(command + '\n');
            await this.characteristicRx.writeValue(data);
            console.log("Sent:", command);
            return true;
        } catch (error) {
            console.error("Send error:", error);
            return false;
        }
    }

    onDataReceived(event) {
        const decoder = new TextDecoder();
        const value = decoder.decode(event.target.value);
        console.log("Received:", value);
        // Handle incoming data from device
    }

    disconnect() {
        if (this.device) {
            this.device.gatt.disconnect();
            this.isConnected = false;
            console.log("Disconnected from Bluetooth");
        }
    }
}

// Export for use in app.js
const bleAdapter = new BLESerialAdapter();
```

#### Update `web-ui/js/app.js` to use BLE:

```javascript
const connectBtn = document.getElementById('btn-connect');

connectBtn.addEventListener('click', async () => {
    const connected = await bleAdapter.connect();
    if (connected) {
        updateConnectionStatus(true);
        // Set initial mode, time, etc.
        await bleAdapter.sendCommand('SET_MODE clock');
    }
});

document.getElementById('btn-disconnect').addEventListener('click', () => {
    bleAdapter.disconnect();
    updateConnectionStatus(false);
});

// Send commands via Bluetooth
async function sendCommand(cmd) {
    if (bleAdapter.isConnected) {
        await bleAdapter.sendCommand(cmd);
    } else {
        console.warn("Device not connected");
    }
}
```

### Step 5: Test on Mobile

#### Android (Chrome 56+)
1. **Enable Developer Mode**: Settings → About Phone → Tap Build Number 7 times
2. **Enable Bluetooth** in device settings
3. **Open Chrome**: `http://localhost:8080/web-ui/` (requires HTTPS or localhost)
4. **Click "Connect Device"**
5. Select your **"HourGlass-ESP32"** device
6. Test all modes!

#### iOS (Limited Support)
- iOS doesn't fully support Web Bluetooth API
- Consider using a **native Swift app** or **React Native** wrapper
- Alternatively: Use PC/Android with Bluetooth relay

## Communication Protocol

### Command Format (Mobile → Device)

Same as serial, sent over Bluetooth:

```
SET_MODE <mode>           // clock, hourglass, dice, flip
SET_TIME <hours> <mins>   // Set current time
SET_HG <hours> <mins>     // Set hourglass duration
ROLL_DICE                 // Roll the dice
GET_STATUS                // Request status update
```

### Status Format (Device → Mobile)

```
STATUS|mode:clock|battery:85|matrix:...
```

## Power Management

### Battery Life Optimization

1. **Reduce BLE advertising interval** in `BLEManager.cpp`:
```cpp
NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
pAdvertising->setMinInterval(160);  // 100ms (default)
pAdvertising->setMaxInterval(240);  // 150ms (default)
pAdvertising->start();
```

2. **Enable light sleep**:
```cpp
esp_sleep_enable_ext0_wakeup(GPIO_NUM_2, LOW);  // Wake on button press
```

3. **Typical battery life**:
   - **Active (connected)**: 8-12 hours
   - **Idle (advertising)**: 24-48 hours
   - **Deep sleep**: Days

## Troubleshooting

### "No Bluetooth devices found"
- Ensure ESP32 is powered and running
- Check if BLE advertising is enabled (see serial output)
- Ensure device name matches filter in `serial.js`

### "Connection drops frequently"
- Move closer to device
- Reduce interference (WiFi, microwaves)
- Increase BLE TX power in firmware:
```cpp
esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9);
```

### "Commands not received"
- Verify MTU size (default 23 bytes)
- Split long commands
- Add small delays between commands

### Battery draining too fast
- Disable unused features (buzzer, matrix updates)
- Increase BLE advertising interval
- Enable WiFi off: `WiFi.mode(WIFI_OFF);`

## Next Steps

1. **Data Logging**: Store activity history on mobile
2. **Multi-device**: Control multiple hourglasses from one app
3. **Analytics Dashboard**: Track usage patterns
4. **Push Notifications**: Alert when timer completes
5. **Native App**: Build iOS/Android apps for better UX

## References

- [Web Bluetooth API Docs](https://developer.mozilla.org/en-US/docs/Web/API/Web_Bluetooth_API)
- [NimBLE Arduino Library](https://github.com/h2zero/NimBLE-Arduino)
- [ESP32 Official Docs](https://docs.espressif.com/projects/esp-idf/en/latest/)
- [BLE GATT Services](https://www.bluetooth.com/specifications/gatt/)

---

**Status**: Ready for mobile Bluetooth testing!
