#include <Arduino.h>
#include <Wire.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "AirQualityTask.h"
#include "HeartRateTask.h"
#include "MotionTask.h"
#include "DataProcessorTask.h"
#include "SharedGlobalData.h"

// System FreeRTOS Primitives
QueueHandle_t sensorQueue;
SemaphoreHandle_t i2cMutex;
SharedGlobalData globalData;

// Task Thread Instances
AirQualityTask airQualityTask(32);            // Analog GPIO 32 (MQ135)
HeartRateTask heartRateTask(21, 22);          // Shared I2C Wire Bus (MAX30102)
MotionTask motionTask(21, 22);                // Shared I2C Wire Bus (MPU6050)
DataProcessorTask consumerTask(&globalData);

void setup() {
    Serial.begin(115200);
    delay(1000); 
    Serial.println("\n--- Starting Baby Monitor System ---");

    // 1. Centralized I2C Bus initialization at 100kHz
    Wire.begin(21, 22);
    Wire.setClock(100000);

    // 2. Initialize synchronization primitives
    sensorQueue = xQueueCreate(15, sizeof(SensorMessage));
    i2cMutex = xSemaphoreCreateMutex();

    if (sensorQueue != NULL && i2cMutex != NULL) {
        // 3. Launch consumer task thread
        consumerTask.startTask(sensorQueue, 1, "DataProcessorTask", 4096);

        // 4. Launch producer sensor task threads with Queue and Mutex dependency injection
        heartRateTask.startTask(sensorQueue, i2cMutex, 3, "HeartRateTask", 3072);  // Priority 3 (High)
        motionTask.startTask(sensorQueue, i2cMutex, 2, "MotionTask", 3072);       // Priority 2 (Medium)
        airQualityTask.startTask(sensorQueue, 2, "AirQualityTask", 3072);          // Priority 2 (Medium)
        
        Serial.println("[SYSTEM] All sensor tasks successfully started with shared I2C Mutex protection.");
    } else {
        Serial.println("[ERROR] Failed to create FreeRTOS sync primitives!");
    }
}

void loop() {
    // Delete default Arduino loopTask to free memory and CPU cycles
    vTaskDelete(NULL);
}