#include <Arduino.h>
#include "Button.h"

Button::Button(int pin) {
    this->pin = pin;
    lastState = HIGH;
    currentState = HIGH;
    debouncedState = HIGH;
    wasJustPressed = false;
    lastDebounceTime = 0;
    pressStartTime = 0;
    longPressDetected = false;
}

void Button::init() {
    pinMode(pin, INPUT_PULLUP);
    lastState = digitalRead(pin);
    currentState = lastState;
}

void Button::update() {
    int reading = digitalRead(pin);
    
    if (reading != lastState) {
        lastDebounceTime = millis();
    }
    
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != debouncedState) {
            // Track state change for wasJustPressed detection
            if (reading == LOW && debouncedState == HIGH) {
                wasJustPressed = true;
            }
            
            debouncedState = reading;
            currentState = reading;
            
            if (currentState == LOW) {
                // Button pressed
                pressStartTime = millis();
                longPressDetected = false;
            } else {
                // Button released
                pressStartTime = 0;
            }
        }
    }
    
    // Check for long press
    if (currentState == LOW && pressStartTime > 0) {
        if ((millis() - pressStartTime) > BUTTON_LONG_PRESS_MS && !longPressDetected) {
            longPressDetected = true;
        }
    }
    
    lastState = reading;
}

bool Button::wasPressed() {
    bool pressed = wasJustPressed;
    wasJustPressed = false;  // Clear after reading
    return pressed;
}

bool Button::wasLongPressed() {
    // Only trigger when button is released after a long press
    bool result = longPressDetected && (currentState == HIGH);
    if (result) {
        longPressDetected = false; // Reset after reading
    }
    return result;
}

bool Button::isPressed() const {
    return currentState == LOW;
}

