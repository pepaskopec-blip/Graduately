#!/usr/bin/env bash
# Build a self-contained Linux AppImage (x86_64).
# Requires: make, pkg-config, GTK4, curl, and fuse/appimagetool (pulled as needed).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

ARCH="$(uname -m)"
case "$ARCH" in
  x86_64|amd64) ARCH=x86_64 ;;
  aarch64|arm64) ARCH=aarch64 ;;
  *)
    echo "Unsupported architecture: $ARCH" >&2
    exit 1
    ;;
esac

OUT_NAME="maturita-linux-${ARCH}.AppImage"
APPDIR="$ROOT/dist/AppDir"
TOOLS="$ROOT/dist/tools"
ICON_DST="assets/icons/hicolor/512x512/apps/maturita.png"

echo "==> Building maturita"
make -C "$ROOT" clean
make -C "$ROOT"

echo "==> Preparing AppDir"
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/bin" \
         "$APPDIR/usr/share/applications" \
         "$APPDIR/usr/share/icons/hicolor/512x512/apps" \
         "$APPDIR/icons/hicolor/512x512/apps" \
         "$APPDIR/share"

cp -f "$ROOT/maturita" "$APPDIR/usr/bin/maturita"
cp -f "$ROOT/style.css" "$APPDIR/style.css"
cp -f "$ROOT/$ICON_DST" "$APPDIR/icons/hicolor/512x512/apps/maturita.png"
cp -f "$ROOT/$ICON_DST" "$APPDIR/usr/share/icons/hicolor/512x512/apps/maturita.png"
cp -f "$ROOT/$ICON_DST" "$APPDIR/maturita.png"
cp -R "$ROOT/share/." "$APPDIR/share/"
cp -R "$ROOT/share/." "$APPDIR/usr/share/"
cp -f "$ROOT/share/applications/org.maturita.Maturita.desktop" \
   "$APPDIR/usr/share/applications/org.maturita.Maturita.desktop"
# AppImage desktop entry must live at AppDir root and Exec= must be AppRun-compatible.
cp -f "$ROOT/share/applications/org.maturita.Maturita.desktop" \
   "$APPDIR/org.maturita.Maturita.desktop"
# linuxdeploy expects Icon= without path and a matching PNG at AppDir root.
sed -i.bak 's|^Exec=.*|Exec=maturita|' "$APPDIR/org.maturita.Maturita.desktop"
sed -i.bak 's|^Icon=.*|Icon=maturita|' "$APPDIR/org.maturita.Maturita.desktop"
rm -f "$APPDIR/org.maturita.Maturita.desktop.bak"

mkdir -p "$TOOLS"
LINUXDEPLOY="$TOOLS/linuxdeploy-${ARCH}.AppImage"
GTK_PLUGIN="$TOOLS/linuxdeploy-plugin-gtk.sh"

if [[ ! -x "$LINUXDEPLOY" ]]; then
  echo "==> Downloading linuxdeploy"
  curl -fsSL -o "$LINUXDEPLOY" \
    "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-${ARCH}.AppImage"
  chmod +x "$LINUXDEPLOY"
fi
if [[ ! -f "$GTK_PLUGIN" ]]; then
  echo "==> Downloading linuxdeploy-plugin-gtk"
  curl -fsSL -o "$GTK_PLUGIN" \
    "https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gtk/master/linuxdeploy-plugin-gtk.sh"
  chmod +x "$GTK_PLUGIN"
fi

# Extract linuxdeploy if FUSE is unavailable (common on CI).
run_linuxdeploy() {
  if "$LINUXDEPLOY" --appimage-help >/dev/null 2>&1; then
    "$LINUXDEPLOY" "$@"
  else
    local extract_dir="$TOOLS/linuxdeploy-extracted"
    rm -rf "$extract_dir"
    cd "$TOOLS"
    "$LINUXDEPLOY" --appimage-extract >/dev/null
    mv squashfs-root "$extract_dir"
    cd "$ROOT"
    "$extract_dir/AppRun" "$@"
  fi
}

echo "==> Bundling GTK libraries into AppDir"
export LDAI_OUTPUT="$ROOT/dist/$OUT_NAME"
export LINUXDEPLOY_OUTPUT_VERSION="${VERSION:-continuous}"
# Place plugin on PATH so linuxdeploy can find it.
export PATH="$TOOLS:$PATH"
cp -f "$GTK_PLUGIN" "$TOOLS/linuxdeploy-plugin-gtk.sh"

run_linuxdeploy \
  --appdir "$APPDIR" \
  --executable "$APPDIR/usr/bin/maturita" \
  --desktop-file "$APPDIR/org.maturita.Maturita.desktop" \
  --icon-file "$APPDIR/maturita.png" \
  --plugin gtk \
  --output appimage

# linuxdeploy may write the AppImage into the cwd; normalise the name/location.
shopt -s nullglob
for f in "$ROOT"/*.AppImage "$ROOT/dist"/*.AppImage; do
  case "$(basename "$f")" in
    linuxdeploy*|appimagetool*) continue ;;
  esac
  mv -f "$f" "$ROOT/dist/$OUT_NAME"
done
shopt -u nullglob

if [[ ! -f "$ROOT/dist/$OUT_NAME" ]]; then
  echo "AppImage was not created" >&2
  exit 1
fi

chmod +x "$ROOT/dist/$OUT_NAME"
echo "Created dist/$OUT_NAME"
