import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from build_part import build_part  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

if __name__ == "__main__":
    # Re-verified 2026-08-14 after Kartenleser_Fotomodul.fzz was corrected
    # from a stale state that was missing the "Connector" 6-pin Displaymodul
    # pass-through (see the "Known gap" section that used to be in
    # hardware/fritzing-parts/README.md, and card-reader.md's board markings
    # table) -- it's now a real component in the sketch, drilled as its own
    # 6-hole group (Y=3286.1mil, X=97.6..597.6mil in 0.1in steps) alongside
    # FM (7-pin) and LM (2-pin). FM's and LM's Gerber-derived Y coordinates
    # also shifted slightly (~10mil) from the previously hand-verified values
    # now that the board has been corrected; X and pin-1 orientation for both
    # are unchanged.
    #
    # Pin 1 for all three headers was confirmed from the chamfered corner on
    # each header's silkscreen outline box (docs/src/assets/img/fotomodul-pcb-top.png):
    # FM's and Connector's chamfers are at their leftmost pin (x=99.1 / x=97.6);
    # LM's chamfer is at its rightmost pin (x=394.4). Connector's 6 pins carry
    # the same signals in the same order as Displaymodul's own header (a
    # straight pass-through), per docs/src/build-guide/modules/display.md.
    # Breadboard graphic: the curated docs PNG, not the tracespace vector SVG
    # -- see media/CLAUDE.md for why (Fritzing's breadboard-view renderer
    # doesn't respect the vector SVG's CSS-based styling).
    overlay_path = os.path.join(os.path.dirname(__file__), "overlays", "Fotomodul_overlay.png")
    out = build_part(
        board_fzz=os.path.join(REPO_ROOT, "hardware", "card-reader", "Kartenleser_Fotomodul.fzz"),
        board_name="Fotomodul",
        connectors=[
            # FM: 7-pin, to Steuermodul.
            ("3V3", 99.1, 3040.1),
            ("GND", 199.1, 3040.1),
            ("A0", 299.1, 3040.1),
            ("A1", 399.1, 3040.1),
            ("A2", 499.1, 3040.1),
            ("A3", 599.1, 3040.1),
            ("A4", 699.1, 3040.1),
            # LM: 2-pin, to Lichtmodul. Pin 1 is the x=394.4 end.
            ("LM1", 394.4, 87.3),
            ("LM2", 294.4, 87.3),
            # Connector: 6-pin pass-through for the Displaymodul cable.
            # Pin 1 is the leftmost pin (x=97.6, chamfer).
            ("Vcc", 97.6, 3286.1),
            ("GND", 197.6, 3286.1),
            ("DS", 297.6, 3286.1),
            ("STCP", 397.6, 3286.1),
            ("SHCP", 497.6, 3286.1),
            ("Buzzer", 597.6, 3286.1),
        ],
        out_dir=os.path.join(REPO_ROOT, "hardware", "fritzing-parts"),
        board_png=os.path.join(REPO_ROOT, "docs", "src", "assets", "img", "fotomodul-pcb-top.png"),
        overlay_png=overlay_path if os.path.exists(overlay_path) else None,
    )
    print(f"Wrote {out}")
