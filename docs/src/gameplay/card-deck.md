# The Card Deck

This page is the **authoritative catalog** of the Boom Balloon deck. Every card's German name, English gloss, printed face value, optical code, firmware effect, and deck count is listed here, cross-verified against the firmware and the design spreadsheets. Other documents that need the deck (including `cards/README.md` in the repository) point back to this page.

## The cards

The full deck, card by card. Printed face values are shown in the captions; see [the catalog](#the-catalog) below for each card's optical code and firmware effect.

### Effect cards

<div class="card-grid" markdown>
<figure markdown="span">
![Hochdruckgebiet 10](../assets/img/cards/hochdruckgebiet-10.png){ loading=lazy }
<figcaption>Hochdruckgebiet 10</figcaption>
</figure>
<figure markdown="span">
![Hochdruckgebiet 20](../assets/img/cards/hochdruckgebiet-20.png){ loading=lazy }
<figcaption>Hochdruckgebiet 20</figcaption>
</figure>
<figure markdown="span">
![Tiefdruckgebiet 10](../assets/img/cards/tiefdruckgebiet-10.png){ loading=lazy }
<figcaption>Tiefdruckgebiet 10</figcaption>
</figure>
<figure markdown="span">
![Tiefdruckgebiet 20](../assets/img/cards/tiefdruckgebiet-20.png){ loading=lazy }
<figcaption>Tiefdruckgebiet 20</figcaption>
</figure>
<figure markdown="span">
![Kurswechsel 30, deflate and reverse](../assets/img/cards/kurswechsel-30-1.png){ loading=lazy }
<figcaption>Kurswechsel 30 ↓</figcaption>
</figure>
<figure markdown="span">
![Kurswechsel 30, inflate and reverse](../assets/img/cards/kurswechsel-30-2.png){ loading=lazy }
<figcaption>Kurswechsel 30 ↑</figcaption>
</figure>
<figure markdown="span">
![Engelsbote 30](../assets/img/cards/engelsbote-30.png){ loading=lazy }
<figcaption>Engelsbote 30</figcaption>
</figure>
<figure markdown="span">
![Teufelsbote 30](../assets/img/cards/teufelsbote-30.png){ loading=lazy }
<figcaption>Teufelsbote 30</figcaption>
</figure>
<figure markdown="span">
![Blockierung](../assets/img/cards/blockierung.png){ loading=lazy }
<figcaption>Blockierung</figcaption>
</figure>
<figure markdown="span">
![Weltuntergang](../assets/img/cards/weltuntergang.png){ loading=lazy }
<figcaption>Weltuntergang</figcaption>
</figure>
<figure markdown="span">
![Zwiegespalten 50](../assets/img/cards/zwiegespalten-50.png){ loading=lazy }
<figcaption>Zwiegespalten 50</figcaption>
</figure>
<figure markdown="span">
![Berg und Tal 10](../assets/img/cards/berg-und-tal-10.png){ loading=lazy }
<figcaption>Berg und Tal 10</figcaption>
</figure>
<figure markdown="span">
![Yin Yang](../assets/img/cards/yin-yang.png){ loading=lazy }
<figcaption>Yin Yang</figcaption>
</figure>
</div>

### Player cards

<div class="card-grid" markdown>
<figure markdown="span">
![1 Spieler](../assets/img/cards/1-spieler.png){ loading=lazy }
<figcaption>1 Spieler</figcaption>
</figure>
<figure markdown="span">
![2 Spieler](../assets/img/cards/2-spieler.png){ loading=lazy }
<figcaption>2 Spieler</figcaption>
</figure>
<figure markdown="span">
![3 Spieler](../assets/img/cards/3-spieler.png){ loading=lazy }
<figcaption>3 Spieler</figcaption>
</figure>
<figure markdown="span">
![4 Spieler](../assets/img/cards/4-spieler.png){ loading=lazy }
<figcaption>4 Spieler</figcaption>
</figure>
<figure markdown="span">
![5 Spieler](../assets/img/cards/5-spieler.png){ loading=lazy }
<figcaption>5 Spieler</figcaption>
</figure>
</div>

### Other cards

<div class="card-grid" markdown>
<figure markdown="span">
![Card back](../assets/img/cards/rueckseite.png){ loading=lazy }
<figcaption>Rückseite (card back)</figcaption>
</figure>
<figure markdown="span">
![Spielerkarte](../assets/img/cards/spielerkarte.png){ loading=lazy }
<figcaption>Spielerkarte</figcaption>
</figure>
<figure markdown="span">
![Moduswechsel](../assets/img/cards/moduswechsel.png){ loading=lazy }
<figcaption>Moduswechsel</figcaption>
</figure>
</div>

## What a card is

Every card is a printed rectangle with a machine-readable hole pattern:

- **Physical size:** 90 × 60 × 1 mm.
- **Holes:** 3 mm in diameter, spaced 10 mm apart.
- **Code:** 5 hole positions, so each card carries a **5-bit optical code** (a hole present or absent at each position). The card reader scans this pattern as the card slides in. See the [Optical Code System](../reference/optical-codes.md) reference for the bit layout and mirror-handling rules.

!!! note "Face value is thematic, not literal"
    The number printed on a card (its **face value**) is part of the game's theme and story. It is **not** the same as the internal **firmware effect** — the actual change applied to the modelled balloon volume. Always treat the firmware effect column below as the source of truth for what a card does.

## The catalog

| German name | English gloss | Face value | Code | Firmware effect | Count |
|---|---|---|---|---|---|
| Hochdruckgebiet 10 | High-pressure zone (deflate) | 10 | 1 | `VolumeCard(-15,200)` ~−15% | 6 |
| Hochdruckgebiet 20 | High-pressure zone | 20 | 2 | `VolumeCard(-25,200)` ~−25% | 6 |
| Kurswechsel 30 ↓ | Change of course (down) | −30 | 3 | `ChangeDirectionCard(-25,200)` deflate + reverse | 4 |
| Engelsbote 30 | Angel's messenger | −30 | 4 | `AngelCard(-25,50)` delayed relief next round | 3 |
| Blockierung | Blocking / miss round | — | 5 | `MissRoundCard()` skip a card (50%) | 2 |
| Tiefdruckgebiet 10 | Low-pressure zone (inflate) | +10 | 6 | `VolumeCard(30,200)` ~+30% | 6 |
| Tiefdruckgebiet 20 | Low-pressure zone | +20 | 7 | `VolumeCard(40,200)` ~+40% | 6 |
| Kurswechsel 30 ↑ | Change of course (up) | +30 | 9 | `ChangeDirectionCard(40,200)` inflate + reverse | 4 |
| Teufelsbote 30 | Devil's messenger | +30 | 10 | `DevilCard(40,50)` malicious inflate (50%) | 3 |
| Weltuntergang | Apocalypse | — | 11 | `SuddenDeathCard()` repeatedly fills to max | 2 |
| Zwiegespalten 50 | Torn / fifty-fifty | ±50 | 13 | `FiftyFiftyCard(100)` random ±50% | 3 |
| Berg und Tal 10 | Up hill and down | 10/10 | 14 | `UpDownCard(35,200)` oscillate | 3 |
| Yin Yang | Yin & Yang | — | 27 | `PushToLimitCard(255)` push to 99% | 1 |
| 1–5 Spieler | Player 1–5 | — | 15/17/19/21/23 | player-select / target card | 1 each |

## Deck total and the player cards

The full deck is **about 54 cards**: **49 effect cards** (the play cards, or *Spielkarten*) plus **5 player cards** (*Spielerkarten*), for **54** in total.

The player cards, **`1 Spieler`…`5 Spieler`**, do two jobs:

- During play they act as **target cards** — you insert one to choose which player a target-based card (Teufelsbote, Blockierung, Weltuntergang) affects.
- The **`2 Spieler`…`5 Spieler`** cards additionally **choose the game size** when inserted at startup (see [Rules → Setting up a game](rules.md#setting-up-a-game)).

The **Yin Yang** card is a rare single wildcard that pushes the modelled volume all the way to the limit.
