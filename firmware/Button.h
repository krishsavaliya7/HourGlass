#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>
#include "config.h"

class Button {
private:
    int pin;
    bool lastState;
    bool currentState;
    bool debouncedState;
    bool wasJustPressed;
    unsigned long lastDebounceTime;
    unsigned long pressStartTime;
    bool longPressDetected;
    
public:
    Button(int pin);
    void init();
    void update();
    bool wasPressed();
    bool wasLongPressed();
    bool isPressed();
};

#endif

