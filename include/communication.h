#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include "Config.h"

class Communication {
public:
    typedef std::function<void(JsonObject)> MessageHandler;
    
    Communication();
    void begin();
    void update();
    void registerMessageHandler(const char* messageType, MessageHandler handler);
    
    // Send responses
    void sendSuccess(const char* message);
    void sendSuccess(const char* message, JsonObject data);
    void sendError(const char* message, const char* errorCode);
    void sendError(const char* message, const char* errorCode, JsonObject data);
    void sendStatus(JsonObject statusPayload);
    
    // Send events
    void sendEvent(const char* eventType, JsonObject payload);
    
private:
    char inputBuffer[Config::Communication::BUFFER_SIZE];
    int bufferIndex;
    MessageHandler handlers[20];
    const char* handlerTypes[20];
    uint8_t handlerCount;
    
    void processMessage(const char* jsonMessage);
    void sendMessage(const char* type, JsonObject payload);
};
