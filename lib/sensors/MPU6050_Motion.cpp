#include "MPU6050_Motion.h"

MPU6050_Motion::MPU6050_Motion(uint8_t sda, uint8_t scl)
    : i2cAddress(0x68), accelScale(9.81f / 16384.0f) {} // 16384 LSB/g for +/- 2G range

bool MPU6050_Motion::init() {
    // 1. Debug: Print the true Chip ID to the Serial Monitor
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x75); // WHO_AM_I register
    if (Wire.endTransmission(false) == 0) {
        Wire.requestFrom(i2cAddress, (uint8_t)1);
        if (Wire.available()) {
            Serial.printf("[DEBUG] MPU6050 true silicon ID: 0x%02X\n", Wire.read());
        }
    }

    // 2. Wake up the sensor (Write 0x00 to Power Management 1 register 0x6B)
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x6B); // PWR_MGMT_1
    Wire.write(0x00); // 0 = Wake up
    if (Wire.endTransmission() != 0) {
        return false; // Hardware NACK - check wiring
    }
    
    // Give the MEMS oscillator time to stabilize
    vTaskDelay(pdMS_TO_TICKS(10)); 

    // 3. Configure Accelerometer to +/- 2G sensitivity
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x1C); // ACCEL_CONFIG register
    Wire.write(0x00); // 0x00 = +/- 2G
    Wire.endTransmission();

    return true;
}

float MPU6050_Motion::readData() {
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x3B); // ACCEL_XOUT_H (Start of accelerometer data)
    if (Wire.endTransmission(false) != 0) {
        return 0.0f; // Bus error
    }

    Wire.requestFrom(i2cAddress, (uint8_t)6); // Request 6 bytes (X, Y, Z axes)
    if (Wire.available() < 6) {
        return 0.0f;
    }

    // Read 16-bit registers (High byte first)
    int16_t axRaw = (Wire.read() << 8) | Wire.read();
    int16_t ayRaw = (Wire.read() << 8) | Wire.read();
    int16_t azRaw = (Wire.read() << 8) | Wire.read();

    // Convert raw LSB to m/s^2
    float ax = axRaw * accelScale;
    float ay = ayRaw * accelScale;
    float az = azRaw * accelScale;

    // Return total magnitude
    return sqrt((ax * ax) + (ay * ay) + (az * az));
}

uint8_t MPU6050_Motion::getSensorID() {
    return SENSOR_ID_MOTION; 
}