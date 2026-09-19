#ifndef MPU6050_MOTION_H
#define MPU6050_MOTION_H

#include <Arduino.h>
#include <Wire.h>
#include "ISensor.h"
#include "SharedGlobalData.h"

class MPU6050_Motion : public ISensor {
private:
    uint8_t i2cAddress;
    float accelScale;

public:
    // Kept identical parameters so MotionTask doesn't break
    MPU6050_Motion(uint8_t sda = 21, uint8_t scl = 22);

    bool init() override;
    float readData() override;
    uint8_t getSensorID() override;
};

#endif // MPU6050_MOTION_H