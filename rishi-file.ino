/*
 * DUAL MODE LED HOURGLASS + DIGITAL CLOCK
 * 
 * Hardware:
 * - Arduino Nano R3
 * - 2x MAX7219 LED Matrix (8x8, Red)
 * - MPU6050 Gyroscope (GY-521)
 * - Push Button (Active-Low on D2)
 * - Piezo Buzzer (5V on D3)
 * 
 * Modes:
 * 1. Hourglass Mode (default)
 * 2. Clock Mode (long press button to switch)
 */

#include <Wire.h>
#include "LedControl.h"
#include "Delay.h"

// Pin Definitions
#define PIN_DIN     11    // MAX7219 Data In
#define PIN_CS      10    // MAX7219 Chip Select
#define PIN_CLK     13    // MAX7219 Clock
#define PIN_BUTTON  2     // Push Button (D2)
#define PIN_BUZZER  3     // Buzzer (D3)
#define PIN_SDA     A4    // MPU6050 SDA
#define PIN_SCL     A5    // MPU6050 SCL

// Button Settings
#define LONG_PRESS_TIME 1500  // 1.5 seconds for mode switch
#define DEBOUNCE_TIME 50      // 50ms debounce

// MPU6050 Settings
#define MPU6050_ADDR 0x68

// Matrix Addresses
#define MATRIX_TOP    1
#define MATRIX_BOTTOM 0

// Hourglass Settings
#define DELAY_FRAME 80
#define PARTICLE_DROP_DELAY 1000
#define TOTAL_PARTICLES 60
#define ROTATION_OFFSET 90

// Modes
#define MODE_HOURGLASS 0
#define MODE_CLOCK 1


// Global Variables
LedControl lc = LedControl(PIN_DIN, PIN_CLK, PIN_CS, 2);
NonBlockDelay dropTimer;
int currentGravity = 0;
bool alarmTriggered = false;
int16_t ax, ay, az;

// Mode & Button
int currentMode = MODE_HOURGLASS;
unsigned long buttonPressStart = 0;
bool buttonWasPressed = false;
bool longPressTriggered = false;

// Clock Variables
int clockHours = 11;
int clockMinutes = 31;
unsigned long lastMinuteUpdate = 0;
bool clockInitialized = true;

// Hourglass Timer Settings
int hourglassTimerMinutes = 1;  // Default 1 minute timer
unsigned long particleDropDelay = PARTICLE_DROP_DELAY;  // Dynamic drop delay

// Large block font for digits (8 columns x 8 rows each digit)
// Each digit fills the entire 8x8 matrix
const byte digit[10][8] = {
  {0x3C, 0x7E, 0xE7, 0xC3, 0xC3, 0xE7, 0x7E, 0x3C}, // 0
  {0x0C, 0x1C, 0x3C, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F}, // 1
  {0x3C, 0x7E, 0xC3, 0x06, 0x1C, 0x30, 0x60, 0xFF}, // 2
  {0x3C, 0x7E, 0xC3, 0x0E, 0x0E, 0xC3, 0x7E, 0x3C}, // 3
  {0x06, 0x0E, 0x1E, 0x36, 0x66, 0xFF, 0x06, 0x06}, // 4
  {0xFF, 0xC0, 0xC0, 0xFC, 0x06, 0x03, 0xC6, 0x7C}, // 5
  {0x3C, 0x60, 0xC0, 0xFC, 0xFE, 0xC3, 0x7E, 0x3C}, // 6
  {0xFF, 0xFF, 0x03, 0x06, 0x0C, 0x18, 0x30, 0x30}, // 7
  {0x3C, 0x7E, 0xC3, 0x7E, 0x7E, 0xC3, 0x7E, 0x3C}, // 8
  {0x3C, 0x7E, 0xC3, 0x7F, 0x3F, 0x03, 0x06, 0x3C}  // 9
};

/*
 * MPU6050 Functions
 */
void setupMPU6050() {
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1C);
  Wire.write(0x00);
  Wire.endTransmission(true);
}

void readAccelerometer() {
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050_ADDR, 6, true);
  
  ax = (Wire.read() << 8 | Wire.read());
  ay = (Wire.read() << 8 | Wire.read());
  az = (Wire.read() << 8 | Wire.read());
}

int getOrientation() {
  readAccelerometer();
  
  // Use ONLY roll (rotation around Y-axis) - completely ignore pitch
  // This means only left/right flipping matters, not forward/backward tilting
  float roll = atan2(ax, az) * 180.0 / PI;
  
  if (roll < 0) roll += 360;
  
  // Simplified and wider ranges to be stable
  // 0° = upright, 90° = right side, 180° = upside down, 270° = left side
  if (roll >= 30 && roll < 150) {
    return 0;    // ✓ Sand falls HERE (roll ~84°)
  } else if (roll >= 150 && roll < 210) {
    return 90;   // On side
  } else if (roll >= 210 && roll < 330) {
    return 180;  // Flipped upside down (roll ~264°, sand also falls)
  } else {
    return 270;  // On other side
  }
}

int getTopMatrix() {
  return (currentGravity == 90) ? MATRIX_TOP : MATRIX_BOTTOM;
}

int getBottomMatrix() {
  return (currentGravity != 90) ? MATRIX_TOP : MATRIX_BOTTOM;
}

/*
 * Button Handler
 */
void checkButton() {
  bool buttonPressed = (digitalRead(PIN_BUTTON) == LOW);
  
  if (buttonPressed && !buttonWasPressed) {
    // Button just pressed
    buttonPressStart = millis();
    buttonWasPressed = true;
    longPressTriggered = false;
  }
  
  if (buttonPressed && buttonWasPressed) {
    // Button held down
    unsigned long pressDuration = millis() - buttonPressStart;
    
    if (pressDuration >= LONG_PRESS_TIME && !longPressTriggered) {
      // Long press detected - switch mode
      longPressTriggered = true;
      
      if (currentMode == MODE_HOURGLASS) {
        currentMode = MODE_CLOCK;
        Serial.println("*** SWITCHED TO CLOCK MODE ***");
      } else {
        currentMode = MODE_HOURGLASS;
        Serial.println("*** SWITCHED TO HOURGLASS MODE ***");
        resetHourglass();
      }
      
      // Clear displays
      lc.clearDisplay(0);
      lc.clearDisplay(1);
      
      // Brief feedback
      tone(PIN_BUZZER, 1000, 100);
      delay(100);
      tone(PIN_BUZZER, 1500, 100);
    }
  }
  
  if (!buttonPressed && buttonWasPressed) {
    // Button released
    unsigned long pressDuration = millis() - buttonPressStart;
    
    if (pressDuration < LONG_PRESS_TIME && !longPressTriggered) {
      // Short press - reset hourglass (only in hourglass mode)
      if (currentMode == MODE_HOURGLASS) {
        resetHourglass();
        tone(PIN_BUZZER, 800, 50);
      }
    }
    
    buttonWasPressed = false;
  }
}

/*
 * Clock Functions
 */
void initializeClockFromSerial() {
  // Format: "T12:30" sets time to 12:30
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    if (input.startsWith("T") && input.length() >= 6) {
      int h = input.substring(1, 3).toInt();
      int m = input.substring(4, 6).toInt();
      
      if (h >= 0 && h <= 23 && m >= 0 && m <= 59) {
        clockHours = h;
        clockMinutes = m;
        lastMinuteUpdate = millis();
        clockInitialized = true;
        
        Serial.print("Clock set to: ");
        Serial.print(clockHours);
        Serial.print(":");
        if (clockMinutes < 10) Serial.print("0");
        Serial.println(clockMinutes);
      }
    }
  }
}

void updateClock() {
  if (!clockInitialized) return;
  
  unsigned long currentTime = millis();
  
  // Update every minute
  if (currentTime - lastMinuteUpdate >= 60000) {
    lastMinuteUpdate = currentTime;
    clockMinutes++;
    
    if (clockMinutes >= 60) {
      clockMinutes = 0;
      clockHours++;
      
      if (clockHours >= 24) {
        clockHours = 0;
      }
    }
  }
}

// Small compact font for digits (3x5 each) to fit 2 digits on one matrix
const byte smallDigit[10][3] = {
  {0x1F, 0x11, 0x1F}, // 0
  {0x00, 0x1F, 0x00}, // 1
  {0x1D, 0x15, 0x17}, // 2
  {0x15, 0x15, 0x1F}, // 3
  {0x07, 0x04, 0x1F}, // 4
  {0x17, 0x15, 0x1D}, // 5
  {0x1F, 0x15, 0x1D}, // 6
  {0x01, 0x01, 0x1F}, // 7
  {0x1F, 0x15, 0x1F}, // 8
  {0x17, 0x15, 0x1F}  // 9
};

void drawTwoDigits(int matrix, int number) {
  // Draw 2-digit number on a single 8x8 matrix with spacing
  // number should be 0-99
  if (number < 0) number = 0;
  if (number > 99) number = 99;
  
  int tens = number / 10;
  int ones = number % 10;
  
  lc.clearDisplay(matrix);
  
  // Draw tens digit on left (columns 0-2)
  for (int col = 0; col < 3; col++) {
    byte columnData = smallDigit[tens][col];
    for (int row = 0; row < 5; row++) {
      if (columnData & (1 << row)) {
        lc.setLed(matrix, row + 1, col + 1, true);
      }
    }
  }
  
  // Column 4 is blank for spacing
  
  // Draw ones digit on right (columns 5-7) - moved from 4-6 to 5-7
  for (int col = 0; col < 3; col++) {
    byte columnData = smallDigit[ones][col];
    for (int row = 0; row < 5; row++) {
      if (columnData & (1 << row)) {
        lc.setLed(matrix, row + 1, col + 5, true);  // Changed from col + 4 to col + 5
      }
    }
  }
}

void drawDots(int matrix, int count, bool fillPattern) {
  // Draw dots representing time
  lc.clearDisplay(matrix);
  
  if (fillPattern) {
    // Fill in diagonal pattern
    int drawn = 0;
    for (int sum = 0; sum < 15 && drawn < count; sum++) {
      for (int x = 0; x <= sum && drawn < count; x++) {
        int y = sum - x;
        if (x < 8 && y < 8) {
          lc.setLed(matrix, y, x, true);
          drawn++;
          if (drawn >= count) break;
        }
      }
    }
  } else {
    // Simple row-by-row fill
    int drawn = 0;
    for (int y = 7; y >= 0 && drawn < count; y--) {
      for (int x = 0; x < 8 && drawn < count; x++) {
        lc.setLed(matrix, y, x, true);
        drawn++;
      }
    }
  }
}

void displayClockMode() {
  if (!clockInitialized) {
    return;
  }
  
  int orientation = currentGravity;
  
  // Add stability - only update if orientation is clearly different
  static int lastOrientation = -1;
  static unsigned long lastOrientationChange = 0;
  
  if (orientation != lastOrientation) {
    if (millis() - lastOrientationChange > 300) {
      lastOrientation = orientation;
      lastOrientationChange = millis();
    } else {
      orientation = lastOrientation;
    }
  }
  
  lc.clearDisplay(0);
  lc.clearDisplay(1);
  
  if (orientation == 0) {
    // Upright: Top=Hours (dots), Bottom=Minutes (dots)
    lc.setRotation((ROTATION_OFFSET + 0) % 360);
    drawDots(MATRIX_TOP, clockHours, true);
    drawDots(MATRIX_BOTTOM, clockMinutes, true);
    
  } else if (orientation == 180) {
    // Flipped 180: Top=Minutes (dots), Bottom=Hours (dots)
    lc.setRotation((ROTATION_OFFSET + 180) % 360);
    drawDots(MATRIX_TOP, clockMinutes, true);
    drawDots(MATRIX_BOTTOM, clockHours, true);
    
  } else if (orientation == 90) {
    // Right side: Left matrix=HH (hours), Right matrix=MM (minutes)
    lc.setRotation((ROTATION_OFFSET + 90) % 360);
    
    // Left matrix (MATRIX_1) shows hours as "HH" (e.g., "11")
    drawTwoDigits(1, clockHours);
    
    // Right matrix (MATRIX_0) shows minutes as "MM" (e.g., "31")
    drawTwoDigits(0, clockMinutes);
    
  } else if (orientation == 270) {
    // Left side: Left matrix=MM (minutes), Right matrix=HH (hours)
    lc.setRotation((ROTATION_OFFSET + 270) % 360);
    
    // Left matrix shows minutes as "MM"
    drawTwoDigits(1, clockMinutes);
    
    // Right matrix shows hours as "HH"
    drawTwoDigits(0, clockHours);
  }
}

/*
 * Hourglass Functions
 */
coord getDown(int x, int y) {
  coord xy;
  xy.x = x - 1;
  xy.y = y + 1;
  return xy;
}

coord getLeft(int x, int y) {
  coord xy;
  xy.x = x - 1;
  xy.y = y;
  return xy;
}

coord getRight(int x, int y) {
  coord xy;
  xy.x = x;
  xy.y = y + 1;
  return xy;
}

bool canGoLeft(int addr, int x, int y) {
  if (x == 0) return false;
  return !lc.getXY(addr, getLeft(x, y));
}

bool canGoRight(int addr, int x, int y) {
  if (y == 7) return false;
  return !lc.getXY(addr, getRight(x, y));
}

bool canGoDown(int addr, int x, int y) {
  if (y == 7) return false;
  if (x == 0) return false;
  if (!canGoLeft(addr, x, y)) return false;
  if (!canGoRight(addr, x, y)) return false;
  return !lc.getXY(addr, getDown(x, y));
}

void goDown(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getDown(x, y), true);
}

void goLeft(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getLeft(x, y), true);
}

void goRight(int addr, int x, int y) {
  lc.setXY(addr, x, y, false);
  lc.setXY(addr, getRight(x, y), true);
}

bool moveParticle(int addr, int x, int y) {
  if (!lc.getXY(addr, x, y)) {
    return false;
  }

  bool can_GoLeft = canGoLeft(addr, x, y);
  bool can_GoRight = canGoRight(addr, x, y);

  if (!can_GoLeft && !can_GoRight) {
    return false;
  }

  bool can_GoDown = canGoDown(addr, x, y);

  if (can_GoDown) {
    goDown(addr, x, y);
  } else if (can_GoLeft && !can_GoRight) {
    goLeft(addr, x, y);
  } else if (can_GoRight && !can_GoLeft) {
    goRight(addr, x, y);
  } else if (random(2) == 1) {
    goLeft(addr, x, y);
  } else {
    goRight(addr, x, y);
  }
  
  return true;
}

bool updateMatrix() {
  int n = 8;
  bool somethingMoved = false;
  byte x, y;
  bool direction;
  
  for (byte slice = 0; slice < 2 * n - 1; ++slice) {
    direction = (random(2) == 1);
    byte z = slice < n ? 0 : slice - n + 1;
    
    for (byte j = z; j <= slice - z; ++j) {
      y = direction ? (7 - j) : (7 - (slice - j));
      x = direction ? (slice - j) : j;
      
      if (moveParticle(MATRIX_BOTTOM, x, y)) {
        somethingMoved = true;
      }
      if (moveParticle(MATRIX_TOP, x, y)) {
        somethingMoved = true;
      }
    }
  }
  
  return somethingMoved;
}

bool dropParticle() {
  if (dropTimer.Timeout()) {
    dropTimer.Delay(particleDropDelay);
    
    if (currentGravity == 0 || currentGravity == 180) {
      if ((lc.getRawXY(MATRIX_TOP, 0, 0) && !lc.getRawXY(MATRIX_BOTTOM, 7, 7)) ||
          (!lc.getRawXY(MATRIX_TOP, 0, 0) && lc.getRawXY(MATRIX_BOTTOM, 7, 7))) {
        
        lc.invertRawXY(MATRIX_TOP, 0, 0);
        lc.invertRawXY(MATRIX_BOTTOM, 7, 7);
        
        return true;
      }
    }
  }
  return false;
}

void fillMatrix(int addr, int maxCount) {
  int n = 8;
  byte x, y;
  int count = 0;
  
  for (byte slice = 0; slice < 2 * n - 1; ++slice) {
    byte z = slice < n ? 0 : slice - n + 1;
    for (byte j = z; j <= slice - z; ++j) {
      y = 7 - j;
      x = slice - j;
      lc.setXY(addr, x, y, (++count <= maxCount));
    }
  }
}

int countParticles(int addr) {
  int count = 0;
  for (byte y = 0; y < 8; y++) {
    for (byte x = 0; x < 8; x++) {
      if (lc.getXY(addr, x, y)) {
        count++;
      }
    }
  }
  return count;
}

void resetHourglass() {
  for (byte i = 0; i < 2; i++) {
    lc.clearDisplay(i);
  }
  
  fillMatrix(getTopMatrix(), TOTAL_PARTICLES);
  
  dropTimer.Delay(particleDropDelay);
  alarmTriggered = false;
}

/*
 * Setup
 */
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
  
  resetHourglass();
  
  Serial.println("=== DUAL MODE DEVICE READY ===");
  Serial.println("Long press button (1.5s) to switch modes");
  Serial.println("Send 'T12:30' to set clock time");
  Serial.println("Current Mode: HOURGLASS");
  Serial.print("Clock initialized at: ");
  Serial.print(clockHours);
  Serial.print(":");
  if (clockMinutes < 10) Serial.print("0");
  Serial.println(clockMinutes);
}

/*
 * Main Loop
 */
/*
 * Serial Command Handler
 */
void handleSerialCommands() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input.length() == 0) return;
    
    // Parse commands
    if (input == "SET_MODE hourglass") {
      currentMode = MODE_HOURGLASS;
      lc.clearDisplay(0);
      lc.clearDisplay(1);
      resetHourglass();
      tone(PIN_BUZZER, 1000, 100);
      delay(100);
      tone(PIN_BUZZER, 1500, 100);
      Serial.println("OK MODE HOURGLASS");
      
    } else if (input == "SET_MODE clock") {
      currentMode = MODE_CLOCK;
      lc.clearDisplay(0);
      lc.clearDisplay(1);
      tone(PIN_BUZZER, 1000, 100);
      delay(100);
      tone(PIN_BUZZER, 1500, 100);
      Serial.println("OK MODE CLOCK");
      
    } else if (input.startsWith("SET_TIME")) {
      // Format: "SET_TIME 12 30"
      int spaceIdx = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', spaceIdx + 1);
      
      if (spaceIdx > 0 && secondSpace > spaceIdx) {
        int h = input.substring(spaceIdx + 1, secondSpace).toInt();
        int m = input.substring(secondSpace + 1).toInt();
        
        if (h >= 0 && h <= 23 && m >= 0 && m <= 59) {
          clockHours = h;
          clockMinutes = m;
          lastMinuteUpdate = millis();
          clockInitialized = true;
          displayClockMode();
          Serial.print("OK TIME ");
          Serial.print(h);
          Serial.print(":");
          if (m < 10) Serial.print("0");
          Serial.println(m);
        } else {
          Serial.println("ERR Invalid time range");
        }
      } else {
        Serial.println("ERR Invalid time format");
      }
      
    } else if (input == "RESET_HG") {
      resetHourglass();
      Serial.println("OK HOURGLASS RESET");
      
    } else if (input == "GET_STATUS") {
      // Return status as JSON
      Serial.print("{\"mode\":\"");
      Serial.print(currentMode == MODE_HOURGLASS ? "hourglass" : "clock");
      Serial.print("\",\"time\":\"");
      if (clockHours < 10) Serial.print("0");
      Serial.print(clockHours);
      Serial.print(":");
      if (clockMinutes < 10) Serial.print("0");
      Serial.print(clockMinutes);
      Serial.print("\",\"orientation\":");
      Serial.print(currentGravity);
      Serial.println("}");
      
    } else if (input == "GET_ORIENTATION") {
      Serial.print("{\"angle\":");
      Serial.print(currentGravity);
      Serial.println("}");
      
    } else if (input.startsWith("SET_HG")) {
      // Format: "SET_HG 0 5" (hours minutes) - from Web UI
      int firstSpace = input.indexOf(' ');
      int secondSpace = input.indexOf(' ', firstSpace + 1);
      if (firstSpace > 0 && secondSpace > firstSpace) {
        int hours = input.substring(firstSpace + 1, secondSpace).toInt();
        int minutes = input.substring(secondSpace + 1).toInt();
        int totalMinutes = hours * 60 + minutes;
        if (totalMinutes >= 1 && totalMinutes <= 60) {
          hourglassTimerMinutes = totalMinutes;
          // Calculate drop delay: total time / 60 particles
          particleDropDelay = ((unsigned long)totalMinutes * 60000UL) / TOTAL_PARTICLES;
          // Reset hourglass to apply new timing
          resetHourglass();
          Serial.print("OK TIMER SET TO ");
          Serial.print(totalMinutes);
          Serial.println(" MIN");
        } else if (totalMinutes > 60) {
          // Cap at 60 minutes
          hourglassTimerMinutes = 60;
          particleDropDelay = (60UL * 60000UL) / TOTAL_PARTICLES;
          resetHourglass();
          Serial.println("OK TIMER SET TO 60 MIN (capped)");
        } else {
          Serial.println("ERR Timer must be at least 1 minute");
        }
      } else {
        Serial.println("ERR Format: SET_HG <hours> <minutes>");
      }
      
    } else if (input.startsWith("SET_BRIGHTNESS")) {
      // Format: "SET_BRIGHTNESS 8"
      int spaceIdx = input.indexOf(' ');
      if (spaceIdx > 0) {
        int brightness = input.substring(spaceIdx + 1).toInt();
        if (brightness >= 0 && brightness <= 15) {
          for (int i = 0; i < 2; i++) {
            lc.setIntensity(i, brightness);
          }
          Serial.println("OK BRIGHTNESS SET");
        } else {
          Serial.println("ERR Brightness 0-15");
        }
      } else {
        Serial.println("ERR Missing brightness value");
      }
      
    } else if (input == "GET_DISPLAY") {
      // Send LED matrix data as compact hex string (16 chars per matrix)
      Serial.print("{\"matrixA\":\"");
      // Matrix A (top)
      for (int row = 0; row < 8; row++) {
        byte rowByte = 0;
        for (int col = 0; col < 8; col++) {
          if (lc.getXY(1, col, row)) rowByte |= (1 << (7 - col));
        }
        if (rowByte < 16) Serial.print("0");
        Serial.print(rowByte, HEX);
      }
      Serial.print("\",\"matrixB\":\"");
      // Matrix B (bottom)
      for (int row = 0; row < 8; row++) {
        byte rowByte = 0;
        for (int col = 0; col < 8; col++) {
          if (lc.getXY(0, col, row)) rowByte |= (1 << (7 - col));
        }
        if (rowByte < 16) Serial.print("0");
        Serial.print(rowByte, HEX);
      }
      Serial.println("\"}");
      
    } else if (input == "GET_ALL") {
      // Combined response: status, orientation, and display in one JSON
      // Using compact hex format for matrices to reduce serial traffic
      Serial.print("{\"status\":{");
      Serial.print("\"mode\":\"");
      Serial.print(currentMode == MODE_HOURGLASS ? "hourglass" : "clock");
      Serial.print("\",\"time\":\"");
      if (clockHours < 10) Serial.print("0");
      Serial.print(clockHours);
      Serial.print(":");
      if (clockMinutes < 10) Serial.print("0");
      Serial.print(clockMinutes);
      Serial.print("\",\"orientation\":");
      Serial.print(currentGravity);
      Serial.print("},\"orientation\":{");
      Serial.print("\"angle\":");
      Serial.print(currentGravity);
      Serial.print("},\"display\":{");
      // Matrix A (top) - compact hex format
      Serial.print("\"matrixA\":\"");
      for (int row = 0; row < 8; row++) {
        byte rowByte = 0;
        for (int col = 0; col < 8; col++) {
          if (lc.getXY(1, col, row)) rowByte |= (1 << (7 - col));
        }
        if (rowByte < 16) Serial.print("0");
        Serial.print(rowByte, HEX);
      }
      // Matrix B (bottom) - compact hex format
      Serial.print("\",\"matrixB\":\"");
      for (int row = 0; row < 8; row++) {
        byte rowByte = 0;
        for (int col = 0; col < 8; col++) {
          if (lc.getXY(0, col, row)) rowByte |= (1 << (7 - col));
        }
        if (rowByte < 16) Serial.print("0");
        Serial.print(rowByte, HEX);
      }
      Serial.println("\"}}");
      
    } else if (input == "ROLL_DICE") {
      // Roll a dice (1-6)
      int diceValue = random(1, 7);
      Serial.print("{\"value\":");
      Serial.print(diceValue);
      Serial.println("}");
      
    } else if (input == "GET_FLIP_COUNT") {
      // Return flip counter (using hourglass reset count as proxy)
      Serial.print("{\"count\":");
      Serial.print(hourglassTimerMinutes);  // Placeholder - can implement real counter
      Serial.println("}");
      
    } else if (input == "RESET_FLIP") {
      // Reset flip counter
      Serial.println("{\"status\":\"reset\"}");
      
    } else {
      // Unknown command
      Serial.println("ERR Unknown command");
    }
  }
}

// Send periodic status updates to web UI
void sendStatusUpdate() {
  Serial.print("STATUS:");
  Serial.print("{\"mode\":\"");
  Serial.print(currentMode == MODE_HOURGLASS ? "hourglass" : "clock");
  Serial.print("\",\"time\":\"");
  if (clockHours < 10) Serial.print("0");
  Serial.print(clockHours);
  Serial.print(":");
  if (clockMinutes < 10) Serial.print("0");
  Serial.print(clockMinutes);
  Serial.print("\",\"orientation\":");
  Serial.print(currentGravity);
  Serial.println("}");
}

void loop() {
  delay(DELAY_FRAME);
  
  // Handle serial commands from web UI
  handleSerialCommands();
  
  // Check button for mode switch and reset
  checkButton();
  
  // Check for clock initialization
  if (currentMode == MODE_CLOCK) {
    initializeClockFromSerial();
    updateClock();
  }
  
  // Update orientation with strong debouncing - require 500ms of stable reading
  int prevGravity = currentGravity;
  int newGravity = getOrientation();
  
  static unsigned long lastGravityReadTime = 0;
  static int lastReadGravity = 0;
  static int stableReadCount = 0;
  
  if (newGravity == lastReadGravity) {
    stableReadCount++;
  } else {
    stableReadCount = 0;
    lastReadGravity = newGravity;
  }
  
  // Only change orientation if we've had 6+ consistent readings (6 * 80ms = 480ms)
  if (stableReadCount >= 6 && newGravity != currentGravity) {
    currentGravity = newGravity;
    stableReadCount = 0;
  }
  
  lc.setRotation((ROTATION_OFFSET + currentGravity) % 360);
  
  // Note: Status updates are now only sent when requested via GET_STATUS or GET_ALL
  // This reduces serial traffic and keeps hourglass animation smooth
  
  // Mode-specific logic
  if (currentMode == MODE_HOURGLASS) {
    // Hourglass physics
    bool particlesMoved = updateMatrix();
    bool particleDropped = dropParticle();
    
    if (!particlesMoved && !particleDropped && !alarmTriggered) {
      int topCount = countParticles(getTopMatrix());
      
      if (topCount == 0) {
        alarmTriggered = true;
        Serial.println("*** Timer Complete! ***");
        
        for (int i = 0; i < 3; i++) {
          tone(PIN_BUZZER, 1000, 200);
          delay(300);
        }
      }
    }
    
    if (particleDropped) {
      alarmTriggered = false;
    }
    
  } else if (currentMode == MODE_CLOCK) {
    // Clock display
    if (currentGravity != prevGravity) {
      displayClockMode();
    }
    
    static unsigned long lastClockUpdate = 0;
    if (millis() - lastClockUpdate > 1000) {
      lastClockUpdate = millis();
      displayClockMode();
    }
  }
}