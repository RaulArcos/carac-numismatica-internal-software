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
    static constexpr uint8_t NUM_RING_CHANNELS = 4;
    
    struct PhotoSequenceState {
        bool active;
        int currentPhoto;
        int totalPhotos;
        float delay;
        bool autoFlip;
        unsigned long lastActionTime;
        unsigned long sequenceStartTime;
    };
    
    Communication comm;
    MotorController motors;
    LEDController leds;
    WeightSensor weight;
    
    bool ledTestState;
    int motorPosition;
    uint8_t ringIntensities[NUM_RING_CHANNELS];
    PhotoSequenceState sequenceState;
    int flipCount;
    
    void registerMessageHandlers();
    
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
    
    void processPhotoSequence();
    void triggerCamera(int duration);
    void flipCoin();
    int getRingIndexFromChannel(const char* channel);
    void resetSystemState();
    void stopAllMotors();
    void clearAllLighting();
    void sendSequenceProgressEvent(const char* action);
    void sendSequenceStoppedEvent(const char* reason, int photosTaken);
    void sendSequenceCompletedEvent(unsigned long totalDuration);
};
