#!/usr/bin/env bash
# Local smoke checks for installers, content packs, and published builds.
# Does not replace CI; catches packaging / content regressions early.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
REPO="${REPO:-pepaskopec-blip/maturita.c}"
fail=0

say() { printf '==> %s\n' "$*"; }
ok() { printf '    ok: %s\n' "$*"; }
bad() { printf '    FAIL: %s\n' "$*" >&2; fail=1; }

say "Rebuild installer zips"
chmod +x installers/pack-installer-zips.sh
./installers/pack-installer-zips.sh
ok "installer zips refreshed"

say "Installer zip contents"
expect_zip() {
  local zip=$1 want=$2
  if unzip -Z1 "$zip" | grep -qxF "$want"; then
    ok "$zip → $want"
  else
    bad "$zip missing $want (has: $(unzip -Z1 "$zip" | tr '\n' ' '))"
  fi
}
expect_zip installers/maturita-installer-macos.zip "Nainstalovat Graduately.app/Contents/MacOS/installer"
expect_zip installers/maturita-installer-linux.zip "maturita-installer-linux.sh"
expect_zip installers/maturita-installer-windows.zip "maturita-installer-windows.cmd"
expect_zip installers/maturita-installer-android.zip "Stahnout Graduately.html"
expect_zip installers/maturita-installer-ios.zip "Jak nainstalovat Graduately.html"

say "Extract content.json"
tmp=$(mktemp)
python3 scripts/extract-android-content.py --out "$tmp" >/dev/null
python3 - "$tmp" <<'PY'
import json, sys
path = sys.argv[1]
d = json.load(open(path, encoding="utf-8"))
books = d.get("books") or []
assert len(books) == 82, f"expected 82 books, got {len(books)}"
for b in books:
    bid = b.get("id")
    assert b.get("quiz"), f"{bid}: empty quiz"
    assert b.get("plot"), f"{bid}: empty plot"
    assert b.get("notes"), f"{bid}: empty notes"
    assert b.get("quizTitle"), f"{bid}: missing quizTitle"
    assert b.get("plotTitle"), f"{bid}: missing plotTitle"
assert d.get("changelog"), "changelog missing from content.json"
print(f"books={len(books)} changelog={len(d['changelog'])}")
PY
ok "content packs"
rm -f "$tmp"

say "GTK binary"
if [[ -x ./maturita ]]; then
  ./maturita --help >/dev/null
  ok "maturita --help"
else
  say "Building GTK binary"
  make -j"$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 2)"
  ./maturita --help >/dev/null
  ok "maturita --help after make"
fi

say "Published builds tip"
sha=$(curl -fsSL --max-time 20 \
  "https://github.com/$REPO/commits/builds.atom" |
  sed -n 's/.*Commit\/\([0-9a-f]\{40\}\).*/\1/p' |
  head -n 1)
if [[ ${#sha} -eq 40 ]]; then
  ok "builds tip ${sha:0:7}"
  for asset in \
    maturita-macos-arm64.zip \
    maturita-windows-x64.zip \
    maturita-linux-x86_64.AppImage \
    maturita-android.apk \
    maturita-ios.ipa \
    VERSION
  do
    code=$(curl -sI -o /dev/null -w '%{http_code}' --max-time 30 \
      "https://raw.githubusercontent.com/$REPO/$sha/$asset")
    if [[ "$code" == "200" ]]; then
      ok "$asset ($code)"
    else
      bad "$asset HTTP $code"
    fi
  done
else
  bad "could not resolve builds tip SHA"
fi

if [[ "$fail" -ne 0 ]]; then
  echo "smoke failed" >&2
  exit 1
fi
echo "smoke passed"
