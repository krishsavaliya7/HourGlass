#ifndef DICE_MODE_H
#define DICE_MODE_H

#include "LedControl.h"
#include "MPU6050.h"
#include "config.h"

class DiceMode {
private:
    LedControl* lc;
    MPU6050* mpu;
    int currentValue;
    unsigned long lastRoll;
    
public:
    DiceMode(LedControl* lc, MPU6050* mpu);
    void init();
    void enter();
    void exit();
    void update();
    void roll();
    int getValue();
    
private:
    void displayDice(int value);
    void drawDicePattern(int value);
};

#endif
