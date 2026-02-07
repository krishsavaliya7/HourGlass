#ifndef UTILS_H
#define UTILS_H

/**
 * Normalize angle to [0, 360) range
 * Handles negative angles by adding 360 as needed
 * Safe for all integer inputs
 */
inline int normalizeAngle(int angle) {
    return ((angle % 360) + 360) % 360;
}

#endif
