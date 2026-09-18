#include "MQ135_AirQ.h"

MQ135_AirQ::MQ135_AirQ(uint8_t pin) : analogPin(pin) {}

bool MQ135_AirQ::init() {
    // Configure the specified pin as an input
    pinMode(analogPin, INPUT);
    
    // The ESP32 ADC (Analog to Digital Converter) has a 12-bit resolution by default (values from 0 to 4095)
    analogReadResolution(12);
    
    // In a production environment, I would place a calibration routine here
    // that reads the sensor for several minutes in clean air to establish the R0 value.
    
    return true;
}

float MQ135_AirQ::readData() {
    // 1. Read the raw 12-bit analog value (0 - 4095)
    int rawADC = analogRead(analogPin);

    // Prevent division by zero if the ADC reads 0
    if (rawADC == 0) return 0.0f;

    // 2. Convert ADC reading to Voltage
    // Assuming a 3.3V reference on the ESP32 ADC after a voltage divider
    float voltage = (float)rawADC * (3.3f / 4095.0f);

    // 3. Calculate Sensor Resistance (Rs)
    // Using a 22k load resistor (RL) on the MQ135 breakout board
    float RL = 10.0f; 
    float Rs = RL * ((3.3f - voltage) / voltage);

    // 4. Calculate the Ratio (Rs/R0)
    float ratio = Rs / R0;

    // 5. Calculate Overall Air Quality Score
    // If ratio is 1.0 (clean air), score is 10.
    // If ratio drops to 0.1 (heavy pollution), score shoots up to 100.
    float estimatedAQI = (1.0f / ratio) * 10.0f; 

    return estimatedAQI;
}

uint8_t MQ135_AirQ::getSensorID() {
    return SENSOR_ID_AIRQ;
}