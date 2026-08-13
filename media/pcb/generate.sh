#!/usr/bin/env bash
# Render top/bottom PCB-view PNGs from the Fritzing sketches (hardware/**/*.fzz)
# and sync them into the docs assets.
#
# Pipeline: Fritzing's `-gerber` export (run with the Qt offscreen platform,
# so no window ever appears) turns each .fzz into real Gerber + drill files,
# then the open-source tracespace renderer (the same Gerber-rendering approach
# fab-house viewers like PCBWay's use — their page credits tracespace author
# Mike Cousins) turns those into realistic green-soldermask/gold-pad board
# SVGs, which Inkscape rasterizes to PNG.
#
# Requires:
#   - The Fritzing flatpak: flatpak install --user flathub org.fritzing.Fritzing
#     Its sandbox needs read access to hardware/ and write access to /tmp:
#       flatpak override --user \
#         --filesystem="$(pwd)/hardware:ro" --filesystem=/tmp \
#         org.fritzing.Fritzing
#   - Node (npm install run once in this folder to pin @tracespace/cli).
#   - Inkscape, on PATH.
set -euo pipefail
cd "$(dirname "$0")/../.."   # repo root

# QT_QPA_PLATFORM=offscreen keeps Fritzing's Qt UI from ever opening a window
# — without it, -gerber still exports headlessly but flashes the GUI on screen.
FRITZING="${FRITZING:-flatpak run --user --env=QT_QPA_PLATFORM=offscreen org.fritzing.Fritzing}"
TRACESPACE="media/pcb/node_modules/.bin/tracespace"
DPI=431   # matches the pixel density of the original hand-exported PCB images
outdir="docs/src/assets/img"

if [[ ! -x "$TRACESPACE" ]]; then
  echo "tracespace CLI not found — run 'npm install' in media/pcb/ first." >&2
  exit 1
fi

tmpdir=$(mktemp -d /tmp/boomballon-pcb-render.XXXXXX)
trap 'rm -rf "$tmpdir"' EXIT

# source .fzz -> output basename (docs/src/assets/img/<name>-pcb-{top,bottom}.png)
boards=(
  "hardware/control/Steuermodul.fzz:steuermodul"
  "hardware/card-reader/Kartenleser_Fotomodul.fzz:fotomodul"
  "hardware/card-reader/Kartenleser_Lichtmodul.fzz:lichtmodul"
  "hardware/display/Displaymodul.fzz:displaymodul"
)

for entry in "${boards[@]}"; do
  src="${entry%%:*}"
  name="${entry##*:}"
  base=$(basename "$src" .fzz)
  work="$tmpdir/$name"
  mkdir -p "$work"
  cp "$src" "$work/"

  echo "== $name: exporting Gerbers from $src =="
  $FRITZING -gerber "$work" >/dev/null

  gerbers=(
    "$work/${base}_copperTop.gtl" "$work/${base}_copperBottom.gbl"
    "$work/${base}_maskTop.gts" "$work/${base}_maskBottom.gbs"
    "$work/${base}_silkTop.gto" "$work/${base}_silkBottom.gbo"
    "$work/${base}_contour.gm1" "$work/${base}_drill.txt"
  )

  echo "== $name: rendering with tracespace =="
  "$TRACESPACE" "${gerbers[@]}" -o "$work/rendered/" >/dev/null

  echo "== $name: rasterizing to PNG =="
  inkscape --export-type=png --export-dpi="$DPI" \
    --export-filename="$outdir/${name}-pcb-top.png" \
    "$work/rendered/${base}_.top.svg" >/dev/null
  inkscape --export-type=png --export-dpi="$DPI" \
    --export-filename="$outdir/${name}-pcb-bottom.png" \
    "$work/rendered/${base}_.bottom.svg" >/dev/null
done

echo "Done. Wrote PNGs to $outdir/"
