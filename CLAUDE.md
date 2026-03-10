# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Sesame Robot is an open-source ESP32-based quadruped robot with an OLED face display. It uses 8 servo motors (2 per leg), a 128x64 OLED screen, WiFi connectivity, and a JSON REST API for control.

## Build & Flash (Arduino IDE)

No CLI build system — firmware is built and flashed via Arduino IDE 2.0+:

1. Open `firmware/sesame-firmware-main/sesame-firmware-main.ino`
2. **Board**: Tools > Board > ESP32 > ESP32 Devkit v1
3. **Upload Speed**: 921600, **Flash Size**: 4MB, **Partition**: Default 4MB with spiffs
4. Click Upload to compile and flash

**Required libraries** (Arduino Library Manager): `ESP32Servo`, `Adafruit SSD1306`, `Adafruit GFX Library`, `ElegantOTA`

**Serial Monitor**: 115200 baud for debugging

## Testing

No automated tests — all testing is manual on hardware:

```bash
# Test API after flashing (connect to WiFi "Sesame-Controller-BETA", password "12345678")
curl http://sesame-robot.local/api/status
curl -X POST http://sesame-robot.local/api/command \
  -H "Content-Type: application/json" \
  -d '{"command":"wave","face":"happy"}'
```

Web UI is accessible at `192.168.4.1` (captive portal) or `http://sesame-robot.local`.

## Sesame Studio (Python tool)

```bash
cd software/sesame-studio/
pip install Pillow
python sesame_studio.py
```

Visual pose/animation editor that generates code for `movement-sequences.h`.

## Architecture

All firmware lives in `firmware/sesame-firmware-main/` as a single Arduino sketch with companion headers:

- **`sesame-firmware-main.ino`** — Main entry point: `setup()` initializes GPIO/servos/I2C/WiFi/mDNS/web routes; `loop()` is non-blocking and calls `server.handleClient()`, `updateAnimatedFace()`, `processMovementCommand()`, touch sensor updates, and OTA handling every cycle.
- **`face-bitmaps.h`** — 25+ OLED faces defined as PROGMEM bitmaps, registered via the `FACE_LIST` macro. Animated faces have multiple frames with configurable FPS via `faceFpsEntries[]`.
- **`movement-sequences.h`** — Servo angle sequences for poses, walks, dances. Each sequence is a fixed-size array of `{servo_id, angle}` pairs.
- **`captive-portal.h`** — Full web UI (HTML/CSS/JS) served as a C string literal for the captive portal at 192.168.4.1.
- **`SoundEngine.cpp/.h`** — Non-blocking buzzer output for audio feedback on moves.

**WiFi**: Dual-mode AP + Station. The robot runs an AP (`Sesame-Controller-BETA`) while optionally connecting to home WiFi, with mDNS as `sesame-robot.local`.

**REST API endpoints**: `GET /api/status`, `POST /api/command`, `GET/POST /cmd?...` (legacy), `GET /getSettings`, `POST /setSettings`, `GET /update` (ElegantOTA).

## Code Style

- **Indentation**: 2 spaces, K&R braces, 120-char soft line limit
- **Naming**: `UPPER_SNAKE_CASE` constants, `camelCase` variables/functions, `PascalCase` types/structs/enums
- **Types**: Prefer `uint8_t`/`uint16_t` for hardware values; use `String` only for web responses, `char[]`/PROGMEM for static strings
- **Memory**: No dynamic allocation — fixed-size arrays only. Large constants in PROGMEM. Use `F()` macro for Serial strings.
- **Non-blocking**: Never use bare `delay()` in the main flow. Always process `server.handleClient()` and `dnsServer.processNextRequest()` inside any waiting loop.
- **Servo safety**: Stagger servo activations to prevent brownout.
- **Debug logging**: Use component tags like `[WIFI]`, `[mDNS]`, `[HOSTNAME]`.

## Adding Features

- **New movement command**: Add to `loop()` switch, add pose function in `movement-sequences.h`
- **New face**: Create 128x64 bitmap → convert with [image2cpp](https://javl.github.io/image2cpp/) → add to `FACE_LIST` in `face-bitmaps.h`
- **New API endpoint**: Register in `setup()` with `server.on("/endpoint", handler)`, implement handler

## Git Workflow

- **Branches**: `feature/description`, `fix/description`, or personal branches (e.g., `Adix-Seasame`)
- **Commits**: Always ask user for confirmation before committing. Start messages with a verb: "Add", "Fix", "Update", "Remove"
- **Push**: Auto-push to `origin` after user confirms commit
- **Force push / merge**: Never — leave to user
