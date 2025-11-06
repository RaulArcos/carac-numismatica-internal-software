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

void LEDController::setRing(uint8_t ringIndex, uint8_t percentage) {
    uint8_t intensity = mapPercentage(percentage);
    
    // Calculate LED range for this ring
    // Ring 4 (ringIndex 3) is closest to coin: LEDs 0-15
    // Ring 3 (ringIndex 2): LEDs 16-31
    // Ring 2 (ringIndex 1): LEDs 32-47
    // Ring 1 (ringIndex 0): LEDs 48-63
    // Formula: startLED = (NUM_RINGS - 1 - ringIndex) * LEDS_PER_RING
    uint8_t startLED = (Config::LED::NUM_RINGS - 1 - ringIndex) * Config::LED::LEDS_PER_RING;
    uint8_t endLED = startLED + Config::LED::LEDS_PER_RING;
    
    // Set all LEDs in this ring
    for (uint8_t i = startLED; i < endLED && i < Config::LED::TOTAL_PIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(intensity, intensity, intensity));
    }
    pixels.show();
}

void LEDController::setSectorAcrossAllRings(uint8_t sectorIndex, uint8_t percentage) {
    uint8_t intensity = mapPercentage(percentage);
    
    // Set this sector (sectorIndex 0-3) across all rings
    // sectorIndex 0 = first sector, 1 = second sector, etc.
    for (uint8_t ringIndex = 0; ringIndex < Config::LED::NUM_RINGS; ringIndex++) {
        // Calculate ring's start LED (ring 4 is closest, ring 1 is farthest)
        uint8_t ringStartLED = (Config::LED::NUM_RINGS - 1 - ringIndex) * Config::LED::LEDS_PER_RING;
        
        // Calculate sector's start LED within this ring
        uint8_t startLED = ringStartLED + (sectorIndex * Config::LED::LEDS_PER_SECTOR);
        uint8_t endLED = startLED + Config::LED::LEDS_PER_SECTOR;
        
        // Set LEDs for this sector in this ring
        for (uint8_t i = startLED; i < endLED && i < Config::LED::TOTAL_PIXELS; i++) {
            pixels.setPixelColor(i, pixels.Color(intensity, intensity, intensity));
        }
    }
    pixels.show();
}

void LEDController::setSectorInternal(uint8_t sector, uint8_t intensity, bool showAfter) {
    uint8_t startLED, endLED;
    
    if (isStripSector(sector)) {
        // For strip sectors, set all LEDs in the strip
        startLED = Config::LED::NUM_RING_PIXELS;
        endLED = Config::LED::NUM_RING_PIXELS + Config::LED::NUM_STRIP_PIXELS;
    } else {
        // Sector is a global sector index (0-15 for 4 rings × 4 sectors)
        // We need to find which ring and which sector within that ring
        uint8_t ringIndex = sector / Config::LED::NUM_SECTORS_PER_RING;
        uint8_t sectorInRing = sector % Config::LED::NUM_SECTORS_PER_RING;
        
        // Calculate ring's start LED (ring 4 is closest, ring 1 is farthest)
        uint8_t ringStartLED = (Config::LED::NUM_RINGS - 1 - ringIndex) * Config::LED::LEDS_PER_RING;
        
        // Calculate sector's start LED within the ring
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
