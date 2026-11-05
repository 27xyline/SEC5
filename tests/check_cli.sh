#!/bin/sh
set -eu
cd "$(dirname "$0")/.."
program=$1
status=0
output=$("$program" list examples/sample.cpp) || status=$?
[ "$status" -eq 0 ]
[ "$output" = "$(printf 'iostream\ndebug.hpp')" ]

output=$("$program" check examples/sample.cpp examples/allowed.txt) || status=$?
[ "$status" -eq 1 ]
[ "$output" = 'examples/sample.cpp:2: header not allowed: debug.hpp' ]

status=0
"$program" check examples/sample.cpp /dev/null >/dev/null || status=$?
[ "$status" -eq 1 ]

status=0
"$program" list examples/missing.cpp >/dev/null 2>&1 || status=$?
[ "$status" -eq 2 ]

status=0
"$program" check examples/sample.cpp examples/missing.txt >/dev/null 2>&1 || status=$?
[ "$status" -eq 2 ]
