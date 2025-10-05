# ESP32 Numismatic Photography System - Internal Firmware

Professional ESP32 firmware for automated numismatic photography system with JSON-based communication protocol, motor control, LED lighting, and weight measurement capabilities.

## 🚀 Quick Start

1. **Install Required Libraries** (via Arduino Library Manager):
   - ArduinoJson (v6.x)
   - ESP32Servo
   - Adafruit_NeoPixel

2. **Configure Hardware**:
   - Review pin configuration in `include/config.h`
   - Adjust pins to match your hardware setup

3. **Upload Firmware**:
   - Open `carac-numismatica-internal-software.ino` in Arduino IDE
   - Select your ESP32 board
   - Upload at 115200 baud

4. **Test Communication**:
   - Open Serial Monitor at 115200 baud
   - Send: `{"type":"test_led_toggle","payload":{}}`
   - Built-in LED should toggle

## 📋 Features

✅ **JSON Communication Protocol** - Full implementation of standardized protocol  
✅ **Connection Monitoring** - Automatic heartbeat (5s) + message acknowledgments  
✅ **Lighting Control** - 4 independent LED ring channels (0-255 intensity)  
✅ **Motor Control** - DC positioning motor + servo-based coin flipper  
✅ **Camera Triggering** - Configurable duration camera shutter control  
✅ **Photo Sequences** - Automated multi-photo sessions with events  
✅ **Weight Sensing** - HX711-based weight measurement  
✅ **Event System** - Real-time progress and status updates  
✅ **Error Handling** - Standardized error codes and messages  
✅ **Non-blocking** - Efficient operation without delays  

## 🔌 Hardware Connections

| Function | Pin | Type |
|----------|-----|------|
| Weight Sensor DOUT | 4 | Input |
| Weight Sensor SCK | 3 | Output |
| DC Motor Driver A1 | 8 | Output |
| DC Motor Driver A2 | 9 | Output |
| Servo Left | 5 | PWM |
| Servo Right | 11 | PWM |
| Limit Switch 1 | 2 | INPUT_PULLUP |
| Limit Switch 2 | 12 | INPUT_PULLUP |
| LED Strip (NeoPixel) | 6 | Output |
| Camera Trigger | 7 | Output |
| Test LED | LED_BUILTIN | Output |

## 📡 Communication Protocol

### Message Format

All messages use JSON with `{type, payload}` structure:

```json
{
  "type": "message_type",
  "payload": { }
}
```

### Connection Monitoring

🔍 **Automatic Heartbeat**: ESP32 sends `event_heartbeat` every 5 seconds with uptime and status  
✉️ **Message Acknowledgments**: Every received message gets an immediate `response_ack` confirmation

```json
// Heartbeat (automatic every 5s)
{"type":"event_heartbeat","payload":{"uptime":305000,"status":"alive"}}

// Acknowledgment (sent for every received message)
{"type":"response_ack","payload":{"received_type":"lighting_set","timestamp":12345}}
```

### Supported Commands

| Command | Description |
|---------|-------------|
| `lighting_set` | Set LED ring intensity (ring_1 to ring_4, 0-255) |
| `motor_position` | Move positioning motor (forward/backward, steps) |
| `motor_flip` | Flip the coin using servos |
| `camera_trigger` | Trigger camera shutter (duration in ms) |
| `photo_sequence_start` | Start automated photo sequence |
| `system_ping` | Check connection (responds with "Pong") |
| `system_status` | Get full system status |
| `system_reset` | Reset all systems to default state |
| `system_emergency_stop` | Immediately stop all operations |
| `test_led_toggle` | Toggle built-in LED for testing |

### Example Commands

**Set Lighting:**
```json
{"type":"lighting_set","payload":{"channel":"ring_1","intensity":200}}
```

**Take Photo:**
```json
{"type":"camera_trigger","payload":{"duration":100}}
```

**Start Photo Sequence:**
```json
{"type":"photo_sequence_start","payload":{"count":10,"delay":2.0,"auto_flip":true}}
```

**Get Status:**
```json
{"type":"system_status","payload":{}}
```

### Response Types

- `response_success` - Command executed successfully
- `response_error` - Command failed with error code
- `response_status` - System status information
- `event_*` - Asynchronous events (sequence progress, motor complete, etc.)

## 📁 Project Structure

```
carac-numismatica-internal-software/
├── carac-numismatica-internal-software.ino  # Main Arduino sketch
├── PROTOCOL_IMPLEMENTATION.md               # Complete implementation guide
├── README.md                                # This file
├── include/
│   ├── config.h                            # Hardware & protocol configuration
│   ├── communication.h                     # Protocol message handling
│   ├── system_controller.h                 # Main system controller
│   ├── motor_controller.h                  # Motor control interface
│   ├── led_controller.h                    # LED control interface
│   └── weight_sensor.h                     # Weight sensor interface
└── src/
    ├── communication.cpp                   # Protocol implementation
    ├── system_controller.cpp               # Command routing & handlers
    ├── motor_controller.cpp                # Motor operations
    ├── led_controller.cpp                  # LED operations
    └── weight_sensor.cpp                   # Weight reading operations
```

## 🔧 Configuration

Edit `include/config.h` to customize:

- **Pin assignments** - Match your hardware
- **Baud rate** - Default 115200
- **LED counts** - Pixels per ring/strip
- **Servo limits** - Pulse width ranges
- **Timing** - Camera trigger duration, motor steps

## 🧪 Testing

### 1. Test Communication
```json
{"type":"test_led_toggle","payload":{}}
```
✓ Built-in LED toggles  
✓ Receives success response

### 2. Test Lighting
```json
{"type":"lighting_set","payload":{"channel":"ring_1","intensity":128}}
```
✓ Ring 1 lights at 50% brightness

### 3. Test Motor
```json
{"type":"motor_position","payload":{"direction":"forward","steps":100}}
```
✓ DC motor moves forward  
✓ Receives motor_complete event

### 4. Test Camera
```json
{"type":"camera_trigger","payload":{"duration":100}}
```
✓ Camera trigger pin pulses  
✓ Receives camera_triggered event

### 5. Test Photo Sequence
```json
{"type":"photo_sequence_start","payload":{"count":3,"delay":2.0,"auto_flip":false}}
```
✓ Takes 3 photos with 2s intervals  
✓ Receives sequence events (started, progress, completed)

## 🔗 Integration

This firmware is designed to work with the **Caracas Numismatic Photography Software** (Python/PySide6). The Python application:

- Auto-detects ESP32 on serial port
- Sends commands via JSON protocol
- Receives responses and events
- Provides GUI for all operations

No additional configuration needed - just upload firmware and run the Python app!

## 📚 Documentation

- **[PROTOCOL_IMPLEMENTATION.md](PROTOCOL_IMPLEMENTATION.md)** - Complete implementation guide with examples, troubleshooting, and hardware details
- **COMMUNICATION_PROTOCOL.md** - Full protocol specification (in Python app repo)
- **PROTOCOL_QUICK_REFERENCE.md** - Quick command reference (in Python app repo)

## 🐛 Troubleshooting

### No response from ESP32
- Check baud rate (115200)
- Verify correct COM port
- Ensure ArduinoJson installed
- Test with ping command

### Parse errors
- Validate JSON format
- Ensure newline terminator
- Check buffer size (512 bytes)

### LEDs not working
- Verify pin 6 connection
- Check NeoPixel power supply
- Test with lighting_set command

### Motor not moving
- Check motor driver power
- Verify pin connections
- Test limit switches

### Camera not triggering
- Verify pin 7 connection
- Check relay/optocoupler wiring
- Test with multimeter

## 🎯 Performance

- **Response time**: < 10ms typical
- **Buffer size**: 512 bytes
- **Non-blocking**: Photo sequences run asynchronously
- **Event rate**: Real-time progress updates
- **Memory**: Static allocation, no fragmentation

## 📄 License

Part of the Caracas Numismatic Photography System

## 🔄 Version

**Firmware v1.0.0** (2025-10-05)
- Initial protocol implementation
- All commands supported
- Event system complete
- Hardware integration complete

---

**Ready to use!** Upload the firmware and start communicating via JSON protocol at 115200 baud.
