#!/usr/bin/env bash
# Linux AppImage (x86_64) of the Compose desktop app, with its own Java runtime.
# Needs python3, curl and a JDK 17+ that ships jmods (set PACKAGE_JDK, e.g.
# Temurin 21). The asset name and the icon path inside the image are what the
# installer and the updater of older builds expect; keep them.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

ARCH="$(uname -m)"
case "$ARCH" in
  x86_64|amd64) ARCH=x86_64 ;;
  *)
    echo "Only x86_64 AppImages are built, this machine is $ARCH" >&2
    exit 1
    ;;
esac

OUT="$ROOT/dist/graduately-linux-${ARCH}.AppImage"
APPDIR="$ROOT/dist/AppDir"
TOOLS="$ROOT/dist/tools"
ICON="$ROOT/data/share/icons/hicolor/512x512/apps/maturita.png"
IMAGE="$ROOT/android/desktop/build/compose/binaries/main-release/app/graduately"

echo "==> Building the desktop app"
(cd android && ./gradlew :desktop:createReleaseDistributable --no-daemon)

echo "==> Preparing AppDir"
rm -rf "$APPDIR"
mkdir -p "$APPDIR/usr/share/icons/hicolor/512x512/apps" "$APPDIR/usr/share/applications"
cp -R "$IMAGE" "$APPDIR/graduately"
cp -f "$ICON" "$APPDIR/maturita.png"
cp -f "$ICON" "$APPDIR/.DirIcon"
cp -f "$ICON" "$APPDIR/usr/share/icons/hicolor/512x512/apps/maturita.png"

cat > "$APPDIR/AppRun" <<'EOF'
#!/bin/sh
HERE="$(dirname "$(readlink -f "$0")")"
exec "$HERE/graduately/bin/graduately" "$@"
EOF
chmod +x "$APPDIR/AppRun"

DESKTOP="$ROOT/data/share/applications/org.maturita.Maturita.desktop"
cp -f "$DESKTOP" "$APPDIR/org.maturita.Maturita.desktop"
cp -f "$DESKTOP" "$APPDIR/usr/share/applications/org.maturita.Maturita.desktop"

TOOL="$TOOLS/appimagetool-${ARCH}.AppImage"
if [[ ! -x "$TOOL" ]]; then
  echo "==> Downloading appimagetool"
  mkdir -p "$TOOLS"
  curl -fsSL -o "$TOOL" \
    "https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-${ARCH}.AppImage"
  chmod +x "$TOOL"
fi

echo "==> Building AppImage"
rm -f "$OUT"
# CI runners have no FUSE; extract-and-run avoids needing it.
APPIMAGE_EXTRACT_AND_RUN=1 ARCH="$ARCH" "$TOOL" --no-appstream "$APPDIR" "$OUT"
chmod +x "$OUT"
echo "Created ${OUT#"$ROOT"/}"
