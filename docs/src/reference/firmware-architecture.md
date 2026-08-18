# Firmware Architecture

This page describes how the firmware source is organised, how the program starts,
and how the pieces cooperate at run
time. For how to
compile and flash it, see [Firmware](../build-guide/firmware.md);
for the card encoding it depends on, see the
[Optical Code System](optical-codes.md).

The firmware is **plain object-oriented C++11**. It targets
the **Arduino Nano (ATmega328P)** — a small 8-bit AVR with 2 KB of RAM — which
shapes many of the design choices below (no STL, manual memory accounting, no
threads).

## Source layout

The source code for the firmware lives under `firmware/src/`:

| Area | Files | Role |
|---|---|---|
| Entry point | `Main.cpp` | Arduino `setup()` / `loop()`; wires everything together |
| Game logic | `Game.*`, `Player.*`, `PlayerChooser.*` | Turn flow, player ring, startup player-count selection |
| Cards | `Cards/Card.*` + subclasses | Card behaviours and the code→card factory |
| Devices | `Devices/CodeDetector.*`, `OutputDevice.*`, `PiezoBuzzer.*`, `SevenSegmentDisplay.*` | Hardware drivers |
| Mock devices | `Mock/CodeDetectorMock.*`, `Mock/OutputDeviceMock.*` | Serial stand-ins for the card reader and pump/valve, compiled in with `-D MOCK_CODE_DETECTOR` / `-D MOCK_OUTPUT_DEVICE` |
| Scheduling | `Tasks/Task.h`, `Tasks/TaskScheduler.*` | Cooperative multitasking |
| Helpers | `Helper/Vector.h`, `Helper/Diagnostic.h`, `Helper/Log.*` | STL-free container, RAM probe, `printf` logging |
| Config | `Constants.h` | Pins, codes, display glyphs, note frequencies, the flash-string `printf` macro, build switches (`AUTOSTART_GAME`, `MOCK_CODE_DETECTOR`, `MOCK_OUTPUT_DEVICE`, `DETECTOR_CALIBRATION`) |

The `Mock/` subtree lets the game run with **missing hardware**: build with
`-D MOCK_CODE_DETECTOR` and/or `-D MOCK_OUTPUT_DEVICE` and `Main.cpp` swaps that
device for a serial-driven stand-in (type codes in, watch the balloon volume
print out). Each is independent. See [Mock devices](#mock-devices) below for
how it works.

There is also a `Devices/` trio that is **not part of the running game**:
`Button`, `Led` and `SoundDetector` are now unused general-purpose drivers
 used during development. `SoundDetector` was used in experiment to detect the
 actual *pop* of the balloon and react to it, instead of the modelled
volume described in [How It Works](../how-it-works.md#the-balloon-volume-model).
It was discarded for the final game.

## Startup and object ownership

Most hardware drivers are **singletons**. Each exposes a public
`static X* instance` pointer, which `setup()` populates with a single `new` at
boot:

```cpp
// Main.cpp (excerpt)
SevenSegmentDisplay::instance = new SevenSegmentDisplay(PIN_9, PIN_10, PIN_8);
TaskScheduler::instance        = new TaskScheduler();
OutputDevice::instance         = new OutputDevice(PIN_6, PIN_7);
CodeDetector::instance         = new CodeDetector(PIN_A0, PIN_A1, PIN_A2, PIN_A3, PIN_A4);
PiezoBuzzer::instance          = new PiezoBuzzer(PIN_11);
PlayerChooser::instance        = new PlayerChooser(CODE_15, CODE_17, CODE_19, CODE_21, CODE_23);
```

Any part of the code can then reach a device through, for example,
`OutputDevice::instance->apply(...)` without passing pointers around. Because
these objects are created once and live for the whole program, there is no
teardown to worry about. The one non-singleton top-level object is the game
itself, declared as a **file-scope global** in `Main.cpp`:

```cpp
Game game;
```

`setup()` also seeds the random-number generator from the floating analog pin
`A5` (`randomSeed(analogRead(PIN_A5))`) — which is why the
[wiring](../build-guide/wiring.md) insists that `A5` stay unconnected.

The pin numbers, code values, and display glyphs all come from `Constants.h`,
which also carries four compile-time switches: `DETECTOR_CALIBRATION` and
`AUTOSTART_GAME`, described under [Build modes](#build-modes-and-logging), and
`MOCK_CODE_DETECTOR`/`MOCK_OUTPUT_DEVICE`, described under
[Mock devices](#mock-devices).

## Cooperative multitasking

The AVR has no operating system and the firmware uses no interrupt-driven
scheduler. Instead there is a tiny **cooperative** scheduler in
`Tasks/TaskScheduler`. The contract is a one-method interface:

```cpp
class Task {
    virtual bool applyChange(unsigned long timeMs) = 0;  // returns "still busy?"
};
```

A task advances its own animation by looking at the current time and returns
`true` while it still has work to do. Both `SevenSegmentDisplay` and
`PiezoBuzzer` are `Task`s, so a melody and a display animation are each just a
task that the scheduler ticks. The scheduler drives them on a fixed **20 ms**
cadence (`TaskScheduler::SLEEP_MS = 20`) and offers two ways to run:

- **`loop()` — blocking.** Ticks every registered task every 20 ms and keeps
  going until *all* of them report done, then clears the task list. Used when
  the game genuinely wants to wait for a jingle/animation to finish (e.g. the
  turn-change fanfare).
- **`iterate()` — non-blocking.** Ticks every task exactly once and returns
  immediately (clearing the list only once everything is done). Used when other
  work must continue in parallel.

The payoff shows up during **balloon inflation**. `OutputDevice::applyIntensities()`
runs a `delay(5 ms)` loop while it pulses the pump/valve, and inside that loop
it calls `TaskScheduler::instance->iterate()` on every step. So the buzzer tone
and the seven-segment "fill" animation keep advancing *concurrently* while the
balloon is being inflated or deflated, even though everything is single-threaded
and there are no interrupts.

## Game state and turn flow

### The player ring

Players are modelled as a **circular doubly-linked list** of `Player` objects
(`prevPlayer` / `nextPlayer`). The list is built by a *recursive* constructor:
`new Player(numPlayers)` creates the whole chain in one call and stitches the
ends together so that following `nextPlayer` forever cycles through the table.
Each `Player` owns its currently-attached cards in a `Vector<Card*>`.

Turn order is just "advance to `nextPlayer`". The
[Change of course](../gameplay/card-deck.md) cards work by literally swapping
every player's `prevPlayer`/`nextPlayer` pointers (`Player::changeDirection()`),
reversing the direction of play.

### Two-phase card insertion

`Game::loop()` reads the card reader once per iteration and drives a small state
machine around a single `currentCard` pointer:

1. **No card pending** (`currentCard == 0`): the detected code is handed to the
   `Card::playCard(code)` factory.
      - If it returns a card that **does not** need a target
        (`needsPlayerSelection() == false`), the card is applied to the *current*
        player immediately.
      - If it **does** need a target (the *Blocking*, *Devil's messenger*, and
        *Apocalypse* cards override `needsPlayerSelection()` to `true`), the card
        is held and the machine waits for a second card.
      - If the factory returns `nullptr`, the code is not a valid play card and
        the player gets an error beep.
2. **Card pending:** the next card must be a **player card**
   (`PlayerChooser::isPlayerCode`). If it is, the held card is attached to that
   target player; otherwise it is an error.

After a card is applied the machine sets `waitCardRemoved`, and the turn only
advances once the slot reads `CODE_ALL` (the "card removed" sentinel — see the
[optical codes](optical-codes.md#sentinel-codes) page). This is what forces the
"insert, then pull the card back out" rhythm of play described in the
[Rules](../gameplay/rules.md).

### Losing

There is exactly one loss condition, checked every `Game::loop()`:

```cpp
if (OutputDevice::instance->volume > 100.0f) { /* current player loses */ }
```

`volume` is the **modelled** balloon fill (a software time-integral, not a
sensor reading — see [How It Works](../how-it-works.md#the-balloon-volume-model)).
When it exceeds 100 %, the current player loses, the device runs a reset/deflate
sequence, and the game waits for a card to be removed before it can be
restarted.

## The card model

`Card` is an abstract base class. Concrete behaviours subclass it and override
`play()`:

- **`VolumeCard`** is the workhorse: apply a percentage volume change at a given
  intensity, then discard itself. Most "pressure" cards are just `VolumeCard`
  with different numbers.
- Subclasses add behaviour: `ChangeDirectionCard` (volume change **and** reverse
  the player ring), `AngelCard` / `DevilCard` (delayed relief / malicious
  inflate), `FiftyFiftyCard` (random ±50 %), `UpDownCard` (oscillate),
  `MissRoundCard`, `SuddenDeathCard`, and `PushToLimitCard` (inflate to 99 %).

The mapping from optical code to card lives in a single static factory,
`Card::playCard(code)`, which is a plain `switch` — the authoritative,
code-verified rendering of that mapping is the deck catalog in
[The Card Deck](../gameplay/card-deck.md). A card marks itself `discard = true`
when its effect is spent, and `Player::loop()` sweeps discarded cards out of the
player's vector.

## Helpers and diagnostics

Because the AVR toolchain here ships no STL, the firmware carries a few
hand-rolled utilities:

- **`Vector<T>` (`Helper/Vector.h`)** — a minimal `std::vector`-like dynamic
  array (`push_back`, iterators, copy-assign, grow-by-5 reserve). Used for the
  per-player card list and the scheduler's task list.
- **`freeRam()` (`Helper/Diagnostic.h`)** — reports free SRAM by measuring the
  gap between the heap break and the stack. Handy on a 2 KB part; it is printed
  on each turn change.
- **`Log` (`Helper/Log.cpp`)** — redirects C `printf` to the Arduino `Serial`
  port, so the rest of the code can log with ordinary `printf(...)` format
  strings. Logging is always on; a `printf` → `printf_P(PSTR(...))` macro in
  `Constants.h` keeps the format strings in flash so it costs almost no RAM.

## Build modes and logging

`Constants.h` exposes several compile-time knobs that change what gets built:

- **`DETECTOR_CALIBRATION`** replaces the game `loop()` with a bring-up harness
  that just prints raw analog readings from the card reader, used to tune the
  reader thresholds. Off by default (the normal game). See the
  [Optical Code System](optical-codes.md#calibration-harness).

Serial `printf` logging is always compiled in — the format strings live in
flash (`printf_P`/`PSTR`), so it costs almost no RAM.

There is also an `AUTOSTART_GAME` switch that skips the player-count selection
and jumps straight into a 2-player game — a convenience for testing.

## Mock devices

The `Mock/` subtree is a **desk-testing path**: run the game with missing
hardware. Each device is mocked independently by its own build flag, so you can
stand in for one and keep the other real, or mock both:

- **`CodeDetectorMock`** (build with `-D MOCK_CODE_DETECTOR`) replaces
  `CodeDetector`. Instead of reading the five photoresistors, it reads a card
  code you type into the serial console — a number followed by `<enter>`
  "inserts" that card, an empty line "pulls it out" again. It subclasses
  `CodeDetector` and overrides `readCode()`.
- **`OutputDeviceMock`** (build with `-D MOCK_OUTPUT_DEVICE`) replaces
  `OutputDevice`. Instead of driving the 12 V pump and valve, it prints the
  balloon volume to serial after each inflate or deflate, as a percentage and a
  little ASCII bar. It keeps the real volume model (fill/deflate timing) and
  only swaps out the physical pin writes.

Either flag can be set on the command line or uncommented in `Constants.h`.

To make the output mock possible without duplicating the volume model,
`OutputDevice` exposes three protected virtual hooks — `writeMotor()`,
`writeValve()`, and `afterApply()` — that the real device implements as pin
writes and a no-op, and the mock overrides to suppress hardware and report
volume.

Because the mocks talk over serial, a build with either mock also gets a 2.5 s
startup delay so the serial link settles before the opening prompt prints
(`printf` logging is always on). Combine them with `-D AUTOSTART_GAME` to boot
straight into a 2-player game and watch the balloon fill on the console.

## The unbuilt mode system

There was a design for a **game-mode selector** built around the **Yin Yang**
card (optical code **27**) that was specced but never implemented. The idea,
captured in the design note *Anforderungen Moduswechsel* ("mode-change
requirements"), was that the Yin Yang card would cycle through four play modes:
you insert the card repeatedly *before* committing a player card, each insertion
advances the selected mode (shown on the display), and you pull the card out once
the mode you want is selected.

The four intended modes were:

| Mode (German) | English gloss | Display glyph | Behaviour |
|---|---|---|---|
| Standard | Standard | `S` | Normal game, no extras. |
| Himmel | "Heaven" | circle | Every third round, deflate a **random** player's balloon a little. |
| Hölle | "Hell" | minus | Every third round, inflate a **random** player's balloon a little. |
| Provokation | "Provocation" | `P` | Start the game with the balloon already partly filled. |

!!! note "What actually shipped for code 27"
    The mode system was never built. In the shipped firmware, optical code **27**
    is instead wired to a single card behaviour — **`PushToLimitCard(255)`**,
    which inflates the balloon to **99 %** in one go (see
    [The Card Deck](../gameplay/card-deck.md)). So the Yin Yang card exists and is
    playable, but as a one-shot "push to the limit" wildcard rather than as the
    four-way mode selector its design note envisioned.

The design note itself is explicit that these were early ideas ("*these are just
ideas for now*"), so nothing here was a broken promise — it is simply a feature
that stopped at the design stage.

**Source:** the original German design note is archived alongside this page as
[`anforderungen-moduswechsel.txt`](anforderungen-moduswechsel.txt).
