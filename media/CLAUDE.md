# media/

Source assets and generation scripts for docs images and enclosure 3D renders.
Nothing here is required at runtime; it feeds `docs/src/assets/img/` and is
otherwise dev tooling.

## Layout

- `generate-pngs.sh` — top-level driver: rebuilds logo/favicon PNGs, the docs
  background tile, the header wordmark, gallery thumbnails, and syncs
  `enclosure/renders/` into `docs/src/assets/img/enclosure/`. Requires
  inkscape + imagemagick (`magick`).
- `flatten-svg-viewports.py` — normalizes nested `<svg>` viewports in logo
  sources; a nested viewport clips silently in browsers but not in Inkscape.
- `logo/`, `gallery/`, `docs/` — source assets for their respective doc
  sections.
- `enclosure/` — Blender-driven 3D renders of the printed enclosure parts, see
  below.
- `pcb/` — Fritzing → Gerber → tracespace pipeline for the PCB top/bottom view
  PNGs, see below.
- top-level `*.jpg` — build-log photos referenced directly from docs pages.

## enclosure/ — Blender render pipeline

- `render.py` + `render-all.sh` regenerate every PNG under
  `enclosure/renders/` (gitignored build output) from the STLs in
  `hardware/enclosure/stl/`. Run `BLENDER=<path> ./media/enclosure/render-all.sh`,
  then `./media/generate-pngs.sh` to sync JPGs into the docs.
- No `blender` binary is on PATH on this machine — use the flatpak build:
  `/var/lib/flatpak/exports/bin/org.blender.Blender` (Blender 5.1, already has
  `host` filesystem permission). It prints an OCIO color-management error at
  startup; harmless, it falls back to the `Standard` view transform, which is
  what the renders use anyway.
- `render.py --glow <parts>` renders selected assembly parts as glowing amber
  sticks (used for the Knicklichter) instead of clay.
- Cut-mode gotcha: the EXACT boolean cut must run per-part *before* joining —
  on a merged multi-part mesh it silently no-ops, because the overlapping
  shells at part seams read as non-solid.

### Roof mount (Knicklichter)

The roof floats ~140 mm above the upper housing on four bent Knicklichter
(bought glow-stick arm bands — not printed). The bend shapes are hand-tuned in
the Blender GUI, not generated:

- `make-roof-assembly.py` bootstraps `roof-assembly.blend` — imports the
  upper housing and roof, adds the four sticks as live bezier curves, saves,
  and then **refuses to run again** once the .blend exists.
- `roof-assembly.blend` is the source of truth for the stick shapes after
  that; edit the curve handles in Blender and save.
- `export-roof-assembly.py` derives the STLs from the *current* .blend:
  `hardware/enclosure/stl/knicklichter.stl` (sticks alone, feeds
  `render.py --glow`) and `roof-assembly.stl` (housing + roof + sticks
  merged). Re-run this after every hand-tuning pass in Blender — it is not
  automatic.

## pcb/ — Fritzing → Gerber → tracespace pipeline

`generate.sh` regenerates `docs/src/assets/img/<board>-pcb-{top,bottom}.png`
for all four boards (`hardware/control/Steuermodul.fzz`,
`hardware/card-reader/Kartenleser_{Fotomodul,Lichtmodul}.fzz`,
`hardware/display/Displaymodul.fzz`) directly from the Fritzing sources.

The original images (committed pre-repo, source unknown but almost certainly
a fab-house Gerber viewer — PCBWay's viewer page credits tracespace author
Mike Cousins) have a realistic green-soldermask/gold-pad look that Fritzing's
own PCB-view export doesn't reproduce. This pipeline gets much closer:

1. Fritzing's `-gerber FOLDER` flag exports real Gerber + drill files from a
   `.fzz` (confirmed working; Fritzing has no documented image export CLI,
   but this batch-conversion flag exists). Run with `QT_QPA_PLATFORM=offscreen`
   (baked into `generate.sh`'s default `$FRITZING`) so the Qt GUI never
   flashes a window on screen — without it the command is still
   non-interactive and exits on its own, it's just not silent about it.
2. `@tracespace/cli` (pinned in `package.json`, MIT-licensed —
   `npm install` once before first use) renders those Gerbers into
   green/gold board SVGs.
3. Inkscape rasterizes the top/bottom composite SVGs to PNG at a fixed
   431 DPI (chosen to match the original control-board image's pixel
   density; other boards land at a different pixel size than their
   originals since those were apparently screenshotted at inconsistent
   zoom levels — a fixed DPI across all four is the reproducible choice).

Requires the Fritzing flatpak: `flatpak install --user flathub
org.fritzing.Fritzing` (marked end-of-life upstream but functional). Its
sandbox only sees `~/Documents` by default; `generate.sh` instead needs a
narrow one-time override so it can read `hardware/` and write to `/tmp`:

```
flatpak override --user --filesystem="$(pwd)/hardware:ro" --filesystem=/tmp org.fritzing.Fritzing
```

Also requires Node (`npx`/`npm`) and Inkscape on PATH.

## pcb/parts/ — custom Fritzing parts + system wiring starter sketch

`build_part.py` turns a board's Gerbers into a real, importable Fritzing part
(`.fzpz`), packaged per Fritzing's `.fzpz` zip convention (flat zip,
`part.<moduleId>.fzp` + `svg.<view>.<file>` entries — verified against a real
contributed Fritzing part, not guessed).

**Breadboard-view graphic:** the curated PNG already used in the docs (e.g.
`docs/src/assets/img/displaymodul-pcb-top.png`), embedded as a raster `<image>`,
not the tracespace vector SVG. Fritzing's breadboard-view renderer doesn't respect
that vector SVG's CSS-based (`currentColor` + `<style>` class) fill styling — parts
built from it show up washed out (plain white pads, no green/gold) when actually
dragged into a sketch in the real Fritzing GUI, even though the identical SVG
renders correctly in Inkscape/Chromium. Confirmed this is specifically a breadboard-
view-renderer issue, not a file-format one, by inspecting a real official Fritzing
part with photographic artwork (`DRV8825_breakout`) — it embeds its image with a
plain `href` attribute (not `xlink:href`), which `_tag_connectors_raster()` matches
(both attributes are written, for compatibility). PCB-view and schematic-view still
use the vector approach — untouched, not the focus, not reported as broken.

An optional transparent overlay PNG (same pixel dimensions as the board PNG,
dropped in `media/pcb/parts/overlays/<Board>_overlay.png`) composites on top —
e.g. hand-drawn "lit" 7-segment digit artwork, since the real board photo only
shows the bare unlit component footprint. Dirk has supplied overlays for all
four boards (realistic LED/photoresistor/buzzer/lit-digit artwork over the
bare-PCB photos) — they're picked up automatically by each `generate_<board>.py`
whenever present, so re-running one after the overlay file changes is enough
to refresh it.

**Headless validation gap:** Fritzing's `-svg` full-sketch batch exporter cannot
render *any* raster-embedded part — confirmed by testing the same official
`DRV8825_breakout` part, which also exports as an empty SVG. This is a general
limitation of that specific export path (likely Qt's SVG re-serializer not
supporting re-emitting `<image>` elements), not evidence the part is broken. For
raster-based parts, validate with the structural check (`.fzp` well-formed, every
`svgId` resolves) plus a direct visual render of the part's own breadboard SVG —
you cannot use the "bundle into a throwaway sketch and check the `-svg` output"
trick that works for vector parts (see Lichtmodul's original spike).

`generate_lichtmodul.py` / `generate_displaymodul.py` / `generate_fotomodul.py` /
`generate_steuermodul.py` each call `build_part()` with a hand-verified connector
list for that board (pin name + raw Gerber drill coordinates). **These coordinates
are not auto-detected** — pin-1
identity in particular needs a human or Claude to cross-check the copper/soldermask
pad shape (square = pin 1, where present — worked for Lichtmodul's `LM` header) or,
where no pad asymmetry exists (Displaymodul, Fotomodul), the real assembled-board
photos in `docs/src/assets/img/gallery/` and the silkscreen chamfer on each header's
outline box. Re-verify by rendering the output breadboard SVG to PNG (with a
temporary visible stroke on the `connectorNpin`/`connectorNterminal` rects, since
they ship with `fill="none"` and no stroke — invisible by design, matching Fritzing's
own convention) and confirming the markers land on the real holes before trusting a
build.

Known gap: Fotomodul's real, assembled boards have a 6-pin `Connector` pass-through
(for the Displaymodul cable) that isn't a component in the source `.fzz` — see
`hardware/fritzing-parts/README.md`.

**Steuermodul.fzpz** is the bare control board only — deliberately no baked-in
Arduino. Its 19 connectors are the board's 5 real external groups (`VA`/`MA` and
`VM`/`MM` — each is one physical 2-pin connector pairing a valve wire with a
motor wire, not two matched pairs, confirmed from the silkscreen, not assumed;
`Vcc12V`; `Fotomodul` 7-pin; `Displaymodul` 6-pin), identified by rendering all
95 drilled holes as a labeled debug overlay and reading real coordinates off it
(`media/pcb/parts/generate_steuermodul.py`'s own comments have the full
reasoning, including which pin-1 markers were chamfers vs which were assumed).
The Arduino Micro footprint in the middle of the board image is intentionally
left without any connectors of its own — since the breadboard graphic is at the
same true physical scale (DPI) as every other part here, and the real PCB's
Arduino footprint was manufactured to that same real Arduino Micro's pin
spacing, dragging Fritzing's own stock Arduino Micro part on top and lining it
up by eye should fit it exactly, no custom connectors needed for that part of
the board.

`assemble_starter_sketch.py` builds `hardware/system-wiring/BoomBalloon-Wiring.fzz`:
copies `Steuermodul.fzz` wholesale (its internal wiring graph is never touched) and
appends new standalone, unconnected instances of the 3 custom parts + a generic power
jack (`SparkFun-Connectors-POWER_JACK-PTH`, a Fritzing core part). No wires — that's
drawn by hand in Fritzing afterward.

Unlike `pcb/generate.sh`, this is **not** a blind one-command pipeline: connector
coordinates are verified data, checked in, not re-derived from scratch on every run.
If a board's header layout ever changes, re-run the relevant `generate_<board>.py`
after re-verifying its coordinates the same way.
