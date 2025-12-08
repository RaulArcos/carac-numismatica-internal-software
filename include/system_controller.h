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
    
    enum CoinSequenceStep {
        STEP_MOVE_FORWARD = 0,
        STEP_FLIP = 1,
        STEP_WAIT1 = 2,
        STEP_BACKLIGHT_ON = 3,
        STEP_WAIT2 = 4,
        STEP_BACKLIGHT_OFF = 5,
        STEP_MOVE_BACKWARD = 6,
        STEP_COMPLETE = 7
    };
    
    struct CoinSequenceState {
        bool active;
        CoinSequenceStep currentStep;
        unsigned long stepStartTime;
        unsigned long sequenceStartTime;
    };
    
    Communication comm;
    MotorController motors;
    LEDController leds;
    WeightSensor weight;
    
    bool ledTestState;
    int motorPosition;
    uint8_t ringIntensities[Config::System::NUM_RING_CHANNELS];
    CoinSequenceState coinSequenceState;
    int flipCount;
    unsigned long lastWeightReading;
    
    void registerMessageHandlers();
    
    void handleLightingSet(JsonObject payload);
    void handleMotorPosition(JsonObject payload);
    void handleMotorFlip(JsonObject payload);
    void handleCameraTrigger(JsonObject payload);
    void handleSystemPing(JsonObject payload);
    void handleSystemStatus(JsonObject payload);
    void handleSystemReset(JsonObject payload);
    void handleSystemEmergencyStop(JsonObject payload);
    void handleTestLedToggle(JsonObject payload);
    void handleSetBacklight(JsonObject payload);
    void handleCoinSequenceStart(JsonObject payload);
    
    void processCoinSequence();
    void flipCoin();
    int getRingIndexFromChannel(const char* channel);
    void resetSystemState();
    void stopAllMotors();
    void clearAllLighting();
};
