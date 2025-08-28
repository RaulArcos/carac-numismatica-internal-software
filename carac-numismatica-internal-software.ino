#include "include/system_controller.h"

SystemController system;

void setup() {
  system.begin();
}

void loop() {
  system.update();
}