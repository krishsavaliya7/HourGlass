#ifndef CONFIG_H
#define CONFIG_H

// Check for local config override BEFORE defaults
// This allows config_local.h to define WIFI_SSID, WIFI_PASSWORD, WIFI_AP_MODE
#ifdef CONFIG_LOCAL_INCLUDED
#include "config_local.h"
#endif

// WiFi Configuration
// NOTE: Do not hardcode sensitive credentials here. Instead:
// 1. Create a config_local.h file (gitignored) with your actual credentials
// 2. Or load credentials from EEPROM/Flash at runtime
// Using default/placeholder values below:
#ifndef WIFI_SSID
#define WIFI_SSID "HourGlass"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "hourglass123"
#endif
#ifndef WIFI_AP_MODE
#define WIFI_AP_MODE true  // Set to false for station mode
#endif

// ============================================================
// Pin Definitions - MODIFIED FOR YOUR ACTUAL WIRING
// ============================================================
#define PIN_DATAIN 11  // MAX7219 DATA IN - Connected to D11
#define PIN_CLK 13     // MAX7219 CLK - Connected to D13
#define PIN_LOAD 10    // MAX7219 LOAD/CS - Connected to D10
#define PIN_BUTTON 2   // Push Button - Connected to D2
#define PIN_BUZZER 3   // Buzzer - Connected to D3

// MPU-6050 I²C Pins (ESP8266 only - Arduino Nano uses hardware I2C A4/A5)
// These pins are ignored on Arduino Nano which uses fixed A4=SDA, A5=SCL
#define PIN_SDA 12     // SDA (D6) - Only for ESP8266
#define PIN_SCL 14     // SCL (D5) - Only for ESP8266

// Display Configuration
#define NUM_MATRICES 2  // You have 2 matrices daisy-chained ✅
#define MATRIX_A 1
#define MATRIX_B 0
#define DISPLAY_INTENSITY 8
#define ROTATION_OFFSET 90

// Sensor Thresholds
#define ACC_THRESHOLD_LOW 300
#define ACC_THRESHOLD_HIGH 360
#define SHAKE_THRESHOLD 0.5
#define FLIP_THRESHOLD 0.8
#define ORIENTATION_Z_THRESHOLD 0.7      // Threshold for horizontal detection
#define ORIENTATION_Z_VERTICAL_MAX 0.3   // Max Z for vertical detection

// Timing Configuration
#define DELAY_FRAME 100           // Main loop delay (ms)
#define BUTTON_LONG_PRESS_MS 2000 // Long press duration
#define DEBOUNCE_DELAY 50         // Button debounce (ms)
#define UPDATE_INTERVAL 1000      // Status update interval (ms)

// Mode Definitions
#define MODE_CLOCK 0
#define MODE_HOURGLASS 1
#define MODE_DICE 2
#define MODE_FLIPCOUNTER 3
#define NUM_MODES 4

// API Configuration
#define API_PORT 80
#define MAX_API_RESPONSE_SIZE 512

// Debug Configuration
#define DEBUG_OUTPUT 1
#define SERIAL_BAUD 9600  // Changed to 9600 for compatibility with reference and Web Serial

// Hourglass Configuration
#define HOURGLASS_PARTICLE_COUNT 60     // Initial particle count
#define HOURGLASS_TONE_FREQ 440         // Buzzer frequency in Hz
#define HOURGLASS_TONE_DURATION 10      // Buzzer duration in ms
#define HOURGLASS_ALARM_CYCLES 5        // Number of alarm beep cycles

// Firmware Version
#define FIRMWARE_VERSION "1.0.0-CUSTOM"
#define BUILD_DATE __DATE__

// Compile-time validation
#if DISPLAY_INTENSITY > 15
    #error "DISPLAY_INTENSITY must be 0-15"
#endif

#if DELAY_FRAME < 10
    #error "DELAY_FRAME too small - may cause instability"
#endif

#endif

