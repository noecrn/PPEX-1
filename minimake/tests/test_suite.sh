#!/bin/bash

ref_out="ref_out"
my_out="my_out"

total_tests=0
passed_tests=0

RED='\033[0;31m'
GRN='\033[0;32m'
YEL='\033[0;33m'
NC='\033[0m'

echo -e $YEL "Running minimake test suite..." $NC

if [ ! -f "./src/minimake" ]; then
    echo -e $RED "Error: No binary to run" $NC
fi

func_file () {
    path="$1/test.sh"
    if [ -f "$path" ] && [ -x "$path" ]; then
        echo -e $YEL "--- Test: $1 ---" $NC
        total_tests=$((total_tests+1))
        cd $1
        ./test.sh
        local exit_status=$?
        cd ..

        # --- CHECK EXIT CODE ---
        if [ $exit_status -eq 0 ] ; then
            echo -e $GRN OK $NC
            passed_tests=$((passed_tests+1))
        else
            echo -e $RED KO $NC
        fi

    fi
}

func_dir () {
	for dir in *; do
		if [ -d "$dir" ]; then
            func_file "$dir"
		fi
	done
}

cd tests
func_dir
echo -e $YEL "Passed $passed_tests/$total_tests tests" $NC
cd ..
echo -e ""

exit 0;
