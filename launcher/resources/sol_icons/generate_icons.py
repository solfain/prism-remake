#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-3.0-only
#
# SOL Launcher - Minecraft Launcher
# Copyright (C) 2026 SOL Launcher Contributors
"""Generates SOL Launcher's line-icon themes ("sol" for light themes, "sol_white" for dark ones).

The drawings are Lucide icons (ISC licence, see LICENSE-lucide.txt), copied unmodified into ./lucide/. This script maps
the icon names the launcher asks for (the same names as the "flat" icon theme) to Lucide drawings, colours them with the
SOL palette (launcher/resources/sol/tokens.json), and writes:

    sol/index.theme         sol/scalable/<name>.svg           dark-on-light, for the SOL Light theme
    sol_white/index.theme   sol_white/scalable/<name>.svg     light-on-dark, for the SOL Dark theme
    sol_icons.qrc

The theme "sol" is adaptive at run time (ThemeManager picks "sol_white" when the active palette is dark), "sol_white" is
hidden from the theme lists. Icons that are not drawn here (the Discord / Reddit brand glyphs) fall back to "flat".

Usage:  python3 launcher/resources/sol_icons/generate_icons.py
"""
import json
import os
import re

HERE = os.path.dirname(os.path.abspath(__file__))
TOKENS = os.path.join(HERE, "..", "sol", "tokens.json")
STROKE_WIDTH = "1.75"  # Lucide's default is 2; a little lighter reads finer at toolbar sizes but still holds at 16 px

# name used by the launcher -> (Lucide drawing, colour role in tokens.json)
MUTED = "inkMuted"
ICONS = {
    "about": ("info", MUTED),
    "accounts": ("circle-user-round", MUTED),
    "appearance": ("palette", MUTED),
    "bug": ("bug", MUTED),
    "cat": ("cat", MUTED),
    "centralmods": ("package", MUTED),
    "checkupdate": ("circle-arrow-down", MUTED),
    "copy": ("copy", MUTED),
    "coremods": ("cpu", MUTED),
    "custom-commands": ("terminal", MUTED),
    "datapacks": ("database", MUTED),
    "delete": ("trash-2", MUTED),
    "export": ("upload", MUTED),
    "externaltools": ("wrench", MUTED),
    "help": ("circle-help", MUTED),
    "instance-settings": ("sliders-horizontal", MUTED),
    "java": ("coffee", MUTED),
    "language": ("languages", MUTED),
    "launch": ("play", MUTED),
    "lock": ("lock", MUTED),
    "loadermods": ("puzzle", MUTED),
    "log": ("scroll-text", MUTED),
    "minecraft": ("box", MUTED),
    "new": ("plus", MUTED),
    "news": ("newspaper", MUTED),
    "notes": ("sticky-note", MUTED),
    "proxy": ("globe-lock", MUTED),
    "refresh": ("refresh-cw", MUTED),
    "rename": ("pencil", MUTED),
    "resourcepacks": ("images", MUTED),
    "screenshot-placeholder": ("image", MUTED),
    "screenshots": ("camera", MUTED),
    "server": ("server", MUTED),
    "settings": ("settings", MUTED),
    "shaderpacks": ("sparkles", MUTED),
    "shortcut": ("square-arrow-out-up-right", MUTED),
    "star": ("star", MUTED),
    "status-bad": ("circle-x", "danger"),
    "status-good": ("circle-check", "success"),
    "status-running": ("circle-play", "success"),
    "status-yellow": ("triangle-alert", "warning"),
    "tag": ("tag", MUTED),
    "unlock": ("lock-open", MUTED),
    "viewfolder": ("folder-open", MUTED),
    "worlds": ("globe", MUTED),
}

THEMES = {
    "sol": dict(
        variant="light",
        name="SOL Line",
        inherits="flat",
        hidden=False,
        comment="Thin line icons (Lucide) that follow the active theme: dark strokes on light themes, light strokes on dark ones",
    ),
    "sol_white": dict(
        variant="dark",
        name="SOL Line (light strokes)",
        inherits="flat_white",
        hidden=True,
        comment="Internal: the light-stroke half of the SOL Line icon theme, chosen automatically on dark themes",
    ),
}


def lucide_body(name):
    with open(os.path.join(HERE, "lucide", f"{name}.svg"), encoding="utf-8") as fh:
        text = fh.read()
    inner = re.search(r"<svg[^>]*>(.*)</svg>", text, re.S).group(1)
    return "\n".join("  " + line.strip() for line in inner.strip().splitlines() if line.strip())


def icon_svg(name, color):
    return (
        '<svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" '
        f'stroke="{color}" stroke-width="{STROKE_WIDTH}" stroke-linecap="round" stroke-linejoin="round">\n'
        f"{lucide_body(name)}\n</svg>\n"
    )


def main():
    with open(TOKENS, encoding="utf-8") as fh:
        tokens = json.load(fh)
    qrc = ["<!DOCTYPE RCC>", '<RCC version="1.0">']
    for theme, spec in THEMES.items():
        palette = tokens[spec["variant"]]
        root = os.path.join(HERE, theme, "scalable")
        os.makedirs(root, exist_ok=True)
        for stale in os.listdir(root):
            os.remove(os.path.join(root, stale))
        for icon, (drawing, role) in sorted(ICONS.items()):
            with open(os.path.join(root, f"{icon}.svg"), "w", encoding="utf-8", newline="\n") as fh:
                fh.write(icon_svg(drawing, palette[role]))
        index = [
            "[Icon Theme]",
            f"Name={spec['name']}",
            f"Comment={spec['comment']}",
            f"Inherits={spec['inherits']}",
            "Directories=scalable",
        ]
        if spec["hidden"]:
            index.append("Hidden=true")
        index += ["", "[scalable]", "Size=48", "Type=Scalable", "MinSize=16", "MaxSize=256", ""]
        with open(os.path.join(HERE, theme, "index.theme"), "w", encoding="utf-8", newline="\n") as fh:
            fh.write("\n".join(index))
        qrc.append(f'    <qresource prefix="/icons/{theme}">')
        qrc.append(f"        <file alias=\"index.theme\">{theme}/index.theme</file>")
        for icon in sorted(ICONS):
            qrc.append(f"        <file alias=\"scalable/{icon}.svg\">{theme}/scalable/{icon}.svg</file>")
        qrc.append("    </qresource>")
        print(f"{theme}: {len(ICONS)} icons, colours from tokens.json ({spec['variant']})")
    qrc.append("</RCC>")
    with open(os.path.join(HERE, "sol_icons.qrc"), "w", encoding="utf-8", newline="\n") as fh:
        fh.write("\n".join(qrc) + "\n")
    print("wrote sol_icons.qrc")


if __name__ == "__main__":
    main()
