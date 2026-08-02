# Cards

This folder holds the Boom Balloon card assets, plus pointers to the
authoritative card documentation.

## Contents

- **`artwork/`** — the printed card artwork: the Inkscape SVG masters
  (`Alle Karten 2.svg`, and `muster.svg` for the hole pattern) and the
  combined print sheets for the effect, player, and Yin-Yang decks.
- **`fonts/`** — the fonts the card SVGs reference. These must be installed
  on your system for the artwork to render correctly (see below).

## Fonts

The card artwork uses three non-standard fonts that ship with this folder:

| File | Font family (as referenced in the SVG) |
| --- | --- |
| `fonts/ARBERKLEY.ttf` | `AR BERKLEY` |
| `fonts/SNAP____.TTF` | `Snap ITC` |
| `fonts/Bell MT Regular.otf` | `Bell MT` |

Without them installed, Inkscape (or any SVG renderer) substitutes a fallback
font and the card layout will look wrong.

### Installing

**Linux**

```sh
mkdir -p ~/.local/share/fonts
cp cards/fonts/*.ttf cards/fonts/*.TTF cards/fonts/*.otf ~/.local/share/fonts/
fc-cache -f
```

**macOS** — double-click each file in `cards/fonts/` and click *Install Font*,
or copy them into `~/Library/Fonts/`.

**Windows** — select both files in `cards\fonts\`, right-click, and choose
*Install*.

After installing, reopen the SVGs in `artwork/` and confirm the text renders
in the correct fonts.

## Deck catalog & optical codes

The full **deck catalog** (every card's German name, English gloss, printed
face value, optical code, firmware effect, and deck count) and the **5-bit
optical code system** (bit weights, mirror invariance, sentinels) are the
single source of truth in the documentation site — this folder does not
duplicate them:

- **Deck catalog** → *Gameplay → The Card Deck* — [`docs/gameplay/card-deck.md`](../docs/gameplay/card-deck.md)
- **Optical code system** → *Reference → Optical Code System* — [`docs/reference/optical-codes.md`](../docs/reference/optical-codes.md)
