#!/usr/bin/env bash
# Test the host's ledge release against synthetic RDRAM; no ROM or generated game code needed.
set -euo pipefail
R=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)
D=$R/deps/N64ModernRuntime
T=$(mktemp -d)
cleanup() {
    if [[ -n "$T" && "$T" == "${TMPDIR:-/tmp}/"* && -d "$T" ]]; then
        rm -rf -- "$T"
    fi
}
trap cleanup EXIT
"${CXX:-c++}" -std=c++20 -pthread \
    -I"$D/N64Recomp/include" -I"$D/librecomp/include" \
    -I"$D/ultramodern/include" -I"$D/thirdparty/concurrentqueue" -I"$D/thirdparty" \
    "$R/tools/testing/test_ledge_release.cpp" -o "$T/test_ledge_release"
timeout 5 "$T/test_ledge_release"
