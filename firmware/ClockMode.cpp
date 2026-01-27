#include "ClockMode.h"
#include "config.h"

ClockMode::ClockMode(LedControl* lc, MPU6050* mpu) {
    this->lc = lc;
    this->mpu = mpu;
    hours = 12;
    minutes = 0;
    horizontal = true;
}

void ClockMode::init() {
    // Initialize time from system or default
    hours = 12;
    minutes = 0;
}

void ClockMode::enter() {
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
}

void ClockMode::exit() {
    // Cleanup if needed
}

void ClockMode::update() {
    horizontal = mpu->isHorizontal();
    
    // Update rotation based on orientation
    int angle = mpu->getAngle();
    lc->setRotation((ROTATION_OFFSET + angle) % 360);
    
    if (horizontal) {
        displayDigitalTime();
    } else {
        displayDotTime();
    }
}

void ClockMode::setTime(int h, int m) {
    hours = constrain(h, 0, 23);
    minutes = constrain(m, 0, 59);
}

String ClockMode::getTimeString() {
    String s = "";
    if (hours < 10) s += "0";
    s += String(hours);
    s += ":";
    if (minutes < 10) s += "0";
    s += String(minutes);
    return s;
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

