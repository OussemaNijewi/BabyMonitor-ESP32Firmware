#ifndef AIR_QUALITY_TASK_H
#define AIR_QUALITY_TASK_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "MQ135_AirQ.h"
#include "SensorMessage.h"

class AirQualityTask {
private:
    QueueHandle_t messageQueue;
    TaskHandle_t taskHandle;
    MQ135_AirQ airSensor;

    static void taskWrapper(void* pvParameters);
    void run();

public:
    AirQualityTask(uint8_t mqPin = 32);
    
    void startTask(QueueHandle_t q, 
                   UBaseType_t priority = 2, 
                   const char* taskName = "AirQualityTask", 
                   configSTACK_DEPTH_TYPE stackSize = 3072);
};

#endif // AIR_QUALITY_TASK_H