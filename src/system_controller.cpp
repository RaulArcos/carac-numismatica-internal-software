#include "../include/system_controller.h"

SystemController::SystemController() 
    : ledTestState(false), motorPosition(0) {
    // Initialize ring intensities
    for (int i = 0; i < 4; i++) {
        ringIntensities[i] = 0;
    }
    
    // Initialize sequence state
    sequenceState.active = false;
    sequenceState.currentPhoto = 0;
    sequenceState.totalPhotos = 0;
    sequenceState.delay = 0.0;
    sequenceState.autoFlip = false;
    sequenceState.lastActionTime = 0;
    sequenceState.sequenceStartTime = 0;
}

void SystemController::begin() {
    // Initialize hardware
    comm.begin();
    motors.begin();
    leds.begin();
    weight.begin();
    
    // Initialize camera trigger pin
    pinMode(Config::Pins::CAMERA_TRIGGER, OUTPUT);
    digitalWrite(Config::Pins::CAMERA_TRIGGER, LOW);
    
    // Initialize test LED pin
    pinMode(Config::Pins::LED_TEST, OUTPUT);
    digitalWrite(Config::Pins::LED_TEST, LOW);
    
    // Register protocol message handlers
    registerMessageHandlers();
}

void SystemController::update() {
    comm.update();
    processPhotoSequence();
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

// ============================================================================
// Protocol Message Handlers
// ============================================================================

void SystemController::handleLightingSet(JsonObject payload) {
    const char* channel = payload["channel"];
    int intensity = payload["intensity"] | 0;
    
    // Validate channel
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
    
    // Validate intensity
    if (intensity < 0 || intensity > 255) {
        StaticJsonDocument<256> errorDoc;
        JsonObject errorData = errorDoc.to<JsonObject>();
        errorData["received_intensity"] = intensity;
        errorData["valid_range"] = "0-255";
        comm.sendError("Intensity must be 0-255", "INVALID_INTENSITY", errorData);
        return;
    }
    
    // Set the lighting using the sector method
    // Map ring index to sector (9 LEDs per sector)
    uint8_t sector = ringIndex * Config::LED::LEDS_PER_SECTOR;
    uint8_t percentage = map(intensity, 0, 255, 0, 100);
    
    leds.setSector(sector, percentage);
    ringIntensities[ringIndex] = intensity;
    
    // Send success response
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["channel"] = channel;
    responseData["intensity"] = intensity;
    
    comm.sendSuccess("Lighting set successfully", responseData);
}

void SystemController::handleMotorPosition(JsonObject payload) {
    const char* direction = payload["direction"];
    int steps = payload.containsKey("steps") ? payload["steps"].as<int>() : Config::Motor::DEFAULT_STEPS;
    
    // Validate direction
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
    
    // Move the DC motor
    unsigned long startTime = millis();
    if (strcmp(direction, "forward") == 0) {
        motors.moveDCMotorRight();
        delay(steps);  // Simulate movement based on steps
        motorPosition += steps;
    } else {
        motors.moveDCMotorLeft();
        delay(steps);
        motorPosition -= steps;
    }
    motors.stopDCMotor();
    unsigned long duration = millis() - startTime;
    
    // Send success response
    StaticJsonDocument<256> responseDoc;
    JsonObject responseData = responseDoc.to<JsonObject>();
    responseData["direction"] = direction;
    responseData["steps"] = steps;
    
    comm.sendSuccess("Motor moved successfully", responseData);
    
    // Send motor complete event
    StaticJsonDocument<256> eventDoc;
    JsonObject eventPayload = eventDoc.to<JsonObject>();
    eventPayload["position"] = motorPosition;
    eventPayload["duration"] = (int)duration;
    
    comm.sendEvent("event_motor_complete", eventPayload);
}

void SystemController::handleMotorFlip(JsonObject payload) {
    flipCoin();
    
    comm.sendSuccess("Coin flipped successfully");
    
    // Send motor complete event
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
    
    // Send camera triggered event
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
    
    // Send sequence started event
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
    
    // Lighting status
    JsonObject lighting = statusPayload.createNestedObject("lighting");
    lighting["ring_1"] = ringIntensities[0];
    lighting["ring_2"] = ringIntensities[1];
    lighting["ring_3"] = ringIntensities[2];
    lighting["ring_4"] = ringIntensities[3];
    
    // Motor status
    JsonObject motor = statusPayload.createNestedObject("motor");
    motor["position"] = motorPosition;
    motor["is_moving"] = false;
    
    // Sequence status
    JsonObject sequence = statusPayload.createNestedObject("sequence");
    sequence["active"] = sequenceState.active;
    sequence["current_photo"] = sequenceState.currentPhoto;
    sequence["total_photos"] = sequenceState.totalPhotos;
    
    comm.sendStatus(statusPayload);
}

void SystemController::handleSystemReset(JsonObject payload) {
    // Reset all lighting
    leds.turnOff();
    for (int i = 0; i < 4; i++) {
        ringIntensities[i] = 0;
    }
    
    // Stop motors
    motors.stopDCMotor();
    motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION);
    motors.moveServoRight(Config::Servo::NEUTRAL_POSITION);
    
    // Stop any active sequence
    sequenceState.active = false;
    
    // Reset test LED
    ledTestState = false;
    digitalWrite(Config::Pins::LED_TEST, LOW);
    
    // Reset motor position
    motorPosition = 0;
    
    comm.sendSuccess("System reset");
}

void SystemController::handleSystemEmergencyStop(JsonObject payload) {
    // Stop everything immediately
    leds.turnOff();
    motors.stopDCMotor();
    
    // Stop sequence
    if (sequenceState.active) {
        StaticJsonDocument<256> eventDoc;
        JsonObject eventPayload = eventDoc.to<JsonObject>();
        eventPayload["reason"] = "emergency_stop";
        eventPayload["photos_taken"] = sequenceState.currentPhoto;
        
        comm.sendEvent("event_sequence_stopped", eventPayload);
        
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

// ============================================================================
// Helper Methods
// ============================================================================

void SystemController::processPhotoSequence() {
    if (!sequenceState.active) {
        return;
    }
    
    unsigned long currentTime = millis();
    unsigned long elapsed = currentTime - sequenceState.lastActionTime;
    
    // Check if it's time to take next photo
    if (elapsed >= (sequenceState.delay * 1000)) {
        sequenceState.currentPhoto++;
        
        if (sequenceState.currentPhoto > sequenceState.totalPhotos) {
            // Sequence complete
            unsigned long totalDuration = (currentTime - sequenceState.sequenceStartTime) / 1000.0;
            
            StaticJsonDocument<256> eventDoc;
            JsonObject eventPayload = eventDoc.to<JsonObject>();
            eventPayload["photos_taken"] = sequenceState.totalPhotos;
            eventPayload["duration"] = totalDuration;
            
            comm.sendEvent("event_sequence_completed", eventPayload);
            
            sequenceState.active = false;
            return;
        }
        
        // Send progress event - waiting
        if (sequenceState.currentPhoto > 1) {
            StaticJsonDocument<256> progressDoc;
            JsonObject progressPayload = progressDoc.to<JsonObject>();
            progressPayload["current_photo"] = sequenceState.currentPhoto;
            progressPayload["total_photos"] = sequenceState.totalPhotos;
            progressPayload["action"] = "waiting";
            comm.sendEvent("event_sequence_progress", progressPayload);
        }
        
        // Send progress event - taking photo
        StaticJsonDocument<256> progressDoc;
        JsonObject progressPayload = progressDoc.to<JsonObject>();
        progressPayload["current_photo"] = sequenceState.currentPhoto;
        progressPayload["total_photos"] = sequenceState.totalPhotos;
        progressPayload["action"] = "taking_photo";
        comm.sendEvent("event_sequence_progress", progressPayload);
        
        // Trigger camera
        triggerCamera(Config::Camera::DEFAULT_TRIGGER_DURATION);
        
        // Send camera triggered event
        StaticJsonDocument<256> cameraDoc;
        JsonObject cameraPayload = cameraDoc.to<JsonObject>();
        cameraPayload["duration"] = Config::Camera::DEFAULT_TRIGGER_DURATION;
        comm.sendEvent("event_camera_triggered", cameraPayload);
        
        // Auto-flip if enabled and not the last photo
        if (sequenceState.autoFlip && sequenceState.currentPhoto < sequenceState.totalPhotos) {
            StaticJsonDocument<256> flipProgressDoc;
            JsonObject flipProgressPayload = flipProgressDoc.to<JsonObject>();
            flipProgressPayload["current_photo"] = sequenceState.currentPhoto;
            flipProgressPayload["total_photos"] = sequenceState.totalPhotos;
            flipProgressPayload["action"] = "flipping_coin";
            comm.sendEvent("event_sequence_progress", flipProgressPayload);
            
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
    // Use servos to flip the coin
    // Move servos in a coordinated motion
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