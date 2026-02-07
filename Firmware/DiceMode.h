#ifndef DICE_MODE_H
#define DICE_MODE_H

#include "LedControl.h"
#include "config.h"

// Forward declarations for variables defined in main .ino file
extern int16_t ax, ay, az;
extern int currentGravity;

class DiceMode {
private:
    LedControl* lc;
    int diceValueA;  // Dice value for Matrix A
    int diceValueB;  // Dice value for Matrix B
    unsigned long lastRoll;
    unsigned long lastShakeCheck;
    bool wasShaking;
    
public:
    DiceMode(LedControl* lc);
    void init();
    void enter();
    void exit();
    void update();
    void roll();           // Roll both dice
    void rollSingle();     // Roll a single dice (for button press)
    int getValue();        // Returns dice A value (for API compatibility)
    int getValueA();       // Get Matrix A dice value
    int getValueB();       // Get Matrix B dice value
    bool isShaking();
    
public:
    void displayDice(int valueA, int valueB);  // Display two dice (one on each matrix)
    
private:
    void drawDicePattern(int matrix, int value);  // Draw dice pattern on specified matrix
};

#endif
