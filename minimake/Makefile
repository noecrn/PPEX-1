CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla -g -fsanitize=address
LDFLAGS=-fsanitize=address
SRCS=src/minimake.c src/parser.c src/expansion.c src/destroy.c src/executor.c src/dlist/dlist.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean minimake check

all: minimake

minimake: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o src/minimake

check: minimake
	./tests/test_suite.sh

clean:
	$(RM) src/minimake .DS_store $(OBJS) src/*.gcda src/*.gcno src/dlist/*.gcda src/dlist/*.gcno ../tags coverage.info
	rm -rf coverage
