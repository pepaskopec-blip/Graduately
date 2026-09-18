#!/bin/sh
# Rebuild the installer zips that README links to. Browsers download a
# zip; they render .command / .sh / .cmd / .html as a text tab.

set -eu

cd "$(dirname "$0")"
here=$PWD

chmod +x maturita-installer-macos.command maturita-installer-linux.sh

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT INT TERM

app="$tmp/Nainstalovat maturita.C.app"
mkdir -p "$app/Contents/MacOS"
cp macos/Info.plist "$app/Contents/Info.plist"
cp maturita-installer-macos.command "$app/Contents/MacOS/installer"
chmod +x "$app/Contents/MacOS/installer"

rm -f "$here/maturita-installer-macos.zip"
(cd "$tmp" && zip -r -q "$here/maturita-installer-macos.zip" \
    "Nainstalovat maturita.C.app")

zip -q -FS maturita-installer-windows.zip maturita-installer-windows.cmd
zip -q -FS maturita-installer-linux.zip maturita-installer-linux.sh

# Browsers download a zip; they render .html as a text tab on GitHub.
# The page looks up the tip of `builds` so Fastly cannot serve a cached 404
# for the branch-named APK URL.
cp maturita-android.html "$tmp/Stahnout maturita.C.html"
rm -f "$here/maturita-installer-android.zip"
(cd "$tmp" && zip -q -FS "$here/maturita-installer-android.zip" \
    "Stahnout maturita.C.html")
