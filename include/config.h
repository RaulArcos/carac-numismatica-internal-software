#pragma once

#include <Arduino.h>

namespace Config {
    namespace Pins {
        const uint8_t DOUT = 4;
        const uint8_t PD_SCK = 3;
        const uint8_t DRIVER_MOTOR_A1 = 8;
        const uint8_t DRIVER_MOTOR_A2 = 9;
        const uint8_t SERVO_LEFT = 5;
        const uint8_t SERVO_RIGHT = 11;
        const uint8_t LIMIT_SWITCH_1 = 2;
        const uint8_t LIMIT_SWITCH_2 = 12;
        const uint8_t LED_STRIP = 6;
    }

    namespace Communication {
        const uint32_t BAUD_RATE = 9600;
        const uint16_t BUFFER_SIZE = 512;
        const uint32_t PING_INTERVAL = 5000;
    }

    namespace LED {
        const uint8_t NUM_RING_PIXELS = 36;
        const uint8_t NUM_STRIP_PIXELS = 16;
        const uint8_t TOTAL_PIXELS = NUM_RING_PIXELS + NUM_STRIP_PIXELS;
        const uint8_t LEDS_PER_SECTOR = 9;
    }

    namespace Servo {
        const uint16_t MIN_PULSE_WIDTH = 650;
        const uint16_t MAX_PULSE_WIDTH = 2700;
        const uint16_t NEUTRAL_POSITION = 710;
        const uint16_t MAX_POSITION = 2500;
        const uint8_t STEP_SIZE = 10;
    }

    namespace Weight {
        const uint8_t DEFAULT_SAMPLES = 3;
        const uint8_t HX711_BITS = 24;
    }
}
