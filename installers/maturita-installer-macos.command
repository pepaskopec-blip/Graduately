#!/bin/sh
# maturita.c installer for macOS.
#
# Carries no application of its own. Fastly caches branch-named URLs on
# raw.githubusercontent.com, so this script looks up the tip of `builds`
# and downloads that commit — not a stale zip from the last five minutes.
#
# Double-click the file to run it. The app updates itself from then on.

set -eu

REPO="pepaskopec-blip/maturita.c"
BRANCH="builds"
ASSET="maturita-macos-arm64.zip"

say() { printf '%s\n' "$*"; }
die() { printf '\nError: %s\n' "$*" >&2; printf 'Press Return to close. '; read -r _; exit 1; }

# Atom feed of the builds branch: curl only, no GitHub API token.
builds_sha() {
    curl -fsSL --max-time 20 \
        "https://github.com/$REPO/commits/$BRANCH.atom" |
        sed -n 's/.*Commit\/\([0-9a-f]\{40\}\).*/\1/p' |
        head -n 1
}

say "Installing maturita.C"
say "---------------------"

arch=$(uname -m)
if [ "$arch" != "arm64" ]; then
    die "this build is for Apple Silicon Macs, but this one reports '$arch'.
Build from source instead: https://github.com/$REPO"
fi

command -v curl >/dev/null 2>&1 || die "curl was not found."

if [ -w /Applications ]; then
    dest_dir="/Applications"
else
    dest_dir="$HOME/Applications"
    mkdir -p "$dest_dir"
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT INT TERM

say "Looking up the latest build..."
sha=$(builds_sha)
[ ${#sha} -eq 40 ] || die "could not find the latest build on GitHub."
URL="https://raw.githubusercontent.com/$REPO/$sha/$ASSET"

say "Downloading build $(printf '%.7s' "$sha")..."
curl -fL --progress-bar --max-time 1800 -o "$tmp/$ASSET" "$URL" ||
    die "the download failed. Check your internet connection."

say "Unpacking..."
mkdir -p "$tmp/new"
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
    [ -e "$dest_dir/$name.old" ] && mv "$dest_dir/$name.old" "$dest_dir/$name"
    die "could not write to $dest_dir."
fi

xattr -cr "$dest_dir/$name" 2>/dev/null || true

say ""
say "Done. Starting the app..."
open "$dest_dir/$name"
