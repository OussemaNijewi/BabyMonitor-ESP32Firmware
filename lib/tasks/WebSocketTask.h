#ifndef WEBSOCKET_TASK_H
#define WEBSOCKET_TASK_H

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "SharedGlobalData.h"

class WebsocketTask {
private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    TaskHandle_t taskHandle;
    SharedGlobalData* globalData;

    static void taskWrapper(void* pvParameters);
    void run();
    void broadcastData();
    void onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len);

public:
    // Defaults to HTTP port 80
    WebsocketTask(SharedGlobalData* data, uint16_t port = 80);
    
    void startTask(UBaseType_t priority = 1, 
                   const char* taskName = "WebsocketTask", 
                   configSTACK_DEPTH_TYPE stackSize = 4096);
};

#endif // WEBSOCKET_TASK_H