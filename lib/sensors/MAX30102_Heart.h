#ifndef MAX30102_HEART_H
#define MAX30102_HEART_H

#include <Arduino.h>
#include <Wire.h> //used to enable I2C communication
#include "MAX30105.h" // SparkFun designed the SparkFun MAX3010x library as a unified driver for the entire Maxim Integrated sensor family (MAX30101, MAX30102, and MAX30105)
#include "ISensor.h"
#include "SharedGlobalData.h"

class MAX30102_Heart : public ISensor {
    private:
        MAX30105 particleSensor;
        uint8_t sdaPin;
        uint8_t sclPin;
        float currentBPM;
        long lastBeat;

    public:
        MAX30102_Heart(uint8_t sda = 21, uint8_t scl = 22);
        bool init();
        float readData();
        uint8_t getSensorID();
};

#endif