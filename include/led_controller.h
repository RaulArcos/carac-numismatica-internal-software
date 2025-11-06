#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "config.h"

class LEDController {
public:
    LEDController();
    void begin();
    void setSector(uint8_t sector, uint8_t percentage);
    void setRing(uint8_t ringIndex, uint8_t percentage);
    void setSectorAcrossAllRings(uint8_t sectorIndex, uint8_t percentage);
    void setAllLEDs(uint8_t red, uint8_t green, uint8_t blue);
    void turnOff();
    void setBrightness(uint8_t brightness);

private:
    Adafruit_NeoPixel pixels;
    
    uint8_t mapPercentage(uint8_t percentage);
    bool isStripSector(uint8_t sector);
    void setSectorInternal(uint8_t sector, uint8_t intensity, bool showAfter);
};
