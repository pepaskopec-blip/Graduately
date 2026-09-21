#!/usr/bin/env bash
# Official macOS package is the native Swift app.
# The GTK bundler used to live here; Linux/Windows still use GTK.
set -euo pipefail
exec "$(cd "$(dirname "$0")" && pwd)/bundle-macos-swift.sh" "$@"
