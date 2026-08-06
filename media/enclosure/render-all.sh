#!/usr/bin/env bash
#
# Regenerate every image under media/enclosure/renders/ from the STL source
# models in hardware/enclosure/stl/, using render.py. This is the reproducible
# definition of the enclosure render set — edit the calls below to change it.
#
# Requires Blender >= 4.2. If blender isn't on your PATH, point BLENDER at it:
#   BLENDER=/opt/blender-4.2/blender ./media/enclosure/render-all.sh
#
set -euo pipefail
cd "$(dirname "$0")/../.."                     # repo root
BLENDER="${BLENDER:-blender}"
R="media/enclosure/render.py"
OUT="media/enclosure/renders"
CLAY=(--res 1200 --samples 64)
CUT=(--res 1200 --samples 100)
r() { "$BLENDER" -b --factory-startup -P "$R" -- "$@" --out "$OUT"; }

# STL basenames (hardware/enclosure/stl/<name>.stl)
GOT=180820_bb_got                              # upper housing
GUT=180123_bb_gut                              # lower housing
DACH=180123_bb_dach                            # roof
STUTZEN=180123_bb_got_stutzen                  # hose nozzle
KM=180123_bb_karten-modul_gehaeuse             # card-module housing
AUFSATZ=260805_got_ballon_aufsatz              # balloon-neck adapter
STUTZEN_ASM=260805_bb_got_stutzen              # hose nozzle, repositioned into assembly coords

# --- upper housing ---
r --target $GOT --name got --mode clay --views top,iso-045 "${CLAY[@]}"
r --target $GOT --name got --mode clay --views bottom --flip "${CLAY[@]}"

# --- lower housing ---
r --target $GUT --name gut --mode clay --views top,iso-045,iso-180,iso-315 "${CLAY[@]}"

# --- roof ---
r --target $DACH --name dach --mode clay --views iso-000,left "${CLAY[@]}"
r --target $DACH --name dach --mode clay --views bottom --flip "${CLAY[@]}"

# --- hose nozzle ---
r --target $STUTZEN --name stutzen --mode clay --views iso-000,iso-045 "${CLAY[@]}"
r --target $STUTZEN --name stutzen --mode clay --views bottom --flip "${CLAY[@]}"

# --- card-module housing ---
r --target $KM --name kartenmodul --mode clay --views back,left,iso-000,iso-045,iso-090,iso-135 "${CLAY[@]}"
r --target $KM --name kartenmodul --mode clay --views bottom --flip "${CLAY[@]}"
r --target $KM --name kartenmodul --mode cut --cut front "${CUT[@]}"
r --target $KM --name kartenmodul --mode cut --cut side "${CUT[@]}"

# --- balloon adapter ---
r --target $AUFSATZ --name aufsatz --mode clay --views iso-045,front,top "${CLAY[@]}"
r --target $AUFSATZ --name aufsatz --mode clay --views bottom --flip "${CLAY[@]}"
r --target $AUFSATZ --name aufsatz --mode cut --cut side "${CUT[@]}"
r --target assembly --parts $AUFSATZ,$STUTZEN_ASM --name aufsatz-nozzle --mode cut --cut side "${CUT[@]}"

# --- full assembly (got + gut + kartenmodul + adapter, shared Creo assembly coords) ---
r --target assembly --name assembly --mode clay --views front,top,iso-045,iso-135 "${CLAY[@]}"
r --target assembly --name assembly --mode cut --cut front "${CUT[@]}"
r --target assembly --name assembly --mode cut --cut side "${CUT[@]}"

# --- assembly split open: lower half from above, upper half from below ---
r --target assembly --parts $GUT,$KM --name assembly-base --mode clay --views iso-045 "${CLAY[@]}"
r --target assembly --parts $GOT,$STUTZEN_ASM,$AUFSATZ --name assembly-lid --no-floor \
    --mode clay --views iso-045-below "${CLAY[@]}"

echo "Done. $(ls "$OUT"/*.png | wc -l) images in $OUT"
