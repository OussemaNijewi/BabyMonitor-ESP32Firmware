#include <Arduino.h>
#include "AirQualityTask.h"
#include "HeartRateTask.h"
#include "DataProcessorTask.h"
#include "SharedGlobalData.h"

QueueHandle_t sensorQueue; // this is a pointer to the queue
SharedGlobalData globalData;

// Independent sensor threads
AirQualityTask airQualityTask(sensorQueue, 32);
HeartRateTask heartRateTask(sensorQueue, 21, 22);
DataProcessorTask consumerTask(sensorQueue, &globalData);

void setup() {
    Serial.begin(115200);

    // Shared FreeRTOS queue for all sensor producers
    sensorQueue = xQueueCreate(10, sizeof(SensorMessage));

    if (sensorQueue != NULL) {
        // Start consumer
        consumerTask.startTask(1, "DataProcessorTask", 4096);

        // Start independent producer tasks
        airQualityTask.startTask(3, "AirQualityTask", 3072);
        heartRateTask.startTask(2, "HeartRateTask", 3072); //heart rate has higher priority than air quality
    }
}

void loop() {
    vTaskDelete(NULL); //deletes the default Arduino task, freeing up system RAM and CPU cycles
}