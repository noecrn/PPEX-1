#!/bin/bash

ref_out="ref_out"
my_out="my_out"

total_tests=0
passed_tests=0

echo "Running minimake test suite..."
echo ""

func_test () {
    total_tests++
    cd $dir
    ./test.sh
    cd ..


    tit_wrap - $PUR $1
    shift

    make -p $@ > $ref_out
    ./minimake -p -f tests/simple_test > $my_out

    diff $ref_out $my_out > /tmp/null

    if [ $? -eq 0 ]; then
        echo $GRN OK
        passed_tests++
    else
        echo $RED KO
        echo $RED EXPECTED: $BRED $(cat $ref_out)
        echo $RED GOT: $BRED $(cat $my_out)
    fi
}

func_file () {
    path="$1/test.sh"
    if [ -f "$path" ] && [ -x "$path" ]; then
        func_test "$1"
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
echo "Passed $passed_tests/$total_tests tests"
cd ..
echo -e ""
