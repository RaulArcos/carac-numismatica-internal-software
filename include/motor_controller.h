#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>
#include "Config.h"

class MotorController {
public:
    MotorController();
    void begin();
    void executeSequence(uint8_t sequenceType);
    void moveServoLeft(uint16_t position);
    void moveServoRight(uint16_t position);
    void stopDCMotor();
    void moveDCMotorLeft();
    void moveDCMotorRight();
    bool isLimitSwitch1Pressed();
    bool isLimitSwitch2Pressed();

private:
    Servo servoLeft;
    Servo servoRight;
    
    void initializePins();
    void setServosToNeutral();
    void waitForLimitSwitch(uint8_t switchPin);
};
