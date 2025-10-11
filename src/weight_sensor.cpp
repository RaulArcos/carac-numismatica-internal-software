#include "../include/weight_sensor.h"

WeightSensor::WeightSensor() 
    : tareOffset(0) {
}

void WeightSensor::begin() {
    pinMode(Config::Pins::PD_SCK, OUTPUT);
    pinMode(Config::Pins::DOUT, INPUT);
    tare();
}

int32_t WeightSensor::getWeight() {
    return readRawValue() - tareOffset;
}

int32_t WeightSensor::getWeightWithSamples(uint8_t samples) {
    return readAverageValue(samples) - tareOffset;
}

void WeightSensor::tare() {
    tareOffset = readAverageValue(Config::Weight::DEFAULT_SAMPLES);
}

void WeightSensor::setTareValue(int32_t tareValue) {
    tareOffset = tareValue;
}

int32_t WeightSensor::getTareValue() {
    return tareOffset;
}

int32_t WeightSensor::readRawValue() {
    int32_t value = 0;
    
    while (digitalRead(Config::Pins::DOUT) == HIGH) {};
    delayMicroseconds(10);
    
    for (uint8_t i = 0; i < Config::Weight::HX711_BITS; i++) {
        digitalWrite(Config::Pins::PD_SCK, HIGH);
        delayMicroseconds(1);
        value = value << 1;
        digitalWrite(Config::Pins::PD_SCK, LOW);
        if (digitalRead(Config::Pins::DOUT)) {
            value++;
        }
        delayMicroseconds(1);
    }
    
    digitalWrite(Config::Pins::PD_SCK, HIGH);
    delayMicroseconds(1);
    digitalWrite(Config::Pins::PD_SCK, LOW);
    
    if (value & 0x800000) {
        value |= ~0xFFFFFF;
    }
    
    return value;
}

int32_t WeightSensor::readAverageValue(uint8_t samples) {
    int32_t total = 0;
    
    for (uint8_t i = 0; i < samples; i++) {
        total += readRawValue();
    }
    
    return total / samples;
}
