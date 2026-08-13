import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from build_part import build_part  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

if __name__ == "__main__":
    # Only 19 drilled holes exist in this board's Gerbers: 10 for the 5
    # photoresistors (F1-F5, not connectors) + 7 for FM + 2 for LM. The
    # "Connector" 6-pin Displaymodul pass-through described in
    # card-reader.md's board markings table is real and visible on the
    # physical assembled boards (docs/src/assets/img/gallery/fotomodul-assembled.jpg
    # clearly shows a 6-pin "Connector" header below FM), but it is NOT a
    # component in Kartenleser_Fotomodul.fzz -- confirmed by listing every
    # moduleIdRef instance in the sketch (only "Verbinder 2 Pins" and
    # "Verbinder 7 Pins" headers exist). The design file and the as-built
    # boards have drifted apart for this one connector. Rather than invent
    # coordinates with no Gerber data behind them, it's omitted here.
    #
    # Pin 1 for both real headers was confirmed from the chamfered corner on
    # each header's silkscreen outline box (docs/src/assets/img/fotomodul-pcb-top.png):
    # FM's chamfer is at its leftmost pin (x=99.1); LM's chamfer is at its
    # rightmost pin (x=394.4).
    # Breadboard graphic: the curated docs PNG, not the tracespace vector SVG
    # -- see media/CLAUDE.md for why (Fritzing's breadboard-view renderer
    # doesn't respect the vector SVG's CSS-based styling).
    overlay_path = os.path.join(os.path.dirname(__file__), "overlays", "Fotomodul_overlay.png")
    out = build_part(
        board_fzz=os.path.join(REPO_ROOT, "hardware", "card-reader", "Kartenleser_Fotomodul.fzz"),
        board_name="Fotomodul",
        connectors=[
            # FM: 7-pin, to Steuermodul.
            ("3V3", 99.1, 3050),
            ("GND", 199.1, 3050),
            ("A0", 299.1, 3050),
            ("A1", 399.1, 3050),
            ("A2", 499.1, 3050),
            ("A3", 599.1, 3050),
            ("A4", 699.1, 3050),
            # LM: 2-pin, to Lichtmodul. Pin 1 is the x=394.4 end.
            ("LM1", 394.4, 97.2),
            ("LM2", 294.4, 97.2),
        ],
        out_dir=os.path.join(REPO_ROOT, "hardware", "fritzing-parts"),
        board_png=os.path.join(REPO_ROOT, "docs", "src", "assets", "img", "fotomodul-pcb-top.png"),
        overlay_png=overlay_path if os.path.exists(overlay_path) else None,
    )
    print(f"Wrote {out}")
