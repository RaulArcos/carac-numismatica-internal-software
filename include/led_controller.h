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
    void setSectorAcrossAllRings(uint8_t ringIndex, uint8_t intensity);
    void setAllLEDs(uint8_t red, uint8_t green, uint8_t blue);
    void turnOff();
    void setBrightness(uint8_t brightness);
    void setBacklight(bool state);
    bool getBacklightState();

private:
    Adafruit_NeoPixel pixels;
    Adafruit_NeoPixel backlightPixels;
    bool backlightState;
    
    uint8_t mapPercentage(uint8_t percentage);
    bool isStripSector(uint8_t sector);
    void setSectorInternal(uint8_t sector, uint8_t intensity, bool showAfter);
};
