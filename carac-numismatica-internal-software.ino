// motor_test.ino

#include "include/motor_controller.h"

MotorController motors;

void setup() {
  Serial.begin(115200);
  Serial.println("Motor Test - Initializing...");
  motors.begin();
  Serial.println("Ready!");
}

void loop() {
  while (true) {
  if (motors.isLimitSwitch1Pressed() || motors.isLimitSwitch2Pressed()) {
    Serial.println("Limit switch pressed - Motors stopped");
    motors.stopDCMotor();
    while (motors.isLimitSwitch1Pressed() || motors.isLimitSwitch2Pressed()) {
      delay(50);
    }
    Serial.println("Limit switch released - Resuming");
    delay(100); 
  }
  
  Serial.println("Starting DC Motor (Left direction)");
  motors.moveDCMotorLeft();
  delay(500);
  
  if (motors.isLimitSwitch1Pressed() || motors.isLimitSwitch2Pressed()) {
    motors.stopDCMotor();
    continue;
  }
  
  Serial.println("Left Servo: neutral -> +300us");
  motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION + 300);
  delay(1000);
  
  if (motors.isLimitSwitch1Pressed() || motors.isLimitSwitch2Pressed()) {
    motors.stopDCMotor();
    continue;
  }
  
  Serial.println("Left Servo: back to neutral");
  motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION);
  delay(1000);
  
  if (motors.isLimitSwitch1Pressed() || motors.isLimitSwitch2Pressed()) {
    motors.stopDCMotor();
    continue;
  }
  
  Serial.println("Right Servo: neutral -> +300us");
  motors.moveServoRight(Config::Servo::NEUTRAL_POSITION + 300);
  delay(1000);
  
  if (motors.isLimitSwitch1Pressed() || motors.isLimitSwitch2Pressed()) {
    motors.stopDCMotor();
    continue;
  }
  
  Serial.println("Right Servo: back to neutral");
  motors.moveServoRight(Config::Servo::NEUTRAL_POSITION);
  delay(1000);
  
  if (motors.isLimitSwitch1Pressed() || motors.isLimitSwitch2Pressed()) {
    motors.stopDCMotor();
    continue;
  }
  
  Serial.println("Stopping DC Motor");
    motors.stopDCMotor();
    delay(1000);
  }
}