#include "../include/weight_sensor.h"

WeightSensor::WeightSensor() 
    : scale(1.0f) {
}

void WeightSensor::begin() {
    loadcell.begin(Config::Pins::DOUT, Config::Pins::PD_SCK);
    loadcell.set_scale(scale);
    tare();
}

float WeightSensor::getWeight() {
    if (loadcell.wait_ready_timeout(1000)) {
        return loadcell.get_units(Config::Weight::DEFAULT_SAMPLES);
    }
    return 0.0f;
}

float WeightSensor::getWeightWithSamples(uint8_t samples) {
    if (loadcell.wait_ready_timeout(1000)) {
        return loadcell.get_units(samples);
    }
    return 0.0f;
}

void WeightSensor::tare() {
    if (loadcell.wait_ready_timeout(1000)) {
        loadcell.tare(Config::Weight::DEFAULT_SAMPLES);
    }
}

void WeightSensor::setScale(float newScale) {
    scale = newScale;
    loadcell.set_scale(scale);
}

float WeightSensor::getScale() {
    return scale;
}

bool WeightSensor::isReady() {
    return loadcell.is_ready();
}
