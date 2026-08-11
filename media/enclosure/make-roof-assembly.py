"""Bootstrap media/enclosure/roof-assembly.blend - how the roof mounts.

The roof does not rest on the upper housing: it floats ~140 mm above it,
carried by four bent Knicklichter (flexible glow sticks). Each stick seats in
one of the housing's four corner holes (d 8 mm, 3.7 mm deep, at +-61.5/+-53.9)
and bends inward to plug into one of the roof's four downward-facing leg bores
(d 5.7 mm, 5.9 mm deep, openings at +-16.4/+-13.9, 164.8 mm up, tilted 14.5
degrees outward). Both STLs already share Creo assembly coordinates - the roof
is stored at its mounted height - so this script only adds the four sticks as
live bezier tubes and saves the editable .blend (Z-up world, millimetres).

The .blend is the source of truth for the stick shapes: the curves were
hand-tuned in Blender after bootstrapping, so this script REFUSES to overwrite
an existing .blend. Derive the committed STLs from the .blend with
export-roof-assembly.py.

Run from the repo root:
    blender -b --factory-startup -P media/enclosure/make-roof-assembly.py
"""

import bpy, math, os, sys
from mathutils import Vector, Euler

STL_DIR = "hardware/enclosure/stl"

# measured in Z-up assembly coords (raw STL Y-up rotated X+90)
HOLE = Vector((61.5, 53.9, 26.8))      # corner hole seat (floor + 0.5)
BORE = Vector((16.4, 13.9, 164.8))     # leg bore opening
INSERT = 1.2                           # tip just seated in the opening
TUBE_R = 2.75                          # Knicklicht d 5.5
TILT0 = 0.14                           # launch: near-vertical, slight outward lean
TILT1 = 1.05                           # arrival: leaning inward (~47 deg tilt)
H0, H1 = 108.0, 40.0                   # bezier handle lengths (launch / arrival)
BLEND = "media/enclosure/roof-assembly.blend"


def import_stl(basename):
    bpy.ops.wm.stl_import(filepath=f"{STL_DIR}/{basename}.stl")
    o = [o for o in bpy.context.selected_objects if o.type == 'MESH'][0]
    o.rotation_euler = Euler((math.radians(90), 0, 0))  # Y-up -> Z-up
    bpy.context.view_layer.objects.active = o
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)
    return o


def seg_len(a, ha, hb, b, n=64):
    length, prev = 0.0, None
    for i in range(n + 1):
        t = i / n
        q = (a * (1 - t) ** 3 + ha * 3 * t * (1 - t) ** 2
             + hb * 3 * t * t * (1 - t) + b * t ** 3)
        if prev is not None:
            length += (q - prev).length
        prev = q
    return length


def knicklicht(sx, sy):
    """One bent glow stick from the (sx,sy) corner hole into the same-quadrant leg.

    A single outward-bowed C-arc, like a real flexed rod: it leaves the corner
    hole near-vertically with a slight outward lean, bulges outward around mid
    height, and sweeps in to the leg bore arriving tilted toward the centre,
    its tip just inside the opening.
    """
    p0 = Vector((sx * HOLE.x, sy * HOLE.y, HOLE.z))
    out = Vector((sx * HOLE.x, sy * HOLE.y, 0)).normalized()  # corner direction
    t0 = (Vector((0, 0, 1)) + out * TILT0).normalized()
    t1 = (Vector((0, 0, 1)) - out * TILT1).normalized()
    p1 = Vector((sx * BORE.x, sy * BORE.y, BORE.z)) + t1 * INSERT

    name = f"knicklicht-{'n' if sy > 0 else 's'}{'e' if sx > 0 else 'w'}"
    cu = bpy.data.curves.new(name, 'CURVE')
    cu.dimensions = '3D'
    cu.resolution_u = 24
    cu.bevel_depth = TUBE_R
    cu.bevel_resolution = 8
    cu.use_fill_caps = True
    sp = cu.splines.new('BEZIER')
    sp.bezier_points.add(1)
    b0, b1 = sp.bezier_points
    b0.co = p0
    b0.handle_left = p0 - t0 * H0
    b0.handle_right = p0 + t0 * H0
    b1.co = p1
    b1.handle_left = p1 - t1 * H1
    b1.handle_right = p1 + t1 * H1
    o = bpy.data.objects.new(name, cu)
    bpy.context.collection.objects.link(o)

    length = seg_len(b0.co, b0.handle_right, b1.handle_left, b1.co)
    print(f"knicklicht ({sx:+d},{sy:+d}): path length {length:.1f} mm")
    return o


if os.path.exists(BLEND):
    print(f"REFUSING to overwrite hand-tuned {BLEND}; "
          f"use export-roof-assembly.py to derive the STLs from it.")
    sys.exit(1)

bpy.ops.wm.read_factory_settings(use_empty=True)
housing = import_stl("upper-housing")
housing.name = "upper-housing"
roof = import_stl("roof")
roof.name = "roof"

for sx in (-1, 1):
    for sy in (-1, 1):
        knicklicht(sx, sy)

# separate objects, sticks as live bezier curves: tweak handles in Blender,
# then run export-roof-assembly.py
bpy.ops.wm.save_as_mainfile(filepath=BLEND)
print(f"WROTE {BLEND}")
