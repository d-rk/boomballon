import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from build_part import build_part  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

if __name__ == "__main__":
    # Re-verified 2026-08-14 after Displaymodul.fzz was corrected from a stale
    # state: the header is now a vertical 6-pad column (drill group T102 in
    # the Gerber output, X=0.9925in constant, Y=1.0957..1.5957in in 0.1in
    # steps -- previously a horizontal row, so the old x=797.8..297.8
    # coordinates no longer landed on real pads at all). Pin 1 orientation
    # confirmed against the real assembled-board photos
    # (docs/src/assets/img/gallery/displaymodul-top.jpg,
    # displaymodul-assembled.jpg): the header column sits nearest the buzzer
    # ("Summer") component at the board's top edge, and pin 1 (Vcc) is the
    # pin closest to it -- larger raw y is nearer the top after the
    # to_png_space flip in build_part.py -- counting down to pin 6 (Buzzer)
    # at the bottom, matching the pin table in
    # docs/src/build-guide/modules/display.md.
    # Breadboard graphic: the same curated PNG used in the docs (real
    # photorealistic render), not the tracespace vector SVG -- Fritzing's
    # breadboard-view renderer shows the vector version washed out (see
    # media/CLAUDE.md). An optional transparent overlay (e.g. a hand-drawn lit
    # 7-segment digit) can be dropped in overlays/Displaymodul_overlay.png,
    # same pixel size as the board PNG; it's picked up automatically if present.
    overlay_path = os.path.join(os.path.dirname(__file__), "overlays", "Displaymodul_overlay.png")
    out = build_part(
        board_fzz=os.path.join(REPO_ROOT, "hardware", "display", "Displaymodul.fzz"),
        board_name="Displaymodul",
        connectors=[
            ("Vcc", 992.5, 1595.7),
            ("GND", 992.5, 1495.7),
            ("DS", 992.5, 1395.7),
            ("STCP", 992.5, 1295.7),
            ("SHCP", 992.5, 1195.7),
            ("Buzzer", 992.5, 1095.7),
        ],
        out_dir=os.path.join(REPO_ROOT, "hardware", "fritzing-parts"),
        board_png=os.path.join(REPO_ROOT, "docs", "src", "assets", "img", "displaymodul-pcb-top.png"),
        overlay_png=overlay_path if os.path.exists(overlay_path) else None,
    )
    print(f"Wrote {out}")
