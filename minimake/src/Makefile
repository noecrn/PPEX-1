CC=gcc
CFLAGS=-std=c99 -pedantic -Werror -Wall -Wextra -Wvla -g -fsanitize=address
LDFLAGS=-fsanitize=address
SRCS=minimake.c parser.c expansion.c destroy.c executor.c dlist/dlist_1.c dlist/dlist_2.c dlist/dlist_3.c
OBJS=$(SRCS:.c=.o)

.PHONY: clean all

all: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o minimake

debug: $(SRCS)
	$(CC) -arch x86_64 -std=c99 -Wall -Wextra -O0 -g -gdwarf-4 -fno-omit-frame-pointer \
		-Wno-deprecated-non-prototype -o minimake $(SRCS)

clean:
	$(RM) minimake .DS_store $(OBJS)

