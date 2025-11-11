#include "../include/motor_controller.h"

MotorController::MotorController() {
}

void MotorController::begin() {
    initializePins();
    setServosToNeutral();
}

void MotorController::executeSequence(uint8_t sequenceType) {
    if (sequenceType == 1) {
        for (int i = Config::Servo::NEUTRAL_POSITION; i <= Config::Servo::MAX_POSITION; i += Config::Servo::STEP_SIZE) {
            servoLeft.writeMicroseconds(i);
            delay(Config::Motor::SERVO_DELAY_MS);
        }
        
        waitForLimitSwitch(Config::Pins::LIMIT_SWITCH_1);
        delay(Config::Timing::LIMIT_SWITCH_WAIT_DELAY_MS);
        
        for (int i = Config::Servo::NEUTRAL_POSITION; i <= Config::Servo::MAX_POSITION; i += Config::Servo::STEP_SIZE) {
            servoRight.writeMicroseconds(i);
            delay(Config::Motor::SERVO_DELAY_MS);
            servoLeft.writeMicroseconds(Config::Motor::SERVO_MAX_CALCULATED - i);
            delay(Config::Motor::SERVO_DELAY_MS);
        }
        delay(Config::Motor::SERVO_SEQUENCE_DELAY_MS);
        
        for (int i = Config::Servo::NEUTRAL_POSITION; i >= Config::Motor::SERVO_MIN_POSITION; i -= Config::Servo::STEP_SIZE) {
            servoRight.writeMicroseconds(i);
            delay(Config::Motor::SERVO_SEQUENCE_DELAY_MS);
        }
    } else if (sequenceType == 2) {
        waitForLimitSwitch(Config::Pins::LIMIT_SWITCH_2);
        delay(Config::Timing::LIMIT_SWITCH_WAIT_DELAY_MS);
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
