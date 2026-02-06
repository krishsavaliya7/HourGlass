#include "DiceMode.h"

DiceMode::DiceMode(LedControl* lc, MPU6050* mpu) : lc(lc), mpu(mpu) {
    lastRoll = 0;
    currentDice = 1;
}

void DiceMode::enter() {
    lc->clearDisplay(0);
    lc->clearDisplay(1);
    rollDice();
}

void DiceMode::exit() {
    lc->clearDisplay(0);
    lc->clearDisplay(1);
}

bool DiceMode::isShaking() {
    // Read accelerometer values
    int16_t ax, ay, az;
    mpu->getAcceleration(&ax, &ay, &az);
    
    // Calculate magnitude of acceleration
    float magnitude = sqrt((float)ax * ax + (float)ay * ay + (float)az * az);
    
    // Detect shake if magnitude exceeds threshold (adjust threshold as needed)
    // Normal gravity is ~16384 for MPU6050 at ±2g scale
    return (magnitude > 20000 || magnitude < 12000);
}

void DiceMode::update() {
    if (isShaking() && (millis() - lastRoll) > 500) {
        rollDice();
        lastRoll = millis();
    }
}

void DiceMode::rollDice() {
    currentDice = random(1, 7);  // Random number 1-6
    displayDice(currentDice);
}

void DiceMode::displayDice(int value) {
    lc->clearDisplay(0);
    lc->clearDisplay(1);
    
    // Display dice pattern on matrix 0
    switch(value) {
        case 1:
            // Center dot
            lc->setLed(0, 3, 3, true);
            break;
            
        case 2:
            // Diagonal dots
            lc->setLed(0, 1, 1, true);
            lc->setLed(0, 5, 5, true);
            break;
            
        case 3:
            // Diagonal dots + center
            lc->setLed(0, 1, 1, true);
            lc->setLed(0, 3, 3, true);
            lc->setLed(0, 5, 5, true);
            break;
            
        case 4:
            // Four corners
            lc->setLed(0, 1, 1, true);
            lc->setLed(0, 1, 5, true);
            lc->setLed(0, 5, 1, true);
            lc->setLed(0, 5, 5, true);
            break;
            
        case 5:
            // Four corners + center
            lc->setLed(0, 1, 1, true);
            lc->setLed(0, 1, 5, true);
            lc->setLed(0, 3, 3, true);
            lc->setLed(0, 5, 1, true);
            lc->setLed(0, 5, 5, true);
            break;
            
        case 6:
            // Six dots (two columns)
            lc->setLed(0, 1, 1, true);
            lc->setLed(0, 1, 5, true);
            lc->setLed(0, 3, 1, true);
            lc->setLed(0, 3, 5, true);
            lc->setLed(0, 5, 1, true);
            lc->setLed(0, 5, 5, true);
            break;
    }
}