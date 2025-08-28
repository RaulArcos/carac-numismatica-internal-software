#include "../include/system_controller.h"

SystemController::SystemController() {}

void SystemController::begin() {
    comm.begin();
    motors.begin();
    leds.begin();
    weight.begin();
    registerCommandHandlers();
}

void SystemController::update() {
    comm.update();
}

void SystemController::registerCommandHandlers() {
    comm.registerCommandHandler("motor", [this](const JsonDocument& doc) { handleMotorCommand(doc); });
    comm.registerCommandHandler("lighting", [this](const JsonDocument& doc) { handleLightingCommand(doc); });
    comm.registerCommandHandler("weight", [this](const JsonDocument& doc) { handleWeightCommand(doc); });
    comm.registerCommandHandler("sequence", [this](const JsonDocument& doc) { handleSequenceCommand(doc); });
    comm.registerCommandHandler("calibration", [this](const JsonDocument& doc) { handleCalibrationCommand(doc); });
}

void SystemController::handleMotorCommand(const JsonDocument& doc) {
    JsonObject data = doc["data"];
    if (!data) {
        comm.sendResponse(false, "Missing data in motor command");
        return;
    }
    
    const char* action = data["action"];
    if (!action) {
        comm.sendResponse(false, "Missing action in motor command");
        return;
    }
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    
    if (strcmp(action, "servo_left") == 0) {
        uint16_t position = data["position"] | Config::Servo::NEUTRAL_POSITION;
        motors.moveServoLeft(position);
        responseData["servo"] = "left";
        responseData["position"] = position;
        comm.sendResponse(true, "Servo left moved", responseData);
    } else if (strcmp(action, "servo_right") == 0) {
        uint16_t position = data["position"] | Config::Servo::NEUTRAL_POSITION;
        motors.moveServoRight(position);
        responseData["servo"] = "right";
        responseData["position"] = position;
        comm.sendResponse(true, "Servo right moved", responseData);
    } else if (strcmp(action, "dc_left") == 0) {
        motors.moveDCMotorLeft();
        responseData["motor"] = "dc";
        responseData["direction"] = "left";
        comm.sendResponse(true, "DC motor moving left", responseData);
    } else if (strcmp(action, "dc_right") == 0) {
        motors.moveDCMotorRight();
        responseData["motor"] = "dc";
        responseData["direction"] = "right";
        comm.sendResponse(true, "DC motor moving right", responseData);
    } else if (strcmp(action, "dc_stop") == 0) {
        motors.stopDCMotor();
        responseData["motor"] = "dc";
        responseData["direction"] = "stop";
        comm.sendResponse(true, "DC motor stopped", responseData);
    } else {
        responseData["action"] = action;
        comm.sendResponse(false, "Unknown motor action", responseData);
    }
}

void SystemController::handleLightingCommand(const JsonDocument& doc) {
    JsonObject data = doc["data"];
    if (!data) {
        comm.sendResponse(false, "Missing data in lighting command");
        return;
    }
    
    const char* action = data["action"];
    if (!action) {
        comm.sendResponse(false, "Missing action in lighting command");
        return;
    }
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    
    if (strcmp(action, "sector") == 0) {
        uint8_t sector = data["sector"] | 0;
        uint8_t percentage = data["percentage"] | 0;
        leds.setSector(sector, percentage);
        responseData["sector"] = sector;
        responseData["percentage"] = percentage;
        comm.sendResponse(true, "Sector lighting set", responseData);
    } else if (strcmp(action, "all") == 0) {
        uint8_t red = data["red"] | 0;
        uint8_t green = data["green"] | 0;
        uint8_t blue = data["blue"] | 0;
        leds.setAllLEDs(red, green, blue);
        responseData["red"] = red;
        responseData["green"] = green;
        responseData["blue"] = blue;
        comm.sendResponse(true, "All LEDs set", responseData);
    } else if (strcmp(action, "off") == 0) {
        leds.turnOff();
        comm.sendResponse(true, "LEDs turned off");
    } else if (strcmp(action, "brightness") == 0) {
        uint8_t brightness = data["brightness"] | 255;
        leds.setBrightness(brightness);
        responseData["brightness"] = brightness;
        comm.sendResponse(true, "Brightness set", responseData);
    } else {
        responseData["action"] = action;
        comm.sendResponse(false, "Unknown lighting action", responseData);
    }
}

void SystemController::handleWeightCommand(const JsonDocument& doc) {
    JsonObject data = doc["data"];
    const char* action = data["action"];
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    
    if (!action || strcmp(action, "read") == 0) {
        uint8_t samples = data["samples"] | Config::Weight::DEFAULT_SAMPLES;
        int32_t weightValue = weight.getWeightWithSamples(samples);
        responseData["weight"] = weightValue;
        responseData["samples"] = samples;
        comm.sendResponse(true, "Weight reading", responseData);
    } else if (strcmp(action, "tare") == 0) {
        weight.tare();
        responseData["tare_value"] = weight.getTareValue();
        comm.sendResponse(true, "Weight tared", responseData);
    } else {
        responseData["action"] = action;
        comm.sendResponse(false, "Unknown weight action", responseData);
    }
}

void SystemController::handleSequenceCommand(const JsonDocument& doc) {
    JsonObject data = doc["data"];
    if (!data) {
        comm.sendResponse(false, "Missing data in sequence command");
        return;
    }
    
    uint8_t sequenceType = data["type"] | 1;
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["sequence_type"] = sequenceType;
    comm.sendResponse(true, "Sequence started", responseData);
    
    motors.executeSequence(sequenceType);
    
    responseData["sequence_type"] = sequenceType;
    comm.sendResponse(true, "Sequence completed", responseData);
}

void SystemController::handleCalibrationCommand(const JsonDocument& doc) {
    JsonObject data = doc["data"];
    if (!data) {
        comm.sendResponse(false, "Missing data in calibration command");
        return;
    }
    
    const char* action = data["action"];
    if (!action) {
        comm.sendResponse(false, "Missing action in calibration command");
        return;
    }
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    
    if (strcmp(action, "weight_tare") == 0) {
        weight.tare();
        responseData["tare_value"] = weight.getTareValue();
        comm.sendResponse(true, "Weight calibration completed", responseData);
    } else if (strcmp(action, "limit_switches") == 0) {
        responseData["switch_1"] = motors.isLimitSwitch1Pressed();
        responseData["switch_2"] = motors.isLimitSwitch2Pressed();
        comm.sendResponse(true, "Limit switches status", responseData);
    } else {
        responseData["action"] = action;
        comm.sendResponse(false, "Unknown calibration action", responseData);
    }
}
