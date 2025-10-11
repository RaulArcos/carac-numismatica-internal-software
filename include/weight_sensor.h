#pragma once

#include <Arduino.h>
#include "config.h"

class WeightSensor {
public:
    WeightSensor();
    void begin();
    int32_t getWeight();
    int32_t getWeightWithSamples(uint8_t samples);
    void tare();
    void setTareValue(int32_t tareValue);
    int32_t getTareValue();

private:
    int32_t tareOffset;
    
    int32_t readRawValue();
    int32_t readAverageValue(uint8_t samples);
};
