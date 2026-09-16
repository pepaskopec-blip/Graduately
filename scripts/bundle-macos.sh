#!/usr/bin/env bash
# Build a self-contained macOS .app with GTK dylibs relocated into the bundle.
# Requires: make, pkg-config, GTK4 (Homebrew).
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
ICON_512="$ROOT/share/icons/hicolor/512x512/apps/maturita.png"
OUT_ZIP="maturita-macos-${ARCH_TAG}.zip"
BREW_PREFIX="$(brew --prefix 2>/dev/null || echo /opt/homebrew)"

echo "==> Building maturita"
make -C "$ROOT" clean
make -C "$ROOT"

echo "==> Creating $APP_NAME"
rm -rf "$APP"
mkdir -p "$MACOS" "$RES/icons/hicolor/512x512/apps" "$RES/share" "$FW"

cp -f "$ROOT/maturita" "$MACOS/maturita-bin"
cp -f "$ROOT/style.css" "$RES/style.css"
cp -f "$ICON_512" "$RES/icons/hicolor/512x512/apps/maturita.png"
cp -R "$ROOT/share/." "$RES/share/"

# Resolve an absolute Homebrew-style dependency to a real file on disk.
resolve_lib() {
  local dep="$1"
  local candidate

  if [[ -f "$dep" ]]; then
    realpath "$dep"
    return 0
  fi

  # Try common Homebrew layouts for the basename.
  local base
  base="$(basename "$dep")"
  for candidate in \
      "$BREW_PREFIX/lib/$base" \
      "$BREW_PREFIX/opt"/*/lib/"$base" \
      "$BREW_PREFIX/Cellar"/*/*/lib/"$base"; do
    if [[ -f "$candidate" ]]; then
      realpath "$candidate"
      return 0
    fi
  done
  return 1
}

# Recursively copy non-system dylibs into Frameworks and rewrite load paths.
# Unlike dylibbundler, this never prompts interactively and fails loudly.
bundle_deps() {
  local target="$1"
  local line dep name dest real

  while IFS= read -r line; do
    dep="$(echo "$line" | awk '{print $1}')"
    case "$dep" in
      /System/*|/usr/lib/*|@executable_path/*|@loader_path/*)
        continue
        ;;
      @rpath/*)
        name="$(basename "$dep")"
        if ! real="$(resolve_lib "$BREW_PREFIX/lib/$name")"; then
          echo "error: cannot resolve @rpath dependency '$dep' needed by $target" >&2
          exit 1
        fi
        ;;
      /*)
        name="$(basename "$dep")"
        case "$name" in
          libjpeg*.dylib) name="libjpeg.8.dylib" ;;
        esac
        if [[ -f "$dep" ]]; then
          real="$(realpath "$dep")"
        elif ! real="$(resolve_lib "$dep")"; then
          echo "error: cannot resolve dependency '$dep' needed by $target" >&2
          exit 1
        fi
        ;;
      *)
        continue
        ;;
    esac

    dest="$FW/$name"

    if [[ ! -f "$dest" ]]; then
      echo "  + $name  (from $real)"
      cp -f "$real" "$dest"
      chmod u+w "$dest"
      install_name_tool -id "@executable_path/../Frameworks/$name" "$dest" 2>/dev/null || true
      codesign --force --sign - "$dest" >/dev/null 2>&1 || true
      bundle_deps "$dest"
    fi

    # Rewrite matching absolute / @rpath references on the target.
    while IFS= read -r changed; do
      changed="$(echo "$changed" | awk '{print $1}')"
      case "$changed" in
        /*|@rpath/*)
          if [[ "$(basename "$changed")" == "$name" ]] || \
             [[ "$(basename "$changed")" == "$(basename "$real")" ]] || \
             { [[ "$name" == "libjpeg.8.dylib" ]] && [[ "$(basename "$changed")" == libjpeg* ]]; }; then
            install_name_tool -change "$changed" \
              "@executable_path/../Frameworks/$name" "$target" 2>/dev/null || true
          fi
          ;;
      esac
    done < <(otool -L "$target" | tail -n +2)
  done < <(otool -L "$target" | tail -n +2)
}

echo "==> Bundling GTK / GLib dylibs into Frameworks"
bundle_deps "$MACOS/maturita-bin"
codesign --force --sign - "$MACOS/maturita-bin" >/dev/null 2>&1 || true

echo "==> Verifying Frameworks completeness"
MISSING=0
check_file_deps() {
  local file="$1"
  local line dep name
  while IFS= read -r line; do
    dep="$(echo "$line" | awk '{print $1}')"
    case "$dep" in
      @executable_path/../Frameworks/*)
        name="$(basename "$dep")"
        if [[ ! -f "$FW/$name" ]]; then
          echo "MISSING: $name (required by $(basename "$file"))" >&2
          MISSING=1
        fi
        ;;
      @rpath/*)
        echo "UNRESOLVED @rpath: $dep (in $(basename "$file"))" >&2
        MISSING=1
        ;;
      /opt/homebrew/*|/usr/local/*)
        echo "UNBUNDLED absolute dep still present: $dep (in $(basename "$file"))" >&2
        MISSING=1
        ;;
    esac
  done < <(otool -L "$file" | tail -n +2)
}
check_file_deps "$MACOS/maturita-bin"
for f in "$FW"/*.dylib; do
  check_file_deps "$f"
done
if [[ "$MISSING" -ne 0 ]]; then
  echo "error: macOS bundle is incomplete" >&2
  exit 1
fi
echo "  Frameworks: $(ls "$FW" | wc -l | tr -d ' ') dylibs"

# Copy GLib / GTK runtime data (schemas, modules, icon themes).
copy_pkg_share() {
  local prefix
  prefix="$(pkg-config --variable=prefix "$1" 2>/dev/null || true)"
  [[ -n "$prefix" && -d "$prefix/share" ]] || return 0
  mkdir -p "$RES/share"
  for sub in glib-2.0 gtk-4.0 icons themes locale; do
    if [[ -d "$prefix/share/$sub" ]]; then
      rsync -a "$prefix/share/$sub" "$RES/share/" 2>/dev/null \
        || cp -R "$prefix/share/$sub" "$RES/share/"
    fi
  done
}

copy_pkg_share gtk4
copy_pkg_share glib-2.0

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
export GTK_DATA_PREFIX="$ROOT/Resources"
export GTK_EXE_PREFIX="$ROOT"
export XDG_DATA_HOME="$ROOT/Resources"
exec "$ROOT/MacOS/maturita-bin" "$@"
EOF
chmod +x "$MACOS/maturita"

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

if command -v sips >/dev/null 2>&1 && command -v iconutil >/dev/null 2>&1; then
  ICONSET="$RES/AppIcon.iconset"
  mkdir -p "$ICONSET"
  for sz in 16 32 64 128 256 512; do
    sips -z "$sz" "$sz" "$ICON_512" --out "$ICONSET/icon_${sz}x${sz}.png" >/dev/null
    sips -z $((sz * 2)) $((sz * 2)) "$ICON_512" \
      --out "$ICONSET/icon_${sz}x${sz}@2x.png" >/dev/null
  done
  iconutil -c icns -o "$RES/AppIcon.icns" "$ICONSET" && rm -rf "$ICONSET" || true
fi

# Ad-hoc sign the .app so Gatekeeper is less likely to kill dyld loads.
codesign --force --deep --sign - "$APP" >/dev/null 2>&1 || true

echo "==> Smoke-testing launch"
if [[ -n "${GITHUB_ACTIONS:-}${CI:-}" ]]; then
  echo "  (CI: skipping GUI smoke test; Frameworks verification already passed)"
else
  SMOKE_LOG="$(mktemp)"
  "$MACOS/maturita" >"$SMOKE_LOG" 2>&1 &
  SMOKE_PID=$!
  sleep 3
  if ! kill -0 "$SMOKE_PID" 2>/dev/null; then
    if grep -q "Library not loaded" "$SMOKE_LOG"; then
      echo "error: Maturita.app missing bundled libraries" >&2
      cat "$SMOKE_LOG" >&2 || true
      exit 1
    fi
    echo "error: Maturita.app exited during smoke test" >&2
    cat "$SMOKE_LOG" >&2 || true
    exit 1
  fi
  kill "$SMOKE_PID" 2>/dev/null || true
  wait "$SMOKE_PID" 2>/dev/null || true
  rm -f "$SMOKE_LOG"
  echo "  smoke test OK (process stayed up)"
fi

echo "==> Zipping"
rm -f "$ROOT/dist/$OUT_ZIP"
(
  cd "$ROOT/dist"
  zip -qry "$OUT_ZIP" "$APP_NAME"
)

echo "Created dist/$OUT_ZIP"
