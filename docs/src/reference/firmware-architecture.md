# Firmware Architecture

This page is a developer's tour of the Boom Balloon firmware — how the source
tree is organised, how the program starts, and how the pieces cooperate at run
time. It is aimed at someone who wants to read or change the code. For how to
compile and flash it, see [Firmware — Build & Flash](../build-guide/firmware-build.md);
for the card encoding it depends on, see the
[Optical Code System](optical-codes.md).

The firmware is **plain object-oriented C++11**, not an `.ino` sketch. It targets
the **Arduino Nano (ATmega328P)** — a small 8-bit AVR with 2 KB of RAM — which
shapes many of the design choices below (no STL, manual memory accounting, no
threads).

## Source layout

The tree lives under `firmware/src/`:

| Area | Files | Role |
|---|---|---|
| Entry point | `Main.cpp` | Arduino `setup()` / `loop()`; wires everything together |
| Game logic | `Game.*`, `Player.*`, `PlayerChooser.*` | Turn flow, player ring, startup player-count selection |
| Cards | `Cards/Card.*` + subclasses | Card behaviours and the code→card factory |
| Devices | `Devices/CodeDetector.*`, `OutputDevice.*`, `PiezoBuzzer.*`, `SevenSegmentDisplay.*` | Hardware drivers |
| Mock devices | `Mock/CodeDetectorMock.*`, `Mock/OutputDeviceMock.*` | Serial stand-ins for the card reader and pump/valve, compiled in only with `-D MOCKED_DEVICES` |
| Scheduling | `Tasks/Task.h`, `Tasks/TaskScheduler.*` | Cooperative multitasking |
| Helpers | `Helper/Vector.h`, `Helper/Diagnostic.h`, `Helper/Log.*` | STL-free container, RAM probe, `printf` logging |
| Config | `Constants.h` | Pins, codes, display glyphs, note frequencies, build switches (`LOGGING_ENABLED`, `AUTOSTART_GAME`, `MOCKED_DEVICES`, `ACTIVE_MODE`) |

The `Mock/` subtree lets the whole game run with **no hardware attached**: build
with `-D MOCKED_DEVICES` and `Main.cpp` swaps the real card reader and
pump/valve for serial-driven stand-ins (type codes in, watch the balloon volume
print out). See [Design Notes](design-notes.md#the-mock-devices) for how it works.

There is also a `Devices/` trio (`Button`, `Led`, `SoundDetector`) that is
**not part of the running game** — see
[Design Notes & Roadmap](design-notes.md) for what they were for.

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
[pin map](../build-guide/wiring-pinmap.md) insists that `A5` stay unconnected.

The pin numbers, code values, and display glyphs all come from `Constants.h`,
which also carries two compile-time switches described under
[Build modes](#build-modes-and-logging).

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
  on each turn change when logging is on.
- **`Log` (`Helper/Log.cpp`)** — redirects C `printf` to the Arduino `Serial`
  port, so the rest of the code can log with ordinary `printf(...)` format
  strings. All logging is compiled out unless `LOGGING_ENABLED` is defined.

## Build modes and logging

`Constants.h` exposes two compile-time knobs that change what gets built:

- **`ACTIVE_MODE()`** selects the top-level `loop()`:
    - `MODE_GAME()` — the normal game (default).
    - `MODE_CODE_DETECTOR_CALIBRATION()` — a bring-up harness that just prints
      raw analog readings from the card reader, used to tune the reader
      thresholds. See the [Optical Code System](optical-codes.md#calibration-harness).
- **`LOGGING_ENABLED`** turns the `printf`-over-serial logging (and a 2.5 s
  startup delay to let the serial link settle) on or off.

There is also an `AUTOSTART_GAME` switch that skips the player-count selection
and jumps straight into a 2-player game — a convenience for testing.

For the honest list of what in this tree is experimental, unused, or left for
later, continue to [Design Notes & Roadmap](design-notes.md).
