#ifndef DICE_MODE_H
#define DICE_MODE_H

#include "LedControl.h"
#include "MPU6050.h"

class DiceMode {
private:
    LedControl* lc;
    MPU6050* mpu;
    unsigned long lastRoll;
    int currentDice;
    
    bool isShaking();
    void rollDice();
    void displayDice(int value);
    
public:
    DiceMode(LedControl* lc, MPU6050* mpu);
    void enter();
    void exit();
    void update();
};

#endif