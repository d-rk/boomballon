# BoomBalloon Firmware

Firmware for the BoomBalloon device (Arduino Nano / ATmega328P).

## Canonical build (original): arduino-cmake + MinGW on Windows

This is the build the project was developed and flashed with.

Prerequisites:
- Arduino IDE/SDK installed
- CMake installed
- MinGW installed (provides `mingw32-make`)

Steps:
1. Configure the serial/upload port in one place: `CMakeLists.txt`, the
   `ARDUINO_SERIAL_PORT` / `ARDUINO_UPLOAD_PORT` cache variables (currently
   set to `com5`). This is the single place the COM port needs to change.
2. Generate the build with CMake, pointing `CMAKE_TOOLCHAIN_FILE` at
   `cmake/ArduinoToolchain.cmake` (already set at the top of
   `CMakeLists.txt`).
3. `mingw32-make arduino-upload` — builds and flashes the firmware to the
   board.
4. `mingw32-make attach` — opens a serial monitor (PuTTY) attached to the
   board for runtime output (used with `LOGGING_ENABLED`, see below).

## Cross-platform build (arduino-cli)

The project is a plain CMake/OOP source tree, not an `.ino` sketch —
`src/Main.cpp` defines `setup()`/`loop()` and headers are included
root-relative with angle brackets (e.g. `#include <Cards/Card.h>`). To build
it with `arduino-cli` outside the Windows/MinGW flow, stage the sources
under an empty sketch's `src/` subfolder (arduino-cli/arduino-builder
compiles sketch `src/` trees recursively, unlike sketch-root subfolders) and
add `firmware/src` to the include path:

```bash
mkdir -p /tmp/bb_build/bb_build/src
: > /tmp/bb_build/bb_build/bb_build.ino   # empty; sources provide setup()/loop()
cp -r firmware/src/* /tmp/bb_build/bb_build/src/
arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old \
  --build-property "build.extra_flags=-std=c++11 -fno-threadsafe-statics -I{build.source.path}/src" \
  /tmp/bb_build/bb_build
```

To flash instead of just compiling, add `--upload -p <port>` (e.g.
`-p /dev/ttyUSB0` on Linux, `-p COM5` on Windows).

## Build switches (`src/Constants.h`)

- `LOGGING_ENABLED` — enables all serial `printf` logging. Off by default;
  strips logging code entirely when disabled.
- `AUTOSTART_GAME` — auto-starts a 2-player game on boot, for testing
  without the player-count detection flow.
- `ACTIVE_MODE()` — selects `MODE_GAME()` (normal gameplay) or
  `MODE_CODE_DETECTOR_CALIBRATION()` (a card-reader bring-up harness that
  prints raw analog values; `<SPACE>` pauses output, `<TAB>` switches to
  changes-only output).

## Build verification notes

This move (`src/`, `cmake/`, `CMakeLists.txt`, `.vscode/` → `firmware/`) was
verified with `arduino-cli` 1.5.2-rc.1 + the `arduino:avr` core (1.8.8),
since the canonical MinGW/Windows toolchain isn't available in this
environment. `CMakeLists.txt`'s paths are all relative to
`CMAKE_CURRENT_SOURCE_DIR` (the include dir via
`get_filename_component(SRC_ABSOLUTE ${CMAKE_CURRENT_SOURCE_DIR}/src ABSOLUTE)`,
and both source/header globs via `file(GLOB_RECURSE ... RELATIVE
${CMAKE_CURRENT_SOURCE_DIR} ...)`), so the move does not break the CMake
build itself.

Two `arduino-cli` attempts were made to compile-verify the moved sources
directly (the canonical build was not re-run, since the MinGW toolchain
isn't available here):

1. **First attempt** — copied `firmware/src/*` flat into an arduino-cli
   sketch directory and compiled with `-I{build.source.path}`:
   ```bash
   mkdir -p /tmp/bb_build/bb_build
   : > /tmp/bb_build/bb_build/bb_build.ino
   cp -r firmware/src/* /tmp/bb_build/bb_build/
   arduino-cli compile --fqbn arduino:avr:nano:cpu=atmega328old \
     --build-property "build.extra_flags=-std=c++11 -fno-threadsafe-statics -I{build.source.path}" \
     /tmp/bb_build/bb_build
   ```
   All includes resolved and every `.cpp` compiled without error, but the
   **link** step failed: `arduino-cli`'s sketch model only compiles `.cpp`
   files that sit directly in the sketch root, not in subfolders. Files
   under `Cards/`, `Devices/`, `Dummy/`, `Helper/`, and `Tasks/` were never
   compiled, producing linker errors such as:
   ```
   PlayerChooser.cpp:52: undefined reference to `PiezoBuzzer::instance'
   Main.cpp:85: undefined reference to `CodeDetector::instance'
   Game.cpp:150: undefined reference to `Card::playCard(unsigned char)'
   collect2: error: ld returned 1 exit status
   ```

2. **Retry (per plan, one adjustment allowed)** — staged the sources under
   the sketch's `src/` subfolder instead of the sketch root, which
   arduino-builder treats as a recursively-compiled library-style tree, and
   pointed the include flag at it (`-I{build.source.path}/src`, the command
   shown above under "Cross-platform build"). This compiled and linked
   cleanly, with all 27 project `.cpp` files (Cards, Devices, Dummy,
   Helper, Tasks, Game, Main, Player, PlayerChooser) picked up:
   ```
   Sketch uses 16186 bytes (52%) of program storage space. Maximum is 30720 bytes.
   Global variables use 744 bytes (36%) of dynamic memory, leaving 1304 bytes for local variables. Maximum is 2048 bytes.
   ```

**Result: clean compile, no source changes required.** The move is verified
not to have broken the build. The canonical build for day-to-day
development and flashing remains the documented arduino-cmake + MinGW/Windows
flow above; the arduino-cli harness here is a portable compile-check, not a
replacement for it (it does not exercise the CMake/MinGW path itself).
