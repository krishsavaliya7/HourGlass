#ifndef FLIP_COUNTER_MODE_H
#define FLIP_COUNTER_MODE_H

#include "display/LedControl.h"
#include "MPU6050.h"
#include "config.h"

class FlipCounterMode {
private:
    LedControl* lc;
    MPU6050* mpu;
    int flipCount;
    float lastZ;
    bool flipDetected;
    
public:
    FlipCounterMode(LedControl* lc, MPU6050* mpu);
    void init();
    void enter();
    void exit();
    void update();
    void reset();
    int getCount();
    
private:
    void displayCount();
    void drawNumber(int number, int matrix);
};

#endif
