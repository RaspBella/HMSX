SRC=hmsx.c
BIN=hmsx

CC=gcc
CFLAGS=-Wall -O3 -D_XOPEN_SOURCE=700

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

test:
	make CFLAGS="$(CFLAGS) -Og -ggdb3"

clean:
	rm -f $(BIN)