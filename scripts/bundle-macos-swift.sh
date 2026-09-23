#!/bin/sh
# Build the native Swift macOS .app and zip it for the builds branch.

set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
cd "$root"

export DEVELOPER_DIR="${DEVELOPER_DIR:-/Applications/Xcode.app/Contents/Developer}"
if [ -z "${COMMIT:-}" ]; then
  if [ ! -e "$root/.git" ] ||
     ! COMMIT="$(git -C "$root" rev-parse --verify HEAD 2>/dev/null)"; then
    echo "error: cannot determine the source commit; set COMMIT when bundling without Git metadata" >&2
    exit 1
  fi
fi
export COMMIT

arch=$(uname -m)
case "$arch" in
  arm64|aarch64) arch_tag=arm64 ;;
  x86_64) arch_tag=x86_64 ;;
  *)
    echo "Unsupported architecture: $arch" >&2
    exit 1
    ;;
esac

dd="$root/macos/DerivedData"
out_zip="graduately-macos-${arch_tag}.zip"
mkdir -p "$dd" "$root/dist"

xcodebuild \
  -project macos/Maturita.xcodeproj \
  -scheme Maturita \
  -configuration Release \
  -destination "generic/platform=macOS" \
  -derivedDataPath "$dd" \
  CODE_SIGNING_ALLOWED=NO \
  CODE_SIGNING_REQUIRED=NO \
  COMMIT="$COMMIT"

app=$(find "$dd" -name 'Graduately.app' -path '*/Release/*' | head -n 1)
if [ -z "$app" ]; then
  echo "Graduately.app not found" >&2
  exit 1
fi

dest="$root/dist/Graduately.app"
rm -rf "$dest"
cp -R "$app" "$dest"

ENTITLEMENTS="$root/macos/Maturita/Maturita.entitlements"

pick_sign_identity() {
  if [ -n "${MACOS_SIGN_IDENTITY:-}" ]; then
    printf '%s\n' "$MACOS_SIGN_IDENTITY"
    return
  fi
  found="$(security find-identity -v -p codesigning 2>/dev/null \
    | awk -F'"' '/Developer ID Application/ { print $2; exit }')"
  if [ -n "$found" ]; then
    printf '%s\n' "$found"
    return
  fi
  printf '%s\n' "-"
}

IDENTITY="$(pick_sign_identity)"
echo "==> Signing Graduately.app ($IDENTITY)"
sign_args="--force --sign $IDENTITY"
if [ "$IDENTITY" != "-" ]; then
  sign_args="$sign_args --options runtime --timestamp"
fi
if [ -f "$ENTITLEMENTS" ]; then
  # shellcheck disable=SC2086
  codesign $sign_args --entitlements "$ENTITLEMENTS" "$dest"
else
  # shellcheck disable=SC2086
  codesign $sign_args "$dest"
fi
codesign --verify --strict --verbose=2 "$dest"

can_notarize() {
  [ "$IDENTITY" != "-" ] || return 1
  if [ -n "${APPLE_API_KEY:-}" ] && [ -n "${APPLE_API_KEY_ID:-}" ] &&
     [ -n "${APPLE_API_ISSUER:-}" ]; then
    return 0
  fi
  if [ -n "${APPLE_ID:-}" ] && [ -n "${APPLE_APP_SPECIFIC_PASSWORD:-}" ] &&
     [ -n "${APPLE_TEAM_ID:-}" ]; then
    return 0
  fi
  return 1
}

if can_notarize; then
  echo "==> Notarizing Graduately.app"
  zip="$root/dist/.notarize-$out_zip"
  rm -f "$zip"
  ditto -c -k --keepParent "$dest" "$zip"
  if [ -n "${APPLE_API_KEY:-}" ]; then
    keyfile="$(mktemp)"
    printf '%s\n' "$APPLE_API_KEY" > "$keyfile"
    xcrun notarytool submit "$zip" --wait \
      --key "$keyfile" --key-id "$APPLE_API_KEY_ID" --issuer "$APPLE_API_ISSUER"
    rm -f "$keyfile"
  else
    xcrun notarytool submit "$zip" --wait \
      --apple-id "$APPLE_ID" \
      --password "$APPLE_APP_SPECIFIC_PASSWORD" \
      --team-id "$APPLE_TEAM_ID"
  fi
  rm -f "$zip"
  xcrun stapler staple "$dest"
else
  if [ "$IDENTITY" != "-" ] && [ -n "${GITHUB_ACTIONS:-}${CI:-}" ]; then
    echo "error: Developer ID is present but notarization secrets are missing." >&2
    exit 1
  fi
fi

rm -f "$root/dist/$out_zip"
(
  cd "$root/dist"
  zip -qry "$out_zip" Graduately.app
)
echo "wrote $root/dist/$out_zip"
