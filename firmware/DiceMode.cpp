#include "DiceMode.h"
#include "config.h"

DiceMode::DiceMode(LedControl* lc, MPU6050* mpu) {
    this->lc = lc;
    this->mpu = mpu;
    currentValue = 1;
    lastRoll = 0;
}

void DiceMode::init() {
    currentValue = 1;
}

void DiceMode::enter() {
    roll();
}

void DiceMode::exit() {
    // Cleanup
}

void DiceMode::update() {
    // Auto-roll on shake
    if (mpu->isShaking() && (millis() - lastRoll) > 500) {
        roll();
    }
    
    // Update rotation
    int angle = mpu->getAngle();
    lc->setRotation((ROTATION_OFFSET + angle) % 360);
}

void DiceMode::roll() {
    currentValue = random(1, 7); // 1-6
    lastRoll = millis();
    displayDice(currentValue);
}

int DiceMode::getValue() {
    return currentValue;
}

void DiceMode::displayDice(int value) {
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
    
    // Display dice value on both matrices (centered)
    drawDicePattern(value);
}

void DiceMode::drawDicePattern(int value) {
    // Dice patterns (3x3 grid on each matrix)
    bool patterns[6][9] = {
        // 1
        {0,0,0,0,1,0,0,0,0},
        // 2
        {1,0,0,0,0,0,0,0,1},
        // 3
        {1,0,0,0,1,0,0,0,1},
        // 4
        {1,0,1,0,0,0,1,0,1},
        // 5
        {1,0,1,0,1,0,1,0,1},
        // 6
        {1,0,1,1,0,1,1,0,1}
    };
    
    if (value < 1 || value > 6) value = 1;
    int patternIndex = value - 1;
    
    // Draw on both matrices (centered)
    for (int m = 0; m < 2; m++) {
        int matrix = (m == 0) ? MATRIX_A : MATRIX_B;
        for (int i = 0; i < 9; i++) {
            int row = i / 3;
            int col = i % 3;
            int x = 2 + col; // Center in 8x8 (columns 2-4)
            int y = 2 + row; // Center in 8x8 (rows 2-4)
            lc->setXY(matrix, x, y, patterns[patternIndex][i]);
        }
    }
}

