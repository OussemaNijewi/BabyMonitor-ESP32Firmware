#ifndef MQ135_AIRQ_H
#define MQ135_AIRQ_H

#include "ISensor.h"
#include "SharedGlobalData.h" // Needed for the SENSOR_ID_AIR enum

class MQ135_AirQ : public ISensor {
private:
    uint8_t analogPin;
    
    // Baseline resistance of the sensor in clean air (used for calibration)
    // This value needs to be calibrated in a clean environment for accurate AQI
    float R0 = 10.0f; 

public:
    // Constructor takes the GPIO pin number
    MQ135_AirQ(uint8_t pin);

    bool init() override;
    float readData() override;
    uint8_t getSensorID() override;
};

#endif // MQ135_AIRQ_H