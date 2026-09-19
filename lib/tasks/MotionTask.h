#ifndef MOTION_TASK_H
#define MOTION_TASK_H

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#include "MPU6050_Motion.h"
#include "SensorMessage.h"

class MotionTask {
private:
    QueueHandle_t messageQueue;
    SemaphoreHandle_t i2cMutex; //mutex to protect shared I2C bus
    TaskHandle_t taskHandle;
    MPU6050_Motion motionSensor;

    static void taskWrapper(void* pvParameters);
    void run();

public:
    MotionTask(uint8_t sdaPin = 21, uint8_t sclPin = 22);

    void startTask(QueueHandle_t q, 
                   SemaphoreHandle_t mutex,
                   UBaseType_t priority = 2, 
                   const char* taskName = "MotionTask", 
                   configSTACK_DEPTH_TYPE stackSize = 3072);
};

#endif