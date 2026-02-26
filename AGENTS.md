# AGENTS.md - Sesame Robot Firmware Development Guide

## Overview

This is the Sesame Robot project - an ESP32-based quadruped robot with an OLED face display. The firmware is written in C++ for Arduino/ESP32 framework.

## Project Structure

```
firmware/
└── sesame-firmware-main/
    ├── sesame-firmware-main.ino   # Main entry point (setup/loop)
    ├── face-bitmaps.h             # OLED face bitmap data
    ├── movement-sequences.h        # Pose and animation definitions
    └── captive-portal.h           # Web UI (HTML/CSS/JS)
```

## Build & Compile Commands

### Arduino IDE (Primary)
1. Open `sesame-firmware-main.ino` in Arduino IDE
2. Select board: **Tools > Board > ESP32 > LOLIN S2 Mini**
3. Configure port: **Tools > Port**
4. Click **Upload** (arrow icon) to compile and flash

### Build Settings
- **Board**: LOLIN S2 Mini (or ESP32 Dev Module for Distro Board)
- **Upload Speed**: 921600
- **Flash Size**: 4MB
- **Partition Scheme**: Default 4MB with spiffs

### Required Libraries (install via Arduino Library Manager)
- ESP32Servo
- Adafruit SSD1306
- Adafruit GFX Library
- ElegantOTA (for OTA updates)
- Preferences (built into ESP32 core)

### Serial Monitor
- **Baud Rate**: 115200
- Used for debugging and manual motor control

## Code Style Guidelines

### General Principles
- Keep code compatible with Arduino/ESP32 framework
- Use C++11 features where appropriate (enums, constexpr)
- Avoid dynamic memory allocation (use fixed-size arrays)
- Minimize blocking delays (use non-blocking patterns)

### File Organization

#### Main .ino File
- Configuration constants at top (pins, timeouts)
- Global state variables
- Function prototypes before implementations
- `setup()` - initialization
- `loop()` - main event loop

#### Header Files
- Use `#pragma once` for include guards
- Keep declarations in header, implementations in .ino or separate .cpp
- Group related functionality

### Naming Conventions

| Element | Convention | Example |
|---------|------------|---------|
| Constants | UPPER_SNAKE_CASE | `AP_PASS`, `MOTOR_COUNT` |
| Variables | camelCase | `currentCommand`, `lastFaceFrameMs` |
| Functions | camelCase | `setServoAngle()`, `handleWiFiScan()` |
| Enums | PascalCase | `FACE_ANIM_LOOP`, `ServoName` |
| Structs | PascalCase | `FaceEntry`, `FaceFpsEntry` |
| Servo IDs | UPPER with number | `R1`, `L2`, `R4` |

### Types

```cpp
// Prefer fixed-width types for hardware
uint8_t   // 0-255 (pin numbers, counters)
uint16_t  // 16-bit unsigned
int8_t    // signed 8-bit (subtrim values)
int16_t   // signed 16-bit

// Use String carefully - can cause heap fragmentation
String    // Arduino String class - OK for web responses
char[]    // Preferred for static strings in PROGMEM
```

### Includes Order

```cpp
// 1. Arduino/ESP32 core headers
#include <WiFi.h>
#include <WebServer.h>

// 2. Third-party libraries
#include <ESP32Servo.h>
#include <Adafruit_SSD1306.h>
#include <ElegantOTA.h>

// 3. Local headers
#include "face-bitmaps.h"
#include "movement-sequences.h"
#include "captive-portal.h"
```

### Formatting

- **Indentation**: 2 spaces (no tabs)
- **Line length**: Soft limit 120 characters
- **Braces**: K&R style
- **Spaces**: After keywords, around operators

```cpp
// Good
void handleCommandWeb() {
    if (server.hasArg("pose")) {
        currentCommand = server.arg("pose");
        recordInput();
    }
}

// Avoid
void handleCommandWeb(){
if(server.hasArg("pose")){
currentCommand=server.arg("pose");
}
}
```

### Error Handling

- Always check return values for display initialization
- Validate HTTP request parameters before processing
- Use serial logging for debugging
- Provide meaningful error messages in API responses

```cpp
// Example: Parameter validation
if (!server.hasArg("ssid") || !server.hasArg("pass")) {
    server.send(400, "text/plain", "Missing ssid or pass");
    return;
}

// Example: Initialization check
if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
    Serial.println(F("SSD1306 allocation failed."));
    while (1);  // Halt on critical failure
}
```

### Non-Blocking Patterns

The codebase uses non-blocking patterns for animations. Always handle client requests during delays:

```cpp
// Good: Process requests during delay
void delayWithFace(unsigned long ms) {
    unsigned long start = millis();
    while (millis() - start < ms) {
        updateAnimatedFace();
        server.handleClient();
        dnsServer.processNextRequest();
        delay(5);
    }
}

// Avoid: Blocking delay
delay(1000);  // This blocks all network requests
```

### Memory Management

- Store large constant data in PROGMEM (flash)
- Use `F()` macro for serial print strings
- Avoid String concatenation in loops

```cpp
// Good - stored in flash
Serial.println(F("This string stays in flash"));

// Good - bitmap arrays in PROGMEM
const unsigned char epd_bitmap_myface[] PROGMEM = { ... };

// Avoid - String concatenation in loop
for (int i = 0; i < 10; i++) {
    json += "data";  // Causes heap fragmentation
}
```

### Preferences (Persistent Storage)

Use Preferences for settings that persist across reboots:

```cpp
// Write
preferences.begin("my-app", false);
preferences.putString("ssid", wifiSSID);
preferences.end();

// Read
preferences.begin("my-app", true);
String ssid = preferences.getString("ssid", "default");
preferences.end();
```

## Git Workflow

### Branch Naming
- Features: `feature/description` (e.g., `feature/ota-wifi`)
- Bug fixes: `fix/description`
- Personal development: `Adix-Seasame` (as used in this project)

### Commit Messages
- Use clear, descriptive messages
- Start with verb: "Add", "Fix", "Update", "Remove"
- Keep subject line under 72 characters

```bash
git commit -m "Add OTA and WiFi manager features

- ElegantOTA for over-the-air firmware updates
- Preferences for persistent WiFi credentials storage
- WiFi scanning and connection from web UI"
```

### Build Artifacts
The `build/` directory should be ignored:

```gitignore
firmware/sesame-firmware-main/build/
*.ino.bin
*.ino.map
```

## Testing

### Manual Testing
1. Flash firmware to device
2. Test via Serial Monitor (115200 baud)
3. Test web UI in browser
4. Test API endpoints with curl

```bash
# Example API tests
curl http://sesame-robot.local/api/status
curl -X POST http://sesame-robot.local/api/command \
  -H "Content-Type: application/json" \
  -d '{"command":"wave","face":"happy"}'
```

### Debug Output
- Use meaningful Serial.println() messages
- Include component tags: `[WIFI]`, `[HOSTNAME]`, `[mDNS]`
- Log important state changes

## Adding New Features

### Adding New Commands
1. Add command handler in `loop()` switch statement
2. Add pose function in `movement-sequences.h`
3. Add face to `face-bitmaps.h` if needed

### Adding New API Endpoints
1. Register handler in `setup()`: `server.on("/endpoint", handler);`
2. Implement handler function
3. Add to API documentation in README

### Adding New Faces
1. Create 128x64 bitmap image
2. Convert using [image2cpp](https://javl.github.io/image2cpp/)
3. Add to `FACE_LIST` macro in `face-bitmaps.h`
4. Optionally add FPS entry in `faceFpsEntries` array

## Important Notes

- This is embedded firmware - test thoroughly on hardware
- The ESP32 has limited RAM (~320KB) - monitor memory usage
- Network operations must be non-blocking
- Servo movements should be staggered to prevent brownouts
- Always call `server.handleClient()` in loops
