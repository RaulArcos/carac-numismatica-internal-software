#pragma once

#include <Arduino.h>
#include "HX711.h"
#include "config.h"

class WeightSensor {
public:
    WeightSensor();
    void begin();
    float getWeight();
    float getWeightWithSamples(uint8_t samples);
    void tare();
    void setScale(float scale);
    float getScale();
    bool isReady();

private:
    HX711 loadcell;
    float scale;
};
