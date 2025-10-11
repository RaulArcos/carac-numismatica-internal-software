#include "../include/system_controller.h"

SystemController::SystemController() 
    : ledTestState(false)
    , motorPosition(0) {
    for (int i = 0; i < NUM_RING_CHANNELS; i++) {
        ringIntensities[i] = 0;
    }
    
    sequenceState.active = false;
    sequenceState.currentPhoto = 0;
    sequenceState.totalPhotos = 0;
    sequenceState.delay = 0.0;
    sequenceState.autoFlip = false;
    sequenceState.lastActionTime = 0;
    sequenceState.sequenceStartTime = 0;
}

void SystemController::begin() {
    comm.begin();
    // motors.begin();
    // leds.begin();
    // weight.begin();
    
    // pinMode(Config::Pins::CAMERA_TRIGGER, OUTPUT);
    // digitalWrite(Config::Pins::CAMERA_TRIGGER, LOW);
    
    // pinMode(Config::Pins::LED_TEST, OUTPUT);
    // digitalWrite(Config::Pins::LED_TEST, LOW);
    
    registerMessageHandlers();
}

void SystemController::update() {
    comm.update();
    // processPhotoSequence();
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
    
    comm.registerMessageHandler("photo_sequence_start", [this](JsonObject payload) { 
        handlePhotoSequenceStart(payload); 
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
}

void SystemController::handleLightingSet(JsonObject payload) {
    const char* channel = payload["channel"];
    int intensity = payload["intensity"] | 0;
    
    int ringIndex = getRingIndexFromChannel(channel);
    if (ringIndex == -1) {
        StaticJsonDocument<256> errorDoc;
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
    
    if (intensity < 0 || intensity > 255) {
        StaticJsonDocument<256> errorDoc;
        JsonObject errorData = errorDoc.to<JsonObject>();
        errorData["received_intensity"] = intensity;
        errorData["valid_range"] = "0-255";
        comm.sendError("Intensity must be 0-255", "INVALID_INTENSITY", errorData);
        return;
    }
    
    uint8_t sector = ringIndex * Config::LED::LEDS_PER_SECTOR;
    uint8_t percentage = map(intensity, 0, 255, 0, 100);
    
    leds.setSector(sector, percentage);
    ringIntensities[ringIndex] = intensity;
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["channel"] = channel;
    responseData["intensity"] = intensity;
    
    comm.sendSuccess("Lighting set successfully", responseData);
}

void SystemController::handleMotorPosition(JsonObject payload) {
    const char* direction = payload["direction"];
    int steps = payload.containsKey("steps") ? payload["steps"].as<int>() : Config::Motor::DEFAULT_STEPS;
    
    if (!direction || (strcmp(direction, "forward") != 0 && strcmp(direction, "backward") != 0)) {
        StaticJsonDocument<256> errorDoc;
        JsonObject errorData = errorDoc.to<JsonObject>();
        errorData["received_direction"] = direction ? direction : "null";
        JsonArray validDirections = errorData.createNestedArray("valid_directions");
        validDirections.add("forward");
        validDirections.add("backward");
        comm.sendError("Invalid direction", "INVALID_DIRECTION", errorData);
        return;
    }
    
    unsigned long startTime = millis();
    if (strcmp(direction, "forward") == 0) {
        motors.moveDCMotorRight();
        delay(steps);
        motorPosition += steps;
    } else {
        motors.moveDCMotorLeft();
        delay(steps);
        motorPosition -= steps;
    }
    motors.stopDCMotor();
    unsigned long duration = millis() - startTime;
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["direction"] = direction;
    responseData["steps"] = steps;
    
    comm.sendSuccess("Motor moved successfully", responseData);
    
    StaticJsonDocument<256> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["position"] = motorPosition;
    eventPayload["duration"] = (int)duration;
    
    comm.sendEvent("event_motor_complete", eventPayload);
}

void SystemController::handleMotorFlip(JsonObject payload) {
    flipCoin();
    
    comm.sendSuccess("Coin flipped successfully");
    
    StaticJsonDocument<256> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["position"] = motorPosition;
    eventPayload["duration"] = 200;
    
    comm.sendEvent("event_motor_complete", eventPayload);
}

void SystemController::handleCameraTrigger(JsonObject payload) {
    int duration = payload.containsKey("duration") ? 
                   payload["duration"].as<int>() : 
                   Config::Camera::DEFAULT_TRIGGER_DURATION;
    
    triggerCamera(duration);
    
    comm.sendSuccess("Camera triggered");
    
    StaticJsonDocument<256> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["duration"] = duration;
    
    comm.sendEvent("event_camera_triggered", eventPayload);
}

void SystemController::handlePhotoSequenceStart(JsonObject payload) {
    if (sequenceState.active) {
        comm.sendError("Sequence already active", "SEQUENCE_ACTIVE");
        return;
    }
    
    sequenceState.active = true;
    sequenceState.totalPhotos = payload["count"] | 1;
    sequenceState.delay = payload["delay"] | 1.0;
    sequenceState.autoFlip = payload.containsKey("auto_flip") ? payload["auto_flip"].as<bool>() : false;
    sequenceState.currentPhoto = 0;
    sequenceState.lastActionTime = millis();
    sequenceState.sequenceStartTime = millis();
    
    comm.sendSuccess("Photo sequence started");
    
    StaticJsonDocument<256> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["total_photos"] = sequenceState.totalPhotos;
    eventPayload["delay"] = sequenceState.delay;
    
    comm.sendEvent("event_sequence_started", eventPayload);
}

void SystemController::handleSystemPing(JsonObject payload) {
    comm.sendSuccess("Pong");
}

void SystemController::handleSystemStatus(JsonObject payload) {
    StaticJsonDocument<Config::Communication::BUFFER_SIZE> statusDoc;
    JsonObject statusPayload = statusDoc.to<JsonObject>();
    
    statusPayload["uptime"] = millis();
    statusPayload["firmware_version"] = Config::Communication::FIRMWARE_VERSION;
    
    JsonObject lighting = statusPayload.createNestedObject("lighting");
    lighting["ring_1"] = ringIntensities[0];
    lighting["ring_2"] = ringIntensities[1];
    lighting["ring_3"] = ringIntensities[2];
    lighting["ring_4"] = ringIntensities[3];
    
    JsonObject motor = statusPayload.createNestedObject("motor");
    motor["position"] = motorPosition;
    motor["is_moving"] = false;
    
    JsonObject sequence = statusPayload.createNestedObject("sequence");
    sequence["active"] = sequenceState.active;
    sequence["current_photo"] = sequenceState.currentPhoto;
    sequence["total_photos"] = sequenceState.totalPhotos;
    
    comm.sendStatus(statusPayload);
}

void SystemController::handleSystemReset(JsonObject payload) {
    resetSystemState();
    comm.sendSuccess("System reset");
}

void SystemController::handleSystemEmergencyStop(JsonObject payload) {
    stopAllMotors();
    clearAllLighting();
    
    if (sequenceState.active) {
        sendSequenceStoppedEvent("emergency_stop", sequenceState.currentPhoto);
        sequenceState.active = false;
    }
    
    comm.sendSuccess("Emergency stop executed");
}

void SystemController::handleTestLedToggle(JsonObject payload) {
    ledTestState = !ledTestState;
    digitalWrite(Config::Pins::LED_TEST, ledTestState ? HIGH : LOW);
    
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["led_state"] = ledTestState;
    
    comm.sendSuccess(ledTestState ? "LED turned on" : "LED turned off", responseData);
}

void SystemController::processPhotoSequence() {
    if (!sequenceState.active) {
        return;
    }
    
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - sequenceState.lastActionTime;
    
    if (elapsed >= (sequenceState.delay * 1000)) {
        sequenceState.currentPhoto++;
        
        if (sequenceState.currentPhoto > sequenceState.totalPhotos) {
            unsigned long totalDuration = (currentTime - sequenceState.sequenceStartTime) / 1000.0;
            sendSequenceCompletedEvent(totalDuration);
            sequenceState.active = false;
            return;
        }
        
        if (sequenceState.currentPhoto > 1) {
            sendSequenceProgressEvent("waiting");
        }
        
        sendSequenceProgressEvent("taking_photo");
        
        triggerCamera(Config::Camera::DEFAULT_TRIGGER_DURATION);
        
        StaticJsonDocument<256> cameraDoc;
        JsonObject cameraPayload = cameraDoc.to<JsonObject>();
        cameraPayload["duration"] = Config::Camera::DEFAULT_TRIGGER_DURATION;
        comm.sendEvent("event_camera_triggered", cameraPayload);
        
        if (sequenceState.autoFlip && sequenceState.currentPhoto < sequenceState.totalPhotos) {
            sendSequenceProgressEvent("flipping_coin");
            flipCoin();
        }
        
        sequenceState.lastActionTime = currentTime;
    }
}

void SystemController::triggerCamera(int duration) {
    digitalWrite(Config::Pins::CAMERA_TRIGGER, HIGH);
    delay(duration);
    digitalWrite(Config::Pins::CAMERA_TRIGGER, LOW);
}

void SystemController::flipCoin() {
    motors.moveServoLeft(Config::Servo::MAX_POSITION);
    delay(100);
    motors.moveServoRight(Config::Servo::MAX_POSITION);
    delay(100);
    motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION);
    motors.moveServoRight(Config::Servo::NEUTRAL_POSITION);
    delay(100);
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
    
    motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION);
    motors.moveServoRight(Config::Servo::NEUTRAL_POSITION);
    
    sequenceState.active = false;
    
    ledTestState = false;
    digitalWrite(Config::Pins::LED_TEST, LOW);
    
    motorPosition = 0;
}

void SystemController::stopAllMotors() {
    motors.stopDCMotor();
}

void SystemController::clearAllLighting() {
    leds.turnOff();
    for (int i = 0; i < NUM_RING_CHANNELS; i++) {
        ringIntensities[i] = 0;
    }
}

void SystemController::sendSequenceProgressEvent(const char* action) {
    StaticJsonDocument<256> progressDoc;
    JsonObject progressPayload = progressDoc.to<JsonObject>();
    progressPayload["current_photo"] = sequenceState.currentPhoto;
    progressPayload["total_photos"] = sequenceState.totalPhotos;
    progressPayload["action"] = action;
    comm.sendEvent("event_sequence_progress", progressPayload);
}

void SystemController::sendSequenceStoppedEvent(const char* reason, int photosTaken) {
    StaticJsonDocument<256> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["reason"] = reason;
    eventPayload["photos_taken"] = photosTaken;
    comm.sendEvent("event_sequence_stopped", eventPayload);
}

void SystemController::sendSequenceCompletedEvent(unsigned long totalDuration) {
    StaticJsonDocument<256> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["photos_taken"] = sequenceState.totalPhotos;
    eventPayload["duration"] = totalDuration;
    comm.sendEvent("event_sequence_completed", eventPayload);
}
