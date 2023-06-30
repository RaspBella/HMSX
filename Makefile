SRC=hmsx.c
BIN=hmsx

CFLAGS=-Wall -Werror -Og -ggdb3

$(BIN): $(SRC)
	cc $(CFLAGS) $< -o $@

clean:
	rm $(BIN)
