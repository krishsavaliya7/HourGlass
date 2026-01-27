/**
 * Smart Hourglass & Clock System - Main Firmware
 * Supports: Clock, Hourglass, Dice, Flip Counter modes
 * USB Serial interface for remote control
 */

#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "display/LedControl.h"
#include "Delay.h"
#include "serial/SerialProtocol.h"
#include "MPU6050.h"
#include "Button.h"
#include "ClockMode.h"
#include "HourglassMode.h"
#include "DiceMode.h"
#include "FlipCounterMode.h"

// Global objects
LedControl lc(PIN_DATAIN, PIN_CLK, PIN_LOAD, NUM_MATRICES);
MPU6050 mpu;
Button button(PIN_BUTTON);
SerialProtocol serialProtocol;
NonBlockDelay statusUpdateDelay;

// Mode objects
ClockMode clockMode(&lc, &mpu);
HourglassMode hourglassMode(&lc, &mpu);
DiceMode diceMode(&lc, &mpu);
FlipCounterMode flipCounterMode(&lc, &mpu);

// State variables
int currentMode = MODE_HOURGLASS; // Start with hourglass like reference
unsigned long lastUpdate = 0;
bool deviceInitialized = false;

// Forward declarations for SerialProtocol
void setMode(int mode);
void setClockTime(int hours, int minutes);
void setHourglassDuration(int hours, int minutes);
void resetHourglass();
void rollDice();
void resetFlipCounter();
int getCurrentMode();
int getDiceValue();
int getFlipCount();
void setBrightness(int level);
String getStatusJSON();
String getOrientationJSON();
String getDisplayJSON();
String matrixToJson(int matrixAddr);

/**
 * Setup function
 */
void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(1000);
    
    Serial.println("\n=== Smart Hourglass System ===");
    
    // Initialize buzzer pin
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    
    // Initialize display
    initDisplay();
    
    // Initialize sensors
    initSensors();
    
    // Initialize button
    button.init();
    
    // Initialize serial protocol
    serialProtocol.init();
    
    // Initialize modes
    clockMode.init();
    hourglassMode.init();
    diceMode.init();
    flipCounterMode.init();
    
    // Set initial mode (hourglass like reference)
    setMode(MODE_HOURGLASS);
    
    deviceInitialized = true;
    Serial.println("System initialized successfully!");
    Serial.println("Ready for commands via USB Serial");
}

/**
 * Main loop
 */
void loop() {
    delay(DELAY_FRAME);
    
    // Update button state
    button.update();
    handleButtonInput();
    
    // Update sensors
    mpu.update();
    
    // Update current mode
    updateCurrentMode();
    
    // Process serial commands
    serialProtocol.update();
    
    // Update status periodically (send to serial if needed)
    if (statusUpdateDelay.Timeout() || lastUpdate == 0) {
        // Status updates sent on-demand via serial commands
        statusUpdateDelay.Delay(UPDATE_INTERVAL);
        lastUpdate = millis();
    }
}

/**
 * Initialize display
 */
void initDisplay() {
    Serial.println("Initializing display...");
    for (byte i = 0; i < NUM_MATRICES; i++) {
        lc.shutdown(i, false);
        lc.setIntensity(i, DISPLAY_INTENSITY);
        lc.clearDisplay(i);
    }
    Serial.println("Display initialized");
}

/**
 * Initialize sensors
 */
void initSensors() {
    Serial.println("Initializing MPU-6050...");
    #if defined(ESP8266) || defined(ESP32)
    Wire.begin(PIN_SDA, PIN_SCL);
    #else
    Wire.begin();
    #endif
    if (!mpu.init()) {
        Serial.println("MPU-6050 initialization failed! Using fallback.");
    } else {
        Serial.println("MPU-6050 initialized");
    }
}

/**
 * Handle button input
 */
void handleButtonInput() {
    if (button.wasPressed()) {
        // Short press - mode-specific action
        switch (currentMode) {
            case MODE_DICE:
                rollDice();
                break;
            case MODE_HOURGLASS:
                hourglassMode.reset();
                break;
            // Other modes don't have short press actions
        }
    }
    
    if (button.wasLongPressed()) {
        // Long press - cycle to next mode
        cycleMode();
    }
}

/**
 * Update current mode
 */
void updateCurrentMode() {
    switch (currentMode) {
        case MODE_CLOCK:
            clockMode.update();
            break;
        case MODE_HOURGLASS:
            hourglassMode.update();
            break;
        case MODE_DICE:
            diceMode.update();
            break;
        case MODE_FLIPCOUNTER:
            flipCounterMode.update();
            break;
    }
}

/**
 * Set mode
 */
void setMode(int mode) {
    if (mode < 0 || mode >= NUM_MODES) return;
    
    // Exit current mode
    switch (currentMode) {
        case MODE_CLOCK:
            clockMode.exit();
            break;
        case MODE_HOURGLASS:
            hourglassMode.exit();
            break;
        case MODE_DICE:
            diceMode.exit();
            break;
        case MODE_FLIPCOUNTER:
            flipCounterMode.exit();
            break;
    }
    
    // Set new mode
    currentMode = mode;
    
    // Enter new mode
    switch (currentMode) {
        case MODE_CLOCK:
            clockMode.enter();
            break;
        case MODE_HOURGLASS:
            hourglassMode.enter();
            break;
        case MODE_DICE:
            diceMode.enter();
            break;
        case MODE_FLIPCOUNTER:
            flipCounterMode.enter();
            break;
    }
    
    Serial.print("Mode changed to: ");
    Serial.println(currentMode);
}

/**
 * Cycle to next mode
 */
void cycleMode() {
    int nextMode = (currentMode + 1) % NUM_MODES;
    setMode(nextMode);
}

/**
 * Set clock time
 */
void setClockTime(int hours, int minutes) {
    clockMode.setTime(hours, minutes);
}

/**
 * Set hourglass duration
 */
void setHourglassDuration(int hours, int minutes) {
    hourglassMode.setDuration(hours, minutes);
}

/**
 * Reset hourglass
 */
void resetHourglass() {
    hourglassMode.reset();
}

/**
 * Roll dice
 */
void rollDice() {
    diceMode.roll();
}

/**
 * Reset flip counter
 */
void resetFlipCounter() {
    flipCounterMode.reset();
}

/**
 * Get current mode
 */
int getCurrentMode() {
    return currentMode;
}

/**
 * Get status as JSON string
 */
String getStatusJSON() {
    String modeStr = "clock";
    if (currentMode == MODE_CLOCK) modeStr = "clock";
    else if (currentMode == MODE_HOURGLASS) modeStr = "hourglass";
    else if (currentMode == MODE_DICE) modeStr = "dice";
    else if (currentMode == MODE_FLIPCOUNTER) modeStr = "flipcounter";
    
    String json = "{\"mode\":\"" + modeStr + "\"";
    json += ",\"orientation\":\"" + String(mpu.isHorizontal() ? "horizontal" : "vertical") + "\"";
    
    // Mode-specific data
    if (currentMode == MODE_CLOCK) {
        json += ",\"time\":\"" + clockMode.getTimeString() + "\"";
    } else if (currentMode == MODE_HOURGLASS) {
        json += ",\"hourglassProgress\":" + String(hourglassMode.getProgress());
    } else if (currentMode == MODE_DICE) {
        json += ",\"diceValue\":" + String(diceMode.getValue());
    } else if (currentMode == MODE_FLIPCOUNTER) {
        json += ",\"flipCount\":" + String(flipCounterMode.getCount());
    }
    
    json += "}";
    return json;
}

/**
 * Get orientation as JSON string
 */
String getOrientationJSON() {
    String json = "{";
    json += "\"angle\":" + String(mpu.getAngle());
    json += ",\"x\":" + String(mpu.getX(), 2);
    json += ",\"y\":" + String(mpu.getY(), 2);
    json += ",\"z\":" + String(mpu.getZ(), 2);
    json += "}";
    return json;
}

/**
 * Get display state as JSON string
 */
String getDisplayJSON() {
    // Return the real 8x8 state for both MAX7219 devices
    // matrixA uses MATRIX_A addr, matrixB uses MATRIX_B addr (see config.h)
    String json;
    json.reserve(1024);
    json = "{\"matrixA\":";
    json += matrixToJson(MATRIX_A);
    json += ",\"matrixB\":";
    json += matrixToJson(MATRIX_B);
    json += "}";
    return json;
}

String matrixToJson(int matrixAddr) {
    String out;
    out.reserve(512);
    out = "[";
    for (int row = 0; row < 8; row++) {
        out += "[";
        for (int col = 0; col < 8; col++) {
            out += (lc.getRawXY(matrixAddr, col, row) ? "true" : "false");
            if (col < 7) out += ",";
        }
        out += "]";
        if (row < 7) out += ",";
    }
    out += "]";
    return out;
}

int getDiceValue() {
    return diceMode.getValue();
}

int getFlipCount() {
    return flipCounterMode.getCount();
}

void setBrightness(int level) {
    level = constrain(level, 0, 15);
    for (byte i = 0; i < NUM_MATRICES; i++) {
        lc.setIntensity(i, level);
    }
}
