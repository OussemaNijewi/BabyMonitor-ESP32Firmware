#ifndef SENSOR_MESSAGE_H
#define SENSOR_MESSAGE_H

#include <Arduino.h>

struct SensorMessage {
    // Maps to the SensorID enum defined in SharedGlobalData.h
    uint8_t sensorID;   
    
    // The actual reading from the sensor (e.g., 98.6 for temp, 110 for heart rate)
    float value;        
    
    // The system time (via millis()) when the reading was taken, useful for debugging or sync
    uint32_t timestamp; 
};

#endif