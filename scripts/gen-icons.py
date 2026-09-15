#!/usr/bin/env python3
"""Regenerate assets/app-icon.ico and share/icons from assets/app-icon.png.

Uses macOS `sips` when available; otherwise requires Pillow.
Run from the repository root.
"""
from __future__ import annotations

import struct
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SRC = ROOT / "assets" / "app-icon.png"
ICO = ROOT / "assets" / "app-icon.ico"
ICO_SIZES = [16, 32, 48, 64, 128, 256]
SHARE_SIZES = [16, 32, 48, 64, 128, 256, 512]


def resize_png(src: Path, dest: Path, size: int) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    if subprocess.call(["which", "sips"], stdout=subprocess.DEVNULL,
                       stderr=subprocess.DEVNULL) == 0:
        subprocess.check_call(
            ["sips", "-z", str(size), str(size), str(src), "--out", str(dest)],
            stdout=subprocess.DEVNULL,
        )
        return
    try:
        from PIL import Image
    except ImportError as exc:
        raise SystemExit(
            "Need macOS sips or Pillow to resize icons"
        ) from exc
    img = Image.open(src).convert("RGBA")
    img = img.resize((size, size), Image.Resampling.LANCZOS)
    img.save(dest, format="PNG")


def write_ico(entries: list[tuple[int, bytes]], dest: Path) -> None:
    count = len(entries)
    header = struct.pack("<HHH", 0, 1, count)
    offset = 6 + 16 * count
    direntries = []
    blobs = []
    for size, data in entries:
        w = 0 if size >= 256 else size
        h = 0 if size >= 256 else size
        direntries.append(
            struct.pack("<BBBBHHII", w, h, 0, 0, 1, 32, len(data), offset)
        )
        blobs.append(data)
        offset += len(data)
    dest.write_bytes(header + b"".join(direntries) + b"".join(blobs))


def main() -> int:
    if not SRC.is_file():
        print(f"missing {SRC}", file=sys.stderr)
        return 1

    with tempfile.TemporaryDirectory(prefix="maturita-ico-") as tmp:
        entries: list[tuple[int, bytes]] = []
        for size in ICO_SIZES:
            png = Path(tmp) / f"icon_{size}.png"
            resize_png(SRC, png, size)
            entries.append((size, png.read_bytes()))
        write_ico(entries, ICO)
        print(f"wrote {ICO.relative_to(ROOT)}")

    for size in SHARE_SIZES:
        dest = ROOT / "share" / "icons" / "hicolor" / f"{size}x{size}" / "apps" / "maturita.png"
        resize_png(SRC, dest, size)
        print(f"wrote {dest.relative_to(ROOT)}")

    assets_icon = ROOT / "assets" / "icons" / "hicolor" / "512x512" / "apps" / "maturita.png"
    resize_png(SRC, assets_icon, 512)
    print(f"wrote {assets_icon.relative_to(ROOT)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
