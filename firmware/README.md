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

Runtime output only appears when `LOGGING_ENABLED` is defined (see below).

## Build switches (`src/Constants.h`)

- `LOGGING_ENABLED` — enables all serial `printf` logging. Off by default;
  disabling it strips the logging code entirely (saves flash/RAM). Toggle it in
  `Constants.h`, or inject it for one build via the environment variable
  (note: `pio run -a "-D..."` does **not** work — `-a` is an avrdude argument,
  not a compiler flag, so the macro stays undefined):

  ```bash
  PLATFORMIO_BUILD_FLAGS="-D LOGGING_ENABLED" pio run -t upload -e nanoatmega328
  ```
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

  Avoid also enabling `LOGGING_ENABLED` alongside the mocks: the mocks narrate
  themselves, and on the ATmega328P the game's `printf` format strings sit in
  SRAM — with logging plus both mocks plus autostart, only ~144 bytes of RAM
  remain free at runtime and the nested card-apply + `printf` path overflows the
  stack and crashes.
- `ACTIVE_MODE()` — selects `MODE_GAME()` (normal gameplay) or
  `MODE_CODE_DETECTOR_CALIBRATION()` (a card-reader bring-up harness that
  prints raw analog values; `<SPACE>` pauses output, `<TAB>` switches to
  changes-only output).

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
