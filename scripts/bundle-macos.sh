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

# Developer ID + notarization is what lets a downloaded app open without
# System Settings. Without a certificate the bundle stays ad-hoc signed, which
# is enough for local runs but Gatekeeper still blocks internet downloads.
ENTITLEMENTS="$ROOT/macos/Maturita.entitlements"

pick_sign_identity() {
  if [[ -n "${MACOS_SIGN_IDENTITY:-}" ]]; then
    printf '%s\n' "$MACOS_SIGN_IDENTITY"
    return
  fi
  local found
  found="$(security find-identity -v -p codesigning 2>/dev/null \
    | awk -F'"' '/Developer ID Application/ { print $2; exit }')"
  if [[ -n "$found" ]]; then
    printf '%s\n' "$found"
    return
  fi
  printf '%s\n' "-"
}

sign_one() {
  local path="$1"
  local use_entitlements="${2:-}"
  local args=(--force --sign "$IDENTITY")

  if [[ "$IDENTITY" != "-" ]]; then
    args+=(--options runtime --timestamp)
  fi
  if [[ "$use_entitlements" == "entitlements" && -f "$ENTITLEMENTS" ]]; then
    args+=(--entitlements "$ENTITLEMENTS")
  fi
  codesign "${args[@]}" "$path"
}

sign_bundle() {
  local f

  echo "==> Signing $APP_NAME"
  IDENTITY="$(pick_sign_identity)"
  if [[ "$IDENTITY" == "-" ]]; then
    echo "  identity: ad-hoc (downloaded copies will still need Gatekeeper approval)"
  else
    echo "  identity: $IDENTITY"
  fi

  shopt -s nullglob
  for f in "$FW"/*.dylib; do
    sign_one "$f"
  done
  shopt -u nullglob

  sign_one "$MACOS/maturita-bin" entitlements
  sign_one "$MACOS/maturita" entitlements
  sign_one "$APP" entitlements

  codesign --verify --strict --verbose=2 "$APP"
}

can_notarize() {
  [[ "$IDENTITY" != "-" ]] || return 1
  if [[ -n "${APPLE_API_KEY:-}" && -n "${APPLE_API_KEY_ID:-}" &&
        -n "${APPLE_API_ISSUER:-}" ]]; then
    return 0
  fi
  if [[ -n "${APPLE_ID:-}" && -n "${APPLE_APP_SPECIFIC_PASSWORD:-}" &&
        -n "${APPLE_TEAM_ID:-}" ]]; then
    return 0
  fi
  return 1
}

notarize_bundle() {
  local zip notary_args keyfile

  if [[ "$IDENTITY" == "-" ]]; then
    return 0
  fi
  if ! can_notarize; then
    if [[ -n "${GITHUB_ACTIONS:-}${CI:-}" ]]; then
      echo "error: Developer ID is present but notarization secrets are missing." >&2
      echo "Set APPLE_API_KEY + APPLE_API_KEY_ID + APPLE_API_ISSUER, or" >&2
      echo "APPLE_ID + APPLE_APP_SPECIFIC_PASSWORD + APPLE_TEAM_ID." >&2
      exit 1
    fi
    echo "  warning: signed, but not notarized — Gatekeeper will still block downloads"
    return 0
  fi

  echo "==> Notarizing $APP_NAME"
  zip="$ROOT/dist/.notarize-$OUT_ZIP"
  rm -f "$zip"
  ditto -c -k --keepParent "$APP" "$zip"

  notary_args=(submit "$zip" --wait)
  keyfile=""
  if [[ -n "${APPLE_API_KEY:-}" ]]; then
    keyfile="$(mktemp)"
    printf '%s\n' "$APPLE_API_KEY" > "$keyfile"
    notary_args+=(--key "$keyfile" --key-id "$APPLE_API_KEY_ID"
                  --issuer "$APPLE_API_ISSUER")
  else
    notary_args+=(--apple-id "$APPLE_ID"
                  --password "$APPLE_APP_SPECIFIC_PASSWORD"
                  --team-id "$APPLE_TEAM_ID")
  fi

  xcrun notarytool "${notary_args[@]}"
  [[ -n "$keyfile" ]] && rm -f "$keyfile"
  rm -f "$zip"
  xcrun stapler staple "$APP"
  echo "  stapled notarization ticket"
}

sign_bundle

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

notarize_bundle

echo "==> Zipping"
rm -f "$ROOT/dist/$OUT_ZIP"
(
  cd "$ROOT/dist"
  zip -qry "$OUT_ZIP" "$APP_NAME"
)

echo "Created dist/$OUT_ZIP"
