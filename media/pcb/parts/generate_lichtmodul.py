import os
import sys

sys.path.insert(0, os.path.dirname(__file__))
from build_part import build_part  # noqa: E402

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

if __name__ == "__main__":
    out = build_part(
        board_fzz=os.path.join(REPO_ROOT, "hardware", "card-reader", "Kartenleser_Lichtmodul.fzz"),
        board_name="Lichtmodul",
        connectors=[
            ("LM1", 393, 99),  # pin 1, square pad -- power for the LEDs
            ("LM2", 493, 99),  # pin 2
        ],
        out_dir=os.path.join(REPO_ROOT, "hardware", "fritzing-parts"),
    )
    print(f"Wrote {out}")
