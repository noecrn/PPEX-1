#!/bin/sh

my_out="my_out"
ref_out="ref_out"

RED='\033[0;31m'
NC='\033[0m'

rm -f "$my_out"
rm -f "$ref_out"

make --no-print-directory -f Makefile > "$ref_out"
expected_code=$?
"../../src/minimake" -f Makefile > "$my_out"
exit_code=$?

# --- CHECK EXIT CODE ---
if [ "$exit_code" -ne "$expected_code" ]; then
    echo $RED KO, Wrong exit code $NC
	echo $RED Expected: $expected_code $NC
	echo $RED Got: $exit_code $NC
	exit 1
fi

diff -u "$ref_out" "$my_out"
