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
    setSectorInternal(sector, intensity, true);
}

void LEDController::setRing(uint8_t ringIndex, uint8_t intensity) {
    uint8_t startLED = (Config::LED::NUM_RINGS - 1 - ringIndex) * Config::LED::LEDS_PER_RING;
    uint8_t endLED = startLED + Config::LED::LEDS_PER_RING;
    
    for (uint8_t i = startLED; i < endLED && i < Config::LED::TOTAL_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(intensity, intensity, intensity));
    }
    pixels.show();
}

void LEDController::setSectorAcrossAllRings(uint8_t sectorIndex, uint8_t intensity) {
    for (uint8_t ringIndex = 0; ringIndex < Config::LED::NUM_RINGS; ringIndex++) {
        uint8_t ledIndex = ringIndex * Config::LED::LEDS_PER_RING + sectorIndex;
        if (ledIndex < Config::LED::TOTAL_PIXELS) {
            pixels.setPixelColor(ledIndex, pixels.Color(intensity, intensity, intensity));
        }
    }
    pixels.show();
}

void LEDController::setSectorInternal(uint8_t sector, uint8_t intensity, bool showAfter) {
    uint8_t startLED, endLED;
    
    if (isStripSector(sector)) {
        startLED = Config::LED::NUM_RING_PIXELS;
        endLED = Config::LED::NUM_RING_PIXELS + Config::LED::NUM_STRIP_PIXELS;
    } else {
        uint8_t ringIndex = sector / Config::LED::NUM_SECTORS_PER_RING;
        uint8_t sectorInRing = sector % Config::LED::NUM_SECTORS_PER_RING;
        
        uint8_t ringStartLED = (Config::LED::NUM_RINGS - 1 - ringIndex) * Config::LED::LEDS_PER_RING;
        
        startLED = ringStartLED + (sectorInRing * Config::LED::LEDS_PER_SECTOR);
        endLED = startLED + Config::LED::LEDS_PER_SECTOR;
    }
    
    for (uint8_t i = startLED; i < endLED && i < Config::LED::TOTAL_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(intensity, intensity, intensity));
    }
    
    if (showAfter) {
        pixels.show();
    }
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
