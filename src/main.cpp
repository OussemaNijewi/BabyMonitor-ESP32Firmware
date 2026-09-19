#include <Arduino.h>
#include "AirQualityTask.h"
#include "HeartRateTask.h"
#include "DataProcessorTask.h"
#include "SharedGlobalData.h"

QueueHandle_t sensorQueue; // this is a pointer to the queue
SharedGlobalData globalData;

// Independent sensor threads
AirQualityTask airQualityTask(32);
HeartRateTask heartRateTask(21, 22);
DataProcessorTask consumerTask(&globalData);

void setup() {
    Serial.begin(115200);
    delay(1000); // Give Serial Monitor time to connect after reboot
    Serial.println("\n--- Starting Baby Monitor System ---");

    // Shared FreeRTOS queue for all sensor producers
    sensorQueue = xQueueCreate(10, sizeof(SensorMessage));

    if (sensorQueue != NULL) {
        // Start consumer
        consumerTask.startTask(sensorQueue, 1, "DataProcessorTask", 4096);

        // Start independent producer tasks
        airQualityTask.startTask(sensorQueue, 3, "AirQualityTask", 3072);
        heartRateTask.startTask(sensorQueue, 2, "HeartRateTask", 3072); //heart rate has higher priority than air quality
        
        Serial.println("[SYSTEM] All tasks successfully started!");
    } else {
        Serial.println("[ERROR] Failed to create FreeRTOS sensorQueue!");
    }
}

void loop() {
    vTaskDelete(NULL); //deletes the default Arduino task, freeing up system RAM and CPU cycles
}