#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
#
# SOL Launcher - Minecraft Launcher
# Copyright (C) 2026 SOL Launcher Contributors
"""Regenerates SOL Launcher's application icons from the iceberg geometry below.

The mark is a deliberately simple iceberg: a lit tip above the waterline and a larger submerged mass below it, separated
by a thin gap (the waterline). It has no strokes, so it stays legible down to 16 px.

Usage (from anywhere):

    pip install PySide6-Essentials Pillow
    python3 program_info/gen_sol_icons.py

Writes next to this script:
    io.github.solfain.SOLLauncher.svg            application icon (rounded dark tile, light iceberg)
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

# --- geometry on a 256x256 grid ------------------------------------------------------------------------------------
TIP_LEFT = "72,136 112,84 134,40 150,78 150,136"  # lit facet, left of the main ridge
TIP_RIGHT = "134,40 150,78 168,62 192,136 150,136"  # shaded facet, with the small second peak
BODY_LEFT = "40,152 150,152 150,246 110,230 64,200 34,172"  # submerged mass, left half
BODY_RIGHT = "150,152 216,152 230,178 200,222 150,246"  # submerged mass, right half
WATERLINE = (22, 141, 212, 5)  # x, y, width, height (rounded)

# --- palette -------------------------------------------------------------------------------------------------------
TILE = "#2B2636"
APP = dict(tip_a="#F6F4FB", tip_b="#D6D0E6", body_a="#8E84AE", body_b="#7A7096", line="#B4ACCB")
MARK_LIGHT = dict(tip_a="#5E4E7C", tip_b="#4A3D63", body_a="#B5ABD0", body_b="#A398C2", line="#9C93B8")
MARK_DARK = dict(tip_a="#F1EEF8", tip_b="#CFC8E0", body_a="#6E6490", body_b="#5E5580", line="#8379A6")

LICENSE_NOTE = "SOL Launcher logo. Copyright (C) 2026 SOL Launcher Contributors. Distributed with SOL Launcher under GPL-3.0-only."


def glyph(colors, indent="  "):
    x, y, w, h = WATERLINE
    return "\n".join(
        indent + line
        for line in (
            f'<polygon points="{BODY_LEFT}" fill="{colors["body_a"]}"/>',
            f'<polygon points="{BODY_RIGHT}" fill="{colors["body_b"]}"/>',
            f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{h / 2}" fill="{colors["line"]}"/>',
            f'<polygon points="{TIP_LEFT}" fill="{colors["tip_a"]}"/>',
            f'<polygon points="{TIP_RIGHT}" fill="{colors["tip_b"]}"/>',
        )
    )


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

    x, y, w, h = WATERLINE
    body = "\n".join(
        [
            f'  <polygon points="{BODY_LEFT}" fill="{APP["body_a"]}"/>',
            f'  <polygon points="{BODY_RIGHT}" fill="{APP["body_b"]}"/>',
            f'  <rect x="{x}" y="{y}" width="{w}" height="{h}" rx="{h / 2}" fill="{APP["line"]}"/>',
        ]
    )
    tip = "\n".join(
        [
            f'  <polygon points="{TIP_LEFT}" fill="{APP["tip_a"]}"/>',
            f'  <polygon points="{TIP_RIGHT}" fill="{APP["tip_b"]}"/>',
        ]
    )
    write(os.path.join(layers, "Assets", "body.svg"), layer_svg("SOL Launcher Logo - submerged mass", body))
    write(os.path.join(layers, "Assets", "tip.svg"), layer_svg("SOL Launcher Logo - tip", tip))

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
