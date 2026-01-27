#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration
// NOTE: Do not hardcode sensitive credentials here. Instead:
// 1. Create a config_local.h file (gitignored) with your actual credentials
// 2. Or load credentials from EEPROM/Flash at runtime
// Using default/placeholder values below:
#define WIFI_SSID "HourGlass"
#define WIFI_PASSWORD "hourglass123"
#define WIFI_AP_MODE true  // Set to false for station mode

// Check for local config override
#ifdef CONFIG_LOCAL_INCLUDED
// Local overrides will be defined in config_local.h
#endif

// Pin Definitions
#define PIN_DATAIN 5   // MAX7219 DATA IN (D1)
#define PIN_CLK 4      // MAX7219 CLK (D2)
#define PIN_LOAD 0     // MAX7219 LOAD/CS (D3) - Note: Reference uses 6, adjust if needed
#define PIN_BUTTON 2   // Push Button (D4)
#define PIN_BUZZER 13  // Buzzer (D7) - Note: Reference uses 14, adjust if needed

// MPU-6050 I²C Pins (ESP8266)
#define PIN_SDA 12     // SDA (D6)
#define PIN_SCL 14     // SCL (D5)

// Display Configuration
#define NUM_MATRICES 2
#define MATRIX_A 1
#define MATRIX_B 0
#define DISPLAY_INTENSITY 8
#define ROTATION_OFFSET 90

// Sensor Thresholds
#define ACC_THRESHOLD_LOW 300
#define ACC_THRESHOLD_HIGH 360
#define SHAKE_THRESHOLD 0.5
#define FLIP_THRESHOLD 0.8

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

#endif

