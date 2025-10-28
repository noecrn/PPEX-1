rm -f file_created
../../minimake -p -f tests/simple_rule/Makefile

if [ -f file_created ]; then
    exit 0
else
    exit 1
fi
