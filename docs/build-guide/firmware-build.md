# Firmware — Build & Flash

The firmware targets the Arduino Nano (ATmega328P). It is a plain CMake/OOP C++
source tree — **not** an `.ino` sketch: `src/Main.cpp` defines
`setup()`/`loop()` and headers are included root-relative with angle brackets
(e.g. `#include <Cards/Card.h>`). The canonical reference is
[`firmware/README.md`](https://github.com/d-rk/boomballon/blob/main/firmware/README.md);
this page summarizes it. If anything here disagrees with that README, the README
wins.

There are two build paths: the original arduino-cmake/MinGW flow the project was
developed with, and a portable arduino-cli flow.

## Canonical build (original): arduino-cmake + MinGW on Windows

This is the build the project was developed and flashed with.

**Prerequisites:** Arduino IDE/SDK, CMake, and MinGW (provides
`mingw32-make`).

**Steps:**

1. Configure the serial/upload port in **one** place — the
   `ARDUINO_SERIAL_PORT` / `ARDUINO_UPLOAD_PORT` cache variables in
   `firmware/CMakeLists.txt` (default `com5`). This is the single place the COM
   port needs to change.
2. Generate the build with CMake, pointing `CMAKE_TOOLCHAIN_FILE` at
   `cmake/ArduinoToolchain.cmake` (already set at the top of `CMakeLists.txt`).
3. Build and flash:
   ```bash
   mingw32-make arduino-upload
   ```
4. Open a serial monitor (PuTTY) attached to the board:
   ```bash
   mingw32-make attach
   ```
   Use this with `LOGGING_ENABLED` (below) to see runtime output.

## Cross-platform build (arduino-cli)

To build outside the Windows/MinGW flow, stage the sources under an empty
sketch's `src/` subfolder (arduino-cli compiles a sketch's `src/` tree
recursively, unlike sketch-root subfolders) and add `firmware/src` to the
include path:

```bash
mkdir -p /tmp/bb_build/bb_build/src
: > /tmp/bb_build/bb_build/bb_build.ino   # empty; sources provide setup()/loop()
cp -r firmware/src/* /tmp/bb_build/bb_build/src/
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old \
  --build-property "build.extra_flags=-std=c++11 -fno-threadsafe-statics -I{build.source.path}/src" \
  /tmp/bb_build/bb_build
```

To **flash** instead of just compiling, add `--upload -p <port>`:

```bash
arduino-cli compile --upload -p /dev/ttyUSB0 \
  --fqbn arduino:avr:nano:cpu=atmega328old \
  --build-property "build.extra_flags=-std=c++11 -fno-threadsafe-statics -I{build.source.path}/src" \
  /tmp/bb_build/bb_build
```

Use `-p /dev/ttyUSB0` on Linux or `-p COM5` on Windows. To watch runtime output,
open the serial monitor:

```bash
arduino-cli monitor -p /dev/ttyUSB0
```

!!! note "arduino-cli is a compile-check, not a replacement"
    The arduino-cli path was used to compile-verify the sources on a
    non-Windows host; it is a portable build/flash harness. The canonical build
    for day-to-day development remains the arduino-cmake + MinGW/Windows flow
    above.

## Build switches

Compile-time switches live in `src/Constants.h`:

- **`LOGGING_ENABLED`** — enables all serial `printf` logging. **Off by
  default**; when disabled it strips the logging code entirely (saves flash).
  Turn it on when using the serial monitor.
- **`AUTOSTART_GAME`** — auto-starts a 2-player game on boot, skipping the
  player-count detection flow. Handy for testing.
- **`ACTIVE_MODE()`** — selects the top-level mode:
    - **`MODE_GAME()`** — normal gameplay (default).
    - **`MODE_CODE_DETECTOR_CALIBRATION()`** — a
      [card-reader](modules/card-reader.md) bring-up harness that prints raw
      analog values. `<SPACE>` pauses output; `<TAB>` switches to changes-only
      output. Use this to pick card-reader thresholds.

## See also

- [Wiring & pin map](wiring-pinmap.md) — which pins the firmware drives.
- [Firmware Architecture](../reference/firmware-architecture.md) — how the code
  is organized.
- [`firmware/README.md`](https://github.com/d-rk/boomballon/blob/main/firmware/README.md)
  — the canonical build reference.
