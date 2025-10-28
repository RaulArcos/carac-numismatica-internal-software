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
  Serial.println("Moving RIGHT");
  motors.moveDCMotorRight();
  delay(1000);
  motors.stopDCMotor();
  delay(1000);
  
  Serial.println("Moving LEFT");
  motors.moveDCMotorLeft();
  delay(1000);
  motors.stopDCMotor();
  delay(1000);
}