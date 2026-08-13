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
