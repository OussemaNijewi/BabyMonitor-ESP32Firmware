#include "HeartRateTask.h"

HeartRateTask::HeartRateTask(uint8_t sdaPin, uint8_t sclPin)
    : taskHandle(NULL), heartSensor(sdaPin, sclPin) {}

void HeartRateTask::startTask(QueueHandle_t q, SemaphoreHandle_t mutex, UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
    this->messageQueue = q; // Store the valid initialized queue pointer!
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

void HeartRateTask::taskWrapper(void* pvParameters) {
    HeartRateTask* instance = static_cast<HeartRateTask*>(pvParameters);
    instance->run();
}

void HeartRateTask::run() {
    // Acquire I2C Mutex for initialization
    if (i2cMutex != NULL && xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        heartSensor.init();
        xSemaphoreGive(i2cMutex);
    }

    float lastBPM = -1.0f;

    while (1) {
        if (messageQueue != nullptr && i2cMutex != nullptr) {
            float currentBPM = 0.0f;

            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
                currentBPM = heartSensor.readData();
                xSemaphoreGive(i2cMutex);
            }

            if (currentBPM != lastBPM) {
                SensorMessage heartMessage;
                heartMessage.sensorID = heartSensor.getSensorID();
                heartMessage.value = currentBPM;

                xQueueSend(messageQueue, &heartMessage, portMAX_DELAY);
                lastBPM = currentBPM;
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}