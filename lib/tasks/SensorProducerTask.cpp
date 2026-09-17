#include "SensorProducerTask.h"

SensorProducerTask::SensorProducerTask(ISensor* s, QueueHandle_t q, uint32_t rate) 
    : sensor(s), messageQueue(q), pollRateMs(rate), taskHandle(NULL) {}

void SensorProducerTask::startTask(UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
    // Create the FreeRTOS task
    xTaskCreate(
        taskWrapper,   // Static wrapper function
        taskName,      // Task name for debugging
        stackSize,     // Stack size allocated for this thread
        this,          // Pass 'this' instance as the parameter to the task
        priority,      // Thread priority (higher number = higher priority)
        &taskHandle    // Store the task handle
    );
}

void SensorProducerTask::taskWrapper(void* pvParameters) {
    // Cast the generic void pointer back into our specific class instance
    SensorProducerTask* instance = static_cast<SensorProducerTask*>(pvParameters);
    
    // Call the member function
    instance->run();
}

void SensorProducerTask::run() {
    // Initialize the hardware before the loop starts
    if (sensor != nullptr) {
        sensor->init();
    }

    // Set up precise timing for vTaskDelayUntil
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pollRateMs / portTICK_PERIOD_MS;

    // The infinite thread loop
    while (1) {
        if (sensor != nullptr && messageQueue != nullptr) {
            // 1. Acquire raw data from the sensor interface
            float value = sensor->readData();

            // 2. Package into our structured message
            SensorMessage msg;
            msg.sensorID = sensor->getSensorID();
            msg.value = value;
            msg.timestamp = millis();

            // 3. Send to the queue. Wait up to 10 ticks if the queue is temporarily full.
            xQueueSend(messageQueue, &msg, (TickType_t)10);
        }

        // 4. Block the task, yielding the CPU to other tasks until the next polling cycle
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}