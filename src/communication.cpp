#include "../include/communication.h"

Communication::Communication() : bufferIndex(0), handlerCount(0), lastHeartbeat(0) {}

void Communication::begin() {
    Serial.begin(Config::Communication::BAUD_RATE);
    
    while (!Serial) {
        delay(10);
    }
    
    // Initialize lastHeartbeat to current time to start heartbeat timer
    lastHeartbeat = millis();
    
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    JsonObject payload = doc.to<JsonObject>();
    payload["message"] = "Ready";
    payload["firmware_version"] = Config::Communication::FIRMWARE_VERSION;
    
    sendMessage("status", payload);
}

void Communication::update() {
    while (Serial.available()) {
        char c = Serial.read();
        
        if (c == '\n') {
            inputBuffer[bufferIndex] = '\0';
            if (bufferIndex > 0) {
                processMessage(inputBuffer);
            }
            bufferIndex = 0;
        } else if (bufferIndex < Config::Communication::BUFFER_SIZE - 1) {
            inputBuffer[bufferIndex++] = c;
        }
    }
    
    unsigned long currentTime = millis();
    if (currentTime - lastHeartbeat >= 5000) {
        sendHeartbeat();
        lastHeartbeat = currentTime;
    }
}

void Communication::registerMessageHandler(const char* messageType, MessageHandler handler) {
    if (handlerCount < 20) {
        handlerTypes[handlerCount] = messageType;
        handlers[handlerCount] = handler;
        handlerCount++;
    }
}

void Communication::processMessage(const char* jsonMessage) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, jsonMessage);
    
    if (error) {
        StaticJsonDocument<Config::Communication::BUFFER_SIZE> errorDoc;
        JsonObject errorData = errorDoc.to<JsonObject>();
        errorData["raw"] = jsonMessage;
        errorData["error"] = error.c_str();
        sendError("Failed to parse JSON", "PARSE_ERROR", errorData);
        return;
    }
    
    const char* type = doc["type"];
    if (!type) {
        sendError("Missing 'type' field", "PARSE_ERROR");
        return;
    }
    
    if (!doc.containsKey("payload")) {
        sendError("Missing 'payload' field", "PARSE_ERROR");
        return;
    }
    
    JsonObject payload = doc["payload"];

    sendMessage(type, payload);
    
    bool handled = false;
    for (uint8_t i = 0; i < handlerCount; i++) {
        if (strcmp(handlerTypes[i], type) == 0) {
            handlers[i](payload);
            handled = true;
            break;
        }
    }
    
    if (!handled) {
        StaticJsonDocument<Config::Communication::BUFFER_SIZE> errorDoc;
        JsonObject errorData = errorDoc.to<JsonObject>();
        errorData["received_type"] = type;
        sendError("Unknown message type", "UNKNOWN_TYPE", errorData);
    }
}

void Communication::sendMessage(const char* type, JsonObject payload) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    doc["type"] = type;
    doc["payload"] = payload;
    
    serializeJson(doc, Serial);
    Serial.println();
}

void Communication::sendSuccess(const char* message) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    JsonObject payload = doc.to<JsonObject>();
    payload["message"] = message;
    payload.createNestedObject("data");
    
    sendMessage("response_success", payload);
}

void Communication::sendSuccess(const char* message, JsonObject data) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    JsonObject payload = doc.to<JsonObject>();
    payload["message"] = message;
    payload["data"] = data;
    
    sendMessage("response_success", payload);
}

void Communication::sendError(const char* message, const char* errorCode) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    JsonObject payload = doc.to<JsonObject>();
    payload["message"] = message;
    payload["error_code"] = errorCode;
    payload.createNestedObject("data");
    
    sendMessage("response_error", payload);
}

void Communication::sendError(const char* message, const char* errorCode, JsonObject data) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    JsonObject payload = doc.to<JsonObject>();
    payload["message"] = message;
    payload["error_code"] = errorCode;
    payload["data"] = data;
    
    sendMessage("response_error", payload);
}

void Communication::sendStatus(JsonObject statusPayload) {
    sendMessage("response_status", statusPayload);
}

void Communication::sendEvent(const char* eventType, JsonObject payload) {
    sendMessage(eventType, payload);
}

void Communication::sendHeartbeat() {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    JsonObject payload = doc.to<JsonObject>();
    payload["uptime"] = millis();
    payload["status"] = "alive";
    
    sendMessage("event_heartbeat", payload);
}

void Communication::sendMessageAck(const char* messageType) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> doc;
    JsonObject payload = doc.to<JsonObject>();
    payload["received_type"] = messageType;
    payload["timestamp"] = millis();
    
    sendMessage("response_ack", payload);
}