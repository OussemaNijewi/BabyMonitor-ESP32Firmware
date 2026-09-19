#include "HeartRateTask.h"

HeartRateTask::HeartRateTask(QueueHandle_t q, uint8_t sdaPin, uint8_t sclPin)
    : messageQueue(q), taskHandle(NULL), heartSensor(sdaPin, sclPin) {}

void HeartRateTask::startTask(UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
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
    heartSensor.init();
    float lastBPM = -1.0f;

    while (1) {
        if (messageQueue != nullptr) {
            float currentBPM = heartSensor.readData();

            // Only enqueue if BPM changes or finger detection state changes
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

        // High-frequency 50Hz polling loop for peak detection
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}