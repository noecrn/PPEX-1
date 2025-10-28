CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla
SRCS=minimake.c parser.c expansion.c destroy.c dlist/dlist_1.c dlist/dlist_2.c dlist/dlist_3.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean all

minimake: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o minimake

check:

clean:
	$(RM) minimake .DS_store $(OBJS)
