#ifndef DATA_PROCESSOR_TASK_H
#define DATA_PROCESSOR_TASK_H

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "SensorMessage.h"
#include "SharedGlobalData.h"

class DataProcessorTask {
    private:
        QueueHandle_t messageQueue;
        SharedGlobalData* globalData;
        TaskHandle_t taskHandle;

        // Static wrapper for FreeRTOS to launch the C++ member function
        static void taskWrapper(void* pvParameters);
    
        // The consumer loop
        void run();

    public:
        // Constructor injects the shared queue and the shared global data struct
        DataProcessorTask(SharedGlobalData* gData);

        // Spawns the FreeRTOS thread
        void startTask(QueueHandle_t q, 
               UBaseType_t priority = 2, 
               const char* taskName = "AirQualityTask", 
               configSTACK_DEPTH_TYPE stackSize = 3072);
};

#endif // DATA_PROCESSOR_TASK_H