#!/usr/bin/env bash
# Windows x64 zip of the Compose desktop app, with its own Java runtime.
# Runs in Git Bash or MSYS2. Needs python3 and a JDK 17+ that ships jmods
# (set PACKAGE_JDK, e.g. Temurin 21).
#
# graduately.exe must sit at the root of the zip: the installer and the
# updater of older builds unpack it over the install folder and start it.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

OUT="$ROOT/dist/graduately-windows-x64.zip"
IMAGE="$ROOT/android/desktop/build/compose/binaries/main-release/app/graduately"

echo "==> Building the desktop app"
(cd android && ./gradlew :desktop:createReleaseDistributable --no-daemon)

[[ -f "$IMAGE/graduately.exe" ]] || { echo "graduately.exe missing in $IMAGE" >&2; exit 1; }

echo "==> Zipping"
mkdir -p "$ROOT/dist"
rm -f "$OUT"
if command -v 7z >/dev/null 2>&1; then
  (cd "$IMAGE" && 7z a -tzip -mx=9 "$OUT" . >/dev/null)
else
  powershell -NoProfile -Command "Compress-Archive -Path '$(cygpath -w "$IMAGE")\\*' -DestinationPath '$(cygpath -w "$OUT")'"
fi
echo "Created ${OUT#"$ROOT"/}"
