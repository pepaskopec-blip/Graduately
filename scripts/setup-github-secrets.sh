#!/usr/bin/env bash
# Configure GitHub Secrets for macOS signing/notarization and Supabase sync.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

P12_PATH=""
while [ "$#" -gt 0 ]; do
  case "$1" in
    --p12)
      P12_PATH="${2:-}"
      shift 2
      ;;
    -h|--help)
      cat <<'EOF'
Usage: ./scripts/setup-github-secrets.sh [--p12 path/to/DeveloperID.p12]

Reads Developer ID Application from Keychain (or --p12), plus optional
notarization & Supabase environment variables, and uploads them via `gh secret set`:

  APPLE_TEAM_ID              (default: C2U9S337H8)
  MACOS_CERTIFICATE          (base64 .p12)
  MACOS_CERTIFICATE_PASSWORD
  MACOS_SIGN_IDENTITY
  APPLE_API_KEY / APPLE_API_KEY_ID / APPLE_API_ISSUER
  APPLE_ID / APPLE_APP_SPECIFIC_PASSWORD
  SUPABASE_URL / SUPABASE_ANON_KEY
EOF
      exit 0
      ;;
    *)
      echo "Unknown option: $1" >&2
      exit 1
      ;;
  esac
done

if ! command -v gh >/dev/null 2>&1; then
  echo "error: GitHub CLI (gh) is required." >&2
  exit 1
fi

set_secret() {
  local name="$1" val="$2"
  printf '%s' "$val" | gh secret set "$name"
  echo "  ok: $name"
}

TEAM_ID="${APPLE_TEAM_ID:-C2U9S337H8}"
echo "==> Nastavuji GitHub Secrets"
set_secret APPLE_TEAM_ID "$TEAM_ID"

DEV_ID_NAME="$(security find-identity -v -p codesigning 2>/dev/null \
  | awk -F'"' '/Developer ID Application/ { print $2; exit }' || true)"

if [ -n "$P12_PATH" ]; then
  if [ ! -f "$P12_PATH" ]; then
    echo "error: soubor nenalezen: $P12_PATH" >&2
    exit 1
  fi
  P12_PASS="${MACOS_CERTIFICATE_PASSWORD:-}"
  if [ -z "$P12_PASS" ]; then
    read -r -s -p "Heslo k souboru $P12_PATH: " P12_PASS
    echo
  fi
  B64="$(base64 -i "$P12_PATH" | tr -d '\r\n')"
  set_secret MACOS_CERTIFICATE "$B64"
  set_secret MACOS_CERTIFICATE_PASSWORD "$P12_PASS"
  if [ -n "${MACOS_SIGN_IDENTITY:-}" ]; then
    set_secret MACOS_SIGN_IDENTITY "$MACOS_SIGN_IDENTITY"
  elif [ -n "$DEV_ID_NAME" ]; then
    set_secret MACOS_SIGN_IDENTITY "$DEV_ID_NAME"
  fi
elif [ -n "$DEV_ID_NAME" ]; then
  echo "==> Nalezen certifikát v Keychain: $DEV_ID_NAME"
  TMP_P12="$(mktemp "${TMPDIR:-/tmp}/graduately-devid.XXXXXX.p12")"
  trap 'rm -f "$TMP_P12"' EXIT
  P12_PASS="${MACOS_CERTIFICATE_PASSWORD:-$(openssl rand -base64 24)}"
  if security export -k login.keychain-db -t identities -f pkcs12 -P "$P12_PASS" -o "$TMP_P12" >/dev/null 2>&1; then
    B64="$(base64 -i "$TMP_P12" | tr -d '\r\n')"
    set_secret MACOS_CERTIFICATE "$B64"
    set_secret MACOS_CERTIFICATE_PASSWORD "$P12_PASS"
    set_secret MACOS_SIGN_IDENTITY "$DEV_ID_NAME"
  else
    echo "Nepodařilo se automaticky exportovat privátní klíč z Keychain." >&2
    echo "Exportuj '$DEV_ID_NAME' ručně z Keychain Access jako .p12 a spusť:" >&2
    echo "  ./scripts/setup-github-secrets.sh --p12 cesta/k/certifikatu.p12" >&2
  fi
else
  echo ""
  echo "Upozornění: v Keychain zatím není certifikát 'Developer ID Application' (pouze 'Apple Development')."
  echo "Jak ho vytvořit a nahrát do GitHub Secrets:"
  echo "  1. Otevři Xcode -> Settings -> Accounts -> Filip Bukovina -> Manage Certificates..."
  echo "  2. Klikni na '+' -> Developer ID Application"
  echo "  3. Znovu spusť: make secrets"
  echo "     (případně exportuj .p12 z Keychain Access a spusť ./scripts/setup-github-secrets.sh --p12 soubor.p12)"
fi

if [ -n "${APPLE_API_KEY:-}" ]; then
  if [ -f "$APPLE_API_KEY" ]; then
    set_secret APPLE_API_KEY "$(cat "$APPLE_API_KEY")"
  else
    set_secret APPLE_API_KEY "$APPLE_API_KEY"
  fi
fi
[ -n "${APPLE_API_KEY_ID:-}" ] && set_secret APPLE_API_KEY_ID "$APPLE_API_KEY_ID"
[ -n "${APPLE_API_ISSUER:-}" ] && set_secret APPLE_API_ISSUER "$APPLE_API_ISSUER"
[ -n "${APPLE_ID:-}" ] && set_secret APPLE_ID "$APPLE_ID"
[ -n "${APPLE_APP_SPECIFIC_PASSWORD:-}" ] && set_secret APPLE_APP_SPECIFIC_PASSWORD "$APPLE_APP_SPECIFIC_PASSWORD"
[ -n "${SUPABASE_URL:-}" ] && set_secret SUPABASE_URL "$SUPABASE_URL"
[ -n "${SUPABASE_ANON_KEY:-}" ] && set_secret SUPABASE_ANON_KEY "$SUPABASE_ANON_KEY"
