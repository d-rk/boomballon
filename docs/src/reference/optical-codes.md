# Optical Code System

Every card carries a 5-bit optical code encoded in a set of holes.
The electronics turn the holes into an analog signal which the firmware then reads.

For what each code *means* (which card, which effect), see the deck
catalog in [The Card Deck](../gameplay/card-deck.md); for the code that
implements the reader, see [Firmware Architecture](firmware-architecture.md).

## How the reader works

The card reader is two facing PCBs with a slot between them (see the
[card-reader module](../build-guide/modules/card-reader.md)):

- The **Lichtmodul** ("light module") carries **5 LEDs** that shine across the
  slot.
- The **Fotomodul** ("photo module") carries **5 photoresistors** directly
  opposite them.

A card is an opaque rectangle punched with up to five holes, one per
LED/photoresistor pair. Where the card has a **hole**, light passes through and
the photoresistor sees it; where the card is **solid**, the light is blocked. So
each of the five positions reads as one **bit** — light (1) or dark (0) — and the
five bits together form an integer from **0 to 31**.

The five photoresistors are wired to the Arduino's analog inputs **A0–A4** (see
the [wiring](../build-guide/wiring.md)). The firmware
(`Devices/CodeDetector`) reads all five with `analogRead()`, thresholds each into
a bit, and assembles the result into a single value.

## Bit positions

The five analog pins together form a binary representation of an integer:
pin `A`*i* is bit *i* of the code, so its value is `2^i`. The order comes
straight from the firmware's pin array (`Main.cpp`: `CodeDetector(PIN_A0,
PIN_A1, PIN_A2, PIN_A3, PIN_A4)`), where `A0` fills bit 0 and `A4` fills bit 4:

| Pin | A0 | A1 | A2 | A3 | A4 |
|---|---|---|---|---|---|
| Bit | 0 | 1 | 2 | 3 | 4 |
| Value (2^bit) | 1 | 2 | 4 | 8 | 16 |

A code is the binary number formed by reading each **open (holed)** position
as a `1` and each solid position as a `0`.

!!! example "example — the Player 1 card"
    The `1 Spieler` (Player 1) card has the pattern `01111` (bit 4 down to bit 0):

    | A4 (bit 4) | A3 (bit 3) | A2 (bit 2) | A1 (bit 1) | A0 (bit 0) |
    |---|---|---|---|---|
    | 0 | 1 | 1 | 1 | 1 |

    Binary `01111` = 8 + 4 + 2 + 1 = **15**.

## Mirror invariance

A card is a physical rectangle, and nothing stops a player from pushing it in
**rotated 180°**. When that happens the reader sees the hole pattern
**bit-reversed**. the code read from the flipped card is the mirror image of the
code read from the upright card.

The firmware makes reading **orientation-independent** by canonicalising every
code: it computes the bit-reversal of the detected value and always stores the
**smaller of the two**. So a card and its 180° rotation both resolve to the same
canonical code, and the deck is designed so that no two *different* cards collide
under this rule.


The codes that are **not** palindromic pair up as follows. Bits are written
`A4 A3 A2 A1 A0`, so the mirror's bit string is just the canonical's read
backwards:

| Canonical code | Canonical bits | Mirror bits | Mirror code |
|---|---|---|---|
| 1 | `00001` | `10000` | 16 |
| 2 | `00010` | `01000` | 8 |
| 3 | `00011` | `11000` | 24 |
| 5 | `00101` | `10100` | 20 |
| 6 | `00110` | `01100` | 12 |
| 7 | `00111` | `11100` | 28 |
| 9 | `01001` | `10010` | 18 |
| 11 | `01011` | `11010` | 26 |
| 13 | `01101` | `10110` | 22 |
| 15 | `01111` | `11110` | 30 |
| 19 | `10011` | `11001` | 25 |
| 23 | `10111` | `11101` | 29 |

Codes that read the same upside-down (the palindromes `0`, `4`, `10`, `14`,
`17`, `21`, `27`, `31`, …) are their own mirror and need no pairing — they are
read identically whichever way round the card goes in.

## Thresholding, calibration, and debounce

Photoresistors are analog and noisy, and the raw `analogRead()` value for
"bright" differs from channel to channel because of component tolerances and
wiring. `CodeDetector` handles this in three steps.

**1. Per-pin calibration.** Each channel's raw 0–1023 reading is rescaled to a
normalised 0–100 range with Arduino's `map()`, using a *different* upper
calibration point per pin (the lower point is shared):

```cpp
case 0: value = map(value, 20, 950, 0, 100); break;  // A0
case 1: value = map(value, 20, 910, 0, 100); break;  // A1
case 2: value = map(value, 20, 885, 0, 100); break;  // A2
case 3: value = map(value, 20, 900, 0, 100); break;  // A3
case 4: value = map(value, 20, 895, 0, 100); break;  // A4
```

These per-pin numbers were found empirically with the calibration harness
below; if the reader hardware changes, they are what you re-tune.

**2. A threshold band with hysteresis.** Rather than a single cutoff, the
normalised value is compared against two thresholds — a `MIN_THRESHOLD` and a
`MAX_THRESHOLD` (20 and 28 in the normalised 0–100 space). A bit is only cleared
to 0 when its value drops **below** the low threshold, and only set to 1 when it
rises **above** the high threshold; in the gap between them the bit keeps its
previous state. This dead-band stops a marginal channel from flickering between
0 and 1 as a card slides through.

**3. Time debounce.** A newly-read code must remain stable for at least
**`TIME_WAIT_MS` (200 ms)** before it is accepted as the new *active* code
(`codeChanged()` remembers when the reading last changed and waits out the
window). This absorbs the transient partial patterns produced while a card is
physically being pushed in or pulled out by hand.

### Calibration harness

Building the firmware with `-D DETECTOR_CALIBRATION` replaces the game loop with a bring-up tool. Instead of playing, it continuously prints each channel's raw
normalised value alongside the decoded bits and the resulting code. Each line has
three columns: the five normalised values for `A0`–`A4`, the thresholded bits in
the same `A0`-first order, and finally `activeCode = mirror` (the canonical code
and its 180° mirror). For example, inserting the **Player 1 card** (code **15**,
holes `01111` from the example above) reads as:

```
 98  95  97  92   8  |  1 1 1 1 0   |  15 = 30
```

Reading the bits `A0`-first gives `1·1 + 1·2 + 1·4 + 1·8 + 0·16 = 15`; the low
`A4` value (8, below the threshold) is the single dark position, and 15's mirror
is 30, so the reader reports `15 = 30`.

Over the serial monitor, `<SPACE>` pauses the stream and `<TAB>` toggles
"changes only" output. This is the mode you use to pick the per-pin `map()`
constants and to verify a freshly assembled reader before trusting it in a game.

## Sentinel codes

Two of the 32 possible values are reserved as **sentinels** rather than cards:

| Code | Name | Meaning |
|---|---|---|
| `0` | `CODE_NONE` | All positions dark — no card, or a card not yet fully inserted. |
| `31` | `CODE_ALL` | All five positions lit — an **empty slot** / "card removed" signal. |

`CODE_ALL` is central to game flow: after a card is played, the machine waits for
the slot to read `31` (the card pulled back out) before advancing the turn, and a
`31` after the game ends is what triggers a restart. See
[Two-phase card insertion](firmware-architecture.md#two-phase-card-insertion) for
how the game consumes these. Because a real card can never present all five holes
*and* be a play card, `0` and `31` are safe to treat as "nothing is being
played."

Everything else — the codes that actually map to cards and their effects — is
catalogued in [The Card Deck](../gameplay/card-deck.md).
