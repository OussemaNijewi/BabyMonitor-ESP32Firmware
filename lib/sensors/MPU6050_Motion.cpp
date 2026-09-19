#include "MPU6050_Motion.h"

MPU6050_Motion::MPU6050_Motion(uint8_t sda, uint8_t scl)
    : i2cAddress(0x68), accelScale(9.81f / 16384.0f), baselineMagnitude(0.0f) {}

bool MPU6050_Motion::init() {
    // 1. Wake up the sensor
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x6B); // PWR_MGMT_1
    Wire.write(0x00); // 0 = Wake up
    if (Wire.endTransmission() != 0) {
        return false;
    }
    
    vTaskDelay(pdMS_TO_TICKS(10)); 

    // 2. Configure Accelerometer to +/- 2G sensitivity
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x1C); // ACCEL_CONFIG register
    Wire.write(0x00); // 0x00 = +/- 2G
    Wire.endTransmission();

    // 3. Calibrate Baseline Offset (Average 50 readings at rest)
    vTaskDelay(pdMS_TO_TICKS(50)); // Let sensor settle
    float sum = 0.0f;
    for (int i = 0; i < 50; i++) {
        sum += getRawMagnitude();
        vTaskDelay(pdMS_TO_TICKS(10)); // 10ms between samples
    }
    baselineMagnitude = sum / 50.0f;
    
    Serial.printf("[SUCCESS] MPU6050 Calibrated. Baseline gravity offset: %.2f m/s^2\n", baselineMagnitude);

    return true;
}

float MPU6050_Motion::getRawMagnitude() {
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x3B); // Start of accelerometer data
    
    // FIX 1: Removed 'false'. Forces a clean I2C stop condition so the ESP32 buffer doesn't freeze.
    if (Wire.endTransmission() != 0) return 0.0f;

    Wire.requestFrom(i2cAddress, (uint8_t)6);
    if (Wire.available() < 6) return 0.0f;

    int16_t axRaw = (Wire.read() << 8) | Wire.read();
    int16_t ayRaw = (Wire.read() << 8) | Wire.read();
    int16_t azRaw = (Wire.read() << 8) | Wire.read();

    float ax = axRaw * accelScale;
    float ay = ayRaw * accelScale;
    float az = azRaw * accelScale;

    return sqrt((ax * ax) + (ay * ay) + (az * az));
}

float MPU6050_Motion::readData() {
    float currentMag = getRawMagnitude();
    
    // Bus error check
    if (currentMag == 0.0f) return 0.0f; 

    // FIX 2: Use fabs() instead of abs() to prevent integer truncation of the decimals.
    float movementDelta = fabs(currentMag - baselineMagnitude);
    
    // Optional Noise Filter: Ignore micro-vibrations below 0.15 m/s^2 
    if (movementDelta < 0.15f) {
        return 0.0f;
    }

    return movementDelta; 
}

uint8_t MPU6050_Motion::getSensorID() {
    return 3;
}