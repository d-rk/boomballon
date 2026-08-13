import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from build_part import build_part  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

if __name__ == "__main__":
    # This is the bare board only -- no Arduino connectors of its own. Its
    # breadboard graphic (steuermodul-pcb-top.png) already shows the real,
    # unpopulated two-row Arduino Micro footprint at true physical scale (the
    # same DPI convention as every other part here), so a real Fritzing
    # "Arduino Micro" part dragged on top and positioned by eye lines up with
    # it directly -- the board was physically designed for that exact
    # footprint (see control-board.md: "footprint drawn for an Arduino Micro").
    #
    # 5 real external connectors identified from the Gerber drill data,
    # cross-referenced against control-board.md's board markings table and
    # confirmed visually (chamfer/square-pad pin-1 markers) against
    # docs/src/assets/img/steuermodul-pcb-top.png:
    #
    # - VA/MA and VM/MM are each a single physical 2-pin connector that pairs
    #   one valve wire with one motor wire (VA+MA on the left, VM+MM on the
    #   right) -- confirmed by direct silkscreen label position, not assumed;
    #   it looks unusual (you'd expect MA+MM together for one pump) but
    #   that's genuinely how the board is laid out.
    # - Vcc12V pin 1 (chamfer at top) is assumed +12V, pin 2 GND -- the
    #   board markings table doesn't call out polarity explicitly, this is
    #   the standard convention, not verified against a photo.
    # - Fotomodul pin 1 (chamfer at bottom of that header) and Displaymodul
    #   pin 1 (chamfer at top of that header) both confirmed visually; pin
    #   order beyond pin 1 taken from the existing wiring.md/display.md pin
    #   tables (Fotomodul: Vcc,GND,A0-A4; Displaymodul: Vcc,GND,DS,STCP,SHCP,Buzzer).
    #
    # Excluded on purpose: the "Motor"/"Ablassmotor" TIP120 3-pin footprints,
    # D1/D2, R1/R2 -- these are fixed, already-populated components baked
    # into the board image, not something a system wiring diagram connects
    # external wires to.
    overlay_path = os.path.join(os.path.dirname(__file__), "overlays", "Steuermodul_overlay.png")
    out = build_part(
        board_fzz=os.path.join(REPO_ROOT, "hardware", "control", "Steuermodul.fzz"),
        board_name="Steuermodul",
        connectors=[
            # VA/MA: 2-pin, left side (valve + motor terminal pair).
            ("VA", 235.9, 2319.6),
            ("MA", 235.9, 2219.6),
            # VM/MM: 2-pin, right side (valve + motor terminal pair).
            ("VM", 1335.9, 2319.6),
            ("MM", 1335.9, 2219.6),
            # Vcc12V: 2-pin, chamfer/pin1 at top -- assumed +12V/GND.
            ("+12V", 1635.9, 1719.6),
            ("GND_12V", 1635.9, 1619.6),
            # Fotomodul: 7-pin, chamfer/pin1 at the bottom of this header.
            ("FM_3V3", 1635.9, 419.6),
            ("FM_GND", 1635.9, 519.6),
            ("FM_A0", 1635.9, 619.6),
            ("FM_A1", 1635.9, 719.6),
            ("FM_A2", 1635.9, 819.6),
            ("FM_A3", 1635.9, 919.6),
            ("FM_A4", 1635.9, 1019.6),
            # Displaymodul: 6-pin, chamfer/pin1 at the top of this header.
            ("DM_Vcc", 235.9, 819.6),
            ("DM_GND", 235.9, 719.6),
            ("DM_DS", 235.9, 619.6),
            ("DM_STCP", 235.9, 519.6),
            ("DM_SHCP", 235.9, 419.6),
            ("DM_Buzzer", 235.9, 319.6),
        ],
        out_dir=os.path.join(REPO_ROOT, "hardware", "fritzing-parts"),
        board_png=os.path.join(REPO_ROOT, "docs", "src", "assets", "img", "steuermodul-pcb-top.png"),
        overlay_png=overlay_path if os.path.exists(overlay_path) else None,
    )
    print(f"Wrote {out}")
