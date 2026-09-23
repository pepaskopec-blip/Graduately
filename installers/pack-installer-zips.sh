#!/bin/sh
# Rebuild the installer zips that README links to. Browsers download a
# zip; they render .command / .sh / .cmd / .html as a text tab.

set -eu

cd "$(dirname "$0")"
here=$PWD

chmod +x graduately-installer-macos.command graduately-installer-linux.sh

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT INT TERM

app="$tmp/Nainstalovat Graduately.app"
mkdir -p "$app/Contents/MacOS"
cp macos/Info.plist "$app/Contents/Info.plist"
cp graduately-installer-macos.command "$app/Contents/MacOS/installer"
chmod +x "$app/Contents/MacOS/installer"

rm -f "$here/graduately-installer-macos.zip"
(cd "$tmp" && zip -r -q "$here/graduately-installer-macos.zip" \
    "Nainstalovat Graduately.app")

zip -q -FS graduately-installer-windows.zip graduately-installer-windows.cmd
zip -q -FS graduately-installer-linux.zip graduately-installer-linux.sh

# Browsers download a zip; they render .html as a text tab on GitHub.
# The page looks up the tip of `builds` so Fastly cannot serve a cached 404
# for the branch-named APK URL.
cp graduately-android.html "$tmp/Stahnout Graduately.html"
rm -f "$here/graduately-installer-android.zip"
(cd "$tmp" && zip -q -FS "$here/graduately-installer-android.zip" \
    "Stahnout Graduately.html")

cp graduately-ios.html "$tmp/Jak nainstalovat Graduately.html"
rm -f "$here/graduately-installer-ios.zip"
(cd "$tmp" && zip -q -FS "$here/graduately-installer-ios.zip" \
    "Jak nainstalovat Graduately.html")
