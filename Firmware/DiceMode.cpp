#include "DiceMode.h"
#include "config.h"
#include "utils.h"
#include <Arduino.h>

DiceMode::DiceMode(LedControl* lc) {
    this->lc = lc;
    diceValueA = 1;
    diceValueB = 1;
    lastRoll = 0;
    lastShakeCheck = 0;
    wasShaking = false;
}

void DiceMode::init() {
    diceValueA = 1;
    diceValueB = 1;
    lastRoll = 0;
    lastShakeCheck = 0;
    wasShaking = false;
}

void DiceMode::enter() {
    // Set rotation based on current orientation
    lc->setRotation(normalizeAngle(ROTATION_OFFSET + currentGravity));
    roll();
}

void DiceMode::exit() {
    // Cleanup - clear displays
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
}

bool DiceMode::isShaking() {
    // Calculate total acceleration magnitude
    // ax, ay, az are global variables from main sketch
    // MPU6050 at +/- 2g range: 16384 LSB/g
    // Normal gravity = ~1g = ~16384
    // Shake detection: look for significant deviation from gravity
    
    float accelMagnitude = sqrt((float)ax * ax + (float)ay * ay + (float)az * az);
    
    // Threshold for shake detection
    // At rest: ~16384, during shake: much higher or lower
    // Use threshold of ~2.5g (40960) for reliable shake detection
    const float SHAKE_THRESHOLD_HIGH = 35000.0;  // Above ~2.1g
    const float SHAKE_THRESHOLD_LOW = 8000.0;    // Below ~0.5g (free fall/rapid direction change)
    
    return (accelMagnitude > SHAKE_THRESHOLD_HIGH) || (accelMagnitude < SHAKE_THRESHOLD_LOW);
}

void DiceMode::update() {
    // Check for shake every 100ms to avoid false triggers
    if (millis() - lastShakeCheck >= 100) {
        lastShakeCheck = millis();
        
        bool shaking = isShaking();
        
        // Detect shake start (transition from not shaking to shaking)
        if (shaking && !wasShaking && (millis() - lastRoll) > 800) {
            roll();
        }
        wasShaking = shaking;
    }
    
    // Update rotation based on current orientation
    lc->setRotation(normalizeAngle(ROTATION_OFFSET + currentGravity));
}

void DiceMode::roll() {
    // Roll two separate dice - one for each matrix
    diceValueA = random(1, 7); // 1-6 for Matrix A
    diceValueB = random(1, 7); // 1-6 for Matrix B
    lastRoll = millis();
    displayDice(diceValueA, diceValueB);
}

void DiceMode::rollSingle() {
    // Roll a single dice value (used for button press)
    diceValueA = random(1, 7);
    diceValueB = diceValueA; // Same value on both for single roll
    lastRoll = millis();
    displayDice(diceValueA, diceValueB);
}

int DiceMode::getValue() {
    return diceValueA; // Return dice A for API compatibility
}

int DiceMode::getValueA() {
    return diceValueA;
}

int DiceMode::getValueB() {
    return diceValueB;
}

void DiceMode::displayDice(int valueA, int valueB) {
    lc->clearDisplay(MATRIX_A);
    lc->clearDisplay(MATRIX_B);
    
    // Display dice A on Matrix A, dice B on Matrix B
    drawDicePattern(MATRIX_A, valueA);
    drawDicePattern(MATRIX_B, valueB);
}

void DiceMode::drawDicePattern(int matrix, int value) {
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
    
    // Draw on specified matrix (centered)
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        int x = 2 + col; // Center in 8x8 (columns 2-4)
        int y = 2 + row; // Center in 8x8 (rows 2-4)
        lc->setXY(matrix, x, y, patterns[patternIndex][i]);
    }
}

