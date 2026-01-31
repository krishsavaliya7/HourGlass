#include "ClockMode.h"
#include "utils.h"
#include "config.h"

ClockMode::ClockMode(LedControl* lc, MPU6050* mpu) {
    this->lc = lc;
    this->mpu = mpu;
    hours = 12;
    minutes = 0;
    seconds = 0;
    lastMillis = 0;
    horizontal = true;
    lastHours = -1;
    lastMinutes = -1;
    lastHorizontal = false;
    lastAngle = -1;
}

void ClockMode::init() {
    // Initialize time from system or default
    hours = 12;
    minutes = 0;
    seconds = 0;
    lastMillis = millis();
}

void ClockMode::enter() {
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
    // Reset tracking to force redraw on enter
    lastHours = -1;
    lastMinutes = -1;
    lastHorizontal = !horizontal;
    lastAngle = -1;
}

void ClockMode::exit() {
    // Cleanup if needed
}

void ClockMode::update() {
    // Auto-increment time every second
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= 1000) {
        lastMillis = currentMillis;
        seconds++;
        if (seconds >= 60) {
            seconds = 0;
            minutes++;
            if (minutes >= 60) {
                minutes = 0;
                hours++;
                if (hours >= 24) {
                    hours = 0;
                }
            }
        }
    }
    
    int angle = mpu->getAngle();
    
    // Determine display mode based on angle:
    // 0° or 180° (±30°) = horizontal = digital time
    // 90° or 270° (±30°) = vertical = dot time
    bool isNear0 = (angle <= 30 || angle >= 330);
    bool isNear180 = (angle >= 150 && angle <= 210);
    horizontal = isNear0 || isNear180;
    
    // Only update rotation if angle changed significantly (reduces flicker)
    if (abs(angle - lastAngle) > 5) {
        lc->setRotation(normalizeAngle(ROTATION_OFFSET + angle));
        lastAngle = angle;
    }
    
    // Only redraw if time, orientation mode, or angle changed (prevents flicker)
    bool needsRedraw = (hours != lastHours) || 
                       (minutes != lastMinutes) || 
                       (horizontal != lastHorizontal);
    
    if (needsRedraw) {
        if (horizontal) {
            displayDigitalTime();
        } else {
            displayDotTime();
        }
        lastHours = hours;
        lastMinutes = minutes;
        lastHorizontal = horizontal;
    }
}

void ClockMode::setTime(int h, int m) {
    hours = constrain(h, 0, 23);
    minutes = constrain(m, 0, 59);
    seconds = 0;  // Reset seconds when time is set
    lastMillis = millis();  // Reset timer
    // Force redraw
    lastHours = -1;
    lastMinutes = -1;
}

String ClockMode::getTimeString() {
    // Avoid reentrancy issues by building owned String directly from snprintf
    // without relying on a shared static buffer
    char buffer[6];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
    return String(buffer);  // Caller gets a fresh owned String
}
void ClockMode::displayDigitalTime() {
    // Display HH:MM across both matrices (16 columns)
    int h1 = hours / 10;
    int h2 = hours % 10;
    int m1 = minutes / 10;
    int m2 = minutes % 10;
    
    // Clear displays
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
    
    // Draw digits (simplified - using basic patterns)
    drawDigit(MATRIX_A, 0, h1);
    drawDigit(MATRIX_A, 4, h2);
    drawDigit(MATRIX_B, 0, m1);
    drawDigit(MATRIX_B, 4, m2);
    
    // Draw colon between hours and minutes (at column 8)
    lc->setXY(MATRIX_B, 0, 2, true);
    lc->setXY(MATRIX_B, 0, 5, true);
}

void ClockMode::displayDotTime() {
    // Left matrix: hours as dots (max 23)
    // Right matrix: minutes as grouped dots
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
    
    drawDots(MATRIX_A, hours);
    drawDots(MATRIX_B, minutes);
}

void ClockMode::drawDigit(int matrix, int x, int digit) {
    // Simple 3x5 digit patterns
    // This is a simplified version - full implementation would use font data
    bool patterns[10][15] = {
        // 0
        {1,1,1,1,0,1,1,0,1,1,0,1,1,1,1},
        // 1
        {0,0,1,0,0,1,0,0,1,0,0,1,0,0,1},
        // 2
        {1,1,1,0,0,1,1,1,1,1,0,0,1,1,1},
        // 3
        {1,1,1,0,0,1,1,1,1,0,0,1,1,1,1},
        // 4
        {1,0,1,1,0,1,1,1,1,0,0,1,0,0,1},
        // 5
        {1,1,1,1,0,0,1,1,1,0,0,1,1,1,1},
        // 6
        {1,1,1,1,0,0,1,1,1,1,0,1,1,1,1},
        // 7
        {1,1,1,0,0,1,0,0,1,0,0,1,0,0,1},
        // 8
        {1,1,1,1,0,1,1,1,1,1,0,1,1,1,1},
        // 9
        {1,1,1,1,0,1,1,1,1,0,0,1,1,1,1}
    };
    
    if (digit < 0 || digit > 9) return;
    
    for (int i = 0; i < 15; i++) {
        int row = i / 3;
        int col = i % 3;
        if (x + col < 8 && row < 5) {
            lc->setXY(matrix, x + col, row + 1, patterns[digit][i]);
        }
    }
}

void ClockMode::drawDots(int matrix, int count) {
    // Draw dots in a grid pattern
    int maxDots = (matrix == MATRIX_A) ? 23 : 59;
    count = constrain(count, 0, maxDots);
    
    for (int i = 0; i < count && i < 64; i++) {
        int x = i % 8;
        int y = i / 8;
        lc->setXY(matrix, x, y, true);
    }
}

