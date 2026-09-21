#include "BodyTempTask.h"

BodyTempTask::BodyTempTask(uint8_t sdaPin, uint8_t sclPin)
    : messageQueue(NULL), i2cMutex(NULL), taskHandle(NULL), tempSensor(sdaPin, sclPin) {}

void BodyTempTask::startTask(QueueHandle_t q, SemaphoreHandle_t mutex, UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
    this->messageQueue = q;
    this->i2cMutex = mutex;

    xTaskCreate(
        taskWrapper,
        taskName,
        stackSize,
        this,
        priority,
        &taskHandle
    );
}

void BodyTempTask::taskWrapper(void* pvParameters) {
    BodyTempTask* instance = static_cast<BodyTempTask*>(pvParameters);
    instance->run();
}

void BodyTempTask::run() {
    bool isInitialized = false;

    // Retry initialization until MAX30205 responds
    while (!isInitialized) {
        if (i2cMutex != NULL && xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            isInitialized = tempSensor.init();
            xSemaphoreGive(i2cMutex);
        }

        if (!isInitialized) {
            Serial.println("[WARNING] MAX30205 Body Temp Sensor failed to init! Retrying in 2s...");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }

    Serial.println("[SUCCESS] MAX30205 Body Temp Sensor initialized and running!");

    while (1) {
        if (messageQueue != nullptr && i2cMutex != nullptr) {
            float currentTemp = 0.0f;

            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                currentTemp = tempSensor.readData();
                xSemaphoreGive(i2cMutex);
            }

            // Filter out 0.0f readings (bus errors) before sending
            if (currentTemp > 10.0f) { 
                SensorMessage tempMessage;
                tempMessage.sensorID = tempSensor.getSensorID();
                tempMessage.value = currentTemp;

                xQueueSend(messageQueue, &tempMessage, portMAX_DELAY);
            }
        }
        
        // Poll at 1 Hz for body temperature
        vTaskDelay(pdMS_TO_TICKS(200)); 
    }
}