# Enclosure

The enclosure (*Gehäuse*, housing) is the 3D-printed body that holds everything
together: the balloon and its nozzle, the display window, the card slot, and the
pump/valve mounts. It is the most visually distinctive — and by far the most
expensive — part of the project.

## Full assembly

The upper housing, lower housing, and card module fitted together — the complete
device body. These renders come straight from the parts in their shared Creo
assembly coordinates (no manual positioning), so they show exactly how the parts
mate.

<div class="gallery" markdown="span">
[![Assembly — iso](../../assets/img/enclosure/thumbs/assembly-iso-045.jpg){ loading=lazy }](../../assets/img/enclosure/assembly-iso-045.jpg){ .glightbox data-gallery="assembly" data-title="Assembly — iso 45°" }
[![Assembly — iso](../../assets/img/enclosure/thumbs/assembly-iso-135.jpg){ loading=lazy }](../../assets/img/enclosure/assembly-iso-135.jpg){ .glightbox data-gallery="assembly" data-title="Assembly — iso 135°" }
[![Assembly — front](../../assets/img/enclosure/thumbs/assembly-front.jpg){ loading=lazy }](../../assets/img/enclosure/assembly-front.jpg){ .glightbox data-gallery="assembly" data-title="Assembly — front" }
[![Assembly — top](../../assets/img/enclosure/thumbs/assembly-top.jpg){ loading=lazy }](../../assets/img/enclosure/assembly-top.jpg){ .glightbox data-gallery="assembly" data-title="Assembly — top" }
</div>

**Overall size:** ≈ 172 × 130 × 88 mm (width × depth × height), excluding the
parts on top. The [roof](roof.md), [hose nozzle](nozzle.md), and
[balloon adapter](balloon-adapter.md) — the spigot rising from the top face in
these renders — fit on top of this body; the display window drops into the top
face.

### Cutaway

Removing half the housing shows the [card module](card-module.md) nested inside
the cavity — the card slot lines up with the wall opening, and the chambers with
their drilled photoresistor bridge sit just below the top face. On the centre
axis the [balloon adapter](balloon-adapter.md) and the [hose nozzle](nozzle.md)
line up into one bore, from the balloon seat on top down to the pump tubing.

<div class="board-pair" markdown>
<figure markdown="span">
![Assembly — front cut](../../assets/img/enclosure/assembly-cut-front.jpg){ loading=lazy }
<figcaption>Front cut — card module inside the cavity</figcaption>
</figure>
<figure markdown="span">
![Assembly — side cut](../../assets/img/enclosure/assembly-cut-side.jpg){ loading=lazy }
<figcaption>Side cut — the adapter and nozzle bore on the centre axis</figcaption>
</figure>
</div>

### Split apart

Separating the device along the housing seam — the lower half seen from above, the
upper half turned over and seen from below — shows how the two halves meet: the
[card module](card-module.md) sits in the base, while the balloon
[adapter](balloon-adapter.md) and [nozzle](nozzle.md) hang from the underside of
the upper housing.

<div class="board-pair" markdown>
<figure markdown="span">
![Lower half from above](../../assets/img/enclosure/assembly-base-iso-045.jpg){ loading=lazy }
<figcaption>Lower half, from above — card module in the base</figcaption>
</figure>
<figure markdown="span">
![Upper half from below](../../assets/img/enclosure/assembly-lid-iso-045-below.jpg){ loading=lazy }
<figcaption>Upper half, from below — nozzle and adapter passage</figcaption>
</figure>
</div>

## Parts

The housing splits into several printed parts plus one cut window. Each printed
part has its own page with renders, dimensions, and CAD downloads:

| Part | German name | Role |
|---|---|---|
| [Upper housing](upper-housing.md) | *Gehäuse Oberteil* | upper body shell — carries the display window and nozzle passage |
| [Lower housing](lower-housing.md) | *Gehäuse Unterteil* | lower body shell / base — houses the pump and valve |
| [Roof](roof.md) | *Dach-Spitz* | top cap over the balloon nozzle |
| [Hose nozzle](nozzle.md) | *Schlauch-Stutzen* | balloon nozzle mount (elbow + barbed fitting) |
| [Balloon adapter](balloon-adapter.md) | *Ballon-Aufsatz* | barbed balloon-neck adapter mounted on top of the upper housing |
| [Card-module housing](card-module.md) | *Kartenmodul-Gehäuse* | tray/shroud for the card slot and optical reader |
| Display window | *Scheibe* | polycarbonate pane (Reely, cut to size) — **not printed** |

The pump and valve mount inside the lower housing; the display window is a
polycarbonate pane (Reely, from Conrad) fitted over the 7-segment display.

## CAD files

Neutral-format CAD exports are committed to the repository, so you don't need
the original Creo/Pro-E files to print or machine the parts:

- **Full-assembly STEP:**
  [`180129_bb_device_cpl_asm.stp`](https://github.com/d-rk/boomballon/blob/main/hardware/enclosure/step/180129_bb_device_cpl_asm.stp)
- **STEP** (`.stp`, for CAD/CAM):
  [`hardware/enclosure/step/`](https://github.com/d-rk/boomballon/tree/main/hardware/enclosure/step)
- **STL** (`.stl`, for slicing/printing):
  [`hardware/enclosure/stl/`](https://github.com/d-rk/boomballon/tree/main/hardware/enclosure/stl)

!!! note "The renders are reproducible"
    Every render on these pages is generated from the STL models by
    [`media/enclosure/render.py`](https://github.com/d-rk/boomballon/blob/main/media/enclosure/render.py);
    [`render-all.sh`](https://github.com/d-rk/boomballon/blob/main/media/enclosure/render-all.sh)
    regenerates the exact set. Requires Blender ≥ 4.2.
