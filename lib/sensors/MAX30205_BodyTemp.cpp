#include "MAX30205_BodyTemp.h"

MAX30205_BodyTemp::MAX30205_BodyTemp(uint8_t sda, uint8_t scl, uint8_t address)
    : i2cAddress(address) {}

bool MAX30205_BodyTemp::init() {
    // 1. Check primary I2C address
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00); // Temperature register pointer
    if (Wire.endTransmission() != 0) {
        Serial.printf("[ERROR] MAX30205 Body Temperature sensor not found on 0x%02X!\n", i2cAddress);
        return false;
    }

    // 2. Configure Configuration Register (0x01) for Continuous Conversion Mode
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x01); // Configuration register
    Wire.write(0x00); // Continuous mode, normal operation
    if (Wire.endTransmission() != 0) {
        return false;
    }

    vTaskDelay(pdMS_TO_TICKS(50));
    Serial.printf("[SUCCESS] MAX30205 Initialized at I2C address: 0x%02X\n", i2cAddress);
    return true;
}

float MAX30205_BodyTemp::readData() {
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00); // Point to Temperature Register
    
    // Removed 'false' to force a clean I2C stop condition and prevent ESP32 buffer freezing
    if (Wire.endTransmission() != 0) return 0.0f;

    // Request 2 bytes of precision temperature data
    if (Wire.requestFrom(i2cAddress, (uint8_t)2) < 2) {
        return 0.0f;
    }

    int16_t rawTemp = (Wire.read() << 8) | Wire.read();
    
    // Convert raw 16-bit value to Celsius (Resolution: 1 LSB = 0.00390625°C)
    float temperatureCelsius = rawTemp * 0.00390625f;

    return temperatureCelsius;
}

uint8_t MAX30205_BodyTemp::getSensorID() {
    return SENSOR_ID_BODYTEMP;
}