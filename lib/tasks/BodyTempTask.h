#ifndef BODY_TEMP_TASK_H
#define BODY_TEMP_TASK_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "MAX30205_BodyTemp.h"
#include "SensorMessage.h"

class BodyTempTask {
private:
    QueueHandle_t messageQueue;
    SemaphoreHandle_t i2cMutex; 
    TaskHandle_t taskHandle;
    MAX30205_BodyTemp tempSensor;

    static void taskWrapper(void* pvParameters);
    void run();

public:
    BodyTempTask(uint8_t sdaPin = 21, uint8_t sclPin = 22);

    void startTask(QueueHandle_t q, 
                   SemaphoreHandle_t mutex,
                   UBaseType_t priority = 2, 
                   const char* taskName = "BodyTempTask", 
                   configSTACK_DEPTH_TYPE stackSize = 3072);
};

#endif // BODY_TEMP_TASK_H