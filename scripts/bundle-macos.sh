#!/usr/bin/env bash
# Build a self-contained macOS .app with GTK dylibs relocated into the bundle.
# Requires: make, pkg-config, GTK4 (Homebrew), dylibbundler (brew install dylibbundler).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

ARCH="$(uname -m)"
case "$ARCH" in
  arm64|aarch64) ARCH_TAG=arm64 ;;
  x86_64) ARCH_TAG=x86_64 ;;
  *)
    echo "Unsupported architecture: $ARCH" >&2
    exit 1
    ;;
esac

APP_NAME="Maturita.app"
APP="$ROOT/dist/$APP_NAME"
MACOS="$APP/Contents/MacOS"
RES="$APP/Contents/Resources"
FW="$APP/Contents/Frameworks"
ICON_DST="assets/icons/hicolor/512x512/apps/maturita.png"
OUT_ZIP="maturita-macos-${ARCH_TAG}.zip"

if ! command -v dylibbundler >/dev/null 2>&1; then
  echo "dylibbundler not found. Install with: brew install dylibbundler" >&2
  exit 1
fi

echo "==> Building maturita"
make -C "$ROOT" clean
make -C "$ROOT"

echo "==> Creating $APP_NAME"
rm -rf "$APP"
mkdir -p "$MACOS" "$RES/icons/hicolor/512x512/apps" "$RES/share" "$FW"

cp -f "$ROOT/maturita" "$MACOS/maturita-bin"
cp -f "$ROOT/style.css" "$RES/style.css"
cp -f "$ROOT/$ICON_DST" "$RES/icons/hicolor/512x512/apps/maturita.png"
cp -R "$ROOT/share/." "$RES/share/"

# Bundle Homebrew / MacPorts GTK stack next to the binary.
echo "==> Relocating dylibs with dylibbundler"
dylibbundler -od -b \
  -x "$MACOS/maturita-bin" \
  -d "$FW" \
  -p "@executable_path/../Frameworks"

# Copy GLib / GTK runtime data (schemas, modules, icons themes used by GTK).
copy_pkg_share() {
  local prefix
  prefix="$(pkg-config --variable=prefix "$1" 2>/dev/null || true)"
  [[ -n "$prefix" && -d "$prefix/share" ]] || return 0
  mkdir -p "$RES/share"
  # Copy only the subtrees GTK needs at runtime.
  for sub in glib-2.0 gtk-4.0 icons themes locale; do
    if [[ -d "$prefix/share/$sub" ]]; then
      rsync -a "$prefix/share/$sub" "$RES/share/" 2>/dev/null \
        || cp -R "$prefix/share/$sub" "$RES/share/"
    fi
  done
}

copy_pkg_share gtk4
copy_pkg_share glib-2.0

# gdk-pixbuf loaders (PNG may be built-in; copy others for GIF/SVG/etc.).
PIXBUF_DIR="$(pkg-config --variable=gdk_pixbuf_moduledir gdk-pixbuf-2.0 2>/dev/null || true)"
LOADERS_DST="$RES/lib/gdk-pixbuf-2.0/2.10.0/loaders"
if [[ -n "$PIXBUF_DIR" && -d "$PIXBUF_DIR" ]]; then
  mkdir -p "$LOADERS_DST"
  # Homebrew keeps loaders as relative symlinks into Cellar — dereference.
  if command -v rsync >/dev/null 2>&1; then
    rsync -aL "$PIXBUF_DIR"/ "$LOADERS_DST"/
  else
    cp -R -L "$PIXBUF_DIR"/. "$LOADERS_DST"/
  fi
  # Drop broken leftovers and relocate real loader modules.
  find "$LOADERS_DST" -type l ! -exec test -e {} \; -delete 2>/dev/null || true
  shopt -s nullglob
  for loader in "$LOADERS_DST"/*; do
    [[ -f "$loader" ]] || continue
    dylibbundler -od -b -x "$loader" -d "$FW" -p "@executable_path/../Frameworks" || true
  done
  shopt -u nullglob
  if command -v gdk-pixbuf-query-loaders >/dev/null 2>&1; then
    export GDK_PIXBUF_MODULEDIR="$LOADERS_DST"
    gdk-pixbuf-query-loaders > "$RES/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache" || true
  fi
fi

# Compile GSettings schemas if glib-compile-schemas is available.
if [[ -d "$RES/share/glib-2.0/schemas" ]] && command -v glib-compile-schemas >/dev/null 2>&1; then
  glib-compile-schemas "$RES/share/glib-2.0/schemas" || true
fi

# Launcher sets GTK search paths then execs the real binary.
cat > "$MACOS/maturita" <<'EOF'
#!/bin/bash
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
export DYLD_LIBRARY_PATH="$ROOT/Frameworks${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"
export XDG_DATA_DIRS="$ROOT/Resources/share${XDG_DATA_DIRS:+:$XDG_DATA_DIRS}"
export GSETTINGS_SCHEMA_DIR="$ROOT/Resources/share/glib-2.0/schemas"
export GDK_PIXBUF_MODULEDIR="$ROOT/Resources/lib/gdk-pixbuf-2.0/2.10.0/loaders"
if [[ -f "$ROOT/Resources/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache" ]]; then
  export GDK_PIXBUF_MODULE_FILE="$ROOT/Resources/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache"
fi
export GTK_DATA_PREFIX="$ROOT/Resources"
export GTK_EXE_PREFIX="$ROOT"
# Adwaita / hicolor icons for GTK widgets.
export XDG_DATA_HOME="$ROOT/Resources"
exec "$ROOT/MacOS/maturita-bin" "$@"
EOF
chmod +x "$MACOS/maturita"

# Info.plist
cat > "$APP/Contents/Info.plist" <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
  "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleName</key>
  <string>maturita.c</string>
  <key>CFBundleDisplayName</key>
  <string>maturita.c</string>
  <key>CFBundleIdentifier</key>
  <string>org.maturita.Maturita</string>
  <key>CFBundleVersion</key>
  <string>${VERSION:-1.0}</string>
  <key>CFBundleShortVersionString</key>
  <string>${VERSION:-1.0}</string>
  <key>CFBundleExecutable</key>
  <string>maturita</string>
  <key>CFBundlePackageType</key>
  <string>APPL</string>
  <key>CFBundleIconFile</key>
  <string>AppIcon</string>
  <key>LSMinimumSystemVersion</key>
  <string>12.0</string>
  <key>NSHighResolutionCapable</key>
  <true/>
</dict>
</plist>
EOF

# Optional .icns from PNG if iconutil / sips available.
if command -v sips >/dev/null 2>&1 && command -v iconutil >/dev/null 2>&1; then
  ICONSET="$RES/AppIcon.iconset"
  mkdir -p "$ICONSET"
  for sz in 16 32 64 128 256 512; do
    sips -z "$sz" "$sz" "$ROOT/$ICON_DST" --out "$ICONSET/icon_${sz}x${sz}.png" >/dev/null
    sips -z $((sz * 2)) $((sz * 2)) "$ROOT/$ICON_DST" \
      --out "$ICONSET/icon_${sz}x${sz}@2x.png" >/dev/null
  done
  iconutil -c icns -o "$RES/AppIcon.icns" "$ICONSET" && rm -rf "$ICONSET" || true
fi

echo "==> Zipping"
rm -f "$ROOT/dist/$OUT_ZIP"
(
  cd "$ROOT/dist"
  zip -qry "$OUT_ZIP" "$APP_NAME"
)

echo "Created dist/$OUT_ZIP"
