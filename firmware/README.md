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
pio run                       # builds both environments
pio run -e nanoatmega328      # just the old-bootloader Nano
```

The compiled image lands at `.pio/build/<env>/firmware.hex`.

## Flash

Pick the environment that matches your board's bootloader:

```bash
pio run -t upload -e nanoatmega328      # original Nano, old bootloader (57600 baud)
pio run -t upload -e nanoatmega328new   # newer Nano clones (115200 baud)
```

PlatformIO auto-detects the serial port; pass `--upload-port /dev/ttyUSB0`
(Linux CH340 clone), `/dev/ttyACM0` (genuine), or `COMx` (Windows) to force it.

## Serial monitor

```bash
pio device monitor            # 9600 baud, matching Serial.begin(9600)
```

Runtime output only appears when `LOGGING_ENABLED` is defined (see below).

## Build switches (`src/Constants.h`)

- `LOGGING_ENABLED` — enables all serial `printf` logging. Off by default;
  disabling it strips the logging code entirely (saves flash/RAM). Toggle it
  in `Constants.h`, or build with `pio run -e nanoatmega328 -a "-DLOGGING_ENABLED"`.
- `AUTOSTART_GAME` — auto-starts a 2-player game on boot, for testing without
  the player-count detection flow.
- `ACTIVE_MODE()` — selects `MODE_GAME()` (normal gameplay) or
  `MODE_CODE_DETECTOR_CALIBRATION()` (a card-reader bring-up harness that
  prints raw analog values; `<SPACE>` pauses output, `<TAB>` switches to
  changes-only output).

## Build verification

Both environments compile cleanly with no source changes:

```
nanoatmega328     SUCCESS
nanoatmega328new  SUCCESS
RAM:   [====      ]  36.3% (744 bytes / 2048)
Flash: [=====     ]  52.7% (16186 bytes / 30720)
```

CI (`.github/workflows/build-firmware.yml`) runs `pio run` on every push, so
the firmware is guaranteed to keep compiling.

## History

The firmware was originally built with **arduino-cmake** (a CMake toolchain)
driven by MinGW on Windows, with `avrdude` upload wrapped in `.bat` scripts.
That toolchain is from ~2013 and is incompatible with modern CMake (CMake 4
removed the pre-3.5 compatibility it relied on), so the project migrated to
PlatformIO. The old build files were removed; they remain in the git history
if you need them.
