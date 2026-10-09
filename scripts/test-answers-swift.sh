#!/usr/bin/env bash
# Shared answer-check cases (tests/answers.json) against the Swift apps' code.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OUT="$ROOT/build/tests"
mkdir -p "$OUT"

python3 "$ROOT/scripts/build-content.py" --out "$OUT/content.json"
xcrun swiftc -O -o "$OUT/swift-answers" \
  "$ROOT/ios/Graduately/Data/Answers.swift" "$ROOT/tests/swift/main.swift"
"$OUT/swift-answers" "$ROOT/tests/answers.json" "$OUT/content.json"
