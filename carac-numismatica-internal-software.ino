// led_test.ino
// Simple LED test using existing LEDController class

#include "include/led_controller.h"

LEDController leds;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== LED Test Program ===");
  Serial.println("Initializing LED controller...");
  
  leds.begin();
  
  Serial.println("LED controller initialized!");
  Serial.println("Total LEDs: " + String(Config::LED::TOTAL_PIXELS));
  Serial.println("Ring LEDs: " + String(Config::LED::NUM_RING_PIXELS));
  Serial.println("Strip LEDs: " + String(Config::LED::NUM_STRIP_PIXELS));
  Serial.println("\nStarting LED tests in 2 seconds...\n");
  delay(2000);
}

void loop() {
  Serial.println("TEST 1: All LEDs WHITE");
  leds.setAllLEDs(255, 255, 255);
  delay(2000);
  
  leds.turnOff();
  delay(500);
  
  Serial.println("\nTEST 2: Color test");
  Serial.println("RED");
  leds.setAllLEDs(255, 0, 0);
  delay(1500);
  
  Serial.println("GREEN");
  leds.setAllLEDs(0, 255, 0);
  delay(1500);
  
  Serial.println("BLUE");
  leds.setAllLEDs(0, 0, 255);
  delay(1500);
  
  leds.turnOff();
  delay(500);
  
  Serial.println("\nTEST 3: Testing ring sectors (1-4)");
  for (int ring = 0; ring < Config::LED::NUM_RINGS; ring++) {
    uint8_t sector = ring * Config::LED::LEDS_PER_SECTOR;
    Serial.println("Ring " + String(ring + 1) + " at 100% (sector " + String(sector) + ")");
    leds.setSector(sector, 100);
    delay(1000);
    leds.turnOff();
    delay(300);
  }
  
  Serial.println("\nTEST 4: All rings at different intensities");
  for (int ring = 0; ring < Config::LED::NUM_RINGS; ring++) {
    uint8_t sector = ring * Config::LED::LEDS_PER_SECTOR;
    uint8_t intensity = (ring + 1) * 25;  // 25%, 50%, 75%, 100%
    Serial.println("Ring " + String(ring + 1) + " at " + String(intensity) + "%");
    leds.setSector(sector, intensity);
    delay(500);
  }
  delay(2000);
  leds.turnOff();
  delay(500);
  
  Serial.println("\nTEST 5: Brightness fade test");
  leds.setAllLEDs(255, 255, 255);
  
  Serial.println("Fading down...");
  for (int brightness = 255; brightness >= 0; brightness -= 15) {
    leds.setBrightness(brightness);
    delay(100);
  }
  
  Serial.println("Fading up...");
  for (int brightness = 0; brightness <= 255; brightness += 15) {
    leds.setBrightness(brightness);
    delay(100);
  }
  
  delay(1000);
  leds.turnOff();
  delay(500);
  
  Serial.println("\nTEST 6: Testing LED strip");
  uint8_t stripSector = Config::LED::NUM_RING_PIXELS;
  Serial.println("Strip at 100% (sector " + String(stripSector) + ")");
  leds.setSector(stripSector, 100);
  delay(2000);
  leds.turnOff();
  delay(500);
  
  Serial.println("\nTEST 7: Rainbow effect");
  for (int hue = 0; hue < 360; hue += 30) {
    uint8_t r, g, b;
    if (hue < 120) {
      r = 255 - (hue * 255 / 120);
      g = hue * 255 / 120;
      b = 0;
    } else if (hue < 240) {
      r = 0;
      g = 255 - ((hue - 120) * 255 / 120);
      b = (hue - 120) * 255 / 120;
    } else {
      r = (hue - 240) * 255 / 120;
      g = 0;
      b = 255 - ((hue - 240) * 255 / 120);
    }
    leds.setAllLEDs(r, g, b);
    delay(200);
  }
  
  leds.turnOff();
  
  Serial.println("\n=== Test cycle complete! ===");
  Serial.println("Waiting 3 seconds before next cycle...\n");
  delay(3000);
}