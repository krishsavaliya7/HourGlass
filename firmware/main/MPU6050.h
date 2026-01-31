#ifndef MPU6050_H
#define MPU6050_H

#include <Arduino.h>
#include <Wire.h>

class MPU6050 {
private:
    int16_t accelX, accelY, accelZ;
    int16_t gyroX, gyroY, gyroZ;
    float angleX, angleY, angleZ;
    unsigned long lastUpdate;
    bool usingAnalogFallback;
    bool sensorX_missing;
    bool sensorY_missing;
    bool sensorZ_missing;
    
    // State for shake/flip detection
    float lastShakeX, lastShakeY, lastShakeZ;
    float lastFlipZ;
    
    // Helper: detect if analog sensor is missing by sampling
    bool isAnalogSensorMissing(int pin, int samples = 10);
    
public:
    MPU6050();
    bool init();
    void update();
    
    // Check if using analog fallback
    bool isAnalogFallbackActive() const;
    
    // Get orientation angle (0-360 degrees)
    int getAngle() const;
    
    // Get normalized acceleration (-1.0 to 1.0)
    float getX() const;
    float getY() const;
    float getZ() const;
    
    // Check if device is horizontal
    bool isHorizontal() const;
    
    // Check if device is vertical
    bool isVertical() const;
    
    // Detect shake
    bool isShaking();
    
    // Detect flip
    bool detectFlip();
};

#endif

