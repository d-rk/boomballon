# overlays/

Optional hand-drawn artwork layered on top of a board's breadboard-view PNG when
building its custom Fritzing part -- e.g. a lit-looking 7-segment digit for
Displaymodul, since the real board photo just shows the bare component footprint.

Drop a file named `<Board>_overlay.png` here (e.g. `Displaymodul_overlay.png`) and
re-run `python3 media/pcb/parts/generate_<board>.py` -- it's picked up automatically
if present, skipped if not.

Requirements:
- **Exact same pixel dimensions** as `docs/src/assets/img/<board>-pcb-top.png`
  (the base image it's layered onto). `build_part.py` errors out if the sizes
  don't match, rather than silently misaligning the artwork.
- Transparent everywhere it isn't adding detail, so the real board photo shows
  through underneath.
