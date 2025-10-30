#!/bin/sh

my_out="my_out"
ref_out="ref_out"

rm -f "$my_out"

"../../src/minimake" -p -f Makefile > "$my_out"

diff -u "$ref_out" "$my_out"
