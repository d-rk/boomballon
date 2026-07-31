# Firmware — Build & Flash

The firmware targets the Arduino Nano (ATmega328P). It is a small OOP C++
source tree under `firmware/src/` (`Main.cpp` provides `setup()`/`loop()`;
headers are included root-relative, e.g. `#include <Cards/Card.h>`). The build
system is [PlatformIO](https://platformio.org/), which pins the toolchain and
Arduino core so a clean checkout builds identically everywhere. The canonical
reference is
[`firmware/README.md`](https://github.com/d-rk/boomballon/blob/main/firmware/README.md);
this page summarizes it. If anything here disagrees with that README, the README
wins.

## Prerequisites

Install PlatformIO Core:

```bash
pip install platformio        # or: pipx install platformio
```

(Or the PlatformIO IDE extension for VS Code — it uses the same
`platformio.ini`.) The first build downloads the `atmelavr` platform and AVR
toolchain automatically.

## Build

```bash
cd firmware
pio run                       # builds both environments
pio run -e nanoatmega328      # just the old-bootloader Nano
```

The compiled image is written to `.pio/build/<env>/firmware.hex`.

## Flash

Two environments cover the two common Nano bootloaders — pick the one that
matches your board:

```bash
pio run -t upload -e nanoatmega328      # original Nano, old bootloader (57600 baud)
pio run -t upload -e nanoatmega328new   # newer clones (115200 baud)
```

PlatformIO auto-detects the serial port; force it with
`--upload-port /dev/ttyUSB0` (Linux CH340 clone), `/dev/ttyACM0` (genuine), or
`COMx` (Windows).

## Serial monitor

```bash
pio device monitor            # 9600 baud, matching Serial.begin(9600)
```

Runtime output only appears when `LOGGING_ENABLED` is defined (below).

## Build switches

Compile-time switches live in `src/Constants.h`:

- **`LOGGING_ENABLED`** — enables all serial `printf` logging. **Off by
  default**; when disabled it strips the logging code entirely (saves
  flash/RAM). Turn it on when using the serial monitor, or build with
  `pio run -a "-DLOGGING_ENABLED"`.
- **`AUTOSTART_GAME`** — auto-starts a 2-player game on boot, skipping the
  player-count detection flow. Handy for testing.
- **`ACTIVE_MODE()`** — selects the top-level mode:
    - **`MODE_GAME()`** — normal gameplay (default).
    - **`MODE_CODE_DETECTOR_CALIBRATION()`** — a
      [card-reader](modules/card-reader.md) bring-up harness that prints raw
      analog values. `<SPACE>` pauses output; `<TAB>` switches to changes-only
      output. Use this to pick card-reader thresholds.

## Verified build

Both environments compile with no source changes — a 16186-byte image (52.7 %
of flash), 744 bytes of RAM. A GitHub Actions workflow
(`.github/workflows/build-firmware.yml`) runs `pio run` on every push, so the
firmware is guaranteed to keep building.

!!! note "Legacy build"
    The firmware was originally built with **arduino-cmake** (a CMake toolchain)
    under MinGW on Windows, flashing via `avrdude` wrapped in `.bat` scripts.
    That ~2013 toolchain is incompatible with modern CMake, so the project
    migrated to PlatformIO and removed the old build files (still in git
    history).

## See also

- [Wiring & pin map](wiring-pinmap.md) — which pins the firmware drives.
- [Firmware Architecture](../reference/firmware-architecture.md) — how the code
  is organized.
- [`firmware/README.md`](https://github.com/d-rk/boomballon/blob/main/firmware/README.md)
  — the canonical build reference.
