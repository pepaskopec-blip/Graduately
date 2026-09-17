#!/bin/sh
# maturita.c installer for macOS.
#
# Carries no application of its own: it downloads whatever the newest release
# is at the moment you run it, so this file never goes stale. GitHub resolves
# the /releases/latest/download/ path to the current tag on every request.
#
# Double-click the file to run it. The app updates itself from then on.

set -eu

REPO="pepaskopec-blip/maturita.c"
ASSET="maturita-macos-arm64.zip"
URL="https://github.com/$REPO/releases/latest/download/$ASSET"

say() { printf '%s\n' "$*"; }
die() { printf '\nError: %s\n' "$*" >&2; printf 'Press Return to close. '; read -r _; exit 1; }

say "Installing maturita.C"
say "---------------------"

# Only Apple Silicon builds are published; Rosetta cannot run an arm64 app.
arch=$(uname -m)
if [ "$arch" != "arm64" ]; then
    die "this build is for Apple Silicon Macs, but this one reports '$arch'.
Build from source instead: https://github.com/$REPO"
fi

command -v curl >/dev/null 2>&1 || die "curl was not found."

# Prefer the shared folder, fall back to the user's own when it is locked down.
if [ -w /Applications ]; then
    dest_dir="/Applications"
else
    dest_dir="$HOME/Applications"
    mkdir -p "$dest_dir"
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT INT TERM

say "Downloading the latest release..."
curl -fL --progress-bar -o "$tmp/$ASSET" "$URL" ||
    die "the download failed. Check your internet connection."

say "Unpacking..."
mkdir -p "$tmp/new"
# ditto keeps bundle symlinks and permissions intact, unlike plain unzip.
ditto -x -k "$tmp/$ASSET" "$tmp/new" || die "the archive could not be unpacked."

app=$(find "$tmp/new" -maxdepth 1 -name '*.app' -print -quit)
[ -n "$app" ] || die "the archive did not contain an application."
name=$(basename "$app")

say "Installing to $dest_dir/$name"
rm -rf "$dest_dir/$name.old"
if [ -e "$dest_dir/$name" ]; then
    mv "$dest_dir/$name" "$dest_dir/$name.old"
fi
if mv "$app" "$dest_dir/$name"; then
    rm -rf "$dest_dir/$name.old"
else
    # Put the previous copy back rather than leaving nothing installed.
    [ -e "$dest_dir/$name.old" ] && mv "$dest_dir/$name.old" "$dest_dir/$name"
    die "could not write to $dest_dir."
fi

# The download carries a quarantine flag, and the app is not signed, so
# without this macOS refuses to open it.
xattr -cr "$dest_dir/$name" 2>/dev/null || true

say ""
say "Done. Starting the app..."
open "$dest_dir/$name"
