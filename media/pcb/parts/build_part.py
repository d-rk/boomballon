import os
import re
import shutil
import subprocess
import tempfile
import zipfile

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))
TRACESPACE = os.path.join(REPO_ROOT, "media", "pcb", "node_modules", ".bin", "tracespace")
FRITZING_CMD = [
    "flatpak", "run", "--user", "--env=QT_QPA_PLATFORM=offscreen",
    "org.fritzing.Fritzing",
]


def render_gerbers(board_fzz: str, work_dir: str) -> dict:
    """Export Gerbers from board_fzz via headless Fritzing, render them with
    tracespace, and return {"top": <svg path>, "bottom": <svg path>}."""
    os.makedirs(work_dir, exist_ok=True)
    fzz_copy = os.path.join(work_dir, os.path.basename(board_fzz))
    shutil.copy(board_fzz, fzz_copy)

    subprocess.run(FRITZING_CMD + ["-gerber", work_dir], check=True,
                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    base = os.path.splitext(os.path.basename(board_fzz))[0]
    gerbers = [
        f"{base}_copperTop.gtl", f"{base}_copperBottom.gbl",
        f"{base}_maskTop.gts", f"{base}_maskBottom.gbs",
        f"{base}_silkTop.gto", f"{base}_silkBottom.gbo",
        f"{base}_contour.gm1", f"{base}_drill.txt",
    ]
    rendered_dir = os.path.join(work_dir, "rendered")
    subprocess.run(
        [TRACESPACE] + [os.path.join(work_dir, g) for g in gerbers] + ["-o", rendered_dir + "/"],
        check=True, cwd=work_dir, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
    )
    return {
        "top": os.path.join(rendered_dir, f"{base}_.top.svg"),
        "bottom": os.path.join(rendered_dir, f"{base}_.bottom.svg"),
    }


def _read_group_translate_y(svg_text: str) -> float:
    m = re.search(r'<g transform="translate\(0,([\d.]+)\) scale\(1,-1\)"', svg_text)
    if not m:
        raise ValueError("expected a translate(0,H) scale(1,-1) group transform")
    return float(m.group(1))


def _tag_connectors(svg_path: str, connectors: list, out_path: str) -> None:
    """Wrap svg_path's content in <g id="breadboard"> and add connectorNpin/
    connectorNterminal rects at the given raw (x, y) pad coordinates."""
    svg_text = open(svg_path).read()
    h = _read_group_translate_y(svg_text)

    body_match = re.search(r"(<svg[^>]*>)(.*)(</svg>)", svg_text, re.S)
    open_tag, body, close_tag = body_match.groups()

    markers = []
    for i, (_name, raw_x, raw_y) in enumerate(connectors):
        local_x, local_y = raw_x, h - raw_y
        markers.append(
            f'<rect id="connector{i}pin" x="{local_x - 7}" y="{local_y - 7}" '
            f'width="14" height="14" fill="none"/>'
            f'<rect id="connector{i}terminal" x="{local_x - 3}" y="{local_y - 3}" '
            f'width="6" height="6" fill="none"/>'
        )

    new_svg = f'{open_tag}{"".join(markers)}<g id="breadboard">{body}</g>{close_tag}'
    with open(out_path, "w") as f:
        f.write(new_svg)


def _tag_connectors_pcb(svg_path: str, connectors: list, out_path: str) -> None:
    """Same as _tag_connectors but wraps in <g id="copper0"> (pcbView convention,
    no terminal rects needed for pcbView)."""
    svg_text = open(svg_path).read()
    h = _read_group_translate_y(svg_text)

    body_match = re.search(r"(<svg[^>]*>)(.*)(</svg>)", svg_text, re.S)
    open_tag, body, close_tag = body_match.groups()

    markers = []
    for i, (_name, raw_x, raw_y) in enumerate(connectors):
        local_x, local_y = raw_x, h - raw_y
        markers.append(
            f'<rect id="connector{i}pin" x="{local_x - 7}" y="{local_y - 7}" '
            f'width="14" height="14" fill="none"/>'
        )

    new_svg = f'{open_tag}{"".join(markers)}<g id="copper0">{body}</g>{close_tag}'
    with open(out_path, "w") as f:
        f.write(new_svg)


def _make_schematic_svg(connectors: list, out_path: str) -> None:
    """A plain labeled rectangle with one pin stub per connector, evenly spaced
    down the left edge. Not photorealistic -- schematic view isn't the point
    of this project, it just needs to be valid."""
    n = len(connectors)
    row_h = 20
    height = max(60, n * row_h + 20)
    width = 140

    parts = [f'<svg xmlns="http://www.w3.org/2000/svg" width="{width / 90}in" '
             f'height="{height / 90}in" viewBox="0 0 {width} {height}">']
    parts.append(f'<g id="schematic">'
                  f'<rect x="30" y="10" width="80" height="{height - 20}" '
                  f'fill="none" stroke="#000" stroke-width="1"/>')
    for i, (name, _x, _y) in enumerate(connectors):
        y = 20 + i * row_h
        parts.append(f'<line x1="0" y1="{y}" x2="30" y2="{y}" stroke="#000" stroke-width="1"/>')
        parts.append(f'<text x="34" y="{y + 4}" font-size="8" font-family="monospace">{name}</text>')
    parts.append('</g>')

    for i, (_name, _x, _y) in enumerate(connectors):
        y = 20 + i * row_h
        parts.append(f'<rect id="connector{i}pin" x="-3" y="{y - 3}" width="6" height="6" fill="none"/>')
        parts.append(f'<rect id="connector{i}terminal" x="-1" y="{y - 1}" width="2" height="2" fill="none"/>')
    parts.append('</svg>')

    with open(out_path, "w") as f:
        f.write("".join(parts))


def _make_fzp(module_id: str, title: str, connectors: list,
              breadboard_file: str, schematic_file: str, pcb_file: str, icon_file: str) -> str:
    connector_xml = []
    for i, (name, _x, _y) in enumerate(connectors):
        connector_xml.append(f'''
  <connector id="connector{i}" name="{name}" type="female">
   <description>{name}</description>
   <views>
    <breadboardView>
     <p layer="breadboard" svgId="connector{i}pin" terminalId="connector{i}terminal"/>
    </breadboardView>
    <schematicView>
     <p layer="schematic" svgId="connector{i}pin" terminalId="connector{i}terminal"/>
    </schematicView>
    <pcbView>
     <p layer="copper0" svgId="connector{i}pin"/>
    </pcbView>
   </views>
  </connector>''')

    return f'''<?xml version='1.0' encoding='UTF-8' standalone='no'?>
<module fritzingVersion="0.9.4" moduleId="{module_id}">
 <version>1</version>
 <author>Dirk Wilden</author>
 <title>{title}</title>
 <label>J</label>
 <date>2026-08-13</date>
 <tags/>
 <properties>
  <property name="family">BoomBalloon</property>
  <property name="package">THT</property>
 </properties>
 <description>{title} -- BoomBalloon custom Fritzing part, generated from the real PCB Gerbers.</description>
 <views>
  <iconView>
   <layers image="icon/{icon_file}">
    <layer layerId="icon"/>
   </layers>
  </iconView>
  <breadboardView>
   <layers image="breadboard/{breadboard_file}">
    <layer layerId="breadboard"/>
   </layers>
  </breadboardView>
  <schematicView>
   <layers image="schematic/{schematic_file}">
    <layer layerId="schematic"/>
   </layers>
  </schematicView>
  <pcbView>
   <layers image="pcb/{pcb_file}">
    <layer layerId="copper0"/>
   </layers>
  </pcbView>
 </views>
 <connectors>{"".join(connector_xml)}
 </connectors>
</module>
'''


def _package_fzpz(module_id: str, fzp_text: str, breadboard_svg: str, schematic_svg: str,
                   pcb_svg: str, icon_svg: str, breadboard_file: str, schematic_file: str,
                   pcb_file: str, icon_file: str, out_path: str) -> None:
    with zipfile.ZipFile(out_path, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr(f"part.{module_id}.fzp", fzp_text)
        z.writestr(f"svg.breadboard.{breadboard_file}", open(breadboard_svg).read())
        z.writestr(f"svg.schematic.{schematic_file}", open(schematic_svg).read())
        z.writestr(f"svg.pcb.{pcb_file}", open(pcb_svg).read())
        z.writestr(f"svg.icon.{icon_file}", open(icon_svg).read())


def build_part(board_fzz: str, board_name: str, connectors: list, out_dir: str) -> str:
    """board_name: e.g. "Lichtmodul". connectors: [(pin_name, raw_x, raw_y), ...].
    Returns the path to the written .fzpz."""
    module_id = f"BoomBalloon_{board_name}"
    with tempfile.TemporaryDirectory(prefix=f"boomballoon-part-{board_name}-") as work_dir:
        svgs = render_gerbers(board_fzz, work_dir)

        breadboard_file = f"{board_name}_breadboard.svg"
        schematic_file = f"{board_name}_schematic.svg"
        pcb_file = f"{board_name}_pcb.svg"
        icon_file = f"{board_name}_icon.svg"

        breadboard_out = os.path.join(work_dir, breadboard_file)
        schematic_out = os.path.join(work_dir, schematic_file)
        pcb_out = os.path.join(work_dir, pcb_file)

        _tag_connectors(svgs["top"], connectors, breadboard_out)
        _tag_connectors_pcb(svgs["top"], connectors, pcb_out)
        _make_schematic_svg(connectors, schematic_out)
        # Icon view: reuse the breadboard graphic, Fritzing scales it down itself.
        shutil.copy(breadboard_out, os.path.join(work_dir, icon_file))

        fzp_text = _make_fzp(module_id, board_name, connectors,
                              breadboard_file, schematic_file, pcb_file, icon_file)

        os.makedirs(out_dir, exist_ok=True)
        out_path = os.path.join(out_dir, f"{board_name}.fzpz")
        _package_fzpz(module_id, fzp_text, breadboard_out, schematic_out, pcb_out,
                      os.path.join(work_dir, icon_file), breadboard_file, schematic_file,
                      pcb_file, icon_file, out_path)
        return out_path
