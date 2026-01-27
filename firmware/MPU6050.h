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
    
    // Helper: detect if analog sensor is missing by sampling
    bool isAnalogSensorMissing(int pin, int samples = 10);
    
public:
    MPU6050();
    bool init();
    void update();
    
    // Check if using analog fallback
    bool isAnalogFallbackActive();
    
    // Get orientation angle (0-360 degrees)
    int getAngle();
    
    // Get normalized acceleration (-1.0 to 1.0)
    float getX();
    float getY();
    float getZ();
    
    // Check if device is horizontal
    bool isHorizontal();
    
    // Check if device is vertical
    bool isVertical();
    
    // Detect shake
    bool isShaking();
    
    // Detect flip
    bool detectFlip();
};

#endif

