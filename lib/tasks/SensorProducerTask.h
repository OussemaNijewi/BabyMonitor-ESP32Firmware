#ifndef SENSOR_PRODUCER_TASK_H
#define SENSOR_PRODUCER_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ISensor.h"
#include "SensorMessage.h"

class SensorProducerTask {
    private:
        ISensor *sensor;
        QueueHandle_t messageQueue;
        uint32_t pollRateMs; //how often the Task will wake up to read data
        TaskHandle_t taskHandle; //uniquely identify and interact with task through this

        // FreeRTOS requires a static C-style function pointer for tasks
        static void taskWrapper(void *pvParameters);

        // The actual thread infinite loop
        void run();

    public:
        // Constructor injects the dependencies
        SensorProducerTask(ISensor *s, QueueHandle_t q, uint32_t rate);

        //Spawn the FreeRTOS thread
        void startTask(UBaseType_t priority, const char *taskName, configSTACK_DEPTH_TYPE = 20248); //allocates 2KB RAM to each Thread/task
};

#endif