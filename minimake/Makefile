NEEDED = foo

all: $(NEEDED)

foo: foo.o
	gcc -o foo foo.o $^

foo.o: foo.c
	gcc -c foo.c $@
