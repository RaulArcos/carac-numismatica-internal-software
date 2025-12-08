#include "../include/system_controller.h"

SystemController::SystemController() 
    : ledTestState(false)
    , motorPosition(0)
    , flipCount(0)
    , lastWeightReading(0) {
    for (int i = 0; i < Config::System::NUM_RING_CHANNELS; i++) {
        ringIntensities[i] = 0;
    }
    
    coinSequenceState.active = false;
    coinSequenceState.currentStep = STEP_MOVE_FORWARD;
    coinSequenceState.stepStartTime = 0;
    coinSequenceState.sequenceStartTime = 0;
}

void SystemController::begin() {
    comm.begin();
    motors.begin();
    leds.begin();
    weight.begin();
    
    pinMode(Config::Pins::LED_TEST, OUTPUT);
    digitalWrite(Config::Pins::LED_TEST, LOW);
    
    lastWeightReading = millis();
    
    registerMessageHandlers();
}

void SystemController::update() {
    comm.update();
    processCoinSequence();
    
    unsigned long currentTime = millis();
    if (currentTime - lastWeightReading >= Config::System::WEIGHT_READING_INTERVAL_MS) {
        float weightValue = weight.getWeight();
        
        StaticJsonDocument<Config::System::JSON_DOC_SIZE> weightDoc;
        JsonObject weightPayload = weightDoc.to<JsonObject>();
        weightPayload["weight"] = weightValue;
        weightPayload["timestamp"] = currentTime;
        
        comm.sendEvent("event_weight_reading", weightPayload);
        
        lastWeightReading = currentTime;
    }
}

void SystemController::registerMessageHandlers() {
    comm.registerMessageHandler("lighting_set", [this](JsonObject payload) { 
        handleLightingSet(payload); 
    });
    
    comm.registerMessageHandler("motor_position", [this](JsonObject payload) { 
        handleMotorPosition(payload); 
    });
    
    comm.registerMessageHandler("motor_flip", [this](JsonObject payload) { 
        handleMotorFlip(payload); 
    });
    
    comm.registerMessageHandler("camera_trigger", [this](JsonObject payload) { 
        handleCameraTrigger(payload); 
    });
    
    comm.registerMessageHandler("system_ping", [this](JsonObject payload) { 
        handleSystemPing(payload); 
    });
    
    comm.registerMessageHandler("system_status", [this](JsonObject payload) { 
        handleSystemStatus(payload); 
    });
    
    comm.registerMessageHandler("system_reset", [this](JsonObject payload) { 
        handleSystemReset(payload); 
    });
    
    comm.registerMessageHandler("system_emergency_stop", [this](JsonObject payload) { 
        handleSystemEmergencyStop(payload); 
    });
    
    comm.registerMessageHandler("test_led_toggle", [this](JsonObject payload) { 
        handleTestLedToggle(payload); 
    });
    
    comm.registerMessageHandler("set_backlight", [this](JsonObject payload) { 
        handleSetBacklight(payload); 
    });
    
    comm.registerMessageHandler("coin_sequence_start", [this](JsonObject payload) { 
        handleCoinSequenceStart(payload); 
    });
}

void SystemController::handleLightingSet(JsonObject payload) {
    if (payload.containsKey("sections")) {
        JsonObject sections = payload["sections"];
        
        const char* ringNames[Config::System::NUM_RING_CHANNELS] = {"ring_1", "ring_2", "ring_3", "ring_4"};
        int intensities[Config::System::NUM_RING_CHANNELS];
        
        for (int i = 0; i < Config::System::NUM_RING_CHANNELS; i++) {
            if (sections.containsKey(ringNames[i])) {
                int intensity = sections[ringNames[i]] | 0;
                
                if (intensity < Config::System::MIN_INTENSITY || intensity > Config::System::MAX_INTENSITY) {
                    StaticJsonDocument<Config::System::JSON_DOC_SIZE> errorDoc;
                    JsonObject errorData = errorDoc.to<JsonObject>();
                    errorData["received_intensity"] = intensity;
                    errorData["ring"] = ringNames[i];
                    errorData["valid_range"] = "0-255";
                    comm.sendError("Intensity must be 0-255", "INVALID_INTENSITY", errorData);
                    return;
                }
                intensities[i] = intensity;
            } else {
                intensities[i] = ringIntensities[i];
            }
        }
        
        for (int i = 0; i < Config::System::NUM_RING_CHANNELS; i++) {
            leds.setSectorAcrossAllRings(i, intensities[i]);
            ringIntensities[i] = intensities[i];
        }
        
        StaticJsonDocument<Config::System::JSON_DOC_SIZE> responseDoc;
        JsonObject responseData = responseDoc.to<JsonObject>();
        JsonObject responseSections = responseData.createNestedObject("sections");
        responseSections["ring_1"] = ringIntensities[0];
        responseSections["ring_2"] = ringIntensities[1];
        responseSections["ring_3"] = ringIntensities[2];
        responseSections["ring_4"] = ringIntensities[3];
        
        comm.sendSuccess("Lighting set successfully", responseData);
    } 
    else if (payload.containsKey("channel")) {
        const char* channel = payload["channel"];
        int intensity = payload["intensity"] | 0;
        
        int ringIndex = getRingIndexFromChannel(channel);
        if (ringIndex == -1) {
            StaticJsonDocument<Config::System::JSON_DOC_SIZE> errorDoc;
            JsonObject errorData = errorDoc.to<JsonObject>();
            errorData["received_channel"] = channel;
            JsonArray validChannels = errorData.createNestedArray("valid_channels");
            validChannels.add("ring_1");
            validChannels.add("ring_2");
            validChannels.add("ring_3");
            validChannels.add("ring_4");
            comm.sendError("Invalid lighting channel", "INVALID_CHANNEL", errorData);
            return;
        }
        
        if (intensity < Config::System::MIN_INTENSITY || intensity > Config::System::MAX_INTENSITY) {
            StaticJsonDocument<Config::System::JSON_DOC_SIZE> errorDoc;
            JsonObject errorData = errorDoc.to<JsonObject>();
            errorData["received_intensity"] = intensity;
            errorData["valid_range"] = "0-255";
            comm.sendError("Intensity must be 0-255", "INVALID_INTENSITY", errorData);
            return;
        }
        
        uint8_t sectorIndex = ringIndex;
        uint8_t percentage = map(intensity, Config::System::MIN_INTENSITY, Config::System::MAX_INTENSITY, Config::System::PERCENTAGE_MIN, Config::System::PERCENTAGE_MAX);
        leds.setSectorAcrossAllRings(sectorIndex, percentage);
        ringIntensities[ringIndex] = intensity;
        
        StaticJsonDocument<Config::System::JSON_DOC_SIZE> responseDoc;
        JsonObject responseData = responseDoc.to<JsonObject>();
        responseData["channel"] = channel;
        responseData["intensity"] = intensity;
        
        comm.sendSuccess("Lighting set successfully", responseData);
    } 
    else {
        comm.sendError("Missing 'sections' or 'channel' field", "INVALID_FORMAT");
    }
}

void SystemController::handleMotorPosition(JsonObject payload) {
    const char* direction = payload["direction"];
    
    if (!direction || (strcmp(direction, "forward") != 0 && strcmp(direction, "backward") != 0)) {
        StaticJsonDocument<Config::System::JSON_DOC_SIZE> errorDoc;
        JsonObject errorData = errorDoc.to<JsonObject>();
        errorData["received_direction"] = direction ? direction : "null";
        JsonArray validDirections = errorData.createNestedArray("valid_directions");
        validDirections.add("forward");
        validDirections.add("backward");
        comm.sendError("Invalid direction", "INVALID_DIRECTION", errorData);
        return;
    }
    
    unsigned long startTime = millis();
    unsigned long timeoutTime = startTime + Config::Motor::POSITION_TIMEOUT_MS;
    bool stoppedByLimitSwitch = false;
    
    if (strcmp(direction, "forward") == 0) {
        motors.moveDCMotorRight();
        while (millis() < timeoutTime) {
            if (motors.isLimitSwitch2Pressed()) {
                motors.stopDCMotor();
                stoppedByLimitSwitch = true;
                break;
            }
            delay(Config::Motor::POSITION_POLL_INTERVAL_MS);
        }
        if (!stoppedByLimitSwitch) {
            motors.stopDCMotor();
        }
        motorPosition += (millis() - startTime);
    } else {
        motors.moveDCMotorLeft();
        while (millis() < timeoutTime) {
            if (motors.isLimitSwitch1Pressed()) {
                motors.stopDCMotor();
                stoppedByLimitSwitch = true;
                break;
            }
            delay(Config::Motor::POSITION_POLL_INTERVAL_MS);
        }
        if (!stoppedByLimitSwitch) {
            motors.stopDCMotor();
        }
        motorPosition -= (millis() - startTime);
    }
    unsigned long duration = millis() - startTime;
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["direction"] = direction;
    responseData["position"] = motorPosition;
    if (stoppedByLimitSwitch) {
        responseData["stopped_by_limit_switch"] = true;
    }
    
    comm.sendSuccess("Motor moved successfully", responseData);
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["position"] = motorPosition;
    eventPayload["duration"] = (int)duration;
    if (stoppedByLimitSwitch) {
        eventPayload["stopped_by_limit_switch"] = true;
    }
    
    comm.sendEvent("event_motor_complete", eventPayload);
}

void SystemController::handleMotorFlip(JsonObject payload) {
    flipCoin();
    flipCount++;
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["flip_count"] = flipCount;
    
    comm.sendSuccess("Coin flipped successfully", responseData);
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["position"] = motorPosition;
    eventPayload["duration"] = Config::Motor::FLIP_EVENT_DURATION_MS;
    
    comm.sendEvent("event_motor_complete", eventPayload);
}

void SystemController::handleCameraTrigger(JsonObject payload) {
    int duration = payload.containsKey("duration") ? 
                   payload["duration"].as<int>() : 
                   Config::Camera::DEFAULT_TRIGGER_DURATION;
    
    comm.sendSuccess("Camera triggered");
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["duration"] = duration;
    
    comm.sendEvent("event_camera_triggered", eventPayload);
}

void SystemController::handleCoinSequenceStart(JsonObject payload) {
    if (coinSequenceState.active) {
        comm.sendError("Coin sequence already active", "SEQUENCE_ACTIVE");
        return;
    }
    
    coinSequenceState.active = true;
    coinSequenceState.currentStep = STEP_MOVE_FORWARD;
    coinSequenceState.stepStartTime = millis();
    coinSequenceState.sequenceStartTime = millis();
    
    comm.sendSuccess("Coin sequence started");
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["step"] = "move_forward";
    comm.sendEvent("event_coin_sequence_started", eventPayload);
}

void SystemController::handleSystemPing(JsonObject payload) {
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["uptime_ms"] = millis();
    
    comm.sendSuccess("Pong", responseData);
}

void SystemController::handleSystemStatus(JsonObject payload) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> statusDoc;
    JsonObject statusPayload = statusDoc.to<JsonObject>();
    
    statusPayload["message"] = "System ready";
    statusPayload["firmware_version"] = Config::Communication::FIRMWARE_VERSION;
    statusPayload["motor_position"] = motorPosition;
    statusPayload["servos_ready"] = true;
    statusPayload["uptime_ms"] = millis();
    
    JsonObject lighting = statusPayload.createNestedObject("lighting");
    lighting["ring_1"] = ringIntensities[0];
    lighting["ring_2"] = ringIntensities[1];
    lighting["ring_3"] = ringIntensities[2];
    lighting["ring_4"] = ringIntensities[3];
    lighting["backlight"] = leds.getBacklightState();
    
    JsonObject motor = statusPayload.createNestedObject("motor");
    motor["position"] = motorPosition;
    motor["is_moving"] = false;
    
    JsonObject coinSequence = statusPayload.createNestedObject("coin_sequence");
    coinSequence["active"] = coinSequenceState.active;
    coinSequence["current_step"] = (int)coinSequenceState.currentStep;
    
    comm.sendStatus(statusPayload);
}

void SystemController::handleSystemReset(JsonObject payload) {
    resetSystemState();
    comm.sendSuccess("System reset");
}

void SystemController::handleSystemEmergencyStop(JsonObject payload) {
    stopAllMotors();
    clearAllLighting();
    
    if (coinSequenceState.active) {
        StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
        JsonObject eventPayload = eventDoc.to<JsonObject>();
        eventPayload["reason"] = "emergency_stop";
        eventPayload["stopped_at_step"] = (int)coinSequenceState.currentStep;
        comm.sendEvent("event_coin_sequence_stopped", eventPayload);
        coinSequenceState.active = false;
    }
    
    comm.sendSuccess("Emergency stop executed");
}

void SystemController::handleTestLedToggle(JsonObject payload) {
    ledTestState = !ledTestState;
    digitalWrite(Config::Pins::LED_TEST, ledTestState ? HIGH : LOW);
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["led_state"] = ledTestState;
    
    comm.sendSuccess(ledTestState ? "LED turned on" : "LED turned off", responseData);
}

void SystemController::handleSetBacklight(JsonObject payload) {
    if (!payload.containsKey("enabled")) {
        comm.sendError("Missing 'state' field", "INVALID_FORMAT");
        return;
    }
    
    bool state = payload["enabled"].as<bool>();
    leds.setBacklight(state);
    
    StaticJsonDocument<Config::System::JSON_DOC_SIZE> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["backlight_state"] = state;
    
    comm.sendSuccess(state ? "Backlight turned on" : "Backlight turned off", responseData);
}

void SystemController::processCoinSequence() {
    if (!coinSequenceState.active) {
        return;
    }
    
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - coinSequenceState.stepStartTime;
    
    switch (coinSequenceState.currentStep) {
        case STEP_MOVE_FORWARD: {
            motors.moveDCMotorRight();
            if (motors.isLimitSwitch2Pressed()) {
                motors.stopDCMotor();
                motorPosition += (currentTime - coinSequenceState.stepStartTime);
                
                StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
                JsonObject eventPayload = eventDoc.to<JsonObject>();
                eventPayload["step"] = "flipping";
                comm.sendEvent("event_coin_sequence_progress", eventPayload);
                
                coinSequenceState.currentStep = STEP_FLIP;
                coinSequenceState.stepStartTime = currentTime;
            }
            break;
        }
            
        case STEP_FLIP: {
            flipCoin();
            flipCount++;
            
            StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
            JsonObject eventPayload = eventDoc.to<JsonObject>();
            eventPayload["step"] = "waiting";
            eventPayload["wait_number"] = 1;
            comm.sendEvent("event_coin_sequence_progress", eventPayload);
            
            coinSequenceState.currentStep = STEP_WAIT1;
            coinSequenceState.stepStartTime = currentTime;
            break;
        }
            
        case STEP_WAIT1: {
            if (elapsed >= Config::System::COIN_SEQUENCE_WAIT_MS) {
                leds.setBacklight(true);
                
                StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
                JsonObject eventPayload = eventDoc.to<JsonObject>();
                eventPayload["step"] = "backlight_on";
                comm.sendEvent("event_coin_sequence_progress", eventPayload);
                
                coinSequenceState.currentStep = STEP_BACKLIGHT_ON;
                coinSequenceState.stepStartTime = currentTime;
            }
            break;
        }
            
        case STEP_BACKLIGHT_ON:
            coinSequenceState.currentStep = STEP_WAIT2;
            coinSequenceState.stepStartTime = currentTime;
            break;
            
        case STEP_WAIT2: {
            if (elapsed >= Config::System::COIN_SEQUENCE_WAIT_MS) {
                leds.setBacklight(false);
                
                StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
                JsonObject eventPayload = eventDoc.to<JsonObject>();
                eventPayload["step"] = "backlight_off";
                comm.sendEvent("event_coin_sequence_progress", eventPayload);
                
                coinSequenceState.currentStep = STEP_BACKLIGHT_OFF;
                coinSequenceState.stepStartTime = currentTime;
            }
            break;
        }
            
        case STEP_BACKLIGHT_OFF: {
            StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
            JsonObject eventPayload = eventDoc.to<JsonObject>();
            eventPayload["step"] = "moving_backward";
            comm.sendEvent("event_coin_sequence_progress", eventPayload);
            
            coinSequenceState.currentStep = STEP_MOVE_BACKWARD;
            coinSequenceState.stepStartTime = currentTime;
            break;
        }
            
        case STEP_MOVE_BACKWARD:
            motors.moveDCMotorLeft();
            if (motors.isLimitSwitch1Pressed()) {
                motors.stopDCMotor();
                motorPosition -= (currentTime - coinSequenceState.stepStartTime);
                
                coinSequenceState.currentStep = STEP_COMPLETE;
                coinSequenceState.stepStartTime = currentTime;
            }
            break;
            
        case STEP_COMPLETE: {
            unsigned long totalDuration = (currentTime - coinSequenceState.sequenceStartTime) / 1000;
            
            StaticJsonDocument<Config::System::JSON_DOC_SIZE> eventDoc;
            JsonObject eventPayload = eventDoc.to<JsonObject>();
            eventPayload["total_duration_seconds"] = totalDuration;
            comm.sendEvent("event_coin_sequence_completed", eventPayload);
            
            coinSequenceState.active = false;
            break;
        }
    }
}

void SystemController::flipCoin() {
    motors.executeSequence(1);
}

int SystemController::getRingIndexFromChannel(const char* channel) {
    if (strcmp(channel, "ring_1") == 0) return 0;
    if (strcmp(channel, "ring_2") == 0) return 1;
    if (strcmp(channel, "ring_3") == 0) return 2;
    if (strcmp(channel, "ring_4") == 0) return 3;
    return -1;
}

void SystemController::resetSystemState() {
    clearAllLighting();
    stopAllMotors();
    
    motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION_LEFT);
    motors.moveServoRight(Config::Servo::NEUTRAL_POSITION_RIGHT);
    
    coinSequenceState.active = false;
    
    ledTestState = false;
    digitalWrite(Config::Pins::LED_TEST, LOW);
    
    leds.setBacklight(false);
    
    motorPosition = 0;
    flipCount = 0;
}

void SystemController::stopAllMotors() {
    motors.stopDCMotor();
}

void SystemController::clearAllLighting() {
    leds.turnOff();
    for (int i = 0; i < Config::System::NUM_RING_CHANNELS; i++) {
        ringIntensities[i] = 0;
    }
}
