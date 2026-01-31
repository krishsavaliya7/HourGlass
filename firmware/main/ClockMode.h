#ifndef CLOCK_MODE_H
#define CLOCK_MODE_H

#include "LedControl.h"
#include "MPU6050.h"
#include "config.h"

class ClockMode {
private:
    LedControl* lc;
    MPU6050* mpu;
    int hours;
    int minutes;
    bool horizontal;
    
public:
    ClockMode(LedControl* lc, MPU6050* mpu);
    void init();
    void enter();
    void exit();
    void update();
    void setTime(int h, int m);
    String getTimeString();
    
private:
    void displayDigitalTime();
    void displayDotTime();
    void drawDigit(int matrix, int x, int digit);
    void drawDots(int matrix, int count);
};

#endif

