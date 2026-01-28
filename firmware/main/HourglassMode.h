#ifndef HOURGLASS_MODE_H
#define HOURGLASS_MODE_H

#include "LedControl.h"
#include "MPU6050.h"
#include "Delay.h"
#include "config.h"
#include "Coord.h"

class HourglassMode {
private:
    LedControl* lc;
    MPU6050* mpu;
    int durationHours;
    int durationMinutes;
    NonBlockDelay dropDelay;
    bool alarmWentOff;
    bool alarmActive;
    unsigned long alarmStartTime;
    int alarmRepeatCount;
    int gravity;
    
    // Helper functions matching reference code
    coord getDown(int x, int y);
    coord getLeft(int x, int y);
    coord getRight(int x, int y);
    bool canGoLeft(int addr, int x, int y);
    bool canGoRight(int addr, int x, int y);
    bool canGoDown(int addr, int x, int y);
    void goDown(int addr, int x, int y);
    void goLeft(int addr, int x, int y);
    void goRight(int addr, int x, int y);
    bool moveParticle(int addr, int x, int y);
    int countParticles(int addr);
    void fill(int addr, int maxcount);
    int getTopMatrix();
    int getBottomMatrix();
    long getDelayDrop();
    bool updateMatrix();
    bool dropParticle();
    void alarm();
    
public:
    HourglassMode(LedControl* lc, MPU6050* mpu);
    void init();
    void enter();
    void exit();
    void update();
    void setDuration(int h, int m);
    void reset();
    int getProgress();
};

#endif
