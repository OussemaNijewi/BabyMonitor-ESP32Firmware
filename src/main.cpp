#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

//Tasks
#include "AirQualityTask.h"
#include "HeartRateTask.h"
#include "MotionTask.h"
#include "DataProcessorTask.h"
#include "SharedGlobalData.h"
#include "WebSocketTask.h"

// in production I would put these variables in an .env
const char* WIFI_SSID = "WIFI_NETWORK_NAME";
const char* WIFI_PASSWORD = "WIFI_PASSWORD";

QueueHandle_t sensorQueue;
SemaphoreHandle_t i2cMutex;
SharedGlobalData globalData;

AirQualityTask airQualityTask(32);            
HeartRateTask heartRateTask(21, 22);          
MotionTask motionTask(21, 22);                
DataProcessorTask consumerTask(&globalData);
WebsocketTask websocketTask(&globalData, 80); // Pass global state to WebSocket

void setup() {
    Serial.begin(115200);
    delay(1000); 

    // 1. Connect to WiFi
    Serial.printf("\n[SYSTEM] Connecting to WiFi: %s", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n[SUCCESS] WiFi Connected! IP Address: %s\n", WiFi.localIP().toString().c_str());

    // 2. I2C Initialization
    Wire.begin(21, 22);
    Wire.setClock(100000);

    // 3. Sync Primitives
    sensorQueue = xQueueCreate(15, sizeof(SensorMessage));
    i2cMutex = xSemaphoreCreateMutex();

    if (sensorQueue != NULL && i2cMutex != NULL) {
        // 4. Launch FreeRTOS Tasks
        consumerTask.startTask(sensorQueue, 1, "DataProcessorTask", 4096);
        
        // Launch WebSocket Task (Runs at priority 1, same as consumer)
        websocketTask.startTask(1, "WebsocketTask", 4096);

        heartRateTask.startTask(sensorQueue, i2cMutex, 3, "HeartRateTask", 3072); 
        motionTask.startTask(sensorQueue, i2cMutex, 2, "MotionTask", 3072);       
        airQualityTask.startTask(sensorQueue, 2, "AirQualityTask", 3072);          
        
        Serial.println("[SYSTEM] All tasks running.");
    }
}

void loop() {
    vTaskDelete(NULL);
}