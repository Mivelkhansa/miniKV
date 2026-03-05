all: build/bin/db

build/bin/db: build/obj/main.o
	gcc -Wall -g -o build/bin/db build/obj/main.o

build/obj/main.o: src/main.c
	gcc -Wall -g -c src/main.c -o build/obj/main.o

clean:
	rm -rf build/bin/*
	rm -rf build/obj/*
