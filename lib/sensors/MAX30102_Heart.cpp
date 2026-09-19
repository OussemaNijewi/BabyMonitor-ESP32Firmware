#include "MAX30102_Heart.h"
#include "heartRate.h"

MAX30102_Heart::MAX30102_Heart(uint8_t sda, uint8_t scl) 
    : sdaPin(sda), sclPin(scl), currentBPM(0.0f), lastBeat(0) {}

bool MAX30102_Heart::init() {
    Wire.begin(sdaPin, sclPin);
    
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
        return false;
    }

    byte ledBrightness = 0x1F; // ~6.4mA LED current
    byte sampleAverage = 4;    // Average 4 samples
    byte ledMode = 2;          // Red + IR mode
    int sampleRate = 400;      // 400 Hz
    int pulseWidth = 411;      // 411us
    int adcRange = 4096;       // 12-bit range

    particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
    particleSensor.setPulseAmplitudeRed(0x0A);

    return true;
}

float MAX30102_Heart::readData() {
    long irValue = particleSensor.getIR();

    // Return 0 if finger is not present
    if (irValue < 50000) {
        currentBPM = 0.0f;
        return 0.0f;
    }

    if (checkForBeat(irValue) == true) {
        long delta = millis() - lastBeat;
        lastBeat = millis();

        float beatsPerMinute = 60.0f / (delta / 1000.0f);

        if (beatsPerMinute < 220.0f && beatsPerMinute > 10.0f) {
            currentBPM = beatsPerMinute;
        }
    }

    return currentBPM;
}

uint8_t MAX30102_Heart::getSensorID() {
    return SENSOR_ID_HEART; // Ensure this matches your Sensor ID definitions
}