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
        
        // Sample accelX from A1
        if (sensorX_missing) {
            accelX = 16384;  // Calibrated default (1g)
        } else {
            int rawX = analogRead(A1);
            accelX = (rawX - 512) * 64;
        }
        
        // Sample accelY from A2
        if (sensorY_missing) {
            accelY = 16384;  // Calibrated default (1g)
        } else {
            int rawY = analogRead(A2);
            accelY = (rawY - 512) * 64;
        }
        
        // Sample accelZ from A3
        if (sensorZ_missing) {
            accelZ = 16384;  // Calibrated default (1g)
        } else {
            int rawZ = analogRead(A3);
            accelZ = (rawZ - 512) * 64;
        }
        
        // No gyro data in fallback mode
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
        
        // Read accelZ
        highByte = Wire.read();
        lowByte = Wire.read();
        accelZ = (highByte << 8) | lowByte;
        
        // Read temperature (unused but required by protocol)
        Wire.read();
        Wire.read();
        
        // Read gyroX
        highByte = Wire.read();
        lowByte = Wire.read();
        gyroX = (highByte << 8) | lowByte;
        
        // Read gyroY
        highByte = Wire.read();
        lowByte = Wire.read();
        gyroY = (highByte << 8) | lowByte;
        
        // Read gyroZ
        highByte = Wire.read();
        lowByte = Wire.read();
        gyroZ = (highByte << 8) | lowByte;
        
        usingAnalogFallback = false;
    }
    
    // Calculate angle from accelerometer
    float dt = (now - lastUpdate) / 1000.0;
    if (dt > 0) {
        angleX = atan2(accelY, accelZ) * 180.0 / PI;
        angleY = atan2(accelX, accelZ) * 180.0 / PI;
        angleZ = atan2(accelY, accelX) * 180.0 / PI;
        
        // Safety check for NaN
        if (isnan(angleX)) angleX = 0.0;
        if (isnan(angleY)) angleY = 0.0;
        if (isnan(angleZ)) angleZ = 0.0;
    }
    lastUpdate = now;
}

int MPU6050::getAngle() const {
    // Convert to 0-360 range
    float angle = angleZ;
    if (angle < 0) angle += 360;
    
    // Correct for sensor mounting orientation
    // Sensor is inverted and rotated by 60 degrees
    // Maps: Sensor 60→0°, 330→90°, 242→180°, 152→270°
    int correctedAngle = (420 - (int)angle) % 360;
    
    return correctedAngle;
}

bool MPU6050::isAnalogFallbackActive() const {
    return usingAnalogFallback;
}

float MPU6050::getX() const {
    return constrain(accelX / 16384.0, -1.0, 1.0);
}

float MPU6050::getY() const {
    return constrain(accelY / 16384.0, -1.0, 1.0);
}

float MPU6050::getZ() const {
    return constrain(accelZ / 16384.0, -1.0, 1.0);
}

bool MPU6050::isHorizontal() const {
    float z = abs(getZ());
    return z > ORIENTATION_Z_THRESHOLD; // Z-axis dominant
}

bool MPU6050::isVertical() const {
    float z = abs(getZ());
    return z < ORIENTATION_Z_VERTICAL_MAX; // Z-axis not dominant
}

bool MPU6050::isShaking() {
    float dx = abs(getX() - lastShakeX);
    float dy = abs(getY() - lastShakeY);
    float dz = abs(getZ() - lastShakeZ);
    
    lastShakeX = getX();
    lastShakeY = getY();
    lastShakeZ = getZ();
    
    return (dx + dy + dz) > SHAKE_THRESHOLD;
}

bool MPU6050::detectFlip() {
    float currentZ = getZ();
    bool flipped = (lastFlipZ > FLIP_THRESHOLD && currentZ < -FLIP_THRESHOLD) ||
                   (lastFlipZ < -FLIP_THRESHOLD && currentZ > FLIP_THRESHOLD);
    lastFlipZ = currentZ;
    return flipped;
}

