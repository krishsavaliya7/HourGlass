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
    int seconds;         // Track seconds for accurate time
    unsigned long lastMillis; // For auto-incrementing time
    bool horizontal;
    int lastHours;      // For dirty checking
    int lastMinutes;    // For dirty checking
    bool lastHorizontal; // For dirty checking
    int lastAngle;       // For dirty checking
    
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

