#pragma once
#include <Arduino.h>

namespace Config {
    namespace Pins {
        constexpr uint8_t DOUT = 4;     
        constexpr uint8_t PD_SCK = 16;  
        constexpr uint8_t DRIVER_MOTOR_A1 = 22;
        constexpr uint8_t DRIVER_MOTOR_A2 = 17;
        constexpr uint8_t SERVO_LEFT  = 18;
        constexpr uint8_t SERVO_RIGHT = 19;
        constexpr uint8_t LIMIT_SWITCH_1 = 21;
        constexpr uint8_t LIMIT_SWITCH_2 = 5;
        constexpr uint8_t LED_STRIP = 13;   
        constexpr uint8_t LED_TEST  = 2;
        constexpr uint8_t LED_BACKLIGHT = 12;    
    }

    namespace Communication {
        constexpr uint32_t BAUD_RATE = 115200;
        constexpr uint16_t BUFFER_SIZE = 512;
        constexpr uint32_t PING_INTERVAL = 5000;
        constexpr const char* FIRMWARE_VERSION = "1.0.0";
    }

    namespace LED {
        constexpr uint8_t NUM_RING_PIXELS = 0;  
        constexpr uint8_t NUM_STRIP_PIXELS = 16; 
        constexpr uint8_t TOTAL_PIXELS = NUM_RING_PIXELS + NUM_STRIP_PIXELS;
        constexpr uint8_t NUM_RINGS = 4;
        constexpr uint8_t NUM_SECTORS_PER_RING = 4;
        constexpr uint8_t LEDS_PER_RING = NUM_STRIP_PIXELS / NUM_RINGS;
        constexpr uint8_t LEDS_PER_SECTOR = LEDS_PER_RING / NUM_SECTORS_PER_RING;
    }

    namespace Servo {
        constexpr uint16_t MIN_PULSE_US = 650;
        constexpr uint16_t MAX_PULSE_US = 2700;
        constexpr uint16_t NEUTRAL_POSITION = 710;
        constexpr uint16_t MAX_POSITION = 2500;
        constexpr uint8_t  STEP_SIZE = 10;
    }

    namespace Weight {
        constexpr uint8_t  DEFAULT_SAMPLES = 3;
        constexpr uint8_t  HX711_BITS = 24;
        constexpr float    SCALE_FACTOR = -345.0f;
    }

    namespace Camera {
        constexpr uint16_t DEFAULT_TRIGGER_DURATION = 100;
    }

    namespace Motor {
        constexpr uint16_t DEFAULT_STEPS = 100;
    }
}
