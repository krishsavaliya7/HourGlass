#include "HourglassMode.h"
#include <Arduino.h>

HourglassMode::HourglassMode(LedControl* lc, MPU6050* mpu) {
    this->lc = lc;
    this->mpu = mpu;
    durationHours = 0;
    durationMinutes = 1;
    alarmWentOff = false;
    gravity = 0;
}

void HourglassMode::init() {
    durationHours = 0;
    durationMinutes = 1;
    alarmWentOff = false;
}

void HourglassMode::enter() {
    reset();
}

void HourglassMode::exit() {
    alarmWentOff = false;
}

void HourglassMode::update() {
    // Get gravity/orientation (matching reference code logic)
    gravity = mpu->getAngle();

    // Update rotation based on orientation
    lc->setRotation((ROTATION_OFFSET + gravity) % 360);

    // Handle non-blocking alarm state
    if (alarmActive) {
        unsigned long alarmElapsed = millis() - alarmStartTime;
        // Alarm pattern: 5 cycles of 200ms tone + 800ms silence = 5000ms total
        unsigned long cycleTime = alarmElapsed % 1000;  // Each cycle is 1000ms
        unsigned long totalCycleCount = alarmElapsed / 1000;
        
        if (totalCycleCount >= 5) {
            // Alarm complete
            alarmActive = false;
            noTone(PIN_BUZZER);
        } else {
            // First 200ms of each cycle: play tone
            if (cycleTime < 200) {
                tone(PIN_BUZZER, 440);  // 440 Hz
            } else {
                noTone(PIN_BUZZER);
            }
        }
    }

    // Update particle animation
    bool moved = updateMatrix();
    bool dropped = dropParticle();

    // Check if hourglass is complete (all particles in bottom)
    if (!moved && !dropped && !alarmWentOff && (countParticles(getTopMatrix()) == 0)) {
        alarmWentOff = true;
        alarm();
    }

    if (dropped) {
        alarmWentOff = false;
    }
}

void HourglassMode::setDuration(int h, int m) {
    durationHours = constrain(h, 0, 23);
    // Allow 0 minutes only if hours > 0, otherwise min 1 minute
    if (durationHours > 0) {
        durationMinutes = constrain(m, 0, 59);
    } else {
        durationMinutes = constrain(m, 1, 59);
    }
    reset();
}

void HourglassMode::reset() {
    for (byte i = 0; i < 2; i++) {
        lc->clearDisplay(i);
    }
    fill(getTopMatrix(), 60); // Start with 60 particles like reference
    dropDelay.Delay(getDelayDrop() * 1000);
    alarmWentOff = false;
    alarmActive = false;
    noTone(PIN_BUZZER);
}

int HourglassMode::getProgress() {
    int topCount = countParticles(getTopMatrix());
    int totalParticles = 60; // Initial particle count
    int bottomCount = totalParticles - topCount;
    return (bottomCount * 100) / totalParticles;
}

// Helper functions matching reference code exactly
long HourglassMode::getDelayDrop() {
    return durationMinutes + durationHours * 60;
}

coord HourglassMode::getDown(int x, int y) {
    coord xy;
    xy.x = x - 1;
    xy.y = y + 1;
    return xy;
}

coord HourglassMode::getLeft(int x, int y) {
    coord xy;
    xy.x = x - 1;
    xy.y = y;
    return xy;
}

coord HourglassMode::getRight(int x, int y) {
    coord xy;
    xy.x = x;
    xy.y = y + 1;
    return xy;
}

bool HourglassMode::canGoLeft(int addr, int x, int y) {
    if (x == 0) return false;
    return !lc->getXY(addr, getLeft(x, y));
}

bool HourglassMode::canGoRight(int addr, int x, int y) {
    if (y == 7) return false;
    return !lc->getXY(addr, getRight(x, y));
}

bool HourglassMode::canGoDown(int addr, int x, int y) {
    if (y == 7) return false;
    if (x == 0) return false;
    if (!canGoLeft(addr, x, y)) return false;
    if (!canGoRight(addr, x, y)) return false;
    return !lc->getXY(addr, getDown(x, y));
}

void HourglassMode::goDown(int addr, int x, int y) {
    lc->setXY(addr, x, y, false);
    lc->setXY(addr, getDown(x, y), true);
}

void HourglassMode::goLeft(int addr, int x, int y) {
    lc->setXY(addr, x, y, false);
    lc->setXY(addr, getLeft(x, y), true);
}

void HourglassMode::goRight(int addr, int x, int y) {
    lc->setXY(addr, x, y, false);
    lc->setXY(addr, getRight(x, y), true);
}

bool HourglassMode::moveParticle(int addr, int x, int y) {
    if (!lc->getXY(addr, x, y)) {
        return false;
    }

    bool can_GoLeft = canGoLeft(addr, x, y);
    bool can_GoRight = canGoRight(addr, x, y);

    if (!can_GoLeft && !can_GoRight) {
        return false;
    }

    bool can_GoDown = canGoDown(addr, x, y);

    if (can_GoDown) {
        goDown(addr, x, y);
    } else if (can_GoLeft && !can_GoRight) {
        goLeft(addr, x, y);
    } else if (can_GoRight && !can_GoLeft) {
        goRight(addr, x, y);
    } else if (random(2) == 1) {
        goLeft(addr, x, y);
    } else {
        goRight(addr, x, y);
    }
    return true;
}

int HourglassMode::countParticles(int addr) {
    int c = 0;
    for (byte y = 0; y < 8; y++) {
        for (byte x = 0; x < 8; x++) {
            if (lc->getXY(addr, x, y)) {
                c++;
            }
        }
    }
    return c;
}

void HourglassMode::fill(int addr, int maxcount) {
    int n = 8;
    byte x, y;
    int count = 0;
    for (byte slice = 0; slice < 2 * n - 1; ++slice) {
        byte z = slice < n ? 0 : slice - n + 1;
        for (byte j = z; j <= slice - z; ++j) {
            y = 7 - j;
            x = (slice - j);
            lc->setXY(addr, x, y, (++count <= maxcount));
        }
    }
}

int HourglassMode::getTopMatrix() {
    // Match reference logic: gravity 90 means MATRIX_A is top
    return (gravity == 90) ? MATRIX_A : MATRIX_B;
}

int HourglassMode::getBottomMatrix() {
    return (gravity != 90) ? MATRIX_A : MATRIX_B;
}

bool HourglassMode::updateMatrix() {
    int n = 8;
    bool somethingMoved = false;
    byte x, y;
    bool direction;
    for (byte slice = 0; slice < 2 * n - 1; ++slice) {
        direction = (random(2) == 1);
        byte z = slice < n ? 0 : slice - n + 1;
        for (byte j = z; j <= slice - z; ++j) {
            y = direction ? (7 - j) : (7 - (slice - j));
            x = direction ? (slice - j) : j;
            if (moveParticle(MATRIX_B, x, y)) {
                somethingMoved = true;
            }
            if (moveParticle(MATRIX_A, x, y)) {
                somethingMoved = true;
            }
        }
    }
    return somethingMoved;
}

bool HourglassMode::dropParticle() {
    if (dropDelay.Timeout()) {
        dropDelay.Delay(getDelayDrop() * 1000);
        // Only drop when horizontal (gravity 0 or 180)
        if (gravity == 0 || gravity == 180) {
            if ((lc->getRawXY(MATRIX_A, 0, 0) && !lc->getRawXY(MATRIX_B, 7, 7)) ||
                (!lc->getRawXY(MATRIX_A, 0, 0) && lc->getRawXY(MATRIX_B, 7, 7))
            ) {
                lc->invertRawXY(MATRIX_A, 0, 0);
                lc->invertRawXY(MATRIX_B, 7, 7);
                // Buzzer feedback for particle drop
                tone(PIN_BUZZER, 440, 10);
                return true;
            }
        }
    }
    return false;
}

void HourglassMode::alarm() {
    // Initiate non-blocking alarm state
    alarmActive = true;
    alarmStartTime = millis();
    alarmRepeatCount = 5;
}

