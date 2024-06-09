src=$(wildcard src/*.c)
obj=$(patsubst src/%.c, obj/%.o, ${src})
inc=inc
bin=hmsx

cflags=-Og -ggdb3 -Wall -Wextra

default: ${bin}

${bin}: ${obj}
	cc ${cflags} $^ -o $@

obj/%.o: src/%.c
	cc ${cflags} -I${inc} -c $< -o $@

clean:
	rm -f ${obj} ${bin}
