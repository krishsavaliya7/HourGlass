/**
 * Smart Hourglass & Clock System - Main Firmware
 * Supports: Clock, Hourglass, Dice, Flip Counter modes
 * USB Serial interface for remote control
 */

#include <Arduino.h>
#include <Wire.h>
#include "config.h"

/* ========= FORWARD DECLARATIONS (REQUIRED) ========= */
void initDisplay();
void initSensors();
void handleButtonInput();
void updateCurrentMode();
void cycleMode();

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
const char* getStatusJSON();
const char* getOrientationJSON();
const char* getDisplayJSON();
void matrixToJson(int matrixAddr, char* buf, size_t bufsize);
/* ================================================== */

#include "LedControl.h"
#include "Delay.h"
#include "SerialProtocol.h"
#include "MPU6050.h"
#include "Button.h"
#include "ClockMode.h"
#include "HourglassMode.h"
#include "DiceMode.h"
#include "FlipCounterMode.h"

/* ========= GLOBAL OBJECTS ========= */
LedControl lc(PIN_DATAIN, PIN_CLK, PIN_LOAD, NUM_MATRICES);
MPU6050 mpu;
Button button(PIN_BUTTON);
SerialProtocol serialProtocol;
// Removed NonBlockDelay statusUpdateDelay - saves 8 bytes RAM

/* ========= MODE OBJECTS ========= */
ClockMode clockMode(&lc, &mpu);
HourglassMode hourglassMode(&lc, &mpu);
DiceMode diceMode(&lc, &mpu);
FlipCounterMode flipCounterMode(&lc, &mpu);

/* ========= STATE ========= */
int currentMode = MODE_HOURGLASS;
bool deviceInitialized = false;
// Removed unused lastUpdate variable - saves 4 bytes RAM

/* ========= SETUP ========= */
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);

#if DEBUG_OUTPUT
  Serial.println(F("\n=== Smart Hourglass System ==="));
  Serial.print(F("Firmware Version: "));
  Serial.println(FIRMWARE_VERSION);
#endif

  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);

  initDisplay();
  initSensors();

  button.init();
  serialProtocol.init();

  clockMode.init();
  hourglassMode.init();
  diceMode.init();
  flipCounterMode.init();

  setMode(MODE_HOURGLASS);

  deviceInitialized = true;

#if DEBUG_OUTPUT
  Serial.println(F("System initialized"));
#endif
}

/* ========= LOOP ========= */
void loop() {
  delay(DELAY_FRAME);

  button.update();
  handleButtonInput();

  mpu.update();
  updateCurrentMode();

  serialProtocol.update();
  // Removed unused status update code - already handled by modes
}

/* ========= INIT ========= */
void initDisplay() {
  for (byte i = 0; i < NUM_MATRICES; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, DISPLAY_INTENSITY);
    lc.clearDisplay(i);
  }
}

void initSensors() {
#if defined(ESP8266) || defined(ESP32)
  Wire.begin(PIN_SDA, PIN_SCL);
#else
  Wire.begin();
#endif
  mpu.init();
}

/* ========= INPUT ========= */
void handleButtonInput() {
  if (button.wasPressed()) {
    if (currentMode == MODE_DICE) rollDice();
    if (currentMode == MODE_HOURGLASS) hourglassMode.reset();
  }
  if (button.wasLongPressed()) {
    cycleMode();
  }
}

/* ========= MODES ========= */
void updateCurrentMode() {
  switch (currentMode) {
    case MODE_CLOCK:       clockMode.update(); break;
    case MODE_HOURGLASS:   hourglassMode.update(); break;
    case MODE_DICE:        diceMode.update(); break;
    case MODE_FLIPCOUNTER: flipCounterMode.update(); break;
  }
}

void setMode(int mode) {
  if (mode < 0 || mode >= NUM_MODES) return;

  switch (currentMode) {
    case MODE_CLOCK:       clockMode.exit(); break;
    case MODE_HOURGLASS:   hourglassMode.exit(); break;
    case MODE_DICE:        diceMode.exit(); break;
    case MODE_FLIPCOUNTER: flipCounterMode.exit(); break;
  }

  currentMode = mode;

  switch (currentMode) {
    case MODE_CLOCK:       clockMode.enter(); break;
    case MODE_HOURGLASS:   hourglassMode.enter(); break;
    case MODE_DICE:        diceMode.enter(); break;
    case MODE_FLIPCOUNTER: flipCounterMode.enter(); break;
  }
}

void cycleMode() {
  setMode((currentMode + 1) % NUM_MODES);
}

/* ========= ACTIONS ========= */
void setClockTime(int h, int m) { clockMode.setTime(h, m); }
void setHourglassDuration(int h, int m) { hourglassMode.setDuration(h, m); }
void resetHourglass() { hourglassMode.reset(); }
void rollDice() { diceMode.roll(); }
void resetFlipCounter() { flipCounterMode.reset(); }

/* ========= GETTERS ========= */
int getCurrentMode() { return currentMode; }
int getDiceValue() { return diceMode.getValue(); }
int getFlipCount() { return flipCounterMode.getCount(); }

/* ========= DISPLAY ========= */
void setBrightness(int level) {
  level = constrain(level, 0, 15);
  for (byte i = 0; i < NUM_MATRICES; i++) {
    lc.setIntensity(i, level);
  }
}

/* ========= JSON ========= */
const char* getStatusJSON() {
  static char buffer[32];
  snprintf(buffer, sizeof(buffer), "{\"mode\":%d}", currentMode);
  return buffer;
}

const char* getOrientationJSON() {
  static char buffer[32];
  snprintf(buffer, sizeof(buffer), "{\"angle\":%d}", mpu.getAngle());
  return buffer;
}

const char* getDisplayJSON() {
  // Minimized buffer sizes for memory-constrained Arduino Nano
  static char buffer[256];   // Reduced from 320
  static char matrixA_buf[100]; // Reduced from 128
  static char matrixB_buf[100]; // Reduced from 128
  
  // Fill each matrix buffer (matrixToJson now takes caller-provided buffer)
  matrixToJson(MATRIX_A, matrixA_buf, sizeof(matrixA_buf));
  matrixToJson(MATRIX_B, matrixB_buf, sizeof(matrixB_buf));
  
  // Build final JSON from the two distinct buffers
  snprintf(buffer, sizeof(buffer), "{\"matrixA\":%s,\"matrixB\":%s}", matrixA_buf, matrixB_buf);
  return buffer;
}

void matrixToJson(int matrixAddr, char* buf, size_t bufsize) {
  // Write 8x8 matrix JSON into caller-provided buffer
  // Format: [[1,0,1,...],[...],...]
  // Prevents reentrancy issues by using caller's buffer instead of static
  if (!buf || bufsize < 100) return;  // Reduced from 120, buffer now 100 bytes
  
  int pos = 0;
  buf[pos++] = '[';
  for (int r = 0; r < 8; r++) {
    buf[pos++] = '[';
    for (int c = 0; c < 8; c++) {
      buf[pos++] = lc.getRawXY(matrixAddr, c, r) ? '1' : '0';
      if (c < 7) buf[pos++] = ',';
    }
    buf[pos++] = ']';
    if (r < 7) buf[pos++] = ',';
  }
  buf[pos++] = ']';
  buf[pos] = '\0';
}