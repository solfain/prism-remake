#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
#
# SOL Launcher - Minecraft Launcher
# Copyright (C) 2026 SOL Launcher Contributors
"""Regenerates SOL Launcher's application icons from the mark defined below.

The mark is one deliberately simple shape: a diamond split at the waterline. The lit tip above the line is the part of
the iceberg you see, the larger, calmer mass below it is the part you don't. Two flat tones, no strokes, so it stays
legible down to 16 px.

Usage (from anywhere):

    pip install PySide6-Essentials Pillow
    python3 program_info/gen_sol_icons.py             # the default mark, "berg"
    python3 program_info/gen_sol_icons.py --mark frost    # or: berg, gem, shard, floe, frost
    python3 program_info/gen_sol_icons.py --list

Writes next to this script:
    io.github.solfain.SOLLauncher.svg            application icon (rounded dark tile, light mark)
    io.github.solfain.SOLLauncher_256.png        256 px render of the application icon
    io.github.solfain.SOLLauncher.logo.svg       plain mark for light backgrounds (README, web)
    io.github.solfain.SOLLauncher.logo-darkmode.svg   plain mark for dark backgrounds
    sollauncher.ico                              Windows icon (16 - 256 px, each size rendered from the SVG)
    sollauncher.icns                             macOS icon (rounded tile with the usual transparent margin)
    SOLLauncher.icon/                            macOS 26 "Icon Composer" layers
and ../launcher/resources/multimc/scalable/launcher.svg (the in-app "launcher" icon).

All colours are the SOL palette (see docs/sol/DESIGN.md).
"""
import io
import json
import os
import struct
import sys

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

HERE = os.path.dirname(os.path.abspath(__file__))
APP_ID = "io.github.solfain.SOLLauncher"
BINARY = "sollauncher"

# --- marks, on a 256x256 grid ---------------------------------------------------------------------------------------
# Every shape has a colour role: "tip" (the lit, light part) or "mass" (the calmer, deeper part). A shape is either
# ("poly", "x,y x,y ...") or ("bar", cx, cy, length, thickness, degrees) for a round-capped bar.
MARKS = {
    # default: a diamond split at the waterline. Two triangles, a thin gap between them.
    "berg": [("mass", ("poly", "52,128 204,128 128,224")), ("tip", ("poly", "128,38 52,116 204,116"))],
    # the same diamond, split vertically
    "gem": [("tip", ("poly", "128,34 60,112 128,224")), ("mass", ("poly", "128,34 196,112 128,224"))],
    # a single two-facet shard
    "shard": [("tip", ("poly", "116,38 46,210 134,210")), ("mass", ("poly", "116,38 134,210 210,210"))],
    # the iceberg as one silhouette: tip above the waterline, mass below
    "floe": [
        ("tip", ("poly", "66,136 110,80 132,38 156,88 172,70 198,136")),
        ("mass", ("poly", "66,136 198,136 226,168 198,214 146,236 94,226 54,186 40,164")),
    ],
    # a six-spoke frost crystal
    "frost": [("tip", ("bar", 128, 128, 176, 24, 90)), ("mass", ("bar", 128, 128, 176, 24, 30)), ("mass", ("bar", 128, 128, 176, 24, 150))],
}
MARK = "berg"

# --- palette -------------------------------------------------------------------------------------------------------
TILE = "#2B2636"
APP = dict(tip="#F6F4FB", mass="#B7ACD6")  # on the dark tile
MARK_LIGHT = dict(tip="#5E4E7C", mass="#B5ABD0")  # on light pages
MARK_DARK = dict(tip="#F1EEF8", mass="#6E6490")  # on dark pages

LICENSE_NOTE = "SOL Launcher logo. Copyright (C) 2026 SOL Launcher Contributors. Distributed with SOL Launcher under GPL-3.0-only."


def element(shape, fill, indent=""):
    if shape[0] == "poly":
        return f'{indent}<polygon points="{shape[1]}" fill="{fill}"/>'
    _, cx, cy, length, thick, deg = shape
    return (
        f'{indent}<rect x="{cx - thick / 2:g}" y="{cy - length / 2:g}" width="{thick}" height="{length}" rx="{thick / 2:g}" '
        f'transform="rotate({deg} {cx} {cy})" fill="{fill}"/>'
    )


def glyph(colors, indent="  ", roles=None):
    """SVG elements of the current mark. `roles` limits the output to some colour roles (used for the macOS layers)."""
    return "\n".join(element(shape, colors[role], indent) for role, shape in MARKS[MARK] if roles is None or role in roles)


def svg(colors, tile=None, glyph_scale=1.0, tile_scale=1.0, width=256, title="SOL Launcher Logo"):
    out = [
        '<?xml version="1.0" encoding="UTF-8"?>',
        f"<!-- {LICENSE_NOTE} -->",
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{width}" viewBox="0 0 256 256">',
        f"  <title>{title}</title>",
    ]
    if tile:
        inset = (1 - tile_scale) * 128
        out.append(
            f'  <rect x="{inset:.2f}" y="{inset:.2f}" width="{256 * tile_scale:.2f}" height="{256 * tile_scale:.2f}" '
            f'rx="{56 * tile_scale:.2f}" ry="{56 * tile_scale:.2f}" fill="{tile}"/>'
        )
    if glyph_scale != 1.0:
        offset = (1 - glyph_scale) * 128
        out.append(f'  <g transform="translate({offset:.2f} {offset:.2f}) scale({glyph_scale:.4f})">')
        out.append(glyph(colors, "    "))
        out.append("  </g>")
    else:
        out.append(glyph(colors))
    out.append("</svg>")
    return "\n".join(out) + "\n"


def write(path, text):
    with open(path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(text)
    print("wrote", os.path.relpath(path, HERE))


def render_png(svg_text, size):
    """Rasterise SVG text with Qt at an exact pixel size, returning a PIL image."""
    from PIL import Image
    from PySide6.QtCore import QBuffer, QByteArray, QIODevice, Qt
    from PySide6.QtGui import QImage, QPainter
    from PySide6.QtSvg import QSvgRenderer

    renderer = QSvgRenderer(QByteArray(svg_text.encode("utf-8")))
    image = QImage(size, size, QImage.Format_ARGB32)
    image.fill(Qt.transparent)
    painter = QPainter(image)
    painter.setRenderHint(QPainter.Antialiasing)
    painter.setRenderHint(QPainter.SmoothPixmapTransform)
    renderer.render(painter)
    painter.end()
    buffer = QBuffer()
    buffer.open(QIODevice.WriteOnly)
    image.save(buffer, "PNG")
    return Image.open(io.BytesIO(bytes(buffer.data()))).convert("RGBA")


def main():
    global MARK
    args = sys.argv[1:]
    if "--list" in args:
        print("marks:", ", ".join(MARKS), f"(default: berg)")
        return
    if "--mark" in args:
        MARK = args[args.index("--mark") + 1]
        if MARK not in MARKS:
            raise SystemExit(f"unknown mark {MARK!r}; choose one of: {', '.join(MARKS)}")
    print("mark:", MARK)

    from PySide6.QtGui import QGuiApplication

    _app = QGuiApplication.instance() or QGuiApplication(sys.argv[:1])

    app_svg = svg(APP, tile=TILE, glyph_scale=0.86)
    write(os.path.join(HERE, f"{APP_ID}.svg"), app_svg)
    write(os.path.join(HERE, f"{APP_ID}.logo.svg"), svg(MARK_LIGHT))
    write(os.path.join(HERE, f"{APP_ID}.logo-darkmode.svg"), svg(MARK_DARK))

    # the in-app "launcher" icon (export menu entry, launcher settings page) must read on light and dark themes
    write(os.path.join(HERE, "..", "launcher", "resources", "multimc", "scalable", "launcher.svg"), app_svg.replace('width="256" height="256"', 'width="48" height="48"'))

    render_png(app_svg, 256).save(os.path.join(HERE, f"{APP_ID}_256.png"), optimize=True)
    print("wrote", f"{APP_ID}_256.png")

    # --- Windows .ico: every size rendered from the SVG (BMP entries below 256 px, PNG at 256 px) ----------------------
    sizes = [256, 128, 64, 48, 32, 24, 16]
    frames = [render_png(app_svg, s) for s in sizes]
    ico_path = os.path.join(HERE, f"{BINARY}.ico")
    frames[0].save(ico_path, format="ICO", sizes=[(s, s) for s in sizes], append_images=frames[1:], bitmap_format="bmp")
    print("wrote", f"{BINARY}.ico", "(sizes: %s)" % ", ".join(str(s) for s in sizes))

    # --- macOS .icns: rounded tile with the customary transparent margin ---------------------------------------------
    mac_svg = svg(APP, tile=TILE, glyph_scale=0.86, tile_scale=0.8)
    mac_sizes = [1024, 512, 256, 128, 64, 32, 16]
    mac_frames = [render_png(mac_svg, s) for s in mac_sizes]
    mac_frames[0].save(os.path.join(HERE, f"{BINARY}.icns"), format="ICNS", append_images=mac_frames[1:])
    print("wrote", f"{BINARY}.icns")

    # --- macOS 26 Icon Composer layers --------------------------------------------------------------------------------
    layers = os.path.join(HERE, "SOLLauncher.icon")
    os.makedirs(os.path.join(layers, "Assets"), exist_ok=True)

    def layer_svg(title, body):
        return (
            '<?xml version="1.0" encoding="UTF-8"?>\n'
            f"<!-- {LICENSE_NOTE} -->\n"
            '<svg xmlns="http://www.w3.org/2000/svg" width="48" height="48" viewBox="0 0 256 256">\n'
            f"  <title>{title}</title>\n{body}\n</svg>\n"
        )

    body = glyph(APP, "  ", roles=("mass",))
    tip = glyph(APP, "  ", roles=("tip",))
    write(os.path.join(layers, "Assets", "body.svg"), layer_svg("SOL Launcher Logo - lower part", body))
    write(os.path.join(layers, "Assets", "tip.svg"), layer_svg("SOL Launcher Logo - upper part", tip))

    def layer(image, name, **extra):
        data = {"image-name": image, "name": name, "position": {"scale": 15.5, "translation-in-points": [0, 0]}}
        data.update(extra)
        return data

    icon = {
        "color-space-for-untagged-svg-colors": "display-p3",
        "fill": {"solid": "display-p3:0.16863,0.14902,0.21176,1.00000"},
        "groups": [
            {
                "layers": [layer("body.svg", "body")],
                "shadow": {"kind": "neutral", "opacity": 0.5},
                "translucency": {"enabled": False, "value": 0.5},
            },
            {
                "layers": [layer("tip.svg", "tip", **{"blend-mode": "normal", "fill": "automatic", "glass": True, "hidden": False})],
                "shadow": {"kind": "neutral", "opacity": 0.5},
                "translucency": {"enabled": False, "value": 0.5},
            },
        ],
        "supported-platforms": {"squares": ["macOS"]},
    }
    with open(os.path.join(layers, "icon.json"), "w", encoding="utf-8", newline="\n") as fh:
        json.dump(icon, fh, indent=2)
        fh.write("\n")
    print("wrote", "SOLLauncher.icon/icon.json")


if __name__ == "__main__":
    main()
