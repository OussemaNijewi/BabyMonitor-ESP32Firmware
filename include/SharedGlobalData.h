#ifndef SHARED_GLOBAL_DATA_H
#define SHARED_GLOBAL_DATA_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

//Enums to map sensor IDs

enum SensorID {
    SENSOR_ID_HEART = 1,
    SENSOR_ID_BODYTEMP,
    SENSOR_ID_MOTION,
    SENSOR_ID_SOUND,
    SENSOR_ID_AIRQ
};

class SharedGlobalData {
    private:
        float heartRate = 0.0f;
        float bodyTemp = 0.0f;
        float movement = 0.0f;
        float soundLevel = 0.0f;
        float airQuality = 0.0f;

        SemaphoreHandle_t dataMutex;

    public: 
        //intialize mutex on object initialization
        SharedGlobalData() {
            dataMutex = xSemaphoreCreateMutex();
        }

        //Thread-safe WRITE: used by DataProcessorTask
        void updateValue(uint8_t SensorID, float value) {
            //wait indefinetly (portMAX_DELAY) until mutex is available
            if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
                switch (SensorID) {
                    case SENSOR_ID_HEART: heartRate = value; break;
                    case SENSOR_ID_BODYTEMP:  bodyTemp = value; break;
                    case SENSOR_ID_MOTION: movement = value; break;
                    case SENSOR_ID_SOUND: soundLevel = value; break;
                    case SENSOR_ID_AIRQ:   airQuality = value; break;
                }
                //release the mutex
                xSemaphoreGive(dataMutex);
            }
        }

        //Thread-safe READ: Used by WebsocketTask (pass by ref as we are not modifying anything)
        void getLatestState(SharedGlobalData& outData) {
            if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE){
                outData.heartRate = heartRate;
                outData.bodyTemp = bodyTemp;
                outData.movement = movement;
                outData.soundLevel = soundLevel;
                outData.airQuality = airQuality;

                xSemaphoreGive(dataMutex);
            }
        }

        // Getters for the WebSocket task to read the copied data securely
        float getHeartRate() const { return heartRate; }
        float getBodyTemp() const  { return bodyTemp; }
        float getMovement() const  { return movement; }
        float getSoundLevel() const { return soundLevel; }
        float getAirQuality() const { return airQuality; }
};

#endif