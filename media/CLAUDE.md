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
- `logo/`, `gallery/`, `wiring/`, `docs/` — source assets for their respective
  doc sections.
- `enclosure/` — Blender-driven 3D renders of the printed enclosure parts, see
  below.
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
