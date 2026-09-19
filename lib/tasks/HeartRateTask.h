#ifndef HEART_RATE_TASK_H
#define HEART_RATE_TASK_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "MAX30102_Heart.h"
#include "SensorMessage.h"

class HeartRateTask {
private:
    QueueHandle_t messageQueue;
    TaskHandle_t taskHandle;
    MAX30102_Heart heartSensor;

    static void taskWrapper(void* pvParameters);
    void run();

public:
    HeartRateTask(uint8_t sdaPin = 21, uint8_t sclPin = 22);
    
    void startTask(QueueHandle_t q, 
                   UBaseType_t priority = 2, 
                   const char* taskName = "AirQualityTask", 
                   configSTACK_DEPTH_TYPE stackSize = 3072);
};

#endif // HEART_RATE_TASK_H