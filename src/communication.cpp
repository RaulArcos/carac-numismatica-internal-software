#include "../include/communication.h"

Communication::Communication() : bufferIndex(0), lastPingTime(0) {}

void Communication::begin() {
    Serial.begin(Config::Communication::BAUD_RATE);
    Serial.println("ESP32 System Ready");
    Serial.println("Waiting for JSON commands...");
    
    registerCommandHandler("ping", [this](const JsonDocument& doc) { handlePingCommand(); });
    registerCommandHandler("status", [this](const JsonDocument& doc) { handleStatusCommand(); });
    
    sendResponse(true, "System initialized");
}

void Communication::update() {
    while (Serial.available()) {
        char c = Serial.read();
        
        if (c == '\n') {
            inputBuffer[bufferIndex] = '\0';
            processCommand(inputBuffer);
            bufferIndex = 0;
        } else if (bufferIndex < Config::Communication::BUFFER_SIZE - 1) {
            inputBuffer[bufferIndex++] = c;
        }
    }
    
    if (millis() - lastPingTime > Config::Communication::PING_INTERVAL) {
        sendStatusUpdate();
        lastPingTime = millis();
    }
}

void Communication::registerCommandHandler(const String& command, CommandHandler handler) {
    commandHandlers[command] = handler;
}

void Communication::processCommand(const char* jsonCommand) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, jsonCommand);
    
    if (error) {
        sendResponse(false, "JSON parsing error");
        return;
    }
    
    const char* commandType = doc["type"];
    if (!commandType) {
        sendResponse(false, "Missing command type");
        return;
    }
    
    String command = String(commandType);
    if (commandHandlers.find(command) != commandHandlers.end()) {
        commandHandlers[command](doc);
    } else {
        sendResponse(false, "Unknown command: " + command);
    }
}

void Communication::handlePingCommand() {
    sendResponse(true, "Pong");
}

void Communication::handleStatusCommand() {
    sendResponse(true, "System status");
}

void Communication::sendResponse(bool success, const String& message) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> response;
    response["success"] = success;
    response["message"] = message;
    response["timestamp"] = millis() / 1000.0;
    
    String jsonResponse;
    serializeJson(response, jsonResponse);
    Serial.println(jsonResponse);
}

void Communication::sendResponse(bool success, const String& message, JsonObject data) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> response;
    response["success"] = success;
    response["message"] = message;
    response["timestamp"] = millis() / 1000.0;
    response["data"] = data;
    
    String jsonResponse;
    serializeJson(response, jsonResponse);
    Serial.println(jsonResponse);
}

void Communication::sendStatusUpdate() {
    sendResponse(true, "Status update");
}

int Communication::getFreeMemory() {
    return ESP.getFreeHeap();
}
