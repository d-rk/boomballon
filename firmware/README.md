# Boom Balloon Firmware

Firmware for the Boom Balloon device — **Arduino Nano (ATmega328P)**, C++11 on
the Arduino AVR core. The sources are a small OOP tree under `src/`
(`Main.cpp` provides `setup()`/`loop()`); see the
[Firmware Architecture](../docs/reference/firmware-architecture.md) doc for how
it fits together.

The build system is [PlatformIO](https://platformio.org/). It pins the
toolchain and Arduino core, so a clean checkout builds identically on Linux,
macOS, and Windows.

## Prerequisites

Install PlatformIO Core (CLI):

```bash
pip install platformio        # or: pipx install platformio
```

(Or use the PlatformIO IDE extension for VS Code — it drives the same
`platformio.ini`.) The first build downloads the `atmelavr` platform and the
AVR toolchain automatically.

## Build

```bash
cd firmware
pio run                       # builds all environments
pio run -e nanoatmega328      # just the Nano (the current board)
```

The compiled image lands at `.pio/build/<env>/firmware.hex`.

## Flash

Pick the environment that matches your board:

```bash
pio run -t upload -e nanoatmega328      # Arduino Nano, old bootloader (57600 baud)
pio run -t upload -e micro              # Arduino Micro (early-prototype board)
```

The project's Nanos are older clones with the old bootloader. If an upload
times out on a newer board, its bootloader expects 115200 baud — switch the
environment to `board = nanoatmega328new` in `platformio.ini`.

PlatformIO auto-detects the serial port; pass `--upload-port /dev/ttyUSB0`
(Linux CH340 clone), `/dev/ttyACM0` (genuine), or `COMx` (Windows) to force it.

## Serial monitor

```bash
pio device monitor            # 9600 baud, matching Serial.begin(9600)
```

Serial `printf` logging is always on. The format strings are kept in flash via
a `printf` → `printf_P(PSTR(...))` macro in `Constants.h`, so on the ATmega328P
logging costs almost no RAM (~16 bytes) and needs no build switch.

## Build switches (`src/Constants.h`)

The switches below are off by default. Enable one in `Constants.h`, or inject
it for a single build via the environment variable (note: `pio run -a "-D..."`
does **not** work — `-a` is an avrdude argument, not a compiler flag, so the
macro stays undefined):

- `AUTOSTART_GAME` — auto-starts a 2-player game on boot, for testing without
  the player-count detection flow.
- `MOCK_CODE_DETECTOR` / `MOCK_OUTPUT_DEVICE` — wire in the serial mocks from
  `src/Mock/` in place of the real devices, so the game can run with missing
  hardware. Each is independent, so you can mock one and keep the other real:
    - `MOCK_CODE_DETECTOR` — read card codes from the serial console (type a
      code + `<enter>`) instead of the photoresistor card reader.
    - `MOCK_OUTPUT_DEVICE` — print the balloon volume to serial (percentage +
      ASCII bar) instead of driving the pump/valve.

  Enable either or both. Pair them with `AUTOSTART_GAME` to boot straight into a
  game with no hardware at all:

  ```bash
  PLATFORMIO_BUILD_FLAGS="-D MOCK_CODE_DETECTOR -D MOCK_OUTPUT_DEVICE -D AUTOSTART_GAME" pio run -t upload -e nanoatmega328
  ```
- `DETECTOR_CALIBRATION` — replaces the game loop with a card-reader bring-up
  harness that prints raw photoresistor values (`<SPACE>` pauses output, `<TAB>`
  switches to changes-only output). Off by default (normal gameplay); build with
  `-D DETECTOR_CALIBRATION` to enable.

## Build verification

Both environments compile cleanly with no source changes:

```
nanoatmega328  SUCCESS
micro          SUCCESS
RAM:   [====      ]  36.3% (744 bytes / 2048)
Flash: [=====     ]  52.7% (16186 bytes / 30720)
```

CI (`.github/workflows/build-firmware.yml`) runs `pio run` on every push, so
the firmware is guaranteed to keep compiling.

## History

The early prototypes (2015–2018) ran on an **Arduino Micro** (ATmega32U4); the
build switched to the Nano in 2019. The pin assignments in `Main.cpp` predate
the switch and work on both boards, so the `micro` environment still targets
the original prototype hardware unchanged.

The firmware was originally built with **arduino-cmake** (a CMake toolchain)
driven by MinGW on Windows, with `avrdude` upload wrapped in `.bat` scripts.
That toolchain is from ~2013 and is incompatible with modern CMake (CMake 4
removed the pre-3.5 compatibility it relied on), so the project migrated to
PlatformIO. The old build files were removed; they remain in the git history
if you need them.
