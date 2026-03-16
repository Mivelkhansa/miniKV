CFLAGS = -I./include -Wall -Wextra -Wpedantic -g

objs = \
build/objs/main.o \
build/objs/dict.o \
build/objs/parser.o \
build/objs/respHelper.o

all: build/bin/server

build/bin/server: ${objs} | build/bin
	gcc ${CFLAGS} -o build/bin/server ${objs}

build/objs/main.o: src/main.c | build/objs
	gcc ${CFLAGS} -c $< -o $@

build/objs/dict.o: src/dict.c | build/objs
	gcc ${CFLAGS} -c $< -o $@

build/objs/parser.o: src/parser.c | build/objs
	gcc ${CFLAGS} -c $< -o $@

build/objs/respHelper.o: src/respHelper.c | build/objs
	gcc ${CFLAGS} -c $< -o $@

build/objs:
	mkdir -p $@

build/bin:
	mkdir -p $@

clean:
	rm -rf build/bin/*
	rm -rf build/objs/*
