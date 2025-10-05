#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "communication.h"
#include "motor_controller.h"
#include "led_controller.h"
#include "weight_sensor.h"

class SystemController {
public:
    SystemController();
    void begin();
    void update();

private:
    Communication comm;
    MotorController motors;
    LEDController leds;
    WeightSensor weight;
    
    bool ledTestState;
    int motorPosition;
    uint8_t ringIntensities[4];
    
    // Photo sequence state
    struct {
        bool active;
        int currentPhoto;
        int totalPhotos;
        float delay;
        bool autoFlip;
        unsigned long lastActionTime;
        unsigned long sequenceStartTime;
    } sequenceState;
    
    void registerMessageHandlers();
    
    // Protocol message handlers
    void handleLightingSet(JsonObject payload);
    void handleMotorPosition(JsonObject payload);
    void handleMotorFlip(JsonObject payload);
    void handleCameraTrigger(JsonObject payload);
    void handlePhotoSequenceStart(JsonObject payload);
    void handleSystemPing(JsonObject payload);
    void handleSystemStatus(JsonObject payload);
    void handleSystemReset(JsonObject payload);
    void handleSystemEmergencyStop(JsonObject payload);
    void handleTestLedToggle(JsonObject payload);
    
    // Helper methods
    void processPhotoSequence();
    void triggerCamera(int duration);
    void flipCoin();
    int getRingIndexFromChannel(const char* channel);
};
