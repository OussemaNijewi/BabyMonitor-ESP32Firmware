#include "DataProcessorTask.h"

DataProcessorTask::DataProcessorTask(SharedGlobalData* gData) 
    : globalData(gData), taskHandle(NULL) {}

void DataProcessorTask::startTask(QueueHandle_t q, UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
    this->messageQueue = q; // Store the valid initialized queue pointer!
    
    xTaskCreate(
        taskWrapper,
        taskName,
        stackSize,
        this,
        priority,
        &taskHandle
    );
}

void DataProcessorTask::taskWrapper(void* pvParameters) {
    DataProcessorTask* instance = static_cast<DataProcessorTask*>(pvParameters);
    instance->run();
}

void DataProcessorTask::run() {
    SensorMessage incomingMessage;

    while (1) {
        if (messageQueue != nullptr && globalData != nullptr) {
            // Block indefinitely until a message arrives in the queue = consume 0 CPU cycles while waiting
            if (xQueueReceive(messageQueue, &incomingMessage, portMAX_DELAY) == pdTRUE) {
                // Safely update the global struct using its built-in Mutex
                globalData->updateValue(incomingMessage.sensorID, incomingMessage.value);
                
                // Optional: Print to Serial for debugging purposes
                Serial.printf("Sensor ID: %d updated value: %.2f\n", incomingMessage.sensorID, incomingMessage.value);
            }
        } else {
            // Failsafe in case dependencies were not injected properly
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}