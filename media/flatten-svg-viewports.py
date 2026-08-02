#!/usr/bin/env python3
"""Flatten nested <svg> viewports into plain <g transform> groups.

A nested <svg> establishes a new viewport, and browsers clip anything that
falls outside it (`overflow: hidden` is the default per spec). Inkscape and
librsvg do not, so a logo composed this way looks perfect in every local
render and PNG export while silently losing pieces in the browser -- that is
how the icon's horns and basket kept disappearing.

Replacing the nested viewport with the equivalent <g transform> keeps the
exact same placement and cannot clip. Rewrites in place; a file with no
nested <svg> is left untouched.
"""
import sys
import xml.etree.ElementTree as ET

SVG = "http://www.w3.org/2000/svg"
ET.register_namespace("", SVG)
ET.register_namespace("xlink", "http://www.w3.org/1999/xlink")


def transform_for(inner):
    """Return the transform equivalent to this nested viewport's mapping."""
    x = float(inner.get("x", 0))
    y = float(inner.get("y", 0))
    w = float(inner.get("width"))
    h = float(inner.get("height"))
    vx, vy, vw, vh = (float(v) for v in inner.get("viewBox").replace(",", " ").split())

    par = (inner.get("preserveAspectRatio") or "xMidYMid meet").split()
    align = par[0]
    slice_ = len(par) > 1 and par[1] == "slice"

    if align == "none":
        sx, sy = w / vw, h / vh
    else:
        s = max(w / vw, h / vh) if slice_ else min(w / vw, h / vh)
        sx = sy = s

    # Alignment of the scaled viewBox inside the viewport.
    fx = {"Min": 0.0, "Mid": 0.5, "Max": 1.0}.get(align[1:4], 0.0)
    fy = {"Min": 0.0, "Mid": 0.5, "Max": 1.0}.get(align[5:8], 0.0)
    tx = x + (w - sx * vw) * fx - sx * vx
    ty = y + (h - sy * vh) * fy - sy * vy

    return f"translate({tx:.6f},{ty:.6f}) scale({sx:.6f},{sy:.6f})"


def flatten(path):
    tree = ET.parse(path)
    root = tree.getroot()
    changed = 0

    # Repeat until stable, since a flattened group may expose another nesting.
    while True:
        parent_of = {c: p for p in root.iter() for c in p}
        nested = [e for e in root.iter(f"{{{SVG}}}svg")
                  if e is not root and e.get("viewBox") and e.get("width")]
        if not nested:
            break
        for inner in nested:
            group = ET.Element(f"{{{SVG}}}g", {"transform": transform_for(inner)})
            for child in list(inner):
                group.append(child)
            parent = parent_of[inner]
            parent[list(parent).index(inner)] = group
            changed += 1

    if changed:
        tree.write(path, xml_declaration=True, encoding="UTF-8")
    return changed


if __name__ == "__main__":
    for path in sys.argv[1:]:
        n = flatten(path)
        if n:
            print(f"  flattened {n} nested viewport(s) in {path}")
