#include "MPU6050_Motion.h"

MPU6050_Motion::MPU6050_Motion(uint8_t sda, uint8_t scl)
    : sdaPin(sda), sclPin(scl) {}

bool MPU6050_Motion::init() {
    Wire.begin(sdaPin, sclPin);

    // Initialize MPU6050 on shared I2C bus (address 0x68)
    if (!mpuSensor.begin(0x68, &Wire)) {
        return false;
    }

    mpuSensor.setAccelerometerRange(MPU6050_RANGE_4_G);
    mpuSensor.setGyroRange(MPU6050_RANGE_500_DEG);
    mpuSensor.setFilterBandwidth(MPU6050_BAND_21_HZ);

    return true;
}

float MPU6050_Motion::readData() {
    sensors_event_t a, g, temp;
    if (!mpuSensor.getEvent(&a, &g, &temp)) {
        return 0.0f;
    }

    float ax = a.acceleration.x;
    float ay = a.acceleration.y;
    float az = a.acceleration.z;

    // Returns total 3D acceleration magnitude in m/s^2 (~9.81 m/s^2 at rest)
    return sqrt((ax * ax) + (ay * ay) + (az * az));
}

uint8_t MPU6050_Motion::getSensorID() {
    return SENSOR_ID_MOTION; // Defined in your shared sensor constants
}