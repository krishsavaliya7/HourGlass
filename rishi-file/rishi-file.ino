/*
 * DUAL MODE LED HOURGLASS + DIGITAL CLOCK
 * 
 * Hardware:
 * - Arduino Nano R3
 * - 2x MAX7219 LED Matrix (8x8, Red)
 * - MPU6050 Gyroscope (GY-521)
 * - Push Button (Active-Low on D2)
 * - Piezo Buzzer (5V on D3)
 */

#include <Wire.h>
#include "LedControl.h"
#include "Delay.h"
#include "DiceMode.h"

// Pin Definitions
#define PIN_DIN     11
#define PIN_CS      10
#define PIN_CLK     13
#define PIN_BUTTON  2
#define PIN_BUZZER  3

// Settings
#define LONG_PRESS_TIME 1500
#define MPU6050_ADDR 0x68
#define MATRIX_TOP    1
#define MATRIX_BOTTOM 0
#define DELAY_FRAME 80
#define TOTAL_PARTICLES 60
#define ROTATION_OFFSET 90

// Modes
#define MODE_HOURGLASS 0
#define MODE_CLOCK 1
#define MODE_DICE 2
#define MODE_FLIPCOUNTER 3

LedControl lc = LedControl(PIN_DIN, PIN_CLK, PIN_CS, 2);
DiceMode diceMode(&lc);
NonBlockDelay dropTimer;
int currentGravity = 0;
bool alarmTriggered = false;
int16_t ax, ay, az;  // Added ay for shake detection

int currentMode = MODE_HOURGLASS;
unsigned long buttonPressStart = 0;
bool buttonWasPressed = false;
bool longPressTriggered = false;

// Flip Counter variables
int flipCount = 0;
int lastFlipOrientation = -1;
unsigned long lastFlipTime = 0;
#define FLIP_DEBOUNCE_MS 500  // Minimum time between flips

int clockHours = 11;
int clockMinutes = 31;
int lastDisplayedHours = -1;
int lastDisplayedMinutes = -1;
int lastDisplayedOrientation = -1;
unsigned long lastMinuteUpdate = 0;
bool clockInitialized = true;

int hourglassTimerMinutes = 1;
unsigned long particleDropDelay = 1000;

const byte digit[10][8] = {
  {0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C},
  {0x0C, 0x1C, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F},
  {0x3C, 0x7E, 0xC3, 0x06, 0x1C, 0x30, 0x60, 0xFF},
  {0x3C, 0x7E, 0xC3, 0x0E, 0x0E, 0xC3, 0x7E, 0x3C},
  {0x06, 0x0E, 0x1E, 0x36, 0x66, 0xFF, 0x06, 0x06},
  {0xFF, 0xC0, 0xC0, 0xFC, 0x06, 0x03, 0xC6, 0x7C},
  {0x3C, 0x60, 0xC0, 0xFC, 0xFE, 0xC3, 0x7E, 0x3C},
  {0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x30},
  {0x3C, 0x7E, 0xC3, 0x7E, 0x7E, 0xC3, 0x7E, 0x3C},
  {0x3C, 0x7E, 0xC3, 0x7F, 0x3F, 0x03, 0x06, 0x3C}
};

const byte smallDigit[10][3] = {
  {0x1F, 0x11, 0x1F}, {0x00, 0x1F, 0x00}, {0x1D, 0x15, 0x17},
  {0x15, 0x15, 0x1F}, {0x07, 0x04, 0x1F}, {0x17, 0x15, 0x1D},
  {0x1F, 0x15, 0x1D}, {0x01, 0x01, 0x1F}, {0x1F, 0x15, 0x1F},
  {0x17, 0x15, 0x1F}
};

coord getDown(int x, int y) { return (coord){(byte)(x - 1), (byte)(y + 1)}; }
coord getLeft(int x, int y) { return (coord){(byte)(x - 1), (byte)y}; }
coord getRight(int x, int y) { return (coord){(byte)x, (byte)(y + 1)}; }

bool canGoLeft(int addr, int x, int y) { return x > 0 && !lc.getXY(addr, getLeft(x, y)); }
bool canGoRight(int addr, int x, int y) { return y < 7 && !lc.getXY(addr, getRight(x, y)); }

bool canGoDown(int addr, int x, int y) {
  if (y == 7 || x == 0) return false;
  return canGoLeft(addr, x, y) && canGoRight(addr, x, y) && !lc.getXY(addr, getDown(x, y));
}

void goDown(int addr, int x, int y) { lc.setXY(addr, x, y, false); lc.setXY(addr, getDown(x, y), true); }
void goLeft(int addr, int x, int y) { lc.setXY(addr, x, y, false); lc.setXY(addr, getLeft(x, y), true); }
void goRight(int addr, int x, int y) { lc.setXY(addr, x, y, false); lc.setXY(addr, getRight(x, y), true); }

int getTopMatrix() { return (currentGravity == 90) ? MATRIX_TOP : MATRIX_BOTTOM; }
int getBottomMatrix() { return (currentGravity != 90) ? MATRIX_TOP : MATRIX_BOTTOM; }

void setupMPU6050() {
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
}

void readAccelerometer() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);
  ax = (Wire.read() << 8 | Wire.read());
  ay = (Wire.read() << 8 | Wire.read());  // Read Y axis
  az = (Wire.read() << 8 | Wire.read());
}

int getOrientation() {
  readAccelerometer();
  float roll = atan2(ax, az) * 180.0 / PI;
  if (roll < 0) roll += 360;
  if (roll >= 315 || roll < 45) return 0;
  if (roll >= 45 && roll < 135) return 90;
  if (roll >= 135 && roll < 225) return 180;
  if (roll >= 225 && roll < 315) return 270;
  return 0;
}

// Forward declaration for clock display function
void displayClockMode(bool forceRefresh = false);
void displayFlipCounter();
void checkFlip();

bool moveParticle(int addr, int x, int y) {
  if (!lc.getXY(addr, x, y)) return false;
  bool canL = canGoLeft(addr, x, y);
  bool canR = canGoRight(addr, x, y);
  if (!canL && !canR) return false;
  bool canD = canGoDown(addr, x, y);
  if (canD) goDown(addr, x, y);
  else if (canL && !canR) goLeft(addr, x, y);
  else if (canR && !canL) goRight(addr, x, y);
  else if (random(2)) goLeft(addr, x, y);
  else goRight(addr, x, y);
  return true;
}

bool updateMatrix() {
  bool moved = false;
  for (byte slice = 0; slice < 15; ++slice) {
    bool dir = random(2);
    byte z = slice < 8 ? 0 : slice - 7;
    for (byte j = z; j <= slice - z; ++j) {
      byte y = dir ? (7 - j) : (7 - (slice - j));
      byte x = dir ? (slice - j) : j;
      if (moveParticle(MATRIX_BOTTOM, x, y)) moved = true;
      if (moveParticle(MATRIX_TOP, x, y)) moved = true;
    }
  }
  return moved;
}

bool dropParticle() {
  if (!dropTimer.Timeout()) return false;
  dropTimer.Delay(particleDropDelay);
  if (currentGravity == 0 || currentGravity == 180) {
    if ((lc.getRawXY(MATRIX_TOP, 0, 0) && !lc.getRawXY(MATRIX_BOTTOM, 7, 7)) ||
        (!lc.getRawXY(MATRIX_TOP, 0, 0) && lc.getRawXY(MATRIX_BOTTOM, 7, 7))) {
      lc.invertRawXY(MATRIX_TOP, 0, 0);
      lc.invertRawXY(MATRIX_BOTTOM, 7, 7);
      return true;
    }
  }
  return false;
}

void fillMatrix(int addr, int maxCount) {
  int count = 0;
  for (byte slice = 0; slice < 15; ++slice) {
    byte z = slice < 8 ? 0 : slice - 7;
    for (byte j = z; j <= slice - z; ++j) {
      byte y = 7 - j;
      byte x = slice - j;
      lc.setXY(addr, x, y, (++count <= maxCount));
    }
  }
}

int countParticles(int addr) {
  int count = 0;
  for (byte y = 0; y < 8; y++)
    for (byte x = 0; x < 8; x++)
      if (lc.getXY(addr, x, y)) count++;
  return count;
}

void resetHourglass() {
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  fillMatrix(getTopMatrix(), TOTAL_PARTICLES);
  dropTimer.Delay(particleDropDelay);
  alarmTriggered = false;
}

void checkButton() {
  bool pressed = (digitalRead(PIN_BUTTON) == LOW);
  
  if (pressed && !buttonWasPressed) {
    buttonPressStart = millis();
    buttonWasPressed = true;
    longPressTriggered = false;
  }
  
  if (pressed && buttonWasPressed) {
    if (millis() - buttonPressStart >= LONG_PRESS_TIME && !longPressTriggered) {
      longPressTriggered = true;
      lc.clearDisplay(0);
      lc.clearDisplay(1);
      int prevMode = currentMode;
      if (currentMode == MODE_HOURGLASS) {
        currentMode = MODE_CLOCK;
        // Reset clock tracking for clean display
        lastDisplayedHours = -1;
        lastDisplayedMinutes = -1;
        lastDisplayedOrientation = -1;
      } else if (currentMode == MODE_CLOCK) {
        currentMode = MODE_DICE;
      } else if (currentMode == MODE_DICE) {
        currentMode = MODE_FLIPCOUNTER;
        resetFlipCounter();
      } else {
        currentMode = MODE_HOURGLASS;
      }
      // Handle mode enter/exit
      if (prevMode == MODE_DICE) diceMode.exit();
      if (currentMode == MODE_DICE) diceMode.enter();
      if (currentMode == MODE_CLOCK) displayClockMode(true); // Force clock display
      if (currentMode == MODE_FLIPCOUNTER) displayFlipCounter();
      if (currentMode == MODE_HOURGLASS) resetHourglass();
      tone(PIN_BUZZER, 1000, 100);
      delay(100);
      tone(PIN_BUZZER, 1500, 100);
    }
  }
  
  if (!pressed && buttonWasPressed) {
    if (millis() - buttonPressStart < LONG_PRESS_TIME && !longPressTriggered) {
      if (currentMode == MODE_HOURGLASS) {
        resetHourglass();
        tone(PIN_BUZZER, 800, 50);
      } else if (currentMode == MODE_DICE) {
        diceMode.rollSingle();  // Single dice on both matrices
        tone(PIN_BUZZER, 800, 50);
      } else if (currentMode == MODE_FLIPCOUNTER) {
        resetFlipCounter();
        displayFlipCounter();
        tone(PIN_BUZZER, 800, 50);
      }
    }
    buttonWasPressed = false;
  }
}

void drawTwoDigits(int matrix, int number) {
  if (number < 0) number = 0;
  if (number > 99) number = 99;
  int tens = number / 10;
  int ones = number % 10;
  lc.clearDisplay(matrix);
  for (int col = 0; col < 3; col++) {
    for (int row = 0; row < 5; row++) {
      if (smallDigit[tens][col] & (1 << row))
        lc.setLed(matrix, row + 1, col + 1, true);
      if (smallDigit[ones][col] & (1 << row))
        lc.setLed(matrix, row + 1, col + 5, true);
    }
  }
}

void drawDots(int matrix, int count, bool fillPattern) {
  lc.clearDisplay(matrix);
  int drawn = 0;
  if (fillPattern) {
    for (int sum = 0; sum < 15 && drawn < count; sum++) {
      for (int x = 0; x <= sum && drawn < count; x++) {
        int y = sum - x;
        if (x < 8 && y < 8) {
          lc.setLed(matrix, y, x, true);
          drawn++;
        }
      }
    }
  } else {
    for (int y = 7; y >= 0 && drawn < count; y--) {
      for (int x = 0; x < 8 && drawn < count; x++) {
        lc.setLed(matrix, y, x, true);
        drawn++;
      }
    }
  }
}

void displayClockMode(bool forceRefresh = false) {
  if (!clockInitialized) return;
  int orientation = currentGravity;
  
  // Only redraw if time changed, orientation changed, or forced
  bool timeChanged = (clockHours != lastDisplayedHours) || (clockMinutes != lastDisplayedMinutes);
  bool orientationChanged = (orientation != lastDisplayedOrientation);
  
  if (!forceRefresh && !timeChanged && !orientationChanged) {
    return; // Nothing changed, don't redraw (prevents flicker)
  }
  
  // Clear displays only when we need to redraw
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  lc.setRotation((ROTATION_OFFSET + orientation) % 360);
  
  if (orientation == 0) {
    drawDots(MATRIX_TOP, clockHours, true);
    drawDots(MATRIX_BOTTOM, clockMinutes, true);
  } else if (orientation == 180) {
    drawDots(MATRIX_TOP, clockMinutes, true);
    drawDots(MATRIX_BOTTOM, clockHours, true);
  } else if (orientation == 90) {
    drawTwoDigits(1, clockHours);
    drawTwoDigits(0, clockMinutes);
  } else if (orientation == 270) {
    drawTwoDigits(1, clockMinutes);
    drawTwoDigits(0, clockHours);
  }
  
  // Update tracked values
  lastDisplayedHours = clockHours;
  lastDisplayedMinutes = clockMinutes;
  lastDisplayedOrientation = orientation;
}

void updateClock() {
  if (!clockInitialized) return;
  if (millis() - lastMinuteUpdate >= 60000) {
    lastMinuteUpdate = millis();
    clockMinutes++;
    if (clockMinutes >= 60) {
      clockMinutes = 0;
      clockHours++;
      if (clockHours >= 24) clockHours = 0;
    }
  }
}

void displayFlipCounter() {
  // Display flip count on the appropriate matrix based on orientation
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  lc.setRotation((ROTATION_OFFSET + currentGravity) % 360);
  
  // At 0° (upright): show count on top matrix (MATRIX_TOP)
  // At 180° (upside down): show count on bottom matrix (MATRIX_BOTTOM)
  if (currentGravity == 0) {
    drawTwoDigits(MATRIX_TOP, flipCount);
  } else if (currentGravity == 180) {
    drawTwoDigits(MATRIX_BOTTOM, flipCount);
  } else {
    // For other orientations, show on both
    drawTwoDigits(0, flipCount);
    drawTwoDigits(1, flipCount);
  }
}

void checkFlip() {
  // Detect flip: only count when going between 0° and 180°
  int currentOrientation = currentGravity;
  
  if (lastFlipOrientation == -1) {
    lastFlipOrientation = currentOrientation;
    return;
  }
  
  // Only count flip when transitioning between 0° and 180°
  bool flipped = (lastFlipOrientation == 0 && currentOrientation == 180) || 
                 (lastFlipOrientation == 180 && currentOrientation == 0);
  
  if (flipped && (millis() - lastFlipTime > FLIP_DEBOUNCE_MS)) {
    flipCount++;
    lastFlipTime = millis();
    // Beep on flip detection
    tone(PIN_BUZZER, 1200, 100);
  }
  
  lastFlipOrientation = currentOrientation;
}

void resetFlipCounter() {
  flipCount = 0;
  lastFlipOrientation = -1;
}

void printModeName() {
  if (currentMode == MODE_HOURGLASS) Serial.print("hourglass");
  else if (currentMode == MODE_CLOCK) Serial.print("clock");
  else if (currentMode == MODE_DICE) Serial.print("dice");
  else Serial.print("flipcounter");
}

void handleSerialCommands() {
  if (!Serial.available()) return;
  String input = Serial.readStringUntil('\n');
  input.trim();
  if (input.length() == 0) return;
  
  if (input == "SET_MODE hourglass") {
    if (currentMode == MODE_DICE) diceMode.exit();
    currentMode = MODE_HOURGLASS;
    resetHourglass();
    tone(PIN_BUZZER, 1000, 100);
    delay(100);
    tone(PIN_BUZZER, 1500, 100);
    Serial.println("OK MODE HOURGLASS");
  }
  else if (input == "SET_MODE clock") {
    if (currentMode == MODE_DICE) diceMode.exit();
    currentMode = MODE_CLOCK;
    // Reset clock display tracking to force refresh
    lastDisplayedHours = -1;
    lastDisplayedMinutes = -1;
    lastDisplayedOrientation = -1;
    displayClockMode(true); // Force immediate display
    tone(PIN_BUZZER, 1000, 100);
    delay(100);
    tone(PIN_BUZZER, 1500, 100);
    Serial.println("OK MODE CLOCK");
  }
  else if (input == "SET_MODE dice") {
    if (currentMode != MODE_DICE) {
      diceMode.exit();  // Exit previous mode if needed
    }
    currentMode = MODE_DICE;
    diceMode.enter();
    tone(PIN_BUZZER, 1000, 100);
    delay(100);
    tone(PIN_BUZZER, 1500, 100);
    Serial.println("OK MODE DICE");
  }
  else if (input == "SET_MODE flipcounter") {
    if (currentMode == MODE_DICE) diceMode.exit();
    currentMode = MODE_FLIPCOUNTER;
    resetFlipCounter();
    displayFlipCounter();
    tone(PIN_BUZZER, 1000, 100);
    delay(100);
    tone(PIN_BUZZER, 1500, 100);
    Serial.println("OK MODE FLIPCOUNTER");
  }
  else if (input.startsWith("SET_TIME")) {
    int s = input.indexOf(' ');
    int s2 = input.indexOf(' ', s + 1);
    if (s > 0 && s2 > s) {
      int h = input.substring(s + 1, s2).toInt();
      int m = input.substring(s2 + 1).toInt();
      if (h >= 0 && h <= 23 && m >= 0 && m <= 59) {
        clockHours = h;
        clockMinutes = m;
        lastMinuteUpdate = millis();
        clockInitialized = true;
        // Reset tracking to force display update
        lastDisplayedHours = -1;
        lastDisplayedMinutes = -1;
        displayClockMode(true); // Force refresh
        Serial.print("OK TIME ");
        Serial.print(h);
        Serial.print(":");
        if (m < 10) Serial.print("0");
        Serial.println(m);
      } else Serial.println("ERR Invalid time range");
    } else Serial.println("ERR Invalid time format");
  }
  else if (input == "RESET_HG") {
    resetHourglass();
    Serial.println("OK HOURGLASS RESET");
  }
  else if (input == "GET_STATUS") {
    Serial.print("{\"mode\":\"");
    printModeName();
    Serial.print("\",\"time\":\"");
    if (clockHours < 10) Serial.print("0");
    Serial.print(clockHours);
    Serial.print(":");
    if (clockMinutes < 10) Serial.print("0");
    Serial.print(clockMinutes);
    Serial.print("\",\"orientation\":");
    Serial.print(currentGravity);
    if (currentMode == MODE_HOURGLASS) {
      int top = countParticles(getTopMatrix());
      Serial.print(",\"hourglassProgress\":");
      Serial.print(((TOTAL_PARTICLES - top) * 100) / TOTAL_PARTICLES);
      Serial.print(",\"particlesRemaining\":");
      Serial.print(top);
    }
    if (currentMode == MODE_DICE) {
      Serial.print(",\"diceValue\":");
      Serial.print(diceMode.getValueA());
      Serial.print(",\"diceA\":");
      Serial.print(diceMode.getValueA());
      Serial.print(",\"diceB\":");
      Serial.print(diceMode.getValueB());
    }
    if (currentMode == MODE_FLIPCOUNTER) {
      Serial.print(",\"flipCount\":");
      Serial.print(flipCount);
    }
    Serial.println("}");
  }
  else if (input == "GET_ORIENTATION") {
    Serial.print("{\"angle\":");
    Serial.print(currentGravity);
    Serial.println("}");
  }
  else if (input.startsWith("SET_HG")) {
    int s1 = input.indexOf(' ');
    int s2 = input.indexOf(' ', s1 + 1);
    if (s1 > 0 && s2 > s1) {
      int h = input.substring(s1 + 1, s2).toInt();
      int m = input.substring(s2 + 1).toInt();
      int total = h * 60 + m;
      if (total >= 1 && total <= 60) {
        hourglassTimerMinutes = total;
        particleDropDelay = ((unsigned long)total * 60000UL) / TOTAL_PARTICLES;
        resetHourglass();
        Serial.print("OK TIMER SET TO ");
        Serial.print(total);
        Serial.println(" MIN");
      } else if (total > 60) {
        hourglassTimerMinutes = 60;
        particleDropDelay = (60UL * 60000UL) / TOTAL_PARTICLES;
        resetHourglass();
        Serial.println("OK TIMER SET TO 60 MIN (capped)");
      } else Serial.println("ERR Timer must be at least 1 minute");
    } else Serial.println("ERR Format: SET_HG <hours> <minutes>");
  }
  else if (input.startsWith("SET_BRIGHTNESS")) {
    int s = input.indexOf(' ');
    if (s > 0) {
      int b = input.substring(s + 1).toInt();
      if (b >= 0 && b <= 15) {
        for (int i = 0; i < 2; i++) lc.setIntensity(i, b);
        Serial.println("OK BRIGHTNESS SET");
      } else Serial.println("ERR Brightness 0-15");
    } else Serial.println("ERR Missing brightness value");
  }
  else if (input == "GET_DISPLAY") {
    Serial.print("{\"matrixA\":\"");
    for (int row = 0; row < 8; row++) {
      byte rb = 0;
      for (int col = 0; col < 8; col++) if (lc.getXY(1, col, row)) rb |= (1 << (7 - col));
      if (rb < 16) Serial.print("0");
      Serial.print(rb, HEX);
    }
    Serial.print("\",\"matrixB\":\"");
    for (int row = 0; row < 8; row++) {
      byte rb = 0;
      for (int col = 0; col < 8; col++) if (lc.getXY(0, col, row)) rb |= (1 << (7 - col));
      if (rb < 16) Serial.print("0");
      Serial.print(rb, HEX);
    }
    Serial.println("\"}");
  }
  else if (input == "GET_ALL") {
    Serial.print("{\"status\":{");
    Serial.print("\"mode\":\"");
    printModeName();
    Serial.print("\",\"time\":\"");
    if (clockHours < 10) Serial.print("0");
    Serial.print(clockHours);
    Serial.print(":");
    if (clockMinutes < 10) Serial.print("0");
    Serial.print(clockMinutes);
    Serial.print("\",\"orientation\":");
    Serial.print(currentGravity);
    if (currentMode == MODE_HOURGLASS) {
      int top = countParticles(getTopMatrix());
      Serial.print(",\"hourglassProgress\":");
      Serial.print(((TOTAL_PARTICLES - top) * 100) / TOTAL_PARTICLES);
      Serial.print(",\"particlesRemaining\":");
      Serial.print(top);
    }
    if (currentMode == MODE_DICE) {
      Serial.print(",\"diceValue\":");
      Serial.print(diceMode.getValueA());
      Serial.print(",\"diceA\":");
      Serial.print(diceMode.getValueA());
      Serial.print(",\"diceB\":");
      Serial.print(diceMode.getValueB());
    }
    if (currentMode == MODE_FLIPCOUNTER) {
      Serial.print(",\"flipCount\":");
      Serial.print(flipCount);
    }
    Serial.print("},\"orientation\":{");
    Serial.print("\"angle\":");
    Serial.print(currentGravity);
    Serial.print("},\"display\":{");
    Serial.print("\"matrixA\":\"");
    for (int row = 0; row < 8; row++) {
      byte rb = 0;
      for (int col = 0; col < 8; col++) if (lc.getXY(1, col, row)) rb |= (1 << (7 - col));
      if (rb < 16) Serial.print("0");
      Serial.print(rb, HEX);
    }
    Serial.print("\",\"matrixB\":\"");
    for (int row = 0; row < 8; row++) {
      byte rb = 0;
      for (int col = 0; col < 8; col++) if (lc.getXY(0, col, row)) rb |= (1 << (7 - col));
      if (rb < 16) Serial.print("0");
      Serial.print(rb, HEX);
    }
    Serial.println("\"}}");
  }
  else if (input == "ROLL_DICE") {
    diceMode.roll();  // Roll two different dice
    int dvA = diceMode.getValueA();
    int dvB = diceMode.getValueB();
    Serial.print("{\"value\":");
    Serial.print(dvA + dvB);  // Sum for compatibility
    Serial.print(",\"diceValue\":");
    Serial.print(dvA);
    Serial.print(",\"diceA\":");
    Serial.print(dvA);
    Serial.print(",\"diceB\":");
    Serial.print(dvB);
    Serial.println("}");
    Serial.flush();
  }
  else if (input == "RESET_FLIP") {
    resetFlipCounter();
    if (currentMode == MODE_FLIPCOUNTER) displayFlipCounter();
    Serial.println("OK FLIP COUNTER RESET");
  }
  else if (input == "GET_FLIP_COUNT") {
    Serial.print("{\"count\":");
    Serial.print(flipCount);
    Serial.println("}");
    Serial.flush();
  }
  else Serial.println("ERR Unknown command");
}

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  setupMPU6050();
  delay(100);
  for (byte i = 0; i < 2; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 4);
    lc.clearDisplay(i);
  }
  currentGravity = getOrientation();
  lc.setRotation((ROTATION_OFFSET + currentGravity) % 360);
  diceMode.init();
  resetHourglass();
}

void loop() {
  delay(DELAY_FRAME);
  handleSerialCommands();
  checkButton();
  if (currentMode == MODE_CLOCK) {
    updateClock();
  }
  
  int prevGravity = currentGravity;
  int newGravity = getOrientation();
  static int lastReadGravity = 0;
  static int stableCount = 0;
  if (newGravity == lastReadGravity) stableCount++;
  else { stableCount = 0; lastReadGravity = newGravity; }
  if (stableCount >= 12 && newGravity != currentGravity) {
    currentGravity = newGravity;
    stableCount = 0;
  }
  lc.setRotation((ROTATION_OFFSET + currentGravity) % 360);
  
  if (currentMode == MODE_HOURGLASS) {
    bool moved = updateMatrix();
    bool dropped = dropParticle();
    if (!moved && !dropped && !alarmTriggered) {
      if (countParticles(getTopMatrix()) == 0) {
        alarmTriggered = true;
        for (int i = 0; i < 3; i++) {
          tone(PIN_BUZZER, 1000, 200);
          delay(300);
        }
      }
    }
    if (dropped) alarmTriggered = false;
  } else if (currentMode == MODE_CLOCK) {
    // Static clock - only updates when time or orientation changes (no flicker)
    displayClockMode(false);
  } else if (currentMode == MODE_DICE) {
    diceMode.update();
  } else if (currentMode == MODE_FLIPCOUNTER) {
    checkFlip();
    // Update display if count changed or orientation changed (0° vs 180°)
    static int lastDisplayedFlipCount = -1;
    static int lastDisplayedFlipOrientation = -1;
    if (flipCount != lastDisplayedFlipCount || currentGravity != lastDisplayedFlipOrientation) {
      displayFlipCounter();
      lastDisplayedFlipCount = flipCount;
      lastDisplayedFlipOrientation = currentGravity;
    }
  }
}
