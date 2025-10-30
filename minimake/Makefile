CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla
SRCS=src/minimake.c src/parser.c src/expansion.c src/destroy.c src/executor.c src/dlist/dlist_1.c src/dlist/dlist_2.c src/dlist/dlist_3.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean minimake check

all: minimake

minimake: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o src/minimake

check: minimake
	./tests/test_suite.sh

clean:
	$(RM) src/minimake .DS_store $(OBJS)
