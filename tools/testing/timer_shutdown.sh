#!/usr/bin/env bash
# Test idle, distant-deadline and repeated timer shutdown without a ROM.
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
    -I"$D/ultramodern/include" -I"$D/thirdparty/concurrentqueue" -I"$D/thirdparty" \
    "$R/tools/testing/test_timer_shutdown.cpp" -o "$T/test_timer_shutdown"
timeout 5 "$T/test_timer_shutdown"
timeout 5 "$T/test_timer_shutdown" distant
printf 'All timer-shutdown regression tests passed.\n'
