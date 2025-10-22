// motor_test.ino

#include "include/motor_controller.h"

MotorController motors;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== Motor Test Program ===");
  Serial.println("Initializing motor controller...");
  
  motors.begin();
  
  Serial.println("Motor controller initialized!");
  Serial.println("Servos set to neutral position");
  Serial.println("\nStarting motor tests in 2 seconds...\n");
  delay(2000);
}

void loop() {
  Serial.println("TEST 1: DC Motor moving RIGHT for 1 second");
  motors.moveDCMotorRight();
  delay(1000);
  motors.stopDCMotor();
  Serial.println("DC Motor STOPPED");
  delay(2000);
  
  Serial.println("\nTEST 2: DC Motor moving LEFT for 1 second");
  motors.moveDCMotorLeft();
  delay(1000);
  motors.stopDCMotor();
  Serial.println("DC Motor STOPPED");
  delay(2000);
  
  Serial.println("\nTEST 3: Left Servo sweeping");
  Serial.println("Moving to position 1500us");
  motors.moveServoLeft(1500);
  delay(1000);
  
  Serial.println("Moving to position 2000us");
  motors.moveServoLeft(2000);
  delay(1000);
  
  Serial.println("Returning to neutral (710us)");
  motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION);
  delay(2000);
  
  Serial.println("\nTEST 4: Right Servo sweeping");
  Serial.println("Moving to position 1500us");
  motors.moveServoRight(1500);
  delay(1000);
  
  Serial.println("Moving to position 2000us");
  motors.moveServoRight(2000);
  delay(1000);
  
  Serial.println("Returning to neutral (710us)");
  motors.moveServoRight(Config::Servo::NEUTRAL_POSITION);
  delay(2000);
  
  Serial.println("\nTEST 5: Both Servos moving together");
  Serial.println("Moving both to 1800us");
  motors.moveServoLeft(1800);
  motors.moveServoRight(1800);
  delay(1500);
  
  Serial.println("Returning both to neutral");
  motors.moveServoLeft(Config::Servo::NEUTRAL_POSITION);
  motors.moveServoRight(Config::Servo::NEUTRAL_POSITION);
  delay(2000);
  
  Serial.println("\nTEST 6: Checking limit switches");
  Serial.print("Limit Switch 1: ");
  Serial.println(motors.isLimitSwitch1Pressed() ? "PRESSED" : "NOT PRESSED");
  Serial.print("Limit Switch 2: ");
  Serial.println(motors.isLimitSwitch2Pressed() ? "PRESSED" : "NOT PRESSED");
  
  Serial.println("\n=== Test cycle complete! ===");
  Serial.println("Waiting 5 seconds before next cycle...\n");
  delay(5000);
}