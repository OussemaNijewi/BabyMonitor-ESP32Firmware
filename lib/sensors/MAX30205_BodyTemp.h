#ifndef MAX30205_BODYTEMP_H
#define MAX30205_BODYTEMP_H

#include <Arduino.h>
#include <Wire.h>
#include "ISensor.h"
#include "SharedGlobalData.h"

class MAX30205_BodyTemp : public ISensor {
private:
    uint8_t i2cAddress;

public:
    // Constructor accepting SDA, SCL, and base I2C address (default 0x48)
    MAX30205_BodyTemp(uint8_t sda = 21, uint8_t scl = 22, uint8_t address = 0x4C);
    
    bool init();
    float readData();
    uint8_t getSensorID();
};

#endif