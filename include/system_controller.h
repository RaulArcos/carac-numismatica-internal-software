#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "Communication.h"
#include "MotorController.h"
#include "LEDController.h"
#include "WeightSensor.h"

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
    
    void registerCommandHandlers();
    void handleMotorCommand(const JsonDocument& doc);
    void handleLightingCommand(const JsonDocument& doc);
    void handleWeightCommand(const JsonDocument& doc);
    void handleSequenceCommand(const JsonDocument& doc);
    void handleCalibrationCommand(const JsonDocument& doc);
};
