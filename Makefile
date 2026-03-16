CFLAGS = -I./include -Wall -Wextra -Wpedantic -g

objs = \
build/objs/main.o \
build/objs/dict.o \
build/objs/parser.o \
build/objs/respHelper.o

all: build/bin/server

build/bin/server: ${objs}
	gcc ${CFLAGS} -o build/bin/server ${objs}

build/objs/main.o: src/main.c
	gcc ${CFLAGS} -c $< -o $@

build/objs/dict.o: src/dict.c
	gcc ${CFLAGS} -c $< -o $@

build/objs/parser.o: src/parser.c
	gcc ${CFLAGS} -c $< -o $@

build/objs/respHelper.o: src/respHelper.c
	gcc ${CFLAGS} -c $< -o $@

clean:
	rm -rf build/bin/*
	rm -rf build/objs/*
