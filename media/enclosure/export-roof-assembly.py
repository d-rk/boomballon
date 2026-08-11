"""Derive the roof-mount STLs from the hand-tuned roof-assembly.blend.

The .blend (see make-roof-assembly.py) holds the upper housing, the roof, and
the four Knicklichter as live bezier tubes, in Z-up world millimetres. This
exports, in the shared Y-up assembly frame of the other STLs:

  hardware/enclosure/stl/knicklichter.stl   the four bent sticks alone
                                            (rendered white-translucent)
  hardware/enclosure/stl/roof-assembly.stl  housing + roof + sticks merged
                                            (the CAD download)

Run from the repo root:
    blender -b --factory-startup -P media/enclosure/export-roof-assembly.py
"""

import bpy, math
from mathutils import Euler

BLEND = "media/enclosure/roof-assembly.blend"
STL_DIR = "hardware/enclosure/stl"

bpy.ops.wm.open_mainfile(filepath=BLEND)

sticks, rest = [], []
for o in list(bpy.data.objects):
    if o.type == 'CURVE' and o.name.startswith("knicklicht"):
        sp = o.data.splines[0]
        print(f"{o.name}: stick length {sp.calc_length():.1f} mm")
        bpy.ops.object.select_all(action='DESELECT')
        o.select_set(True)
        bpy.context.view_layer.objects.active = o
        bpy.ops.object.convert(target='MESH')
        sticks.append(bpy.context.active_object)
    elif o.type == 'MESH':
        rest.append(o)

# bake any object transforms, then rotate everything about the world origin
# into the Y-up frame of the source STLs
for o in sticks + rest:
    bpy.ops.object.select_all(action='DESELECT')
    o.select_set(True)
    bpy.context.view_layer.objects.active = o
    bpy.ops.object.transform_apply(location=True, rotation=True, scale=True)
    o.rotation_euler = Euler((math.radians(-90), 0, 0))  # Z-up -> Y-up
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)

bpy.ops.object.select_all(action='DESELECT')
for o in sticks:
    o.select_set(True)
bpy.ops.wm.stl_export(filepath=f"{STL_DIR}/knicklichter.stl",
                      export_selected_objects=True)
print(f"WROTE {STL_DIR}/knicklichter.stl")

for o in rest:
    o.select_set(True)
bpy.ops.wm.stl_export(filepath=f"{STL_DIR}/roof-assembly.stl",
                      export_selected_objects=True)
print(f"WROTE {STL_DIR}/roof-assembly.stl")
