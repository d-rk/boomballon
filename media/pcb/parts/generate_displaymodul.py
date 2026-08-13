import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from build_part import build_part  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

if __name__ == "__main__":
    # Pin 1 orientation: none of the 6 header pads is square in copper (unlike
    # Lichtmodul's LM header), so pin-1 identity was cross-checked against the
    # real assembled-board photos instead
    # (docs/src/assets/img/gallery/displaymodul-top.jpg,
    # displaymodul-bottom.jpg): the buzzer ("Summer") and the header both sit
    # at consistent positions between the photos and this render under a 90
    # degree CCW rotation, which places pin 1 at the raw x=797.8 end (nearest
    # the buzzer in the top-view photo), counting down to x=297.8 for pin 6.
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
            ("Vcc", 797.8, 124.2),
            ("GND", 697.8, 124.2),
            ("DS", 597.8, 124.2),
            ("STCP", 497.8, 124.2),
            ("SHCP", 397.8, 124.2),
            ("Buzzer", 297.8, 124.2),
        ],
        out_dir=os.path.join(REPO_ROOT, "hardware", "fritzing-parts"),
        board_png=os.path.join(REPO_ROOT, "docs", "src", "assets", "img", "displaymodul-pcb-top.png"),
        overlay_png=overlay_path if os.path.exists(overlay_path) else None,
    )
    print(f"Wrote {out}")
