"""Render an enclosure part (or the full assembly) from its STL source.

Headless Blender script — driven by render-all.sh, which regenerates the exact
set of images under media/enclosure/renders/. Requires Blender >= 4.2.

Usage:
    blender -b --factory-startup -P render.py -- \
        --target <stl-basename|assembly> --name <prefix> \
        --mode clay|wire|xray|cut [--views top,iso-045,...] [--flip] \
        [--cut front|back|side|left] --out <dir> [--res N --samples N --color r,g,b]

Notes on orientation:
  * A single part is laid flat automatically (its thinnest bounding-box axis is
    rotated to vertical). --flip adds a 180 deg roll so the "bottom" view shows
    the shell interior (otherwise the floor hides it).
  * "assembly" loads the upper housing, lower housing, card-module housing,
    and balloon adapter, which share Creo assembly coordinates, and stands
    them upright (their Y axis becomes Z).
"""

import bpy, sys, math, os, argparse
from mathutils import Vector, Euler

STL_DIR = "hardware/enclosure/stl"
ASSEMBLY_PARTS = ["upper-housing", "lower-housing", "card-module-housing",
                  "balloon-adapter", "hose-nozzle"]


def parse_args():
    argv = sys.argv[sys.argv.index("--") + 1:]
    p = argparse.ArgumentParser()
    p.add_argument("--target", required=True)
    p.add_argument("--parts", default="",
                   help="comma-separated STL basenames to render as an assembly subset "
                        "(in shared Creo coords); overrides the default assembly part list")
    p.add_argument("--no-floor", dest="floor", action="store_false",
                   help="omit the floor plane and add an under-light, so iso-*-below "
                        "cameras can look up at the model's underside")
    p.add_argument("--name", required=True)
    p.add_argument("--mode", default="clay", choices=["clay", "wire", "xray", "cut"])
    p.add_argument("--views", default="")
    p.add_argument("--cut", default="front", choices=["front", "back", "side", "left"])
    p.add_argument("--flip", action="store_true")
    p.add_argument("--no-edges", dest="edges", action="store_false",
                   help="skip the freestyle outline on wire/cut renders (pure clay)")
    p.add_argument("--out", required=True)
    p.add_argument("--res", type=int, default=1200)
    p.add_argument("--samples", type=int, default=64)
    p.add_argument("--color", default="0.52,0.37,0.25")
    p.add_argument("--xray-edge", type=float, default=0.55)
    p.add_argument("--xray-face", type=float, default=0.05)
    return p.parse_args(argv)


def import_stl(basename):
    bpy.ops.wm.stl_import(filepath=f"{STL_DIR}/{basename}.stl")
    return [o for o in bpy.context.selected_objects if o.type == 'MESH'][0]


def apply_rotation(o, euler):
    o.rotation_euler = euler
    bpy.context.view_layer.objects.active = o
    bpy.ops.object.transform_apply(location=False, rotation=True, scale=True)


def world_bbox(o):
    cs = [o.matrix_world @ Vector(c) for c in o.bound_box]
    mn = Vector([min(c[i] for c in cs) for i in range(3)])
    mx = Vector([max(c[i] for c in cs) for i in range(3)])
    return mn, mx


def part_views():
    v = {"top": Vector((0, -0.12, 1)), "bottom": Vector((0, -0.12, 1)),
         "left": Vector((-1, 0, 0.12)), "right": Vector((1, 0, 0.12)),
         "front": Vector((0, -1, 0.12)), "back": Vector((0, 1, 0.12))}
    el = math.radians(35)
    for az in range(0, 360, 45):
        a = math.radians(az)
        v[f"iso-{az:03d}"] = Vector((math.cos(a)*math.cos(el), math.sin(a)*math.cos(el), math.sin(el)))
    return v


def assembly_views():
    v = {"top": Vector((0, -0.12, 1)), "front": Vector((0, -1, 0.15)), "side": Vector((1, 0, 0.15))}
    el = math.radians(30)
    for az in range(0, 360, 45):
        a = math.radians(az)
        c = math.cos(a) * math.cos(el); s = math.sin(a) * math.cos(el)
        v[f"iso-{az:03d}"] = Vector((c, s, math.sin(el)))          # looking down from above
        v[f"iso-{az:03d}-below"] = Vector((c, s, -math.sin(el)))   # looking up from below
    return v


def main():
    args = parse_args()
    color = tuple(float(x) for x in args.color.split(","))
    parts = args.parts.split(",") if args.parts else ASSEMBLY_PARTS
    is_asm = args.target == "assembly" or bool(args.parts)
    os.makedirs(args.out, exist_ok=True)

    bpy.ops.wm.read_factory_settings(use_empty=True)
    scene = bpy.context.scene

    # ---- import + orient ----
    objs = []
    if is_asm:
        for b in parts:
            o = import_stl(b)
            apply_rotation(o, Euler((math.radians(90), 0, 0)))   # Y-up -> Z-up
            objs.append(o)
    else:
        o = import_stl(args.target)
        mn, mx = world_bbox(o); d = mx - mn
        s = min(range(3), key=lambda i: d[i])                     # lay flattest axis to Z
        rot = Euler((0, math.radians(-90), 0)) if s == 0 else Euler((math.radians(90), 0, 0)) if s == 1 else Euler((0, 0, 0))
        o.rotation_euler = rot
        if args.flip:
            o.rotation_euler.rotate(Euler((math.radians(180), 0, 0)))
        apply_rotation(o, o.rotation_euler)
        objs.append(o)

    # ---- center on origin, sit on floor (one shared transform for all parts) ----
    mns, mxs = zip(*(world_bbox(o) for o in objs))
    mn = Vector([min(m[i] for m in mns) for i in range(3)])
    mx = Vector([max(m[i] for m in mxs) for i in range(3)])
    dims = mx - mn
    center = (mn + mx) / 2.0
    offset = Vector((center.x, center.y, mn.z))
    for o in objs:
        o.location -= offset
    bpy.context.view_layer.update()
    h = dims.z
    tgt_pt = Vector((0, 0, h / 2.0))
    radius = 0.5 * math.sqrt(dims.x**2 + dims.y**2 + dims.z**2)

    # ---- optional half-cut ----
    # Cut each part separately, before joining: the EXACT boolean solver needs a
    # single closed solid, so it silently no-ops on a merged multi-part mesh (the
    # overlapping shells where parts mate read as non-solid). Each STL on its own
    # is a clean manifold, so a per-part difference against the same half-space
    # cube slices them all consistently.
    if args.mode == "cut":
        S = radius * 10
        cut_loc = {"front": (0, S/2, h/2), "back": (0, -S/2, h/2),
                   "side": (S/2, 0, h/2), "left": (-S/2, 0, h/2)}
        bpy.ops.mesh.primitive_cube_add(size=1)
        cutter = bpy.context.active_object; cutter.scale = (S, S, S)
        cutter.location = cut_loc[args.cut]
        for o in objs:
            bpy.ops.object.select_all(action='DESELECT')
            o.select_set(True)
            bpy.context.view_layer.objects.active = o
            m = o.modifiers.new("cut", 'BOOLEAN'); m.operation = 'DIFFERENCE'
            m.solver = 'EXACT'; m.object = cutter
            bpy.ops.object.modifier_apply(modifier="cut")
        bpy.data.objects.remove(cutter, do_unlink=True)

    # ---- join for one material + render ----
    with bpy.context.temp_override(active_object=objs[0], selected_editable_objects=objs):
        bpy.ops.object.join()
    obj = objs[0]; obj.name = args.name

    # ---- clay material (+ optional x-ray) ----
    mat = bpy.data.materials.new("clay"); mat.use_nodes = True
    b = mat.node_tree.nodes["Principled BSDF"]
    b.inputs["Base Color"].default_value = (*color, 1.0)
    b.inputs["Roughness"].default_value = 0.5
    b.inputs["Metallic"].default_value = 0.0
    if args.mode == "xray":
        nt = mat.node_tree
        lw = nt.nodes.new("ShaderNodeLayerWeight"); lw.inputs["Blend"].default_value = 0.35
        mr = nt.nodes.new("ShaderNodeMapRange")
        mr.inputs["To Min"].default_value = args.xray_edge
        mr.inputs["To Max"].default_value = args.xray_face
        nt.links.new(lw.outputs["Facing"], mr.inputs["Value"])
        nt.links.new(mr.outputs["Result"], b.inputs["Alpha"])
        b.inputs["Roughness"].default_value = 0.35
        scene.cycles.transparent_max_bounces = 24
    obj.data.materials.clear(); obj.data.materials.append(mat)
    for poly in obj.data.polygons:
        poly.use_smooth = False

    # ---- floor, world, lights ----
    if args.floor:
        bpy.ops.mesh.primitive_plane_add(size=radius * 40, location=(0, 0, 0))
        fm = bpy.data.materials.new("floor"); fm.use_nodes = True
        fbs = fm.node_tree.nodes["Principled BSDF"]
        fbs.inputs["Base Color"].default_value = (0.82, 0.82, 0.85, 1)
        fbs.inputs["Roughness"].default_value = 0.95
        bpy.context.active_object.data.materials.append(fm)

    world = bpy.data.worlds.new("studio"); scene.world = world; world.use_nodes = True
    bg = world.node_tree.nodes["Background"]
    bg.inputs["Color"].default_value = (0.80, 0.81, 0.85, 1)
    bg.inputs["Strength"].default_value = 1.0

    tgt = bpy.data.objects.new("target", None); tgt.location = tgt_pt
    scene.collection.objects.link(tgt)

    def add_area(nm, loc, energy, size):
        ld = bpy.data.lights.new(nm, 'AREA'); ld.energy = energy; ld.size = size
        lo = bpy.data.objects.new(nm, ld); lo.location = loc
        scene.collection.objects.link(lo); lo.constraints.new('TRACK_TO').target = tgt

    if args.mode == "cut":  # light the cut cavity from the camera side
        add_area("key",  ( radius*3,  radius*3,  radius*4.5), radius**2*90, radius*4)
        add_area("fill", (-radius*2,  radius*3.5, radius*2),  radius**2*30, radius*5)
    else:
        add_area("key",  ( radius*3,  -radius*3,  radius*4.5), radius**2*90, radius*4)
        add_area("fill", (-radius*3.5, -radius*1.5, radius*2), radius**2*30, radius*5)
    if not args.floor:   # light the underside for iso-*-below cameras
        add_area("under", (radius*2.5, -radius*2.5, -radius*4.5), radius**2*70, radius*4)

    # ---- camera ----
    lens = 60 if args.mode == "cut" else 62
    cd = bpy.data.cameras.new("cam"); cd.lens = lens
    cam = bpy.data.objects.new("cam", cd); scene.collection.objects.link(cam)
    scene.camera = cam; cam.constraints.new('TRACK_TO').target = tgt
    halffov = math.atan((cd.sensor_width / 2.0) / lens)
    dist = radius / math.tan(halffov) * (1.28 if args.mode == "cut" else 1.3)

    # ---- freestyle edges for wire + cut ----
    if args.mode in ("wire", "cut") and args.edges:
        scene.render.use_freestyle = True
        vl = bpy.context.view_layer; vl.use_freestyle = True
        fs = vl.freestyle_settings; fs.crease_angle = math.radians(135)
        ls = fs.linesets[0] if fs.linesets else fs.linesets.new("ls")
        ls.select_silhouette = True; ls.select_border = True; ls.select_crease = True
        if ls.linestyle is None:
            ls.linestyle = bpy.data.linestyles.new("clayline")
        ls.linestyle.color = (0.10, 0.10, 0.12); ls.linestyle.thickness = 1.7

    # ---- render settings ----
    scene.render.engine = 'CYCLES'; scene.cycles.device = 'CPU'
    scene.cycles.samples = args.samples; scene.cycles.use_denoising = True
    scene.render.resolution_x = args.res; scene.render.resolution_y = args.res
    scene.render.film_transparent = False
    scene.view_settings.view_transform = 'Standard'

    # ---- render ----
    if args.mode == "cut":
        cut_cam = {"front": Vector((0.35, 0.9, 0.45)), "back": Vector((0.35, -0.9, 0.45)),
                   "side": Vector((0.9, 0.35, 0.45)), "left": Vector((-0.9, 0.35, 0.45))}
        d = cut_cam[args.cut]
        cam.location = tgt_pt + d.normalized() * dist
        scene.render.filepath = os.path.join(args.out, f"{args.name}-cut-{args.cut}.png")
        bpy.ops.render.render(write_still=True)
        print("WROTE", scene.render.filepath)
    else:
        suffix = {"clay": "", "wire": "-wire", "xray": "-xw"}[args.mode]
        views = assembly_views() if is_asm else part_views()
        for name in args.views.split(","):
            cam.location = tgt_pt + views[name].normalized() * dist
            bpy.context.view_layer.update()
            scene.render.filepath = os.path.join(args.out, f"{args.name}-{name}{suffix}.png")
            bpy.ops.render.render(write_still=True)
            print("WROTE", scene.render.filepath)


main()
