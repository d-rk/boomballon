import os
import re
import zipfile

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
STEUERMODUL_FZZ = os.path.join(REPO_ROOT, "hardware", "control", "Steuermodul.fzz")
OUT_DIR = os.path.join(REPO_ROOT, "hardware", "system-wiring")
OUT_FZZ = os.path.join(OUT_DIR, "BoomBalloon-Wiring.fzz")

# (moduleIdRef, title, x, y) -- positions are a starting layout, not final;
# Dirk repositions freely in the Fritzing GUI before wiring.
NEW_INSTANCES = [
    ("BoomBalloon_Fotomodul", "Fotomodul", 900, 200),
    ("BoomBalloon_Lichtmodul", "Lichtmodul", 900, 600),
    ("BoomBalloon_Displaymodul", "Displaymodul", -400, 200),
    ("SparkFun-Connectors-POWER_JACK-PTH", "Power Jack", -400, -300),
]


def _max_model_index(fz_text: str) -> int:
    indices = [int(m) for m in re.findall(r'modelIndex="(\d+)"', fz_text)]
    return max(indices) if indices else 0


def _instance_xml(module_id: str, title: str, model_index: int, x: float, y: float) -> str:
    return f'''<instance moduleIdRef="{module_id}" modelIndex="{model_index}" path="">
            <title>{title}</title>
            <views>
                <breadboardView layer="breadboard">
                    <geometry z="1" x="{x}" y="{y}"/>
                </breadboardView>
            </views>
        </instance>
'''


def main():
    os.makedirs(OUT_DIR, exist_ok=True)

    with zipfile.ZipFile(STEUERMODUL_FZZ) as z:
        inner_name = [n for n in z.namelist() if n.endswith(".fz")][0]
        fz_text = z.read(inner_name).decode("utf-8")

    next_index = _max_model_index(fz_text) + 1
    new_blocks = []
    for module_id, title, x, y in NEW_INSTANCES:
        new_blocks.append(_instance_xml(module_id, title, next_index, x, y))
        next_index += 1

    insertion_point = fz_text.rindex("</instances>")
    fz_text = fz_text[:insertion_point] + "".join(new_blocks) + fz_text[insertion_point:]

    with zipfile.ZipFile(OUT_FZZ, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr(inner_name.replace("Steuermodul", "BoomBalloon-Wiring"), fz_text)

    print(f"Wrote {OUT_FZZ}")


if __name__ == "__main__":
    main()
