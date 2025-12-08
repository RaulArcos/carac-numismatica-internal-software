#include "../include/motor_controller.h"

MotorController::MotorController() {
}

void MotorController::begin() {
    initializePins();
    setServosToNeutral();
}

void MotorController::executeSequence(uint8_t sequenceType) {
    if (sequenceType == 1) {
        for (int i = Config::Servo::NEUTRAL_POSITION_LEFT; i <= Config::Servo::MAX_POSITION_LEFT; i += Config::Servo::STEP_SIZE) {
            servoLeft.writeMicroseconds(i);
            delay(Config::Motor::SERVO_DELAY_MS);
        }
        int j = 0;
        for (int i = Config::Servo::NEUTRAL_POSITION_RIGHT; i <= Config::Servo::MAX_POSITION_RIGHT; i += Config::Servo::STEP_SIZE) {
            servoRight.writeMicroseconds(i);
            delay(Config::Motor::SERVO_DELAY_MS);
            if ((Config::Servo::MAX_POSITION_LEFT - j) >= Config::Servo::NEUTRAL_POSITION_LEFT){
                servoLeft.writeMicroseconds(Config::Servo::MAX_POSITION_LEFT - j);
            }
            j+= Config::Servo::STEP_SIZE;
            delay(Config::Motor::SERVO_DELAY_MS);
        }
        
        for (int i = Config::Servo::MAX_POSITION_RIGHT; i >= Config::Servo::NEUTRAL_POSITION_RIGHT; i -= Config::Servo::STEP_SIZE) {
            servoRight.writeMicroseconds(i);
            delay(Config::Motor::SERVO_DELAY_MS);
        }
    }
    setServosToNeutral();
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
    digitalWrite(Config::Pins::DRIVER_MOTOR_A1, HIGH);
    digitalWrite(Config::Pins::DRIVER_MOTOR_A2, LOW);
}

void MotorController::moveDCMotorRight() {
    digitalWrite(Config::Pins::DRIVER_MOTOR_A1, LOW);
    digitalWrite(Config::Pins::DRIVER_MOTOR_A2, HIGH);
}

bool MotorController::isLimitSwitch1Pressed() {
    return digitalRead(Config::Pins::LIMIT_SWITCH_1) == HIGH;
}

bool MotorController::isLimitSwitch2Pressed() {
    return digitalRead(Config::Pins::LIMIT_SWITCH_2) == HIGH;
}

void MotorController::initializePins() {
    pinMode(Config::Pins::DRIVER_MOTOR_A1, OUTPUT);
    pinMode(Config::Pins::DRIVER_MOTOR_A2, OUTPUT);
    pinMode(Config::Pins::LIMIT_SWITCH_1, INPUT_PULLUP);
    pinMode(Config::Pins::LIMIT_SWITCH_2, INPUT_PULLUP);
    
    servoLeft.attach(Config::Pins::SERVO_LEFT, Config::Servo::MIN_PULSE_US, Config::Servo::MAX_PULSE_US);
    servoRight.attach(Config::Pins::SERVO_RIGHT, Config::Servo::MIN_PULSE_US, Config::Servo::MAX_PULSE_US);
}

void MotorController::setServosToNeutral() {
    servoLeft.writeMicroseconds(Config::Servo::NEUTRAL_POSITION_LEFT);
    servoRight.writeMicroseconds(Config::Servo::NEUTRAL_POSITION_RIGHT);
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
