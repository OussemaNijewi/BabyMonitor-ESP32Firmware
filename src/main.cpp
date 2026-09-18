#include <Arduino.h>
#include "SharedGlobalData.h"
#include "MQ135_AirQ.h"
#include "SensorProducerTask.h"
#include "DataProcessorTask.h"

// --- Global RTOS Objects ---
// The queue that holds the SensorMessage structs
QueueHandle_t sensorQueue;

// The thread-safe global state protected by a Mutex
SharedGlobalData globalData;

// --- Physical Hardware Instances ---
// Instantiate the air quality sensor on GPIO 35
MQ135_AirQ physicalAirSensor(32); 

// --- Task Instances ---
SensorProducerTask* airTask;
DataProcessorTask* processorTask;

void setup() {
    // Start the serial monitor for debugging
    Serial.begin(115200);
    
    // Allow a brief moment for the serial monitor to connect
    delay(1000);
    Serial.println("Starting Baby Monitor System...");

    // 1. Create a FreeRTOS queue that can hold up to 20 messages
    sensorQueue = xQueueCreate(20, sizeof(SensorMessage));

    if (sensorQueue != NULL) {
        // 2. Inject dependencies into Tasks
        // Poll the MQ135 every 2000 milliseconds (2 seconds)
        airTask = new SensorProducerTask(&physicalAirSensor, sensorQueue, 2000); 
        processorTask = new DataProcessorTask(sensorQueue, &globalData);

        // 3. Start Tasks
        // The Data Processor gets priority 3 (highest) so it empties the queue instantly
        processorTask->startTask(3, "ProcessorTask"); 
        
        // The sensor producer gets priority 2
        airTask->startTask(2, "AirTask");
        
        Serial.println("FreeRTOS Scheduler running. Waiting for sensor data...");
    } else {
        Serial.println("Failed to create the FreeRTOS Queue!");
    }
}

void loop() {
    // In ESP-IDF/FreeRTOS, loop() runs as a low-priority background task.
    // We use it here to securely read the global state and print the results.
    
    SharedGlobalData currentState;
    
    // Securely copy the latest thread-safe data using the Mutex
    globalData.getLatestState(currentState);
    
    // Fetch the AQI score calculated by your MQ135 class
    float currentAQI = currentState.getAirQuality();
    
    // Print the result to the Serial Monitor
    Serial.printf("[SYSTEM STATE] Overall Air Quality Score: %.2f\n", currentAQI);
                  
    // Print the state every 2 seconds to monitor stability
    delay(2000); 
}