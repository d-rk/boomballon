# Control Board

The control board (*Steuermodul*, control module) is the **hub** of the device.
The Arduino Nano sits on it, the 12 V power comes in here, and the other two
modules — the [card reader](card-reader.md) and the [display](display.md) — plug
into it. It also carries the power electronics that switch the two 12 V loads:
the inflate pump and the deflate valve.

<div class="board-pair" markdown>
<figure markdown="span">
![Steuermodul PCB, top side](../../assets/img/steuermodul-pcb-top.png){ loading=lazy }
<figcaption>Steuermodul PCB — top (Arduino and connectors)</figcaption>
</figure>
<figure markdown="span">
![Steuermodul PCB, bottom side](../../assets/img/steuermodul-pcb-bottom.png){ loading=lazy }
<figcaption>Steuermodul PCB — bottom</figcaption>
</figure>
</div>

### Board markings

| Marking | What it is |
|---|---|
| *(large centre outline)* | The Arduino footprint, with the board's pin names printed alongside it. The sketch was drawn around an Arduino **Micro**, so those names follow the Micro's layout — see [the story](../../story.md#the-controller-from-micro-to-nano) and [fitting the Nano](#fitting-the-nano-in-the-micro-footprint) below. |
| `Motor` | The 3-pin footprint for the **pump** channel's TIP120 (*Motor* = the inflate pump). |
| `Ablassmotor` | The same for the **valve** channel (*Ablassmotor* = release motor, the deflate solenoid). |
| `D1`, `D2` | The 1N4004 flyback diodes, one per channel, cathode toward +12 V. |
| `R1`, `R2` | The 2.2 kΩ base resistors, one per channel, between the MCU pin and the transistor base. |
| `MA`, `MM` | The **pump** output terminals, 12 V switched by `D6`. |
| `VA`, `VM` | The **valve** output terminals, 12 V switched by `D7`. |
| `Vcc 12V` | The 2-pin 12 V supply input. |
| `Fotomodul` | The 7-pin header to the [card reader](card-reader.md). |
| `Displaymodul` | The 6-pin header to the [display](display.md). |
| `1` | Pin-1 marker, printed next to each keyed header. |

The single unlabelled pads in rows beside the Arduino footprint are direct
break-outs of its pins, there so a signal can be tapped or jumpered by hand.

### Fitting the Nano in the Micro footprint

The footprint has **17 holes per row** — the Micro's pin count. The Nano has
15 per row, but the two layouts are compatible by design: the Micro's pinout
is the Nano's plus two extra pins per row at the far-from-USB end (its SPI
pins `D14`–`D17`), and its `NC` positions sit where the Nano has `A6`/`A7`.
Compare the two boards (both shown from the top, USB up):

<div class="board-pair" markdown>
<figure markdown="span">
![Arduino Micro pinout](../../assets/img/arduino-micro-pinout.jpg){ loading=lazy }
<figcaption>Arduino Micro — its extra SPI pins (D14–D17) sit at the far-from-USB end; the two NC holes fall where the Nano instead has A6/A7</figcaption>
</figure>
<figure markdown="span">
![Arduino Nano pinout](../../assets/img/arduino-nano-pinout.jpg){ loading=lazy }
<figcaption>Arduino Nano — all 30 pins land in the footprint, aligned to the USB end (A6/A7 fall in the Micro's NC holes)</figcaption>
</figure>
</div>

To place the Nano on the board:

- **USB connector toward the `Displaymodul`/`Fotomodul` headers** — the same
  direction the Micro's USB would face. The `D12` and `D13` holes are in that
  corner of the footprint.
- **Align flush with that USB end.** Every Nano pin then sits in the hole
  carrying its own name: `TX1`→`TX1` … `D12`→`D12` on one row, `VIN`→`VIN` …
  `D13`→`D13` on the other.
- The **two holes at the far end of each row stay empty** — they are the
  Micro's SPI pins (marked `MOSI`/`SS` and `SCK`/`MISO`, next to the `Motor`
  transistor footprint), which this board does not route.
- Nano `A6`/`A7` land in the holes marked `NC` — not connected on the Micro
  and unused by the firmware, so this is harmless.

!!! warning "Check the alignment before powering up"
    An off-by-one placement toward the empty SPI end puts `5V` on the `RST`
    hole. Before applying power, confirm the Nano's `5V` pin sits in the hole
    marked `5V` and `D13` is in the corner hole next to the `Fotomodul`
    header.

### Gallery

<div class="gallery">
<div class="gallery__empty">No photos of this board yet</div>
</div>

## Power switching: two TIP120 channels

The Arduino cannot drive a 12 V pump or solenoid directly, so each load has a
**TIP120 Darlington transistor** acting as a low-side switch. There are two
identical channels:

- **Pump channel** — output terminals **MA/MM** (*Motor*), drives the AIRPO
  D2028B membrane pump (inflate). Controlled by MCU pin **D6**.
- **Valve channel** — output terminals **VA/VM** (*Ventil*, valve), drives the
  CEME 5000EN1,5P solenoid valve (deflate). Controlled by MCU pin **D7**.

Each channel is the textbook TIP120 low-side driver:

- a **2.2 kΩ base resistor** between the MCU pin and the transistor base,
- the load (pump or valve coil) between +12 V and the transistor collector,
- the emitter to ground, and
- a **1N4004 flyback diode** across the coil (cathode to +12 V) to clamp the
  inductive kick when the transistor turns off.

The design basis is the well-known **bildr TIP120 tutorial**, archived here as
[`Motorsteuerung.pdf`](https://github.com/d-rk/boomballon/blob/main/hardware/control/Motorsteuerung.pdf)
(*Motorsteuerung* = motor control).

!!! danger "Don't omit the flyback diodes"
    Both the pump and the valve are inductive. Without the **1N4004** clamp
    diode across each coil, turning the transistor off produces a large voltage
    spike that will eventually destroy the TIP120. One diode per channel,
    oriented cathode toward +12 V.

## Connectors

Everything meets on this board:

| Connector | To module | Pins |
|---|---|---|
| Fotomodul header | [Card reader](card-reader.md) | 7-pin (3.3 V, `A0`–`A4`) |
| Displaymodul header | [Display](display.md) | 6-pin (5 V, `DS`/`STCP`/`SHCP`/buzzer) |
| Power input | 12 V / 3 A supply | via switched DC socket |
| Pump output (MA/MM) | AIRPO pump | 12 V, switched by D6 |
| Valve output (VA/VM) | CEME valve | 12 V, switched by D7 |

Fritzing source:
[`Steuermodul.fzz`](https://github.com/d-rk/boomballon/blob/main/hardware/control/Steuermodul.fzz).

For the complete, authoritative signal map — including which analog and digital
pins each connector carries — see the [wiring & pin map](../wiring-pinmap.md).
