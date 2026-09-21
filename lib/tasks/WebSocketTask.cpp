#include "WebSocketTask.h"

WebsocketTask::WebsocketTask(SharedGlobalData* data, uint16_t port) 
    : server(port), ws("/ws"), globalData(data), taskHandle(NULL) {}

void WebsocketTask::startTask(UBaseType_t priority, const char* taskName, configSTACK_DEPTH_TYPE stackSize) {
    // 1. Bind WebSocket event handler
    ws.onEvent([this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
        this->onWebSocketEvent(server, client, type, arg, data, len);
    });
    
    // 2. Attach WebSocket to the AsyncWebServer and start
    server.addHandler(&ws);
    server.begin();

    // 3. Launch the FreeRTOS loop
    xTaskCreate(taskWrapper, taskName, stackSize, this, priority, &taskHandle);
}

void WebsocketTask::taskWrapper(void* pvParameters) {
    WebsocketTask* instance = static_cast<WebsocketTask*>(pvParameters);
    instance->run();
}

void WebsocketTask::onWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("[WebSocket] Client %u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("[WebSocket] Client %u disconnected\n", client->id());
    }
}

void WebsocketTask::broadcastData() {
    // Only process and broadcast if at least one client is listening
    if (ws.count() > 0) {
        JsonDocument doc; 
        
        // ------------------------------------------------------------------
        // Thread-safe read: Create a local snapshot object and populate it
        // ------------------------------------------------------------------
        SharedGlobalData snapshot;
        globalData->getLatestState(snapshot);

        // Use the public getters from the securely copied snapshot
        doc["motion"] = snapshot.getMovement(); 
        doc["heartRate"] = snapshot.getHeartRate();
        doc["airQuality"] = snapshot.getAirQuality();
        doc["bodyTemp"] = snapshot.getBodyTemp();
        // ------------------------------------------------------------------

        char buffer[256];
        size_t len = serializeJson(doc, buffer);
        ws.textAll(buffer, len);
    }
}

void WebsocketTask::run() {
    while (1) {
        ws.cleanupClients(); // Free resources for dropped connections
        broadcastData();
        
        // Broadcast data to clients at 2Hz (every 500ms)
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}