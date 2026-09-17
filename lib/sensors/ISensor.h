#ifndef I_SENSOR_H
#define I_SENSOR_H

#include <Arduino.h>

class ISensor {
    public:
        // Initializes the hardware (e.g., I2C setup). Returns true if successful.
        virtual bool init() = 0;

        // Reads raw hardware data and returns a standardized float value.
        virtual float readData() = 0;

        // Returns a unique identifier for the sensor (e.g., 1 for Heart, 2 for Temp).
        virtual uint8_t getSensorID() = 0;

        // Virtual destructor ensures proper memory cleanup for derived classes
        virtual ~ISensor() = default;
};
#endif