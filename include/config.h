#pragma once
#include <Arduino.h>

namespace Config {
    namespace Pins {
        constexpr uint8_t DOUT = 4;
        constexpr uint8_t PD_SCK = 16;
        constexpr uint8_t DRIVER_MOTOR_A1 = 22;
        constexpr uint8_t DRIVER_MOTOR_A2 = 17;
        constexpr uint8_t SERVO_LEFT = 18;
        constexpr uint8_t SERVO_RIGHT = 19;
        constexpr uint8_t LIMIT_SWITCH_1 = 21;
        constexpr uint8_t LIMIT_SWITCH_2 = 5;
        constexpr uint8_t LED_STRIP = 13;
        constexpr uint8_t LED_BACKLIGHT = 26;
        constexpr uint8_t LED_TEST = 2;
    }

    namespace Communication {
        constexpr uint32_t BAUD_RATE = 115200;
        constexpr uint16_t BUFFER_SIZE = 512;
        constexpr uint32_t HEARTBEAT_INTERVAL_MS = 5000;
        constexpr const char* FIRMWARE_VERSION = "1.0.0";
    }

    namespace LED {
        constexpr uint8_t NUM_RING_PIXELS = 0;
        constexpr uint8_t NUM_STRIP_PIXELS = 16;
        constexpr uint8_t TOTAL_PIXELS = NUM_RING_PIXELS + NUM_STRIP_PIXELS;
        constexpr uint8_t NUM_RINGS = 3;
        constexpr uint8_t NUM_SECTORS = TOTAL_PIXELS / NUM_RINGS;  
        constexpr uint8_t LEDS_PER_SECTOR = NUM_RINGS;
        constexpr uint8_t NUM_SECTORS_PER_RING = 4; 
        constexpr uint8_t LEDS_PER_RING = NUM_STRIP_PIXELS / NUM_RINGS; 
    }

    namespace Servo {
        constexpr uint16_t MIN_PULSE_US = 650;
        constexpr uint16_t MAX_PULSE_US = 2700;
        constexpr uint16_t NEUTRAL_POSITION = 710;
        constexpr uint16_t MAX_POSITION = 2500;
        constexpr uint8_t STEP_SIZE = 10;
    }

    namespace Weight {
        constexpr uint8_t DEFAULT_SAMPLES = 3;
        constexpr uint8_t HX711_BITS = 24;
        constexpr float SCALE_FACTOR = -345.0f;
        constexpr uint32_t READY_TIMEOUT_MS = 1000;
    }

    namespace Camera {
        constexpr uint16_t DEFAULT_TRIGGER_DURATION = 100;
    }

    namespace Motor {
        constexpr uint16_t DEFAULT_STEPS = 100;
        constexpr uint16_t POSITION_TIMEOUT_MS = 5000;
        constexpr uint16_t POSITION_POLL_INTERVAL_MS = 10;
        constexpr uint16_t SERVO_DELAY_MS = 10;
        constexpr uint16_t SERVO_FLIP_DELAY_MS = 100;
        constexpr uint16_t SERVO_SEQUENCE_DELAY_MS = 500;
        constexpr uint16_t SERVO_MIN_POSITION = 700;
        constexpr uint16_t SERVO_MAX_CALCULATED = 3210;
        constexpr uint16_t FLIP_EVENT_DURATION_MS = 200;
    }

    namespace System {
        constexpr uint32_t WEIGHT_READING_INTERVAL_MS = 1000;
        constexpr uint32_t COIN_SEQUENCE_WAIT_MS = 5000;
        constexpr uint8_t NUM_RING_CHANNELS = 4;
        constexpr uint8_t JSON_DOC_SIZE = 256;
        constexpr uint8_t MAX_INTENSITY = 255;
        constexpr uint8_t MIN_INTENSITY = 0;
        constexpr uint8_t PERCENTAGE_MAX = 100;
        constexpr uint8_t PERCENTAGE_MIN = 0;
    }

    namespace Timing {
        constexpr uint16_t SERIAL_WAIT_DELAY_MS = 10;
        constexpr uint16_t LIMIT_SWITCH_WAIT_DELAY_MS = 1000;
        constexpr uint16_t SEQUENCE_STEP_DELAY_MS = 100;
    }
}
