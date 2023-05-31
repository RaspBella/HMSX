SRC=hmsx.c
BIN=hmsx

CC=gcc
CFLAGS=-Wall -O3

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

test:
	make CFLAGS="$(CFLAGS) -Og -ggdb3"

clean:
	rm -f $(BIN)