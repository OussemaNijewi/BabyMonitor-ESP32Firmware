#include "AirQualityTask.h"

AirQualityTask::AirQualityTask(uint8_t mqPin)
    : taskHandle(NULL), airSensor(mqPin) {}

void AirQualityTask::startTask(QueueHandle_t q, UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
    this->messageQueue = q;
    
    xTaskCreate(
        taskWrapper,
        taskName,
        stackSize,
        this,
        priority,
        &taskHandle
    );
}

void AirQualityTask::taskWrapper(void* pvParameters) {
    AirQualityTask* instance = static_cast<AirQualityTask*>(pvParameters);
    instance->run();
}

void AirQualityTask::run() {
    airSensor.init();

    while (1) {
        if (messageQueue != nullptr) {
            float aqi = airSensor.readData();

            SensorMessage airMessage;
            airMessage.sensorID = airSensor.getSensorID();
            airMessage.value = aqi;

            xQueueSend(messageQueue, &airMessage, portMAX_DELAY);
        } else { //if queue is full
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        // MQ135 samples once every 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}