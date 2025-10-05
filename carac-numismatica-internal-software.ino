#include "include/system_controller.h"

SystemController systemController;

void setup() {
  systemController.begin();
}

void loop() {
  systemController.update();
}