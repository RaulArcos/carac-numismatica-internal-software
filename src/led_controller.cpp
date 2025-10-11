#include "../include/led_controller.h"

LEDController::LEDController() 
    : pixels(Config::LED::TOTAL_PIXELS, Config::Pins::LED_STRIP, NEO_GRB + NEO_KHZ800) {
}

void LEDController::begin() {
    pixels.begin();
    turnOff();
}

void LEDController::setSector(uint8_t sector, uint8_t percentage) {
    uint8_t intensity = mapPercentage(percentage);
    uint8_t startLED, endLED;
    
    if (isStripSector(sector)) {
        startLED = sector;
        endLED = sector + Config::LED::NUM_STRIP_PIXELS;
    } else {
        startLED = sector;
        endLED = sector + Config::LED::LEDS_PER_SECTOR;
    }
    
    for (uint8_t i = startLED; i < endLED && i < Config::LED::TOTAL_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(intensity, intensity, intensity));
    }
    pixels.show();
}

void LEDController::setAllLEDs(uint8_t red, uint8_t green, uint8_t blue) {
    for (uint16_t i = 0; i < Config::LED::TOTAL_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(red, green, blue));
    }
    pixels.show();
}

void LEDController::turnOff() {
    pixels.clear();
    pixels.show();
}

void LEDController::setBrightness(uint8_t brightness) {
    pixels.setBrightness(brightness);
    pixels.show();
}

uint8_t LEDController::mapPercentage(uint8_t percentage) {
    return map(percentage, 0, 100, 0, 255);
}

bool LEDController::isStripSector(uint8_t sector) {
    return sector > Config::LED::NUM_RING_PIXELS;
}
