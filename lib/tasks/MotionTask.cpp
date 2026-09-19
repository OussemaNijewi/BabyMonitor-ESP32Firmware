#include "MotionTask.h"

MotionTask::MotionTask(uint8_t sdaPin, uint8_t sclPin)
    : messageQueue(NULL), i2cMutex(NULL), taskHandle(NULL), motionSensor(sdaPin, sclPin) {}

void MotionTask::startTask(QueueHandle_t q, SemaphoreHandle_t mutex, UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
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

void MotionTask::taskWrapper(void* pvParameters) {
    MotionTask* instance = static_cast<MotionTask*>(pvParameters);
    instance->run();
}

void MotionTask::run() {
    bool isInitialized = false;

    // Retry initialization until MPU6050 responds
    while (!isInitialized) {
        if (i2cMutex != NULL && xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
            isInitialized = motionSensor.init();
            xSemaphoreGive(i2cMutex);
        }

        if (!isInitialized) {
            Serial.println("[WARNING] MPU6050 Motion Sensor failed to init! Retrying in 2s...");
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }

    Serial.println("[SUCCESS] MPU6050 Motion Sensor initialized and running!");

    while (1) {
        if (messageQueue != nullptr && i2cMutex != nullptr) {
            float motionMagnitude = 0.0f;

            if (xSemaphoreTake(i2cMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
                motionMagnitude = motionSensor.readData();
                xSemaphoreGive(i2cMutex);
            }

            if (motionMagnitude > 0.0f) {
                SensorMessage motionMessage;
                motionMessage.sensorID = motionSensor.getSensorID();
                motionMessage.value = motionMagnitude;

                xQueueSend(messageQueue, &motionMessage, portMAX_DELAY);
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // 10 Hz polling rate
    }
}