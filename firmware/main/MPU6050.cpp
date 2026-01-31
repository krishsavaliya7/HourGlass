    #include "MPU6050.h"
#include "config.h"

#define MPU6050_ADDR 0x68
#define MPU6050_WHO_AM_I 0x75
#define MPU6050_PWR_MGMT_1 0x6B
#define MPU6050_ACCEL_XOUT_H 0x3B

MPU6050::MPU6050() {
    accelX = accelY = accelZ = 0;
    gyroX = gyroY = gyroZ = 0;
    angleX = angleY = angleZ = 0.0;
    lastUpdate = 0;
    usingAnalogFallback = false;
    sensorX_missing = false;
    sensorY_missing = false;
    sensorZ_missing = false;
    lastShakeX = lastShakeY = lastShakeZ = 0.0;
    lastFlipZ = 0.0;
}

bool MPU6050::init() {
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(MPU6050_PWR_MGMT_1);
    Wire.write(0); // Wake up MPU6050
    byte error = Wire.endTransmission();
    
    if (error != 0) {
        // MPU6050 not available, check for analog sensors
        usingAnalogFallback = true;
        sensorX_missing = isAnalogSensorMissing(A1);
        sensorY_missing = isAnalogSensorMissing(A2);
        sensorZ_missing = isAnalogSensorMissing(A3);
        return false;
    }
    
    delay(100);
    usingAnalogFallback = false;
    return true;
}

bool MPU6050::isAnalogSensorMissing(int pin, int samples) {
    // NOTE: This detection assumes floating analog pins read ~512 (mid-rail)
    // This may vary on different Arduino boards. Test on your hardware!
    
    // Sample the analog pin multiple times
    int sum = 0;
    int minVal = 1023;
    int maxVal = 0;
    
    for (int i = 0; i < samples; i++) {
        int reading = analogRead(pin);
        sum += reading;
        if (reading < minVal) minVal = reading;
        if (reading > maxVal) maxVal = reading;
        delayMicroseconds(100);  // Small delay between samples
    }
    
    int average = sum / samples;
    int variance = maxVal - minVal;
    
    // Sensor is missing if readings are consistently at mid-rail (512)
    // or have very low variance (< 10) and are near mid-rail (480-544)
    return (variance < 10) && (average >= 480 && average <= 544);
}

void MPU6050::update() {
    unsigned long now = millis();
    
    // Try to read from MPU6050
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(MPU6050_ACCEL_XOUT_H);
    Wire.endTransmission(false);
    
    // Check if we got the expected number of bytes
    if (Wire.requestFrom(MPU6050_ADDR, 14, true) != 14) {
        // I2C communication failed, use analog fallback
        usingAnalogFallback = true;
        
        // Only read X and Y for Z-axis rotation calculation
        if (sensorX_missing) {
            accelX = 0;
        } else {
            int rawX = analogRead(A1);
            accelX = (rawX - 512) * 64;
        }
        
        if (sensorY_missing) {
            accelY = 0;
        } else {
            int rawY = analogRead(A2);
            accelY = (rawY - 512) * 64;
        }
        
        // Z-axis accel not needed for rotation
        accelZ = 16384;  // Default 1g
        gyroX = gyroY = gyroZ = 0;
    } else if (Wire.available() >= 14) {
        int16_t highByte, lowByte;
        
        // Read accelX
        highByte = Wire.read();
        lowByte = Wire.read();
        accelX = (highByte << 8) | lowByte;
        
        // Read accelY
        highByte = Wire.read();
        lowByte = Wire.read();
        accelY = (highByte << 8) | lowByte;
        
        // Skip accelZ (not needed for Z rotation)
        Wire.read();
        Wire.read();
        accelZ = 16384;  // Default 1g
        
        // Skip temperature
        Wire.read();
        Wire.read();
        
        // Skip gyroX, gyroY (not needed)
        Wire.read();
        Wire.read();
        Wire.read();
        Wire.read();
        gyroX = gyroY = 0;
        
        // Skip gyroZ (we use accelerometer for angle)
        Wire.read();
        Wire.read();
        gyroZ = 0;
        
        usingAnalogFallback = false;
    }
    
    // Calculate only Z-axis rotation angle from accelerometer X and Y
    float dt = (now - lastUpdate) / 1000.0;
    if (dt > 0) {
        // Only calculate angleZ (rotation around Z-axis)
        angleZ = atan2(accelY, accelX) * 180.0 / PI;
        
        // Safety check for NaN
        if (isnan(angleZ)) angleZ = 0.0;
        
        // X and Y angles disabled
        angleX = 0.0;
        angleY = 0.0;
    }
    lastUpdate = now;
}

int MPU6050::getAngle() const {
    // Convert to 0-360 range
    float angle = angleZ;
    if (angle < 0) angle += 360;
    
    // Correct for sensor mounting orientation
    // New calibration: Sensor 11→0°, 102→90°, 189→180°, 281→270°
    // Simple offset of -11 degrees
    int correctedAngle = ((int)angle - 11 + 360) % 360;
    
    return correctedAngle;
}

bool MPU6050::isAnalogFallbackActive() const {
    return usingAnalogFallback;
}

float MPU6050::getX() const {
    // Disabled - only Z rotation used
    return 0.0;
}

float MPU6050::getY() const {
    // Disabled - only Z rotation used
    return 0.0;
}

float MPU6050::getZ() const {
    // Disabled - only Z rotation used
    return 0.0;
}

bool MPU6050::isHorizontal() const {
    // Based on Z rotation angle: horizontal at 0° or 180°
    int angle = getAngle();
    return (angle < 45 || angle > 315 || (angle > 135 && angle < 225));
}

bool MPU6050::isVertical() const {
    // Based on Z rotation angle: vertical at 90° or 270°
    int angle = getAngle();
    return ((angle >= 45 && angle <= 135) || (angle >= 225 && angle <= 315));
}

bool MPU6050::isShaking() {
    // Detect shaking based on rapid changes in raw accelerometer values
    float currentX = accelX / 16384.0;
    float currentY = accelY / 16384.0;
    
    float dx = abs(currentX - lastShakeX);
    float dy = abs(currentY - lastShakeY);
    
    lastShakeX = currentX;
    lastShakeY = currentY;
    
    return (dx + dy) > SHAKE_THRESHOLD;
}

bool MPU6050::detectFlip() {
    // Detect flip based on Z rotation crossing 180° boundary
    static int lastAngle = 0;
    int currentAngle = getAngle();
    
    bool flipped = false;
    // Detect if we crossed from top half (0-180) to bottom half (180-360) or vice versa
    if ((lastAngle < 90 && currentAngle > 270) || (lastAngle > 270 && currentAngle < 90)) {
        flipped = true;
    }
    
    lastAngle = currentAngle;
    return flipped;
}

