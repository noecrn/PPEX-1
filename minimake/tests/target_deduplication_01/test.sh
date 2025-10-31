#!/bin/sh

my_out="my_out"
ref_out="ref_out"
expected_code=0

RED='\033[0;31m'
NC='\033[0m'

rm -f "$my_out"

"../../src/minimake" -f Makefile all all > "$my_out"
exit_code=$?

# --- CHECK EXIT CODE ---
if [ "$exit_code" -ne "$expected_code" ]; then
    echo $RED KO, Wrong exit code $NC
	echo $RED Expected: $expected_code $NC
	echo $RED Got: $exit_code $NC
	exit 1
fi

diff -u "$ref_out" "$my_out"
