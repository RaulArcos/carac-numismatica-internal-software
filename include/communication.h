#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include "config.h"

class Communication {
public:
    typedef std::function<void(JsonObject)> MessageHandler;
    
    Communication();
    void begin();
    void update();
    void registerMessageHandler(const char* messageType, MessageHandler handler);
    
    void sendSuccess(const char* message);
    void sendSuccess(const char* message, JsonObject data);
    void sendError(const char* message, const char* errorCode);
    void sendError(const char* message, const char* errorCode, JsonObject data);
    void sendStatus(JsonObject statusPayload);
    void sendEvent(const char* eventType, JsonObject payload);
    
private:
    static constexpr uint8_t MAX_HANDLERS = 20;
    
    char inputBuffer[Config::Communication::BUFFER_SIZE];
    int bufferIndex;
    MessageHandler handlers[MAX_HANDLERS];
    const char* handlerTypes[MAX_HANDLERS];
    uint8_t handlerCount;
    unsigned long lastHeartbeat;
    
    void processMessage(const char* jsonMessage);
    void sendMessage(const char* type, JsonObject payload);
    void sendHeartbeat();
    void sendMessageAck(const char* messageType);
};
