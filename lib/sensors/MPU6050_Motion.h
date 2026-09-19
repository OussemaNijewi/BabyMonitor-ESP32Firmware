#ifndef MPU6050_MOTION_H
#define MPU6050_MOTION_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "ISensor.h"
#include "SharedGlobalData.h"

class MPU6050_Motion : public ISensor {
private:
    Adafruit_MPU6050 mpuSensor;
    uint8_t sdaPin;
    uint8_t sclPin;

public:
    MPU6050_Motion(uint8_t sda = 21, uint8_t scl = 22);

    // ISensor interface contract implementation
    bool init() override;
    float readData() override;
    uint8_t getSensorID() override;
};

#endif // MPU6050_MOTION_H