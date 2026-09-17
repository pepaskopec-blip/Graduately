#!/bin/sh
# maturita.c installer for macOS.
#
# Carries no application of its own. Fastly caches branch-named URLs on
# raw.githubusercontent.com, so this script looks up the tip of `builds`
# and downloads that commit — not a stale zip from the last five minutes.
#
# Double-click the bundled .app (or this file). No Terminal commands needed.

set -eu

REPO="pepaskopec-blip/maturita.c"
BRANCH="builds"
ASSET="maturita-macos-arm64.zip"

# An .app has no TTY; show dialogs so a regular user never needs Terminal.
GUI=0
if [ ! -t 1 ] && command -v osascript >/dev/null 2>&1; then
    GUI=1
fi

say() { printf '%s\n' "$*"; }

gui_dialog() {
    osascript - "$1" <<'APPLESCRIPT'
on run argv
    display dialog (item 1 of argv) with title "maturita.C" buttons {"OK"} default button 1
end run
APPLESCRIPT
}

gui_alert() {
    osascript - "$1" <<'APPLESCRIPT'
on run argv
    display dialog (item 1 of argv) with title "maturita.C" buttons {"OK"} default button 1 with icon stop
end run
APPLESCRIPT
}

die() {
    if [ "$GUI" = 1 ]; then
        gui_alert "$*" >/dev/null || true
    else
        printf '\nChyba: %s\n' "$*" >&2
        printf 'Stiskněte Return. '
        read -r _ || true
    fi
    exit 1
}

builds_sha() {
    curl -fsSL --max-time 20 \
        "https://github.com/$REPO/commits/$BRANCH.atom" |
        sed -n 's/.*Commit\/\([0-9a-f]\{40\}\).*/\1/p' |
        head -n 1
}

if [ "$GUI" = 1 ]; then
    if ! osascript <<'APPLESCRIPT'
display dialog "Nainstalovat maturita.C do složky Aplikace? Stáhne se aktuální verze." with title "maturita.C" buttons {"Zrušit", "Instalovat"} default button "Instalovat"
APPLESCRIPT
    then
        exit 0
    fi
else
    say "Installing maturita.C"
    say "---------------------"
fi

arch=$(uname -m)
if [ "$arch" != "arm64" ]; then
    die "Tento instalátor je pro Mac s čipem Apple. Tento počítač hlásí '$arch'."
fi

command -v curl >/dev/null 2>&1 || die "Na Macu chybí curl, bez něj nejde nic stáhnout."

# ~/Applications is writable from the GUI updater later. /Applications
# often is not, even when this installer can drop a .app there once.
dest_dir="$HOME/Applications"
mkdir -p "$dest_dir"
if [ -e "/Applications/Maturita.app" ] && [ -w "/Applications/Maturita.app" ]; then
    rm -rf "/Applications/Maturita.app"
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT INT TERM

if [ "$GUI" = 1 ]; then
    osascript <<'APPLESCRIPT' >/dev/null || true
display notification "Stahuji aktuální verzi…" with title "maturita.C"
APPLESCRIPT
fi

sha=$(builds_sha)
[ ${#sha} -eq 40 ] || die "Nepodařilo se najít aktuální verzi na GitHubu."
URL="https://raw.githubusercontent.com/$REPO/$sha/$ASSET"

if [ "$GUI" = 1 ]; then
    curl -fsSL --max-time 1800 -o "$tmp/$ASSET" "$URL" ||
        die "Stažení se nezdařilo. Zkontrolujte internet a zkuste to znovu."
else
    say "Downloading build $(printf '%.7s' "$sha")..."
    curl -fL --progress-bar --max-time 1800 -o "$tmp/$ASSET" "$URL" ||
        die "Stažení se nezdařilo. Zkontrolujte internet a zkuste to znovu."
    say "Unpacking..."
fi

mkdir -p "$tmp/new"
ditto -x -k "$tmp/$ASSET" "$tmp/new" || die "Archiv se nepodařilo rozbalit."

app=$(find "$tmp/new" -maxdepth 1 -name '*.app' -print -quit)
[ -n "$app" ] || die "V archivu není žádná aplikace."
name=$(basename "$app")

rm -rf "$dest_dir/$name.old"
if [ -e "$dest_dir/$name" ]; then
    mv "$dest_dir/$name" "$dest_dir/$name.old"
fi
if mv "$app" "$dest_dir/$name"; then
    rm -rf "$dest_dir/$name.old"
else
    [ -e "$dest_dir/$name.old" ] && mv "$dest_dir/$name.old" "$dest_dir/$name"
    die "Do $dest_dir se nepodařilo zapsat."
fi

xattr -cr "$dest_dir/$name" 2>/dev/null || true

if [ "$GUI" = 1 ]; then
    gui_dialog "Hotovo. Aplikace se teď otevře." >/dev/null || true
else
    say ""
    say "Done. Starting the app..."
fi
open "$dest_dir/$name"
