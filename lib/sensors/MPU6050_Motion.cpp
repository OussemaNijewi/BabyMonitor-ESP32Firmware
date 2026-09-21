#include "MPU6050_Motion.h"

MPU6050_Motion::MPU6050_Motion(uint8_t sda, uint8_t scl)
    : i2cAddress(0x68), accelScale(9.81f / 16384.0f), baselineOffset(0.0f) {}

bool MPU6050_Motion::init() {
    // 1. Wake up the sensor (Write 0x00 to Power Management 1 register 0x6B)
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

    // 3. Quick Baseline Calibration at startup (Average 20 readings while stationary)
    vTaskDelay(pdMS_TO_TICKS(50));
    float sum = 0.0f;
    int validSamples = 0;
    
    for (int i = 0; i < 20; i++) {
        Wire.beginTransmission(i2cAddress);
        Wire.write(0x3B);
        if (Wire.endTransmission() == 0) {
            Wire.requestFrom(i2cAddress, (uint8_t)6);
            if (Wire.available() >= 6) {
                int16_t axRaw = (Wire.read() << 8) | Wire.read();
                int16_t ayRaw = (Wire.read() << 8) | Wire.read();
                int16_t azRaw = (Wire.read() << 8) | Wire.read();

                float ax = axRaw * accelScale;
                float ay = ayRaw * accelScale;
                float az = azRaw * accelScale;

                sum += sqrt((ax * ax) + (ay * ay) + (az * az));
                validSamples++;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    if (validSamples > 0) {
        baselineOffset = sum / validSamples; // Captures your 10.4-10.5 resting baseline
    } else {
        baselineOffset = 9.81f; // Fallback default if calibration fails
    }

    Serial.printf("[SUCCESS] MPU6050 Calibrated. Baseline offset set to: %.2f m/s^2\n", baselineOffset);
    return true;
}

float MPU6050_Motion::readData() {
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x3B); // Start of accelerometer data
    if (Wire.endTransmission() != 0) return 0.0f;

    Wire.requestFrom(i2cAddress, (uint8_t)6);
    if (Wire.available() < 6) return 0.0f;

    int16_t axRaw = (Wire.read() << 8) | Wire.read();
    int16_t ayRaw = (Wire.read() << 8) | Wire.read();
    int16_t azRaw = (Wire.read() << 8) | Wire.read();

    float ax = axRaw * accelScale;
    float ay = ayRaw * accelScale;
    float az = azRaw * accelScale;

    float currentMagnitude = sqrt((ax * ax) + (ay * ay) + (az * az));

    // Subtract the starting baseline offset so it sits at ~0.00 when still,
    // while updating continuously with any changes or movement.
    return currentMagnitude - baselineOffset;
}

uint8_t MPU6050_Motion::getSensorID() {
    return SENSOR_ID_MOTION;
}