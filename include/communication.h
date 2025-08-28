#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include <map>
#include "Config.h"

class Communication {
public:
    typedef std::function<void(const JsonDocument&)> CommandHandler;
    
    Communication();
    void begin();
    void update();
    void registerCommandHandler(const String& command, CommandHandler handler);
    void sendResponse(bool success, const String& message);
    void sendResponse(bool success, const String& message, JsonObject data);
    void sendStatusUpdate();

private:
    char inputBuffer[Config::Communication::BUFFER_SIZE];
    int bufferIndex;
    unsigned long lastPingTime;
    std::map<String, CommandHandler> commandHandlers;
    
    void processCommand(const char* jsonCommand);
    void handlePingCommand();
    void handleStatusCommand();
    int getFreeMemory();
};
