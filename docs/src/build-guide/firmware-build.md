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
pio run                       # builds all environments
pio run -e nanoatmega328      # just the Nano (the current board)
```

The compiled image is written to `.pio/build/<env>/firmware.hex`.

## Flash

Pick the environment that matches your board:

```bash
pio run -t upload -e nanoatmega328      # Arduino Nano, old bootloader (57600 baud)
pio run -t upload -e micro              # Arduino Micro (early-prototype board)
```

The project's Nanos are older clones with the old bootloader. If an upload
times out on a newer board, its bootloader expects 115200 baud — switch the
environment to `board = nanoatmega328new` in `platformio.ini`.

The `micro` environment targets the Arduino Micro (ATmega32U4) that ran the
2015–2018 prototypes (see
[the story](../story.md#the-controller-from-micro-to-nano)). The pin map is
unchanged since then, so a Micro still runs the current firmware.

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
  flash/RAM). Turn it on for the serial monitor by uncommenting the `#define`
  in `Constants.h`, or inject it for one build without editing the file:

  ```bash
  PLATFORMIO_BUILD_FLAGS="-D LOGGING_ENABLED" pio run -t upload -e nanoatmega328
  ```

- **`AUTOSTART_GAME`** — auto-starts a 2-player game on boot, skipping the
  player-count detection flow. Handy for testing.
- **`MOCKED_DEVICES`** — runs the game with **no hardware attached** by wiring in
  the serial mocks from `src/Mock/` in place of the real card reader and
  pump/valve. Type card codes into the serial console; the balloon volume prints
  back as a percentage and an ASCII bar. Pair it with `AUTOSTART_GAME` to boot
  straight into a game:

  ```bash
  PLATFORMIO_BUILD_FLAGS="-D MOCKED_DEVICES -D AUTOSTART_GAME" pio run -t upload -e nanoatmega328
  ```

    !!! warning "Don't combine `MOCKED_DEVICES` with `LOGGING_ENABLED`"
        The mock narrates itself (it always prints its prompts and the volume
        bar), so it does not need `LOGGING_ENABLED`. Enabling both at once is a
        RAM footgun on the ATmega328P: the game's `printf` format strings live
        in SRAM, and the extra pressure leaves only ~144 bytes free at runtime,
        so the nested card-apply + `printf` path overflows the stack into the
        heap and the firmware crashes (garbage on the serial line). Use
        `MOCKED_DEVICES` on its own.
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
