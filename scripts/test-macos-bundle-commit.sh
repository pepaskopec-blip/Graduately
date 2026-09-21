#!/usr/bin/env bash
# Exercise build identity at the real bundler entry point without Xcode.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d "${TMPDIR:-/tmp}/maturita-bundle-commit.XXXXXX")"
trap 'rm -rf "$TMP"' EXIT

fail() {
  echo "FAIL: $*" >&2
  exit 1
}

mkdir -p "$TMP/bin"
cat > "$TMP/bin/xcodebuild" <<'EOF'
#!/usr/bin/env bash
printf '%s\n' "${COMMIT-UNSET}" >> "$MATURITA_TEST_MAKE_LOG"
# Stop before packaging, after observing the real build's environment.
exit 97
EOF
cat > "$TMP/bin/uname" <<'EOF'
#!/bin/sh
echo arm64
EOF
chmod +x "$TMP/bin/xcodebuild" "$TMP/bin/uname"

new_fixture() {
  mkdir -p "$1/scripts"
  cp "$ROOT/scripts/bundle-macos.sh" "$1/scripts/bundle-macos.sh"
  cp "$ROOT/scripts/bundle-macos-swift.sh" "$1/scripts/bundle-macos-swift.sh"
  chmod +x "$1/scripts/bundle-macos.sh" "$1/scripts/bundle-macos-swift.sh"
}

run_bundle() {
  local fixture="$1" mode="$2" value="${3-}"
  MAKE_LOG="$fixture/make.log"
  OUTPUT_LOG="$fixture/output.log"
  rm -f "$MAKE_LOG"
  if (
    unset COMMIT GIT_DIR GIT_WORK_TREE GIT_COMMON_DIR GIT_INDEX_FILE
    export PATH="$TMP/bin:$PATH"
    export MATURITA_TEST_MAKE_LOG="$MAKE_LOG"
    # A source archive must not accidentally inherit an enclosing checkout.
    export GIT_CEILING_DIRECTORIES="$TMP"
    if [[ "$mode" == supplied ]]; then
      export COMMIT="$value"
    fi
    bash "$fixture/scripts/bundle-macos.sh"
  ) > "$OUTPUT_LOG" 2>&1; then
    STATUS=0
  else
    STATUS=$?
  fi
}

expect_build_commit() {
  local expected="$1" actual
  [[ "$STATUS" -eq 97 ]] || {
    cat "$OUTPUT_LOG" >&2
    fail "expected the stubbed build to run (status $STATUS)"
  }
  [[ -f "$MAKE_LOG" ]] || fail "xcodebuild was not called"
  actual="$(cat "$MAKE_LOG")"
  [[ "$actual" == "$expected" ]] ||
    fail "build did not receive COMMIT=$expected (got '$actual')"
}

expect_missing_identity() {
  [[ "$STATUS" -ne 0 ]] || fail 'missing identity unexpectedly succeeded'
  [[ ! -e "$MAKE_LOG" ]] || fail 'missing identity reached xcodebuild'
  grep -q 'COMMIT' "$OUTPUT_LOG" || fail 'missing identity has no COMMIT guidance'
}

GIT_FIXTURE="$TMP/checkout with spaces"
new_fixture "$GIT_FIXTURE"
git init -q "$GIT_FIXTURE"
git -C "$GIT_FIXTURE" -c user.name='Bundle test' \
  -c user.email='bundle-test@example.invalid' -c commit.gpgsign=false \
  -c core.hooksPath=/dev/null commit --allow-empty -qm 'Fixture commit'
EXPECTED="$(git -C "$GIT_FIXTURE" rev-parse HEAD)"

run_bundle "$GIT_FIXTURE" unset
expect_build_commit "$EXPECTED"
echo 'PASS: Git checkout identity with spaces in its path'

run_bundle "$GIT_FIXTURE" supplied ''
expect_build_commit "$EXPECTED"
echo 'PASS: empty COMMIT falls back to checkout identity'

ARCHIVE_FIXTURE="$TMP/source archive"
new_fixture "$ARCHIVE_FIXTURE"
EXPLICIT=0123456789abcdef0123456789abcdef01234567
run_bundle "$ARCHIVE_FIXTURE" supplied "$EXPLICIT"
expect_build_commit "$EXPLICIT"
echo 'PASS: explicit COMMIT works without Git metadata'

run_bundle "$ARCHIVE_FIXTURE" supplied dev
expect_build_commit dev
echo 'PASS: explicit development identity is preserved'

run_bundle "$ARCHIVE_FIXTURE" unset
expect_missing_identity
echo 'PASS: source archive without COMMIT fails before building'

NESTED_FIXTURE="$GIT_FIXTURE/exported sources"
new_fixture "$NESTED_FIXTURE"
run_bundle "$NESTED_FIXTURE" unset
expect_missing_identity
echo 'PASS: source archive does not inherit an unrelated parent Git identity'
