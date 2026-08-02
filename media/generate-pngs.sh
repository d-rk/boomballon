#!/usr/bin/env bash
# Regenerate the PNG assets from the SVG sources in media/logo/ and sync the
# copies used by the docs site. Pass --docs to also rebuild the MkDocs site.
#
# Requires: inkscape, imagemagick (magick)
set -euo pipefail
cd "$(dirname "$0")"

tmpdir=$(mktemp -d)
trap 'rm -rf "$tmpdir"' EXIT

# A nested <svg> in a logo silently clips in browsers but not in Inkscape, so
# normalise the sources before anything is rendered or published from them.
echo "Checking logo SVGs for nested viewports ..."
python3 flatten-svg-viewports.py logo/logo.svg logo/logo-icon.svg

echo "Rendering logo.png from logo/logo.svg ..."
inkscape logo/logo.svg --export-type=png --export-width=1200 -o logo/logo.png

for size in 64 128 256; do
  echo "Rendering favicon-$size.png from logo/logo-icon.svg ..."
  inkscape logo/logo-icon.svg --export-type=png \
    --export-width="$size" --export-height="$size" -o "logo/favicon-$size.png"
done

echo "Rendering docs background tile from cards/artwork/muster.svg ..."
# Seamless tile: the weave repeats roughly every 92x77 px at this render
# width; the crop below (544x679 at +111+68) was fitted so its edges wrap
# onto each other with minimal mismatch, so the tile repeats naturally with
# no mirroring. Used as the docs header backdrop, where a scrim in
# docs/src/assets/extra.css darkens it for legibility.
#
# The black grout lines are recoloured to a dark brown. They are ~17% of the
# tile, and leaving them black is what made the header hard to read: the
# background flickered between near-black and mid-orange behind every letter.
# Lifting them cuts that luminance variance by about a quarter. Enlarging the
# pattern instead does not help - it only gives the dark bars more area.
#
# Note: do not add -colorspace Gray here - combined with the alpha channel it
# makes the PNG8 quantizer collapse the whole tile to a single colour.
inkscape ../cards/artwork/muster.svg --export-type=png --export-width=1000 \
  -o "$tmpdir/muster-full.png"
magick "$tmpdir/muster-full.png" -crop 544x679+111+68 +repage \
  -fuzz 22% -fill '#7a3a06' -opaque '#010100' \
  -strip -colors 32 PNG8:../docs/src/assets/img/muster-tile.png

echo "Rendering docs header wordmark ..."
# The header title as artwork, set in the logo's own face (Snap ITC, shipped
# in cards/fonts/) so the docs match the card lettering. Rendered white on
# transparent at ~5x the display size for hidpi; extra.css swaps it in for
# the site-name text, which stays in the DOM for screen readers.
magick -background none -fill white -font ../cards/fonts/SNAP____.TTF \
  -pointsize 120 label:"Boom Balloon" -trim +repage \
  -bordercolor none -border 4 -strip \
  ../docs/src/assets/img/wordmark.png

echo "Syncing docs copies ..."
cp logo/logo.svg logo/logo-icon.svg logo/favicon-128.png ../docs/src/assets/img/

if [[ "${1:-}" == "--docs" ]]; then
  echo "Rebuilding docs site ..."
  (cd ../docs && ../.venv-docs/bin/mkdocs build)
fi

echo "Done."
