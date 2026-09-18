#!/bin/sh
# Build an unsigned iOS IPA for sideloading (Xcode / AltStore / Sideloadly).

set -eu

root=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
cd "$root"

python3 scripts/extract-android-content.py

export DEVELOPER_DIR="${DEVELOPER_DIR:-/Applications/Xcode.app/Contents/Developer}"
COMMIT="${COMMIT:-$(git -C "$root" rev-parse HEAD 2>/dev/null || echo dev)}"
dd="$root/ios/DerivedData"

rm -rf "$root/ios/Payload"
mkdir -p "$dd" "$root/dist-ios"

xcodebuild \
  -project ios/Maturita.xcodeproj \
  -scheme Maturita \
  -configuration Release \
  -sdk iphoneos \
  -destination 'generic/platform=iOS' \
  -derivedDataPath "$dd" \
  CODE_SIGNING_ALLOWED=NO \
  CODE_SIGNING_REQUIRED=NO \
  COMMIT="$COMMIT"

app=$(find "$dd" -name 'Maturita.app' -path '*/Release-iphoneos/*' | head -n 1)
if [ -z "$app" ]; then
  echo "Maturita.app not found" >&2
  exit 1
fi

mkdir -p "$root/ios/Payload"
cp -R "$app" "$root/ios/Payload/Maturita.app"
(cd "$root/ios" && zip -r -q "$root/dist-ios/maturita-ios.ipa" Payload)
echo "wrote $root/dist-ios/maturita-ios.ipa"
