# Design Notes & Roadmap

This page is the honest engineering commentary on BoomBalloon: the history behind
some of the code, the parts that are experimental or unused, the shortcuts that
were taken deliberately, and the ideas that were specced but never built. If you
are reading the source and something looks half-finished or contradictory, this
is the page that explains why. For the clean architectural overview see
[Firmware Architecture](firmware-architecture.md).

## Controller migration: Arduino Micro → Nano

The project was originally developed on an **Arduino Micro** and later moved to
an **Arduino Nano (ATmega328P)**, which is what the shipped device uses. Most of
the migration is invisible, but one residue survives in the build system.

`firmware/CMakeLists.txt` keeps **two separate port variables** —
`ARDUINO_SERIAL_PORT` and `ARDUINO_UPLOAD_PORT` — with a comment explaining the
split:

```cmake
# for nano boards SERIAL_PORT equals UPLOAD_PORT
# for micro boards they are different
```

On the Nano both are the same COM port (a single USB-serial bridge), so the two
variables are set identically. The distinction only mattered on the Micro, whose
native-USB bootloader exposes a separate upload port. The split is kept as
harmless residual Micro support; on the Nano you configure one port and set both
to it (see [Firmware — Build & Flash](../build-guide/firmware-build.md)).

## Simulated balloon volume (not sensed)

The single most important thing to understand about the firmware is that the
balloon's fill level is **modelled in software, never measured**. `OutputDevice`
keeps a `float volume` that it integrates over time: while the pump runs it adds
a per-millisecond increment, while the valve opens it subtracts one, and the
player loses when that number crosses 100 %. There is **no pressure sensor and no
feedback** from the physical balloon — the machine knows the fill level only
because it is the one driving the pump and valve. This is explained for players
in [How It Works](../how-it-works.md#the-balloon-volume-model); the takeaway for a
developer is that "volume" is an open-loop estimate, and the physical balloon is
there for drama, not for measurement.

`SoundDetector` (`Devices/SoundDetector.*`) is the fossil of an abandoned
**closed-loop** idea: detect the actual *pop* of the balloon (or its pressure via
sound) and react to it. It was never wired into the game — nothing in `Main.cpp`
or `Game.cpp` constructs or references it — and it remains as a stub of a road
not taken.

## Dead and utility code

A few things in the tree are **not part of the running game**. They compile-guard
cleanly or are simply never referenced, but they can mislead a first-time reader:

- **The `Dummy/` subtree** — `OutputDeviceDummy`, `LedBar`, and
  `CodeDetectorSerial`. This was a **simulation / desk-testing path**: drive an
  LED bar graph instead of a real pump, and feed codes over the serial port
  instead of the optical reader, so the game logic could be exercised without the
  hardware. Its include in `Main.cpp` is commented out and it is not built.
  Worth noting: `OutputDeviceDummy` has **drifted out of sync** with its base
  class — it overrides `applyPositive()` / `applyNegative()`, but `OutputDevice`
  was since refactored and no longer has those virtuals (it now works through
  `applyIntensities()`). So the Dummy path would need repair before it could run
  again.
- **`Devices/Button` and `Devices/Led`** — general-purpose helper drivers that
  are **not used** anywhere in the game (BoomBalloon has no gameplay buttons; the
  only input is the card reader). They are unused scaffolding.
- **`Devices/SoundDetector`** — unused, as described above.

None of these are on the critical path; they are listed here so nobody mistakes
them for live code.

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

## Roadmap

None of these are committed work — they are the natural next steps that the code
above points toward:

- **Finish the mode system.** Implement the four-mode Yin Yang selector from the
  design note (Standard / Himmel / Hölle / Provokation), including the
  insert-repeatedly-to-cycle interaction and the every-third-round random-player
  effects. The card, its code (27), and the display glyphs already exist.
- **Revive the simulation / `Dummy` path.** Bring `OutputDeviceDummy` back in
  line with the refactored `OutputDevice` base class so the game can once again
  be run and demoed on an LED bar graph with codes fed over serial — a genuinely
  useful test rig.
- **Add real pressure sensing.** Close the loop that `SoundDetector` only
  gestured at: measure actual balloon pressure (or detect the pop) and reconcile
  it with the software volume model, so the game reacts to the real balloon
  instead of a pure time-integral.

For where the current code lives and how the running parts fit together, return
to [Firmware Architecture](firmware-architecture.md).
