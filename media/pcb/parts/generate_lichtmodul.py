import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from build_part import build_part  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

if __name__ == "__main__":
    # Breadboard graphic: the curated docs PNG, not the tracespace vector SVG
    # -- see media/CLAUDE.md for why (Fritzing's breadboard-view renderer
    # doesn't respect the vector SVG's CSS-based styling).
    overlay_path = os.path.join(os.path.dirname(__file__), "overlays", "Lichtmodul_overlay.png")
    out = build_part(
        board_fzz=os.path.join(REPO_ROOT, "hardware", "card-reader", "Kartenleser_Lichtmodul.fzz"),
        board_name="Lichtmodul",
        connectors=[
            ("LM1", 393, 99),  # pin 1, square pad -- power for the LEDs
            ("LM2", 493, 99),  # pin 2
        ],
        out_dir=os.path.join(REPO_ROOT, "hardware", "fritzing-parts"),
        board_png=os.path.join(REPO_ROOT, "docs", "src", "assets", "img", "lichtmodul-pcb-top.png"),
        overlay_png=overlay_path if os.path.exists(overlay_path) else None,
    )
    print(f"Wrote {out}")
