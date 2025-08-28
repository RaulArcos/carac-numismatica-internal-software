#include "../include/motor_controller.h"

MotorController::MotorController() {}

void MotorController::begin() {
    initializePins();
    setServosToNeutral();
}

void MotorController::executeSequence(uint8_t sequenceType) {
    if (sequenceType == 1) {
        for (int i = Config::Servo::NEUTRAL_POSITION; i <= Config::Servo::MAX_POSITION; i += Config::Servo::STEP_SIZE) {
            servoLeft.writeMicroseconds(i);
            delay(10);
        }
        
        waitForLimitSwitch(Config::Pins::LIMIT_SWITCH_1);
        delay(1000);
        
        for (int i = Config::Servo::NEUTRAL_POSITION; i <= Config::Servo::MAX_POSITION; i += Config::Servo::STEP_SIZE) {
            servoRight.writeMicroseconds(i);
            delay(10);
            servoLeft.writeMicroseconds(3210 - i);
            delay(10);
        }
        delay(500);
        
        for (int i = Config::Servo::NEUTRAL_POSITION; i >= 700; i -= Config::Servo::STEP_SIZE) {
            servoRight.writeMicroseconds(i);
            delay(500);
        }
    } else if (sequenceType == 2) {
        waitForLimitSwitch(Config::Pins::LIMIT_SWITCH_2);
        delay(1000);
    }
}

void MotorController::moveServoLeft(uint16_t position) {
    servoLeft.writeMicroseconds(position);
}

void MotorController::moveServoRight(uint16_t position) {
    servoRight.writeMicroseconds(position);
}

void MotorController::stopDCMotor() {
    digitalWrite(Config::Pins::DRIVER_MOTOR_A1, LOW);
    digitalWrite(Config::Pins::DRIVER_MOTOR_A2, LOW);
}

void MotorController::moveDCMotorLeft() {
    digitalWrite(Config::Pins::DRIVER_MOTOR_A1, LOW);
    digitalWrite(Config::Pins::DRIVER_MOTOR_A2, HIGH);
}

void MotorController::moveDCMotorRight() {
    digitalWrite(Config::Pins::DRIVER_MOTOR_A1, HIGH);
    digitalWrite(Config::Pins::DRIVER_MOTOR_A2, LOW);
}

bool MotorController::isLimitSwitch1Pressed() {
    return digitalRead(Config::Pins::LIMIT_SWITCH_1) == LOW;
}

bool MotorController::isLimitSwitch2Pressed() {
    return digitalRead(Config::Pins::LIMIT_SWITCH_2) == LOW;
}

void MotorController::initializePins() {
    pinMode(Config::Pins::DRIVER_MOTOR_A1, OUTPUT);
    pinMode(Config::Pins::DRIVER_MOTOR_A2, OUTPUT);
    pinMode(Config::Pins::LIMIT_SWITCH_1, INPUT_PULLUP);
    pinMode(Config::Pins::LIMIT_SWITCH_2, INPUT_PULLUP);
    
    servoLeft.attach(Config::Pins::SERVO_LEFT, Config::Servo::MIN_PULSE_WIDTH, Config::Servo::MAX_PULSE_WIDTH);
    servoRight.attach(Config::Pins::SERVO_RIGHT, Config::Servo::MIN_PULSE_WIDTH, Config::Servo::MAX_PULSE_WIDTH);
}

void MotorController::setServosToNeutral() {
    servoLeft.writeMicroseconds(Config::Servo::NEUTRAL_POSITION);
    servoRight.writeMicroseconds(Config::Servo::NEUTRAL_POSITION);
}

void MotorController::waitForLimitSwitch(uint8_t switchPin) {
    while (digitalRead(switchPin) == HIGH) {
        if (switchPin == Config::Pins::LIMIT_SWITCH_1) {
            moveDCMotorLeft();
        } else {
            moveDCMotorRight();
        }
    }
    stopDCMotor();
}
