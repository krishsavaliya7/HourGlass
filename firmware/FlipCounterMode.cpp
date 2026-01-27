#include "FlipCounterMode.h"
#include "config.h"

FlipCounterMode::FlipCounterMode(LedControl* lc, MPU6050* mpu) {
    this->lc = lc;
    this->mpu = mpu;
    flipCount = 0;
    lastZ = 0;
    flipDetected = false;
}

void FlipCounterMode::init() {
    flipCount = 0;
}

void FlipCounterMode::enter() {
    displayCount();
}

void FlipCounterMode::exit() {
    // Cleanup
}

void FlipCounterMode::update() {
    // Detect flip
    if (mpu->detectFlip() && !flipDetected) {
        flipCount++;
        flipDetected = true;
        displayCount();
    } else if (!mpu->detectFlip()) {
        flipDetected = false;
    }
    
    // Update rotation
    int angle = mpu->getAngle();
    lc->setRotation((ROTATION_OFFSET + angle) % 360);
}

void FlipCounterMode::reset() {
    flipCount = 0;
    displayCount();
}

int FlipCounterMode::getCount() {
    return flipCount;
}

void FlipCounterMode::displayCount() {
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
    
    // Display count across both matrices
    // Left matrix: tens digit
    // Right matrix: ones digit
    int tens = flipCount / 10;
    int ones = flipCount % 10;
    
    drawNumber(tens, MATRIX_A);
    drawNumber(ones, MATRIX_B);
}

void FlipCounterMode::drawNumber(int number, int matrix) {
    // Draw number on specified matrix
    // Simplified - using basic 7-segment style
    bool digits[10][7] = {
        // a,b,c,d,e,f,g segments
        {1,1,1,1,1,1,0}, // 0
        {0,1,1,0,0,0,0}, // 1
        {1,1,0,1,1,0,1}, // 2
        {1,1,1,1,0,0,1}, // 3
        {0,1,1,0,0,1,1}, // 4
        {1,0,1,1,0,1,1}, // 5
        {1,0,1,1,1,1,1}, // 6
        {1,1,1,0,0,0,0}, // 7
        {1,1,1,1,1,1,1}, // 8
        {1,1,1,1,0,1,1}  // 9
    };

    if (number < 0 || number > 9) return;

    // Map segments to LED positions (very small 7-seg-ish glyph)
    int positions[7][2] = {
        {3,0}, // a
        {4,1}, // b
        {4,3}, // c
        {3,4}, // d
        {2,3}, // e
        {2,1}, // f
        {3,2}  // g
    };

    for (int i = 0; i < 7; i++) {
        if (digits[number][i]) {
            lc->setXY(matrix, positions[i][0], positions[i][1], true);
        }
    }
}

