

run: flark
	./flark

# flark: src
# 	gcc -g -Wall -Wpedantic -std=c23 src/main.c src/geometry.c src/text.c src/level.c -lc -lSDL3 -o flark

flark: src
	gcc -g -Wall -Wpedantic -std=c23 -I./plustypes $(wildcard src/*.c) -lc -lSDL3 -o flark

vg: flark
	pager --spawn "valgrind -s --track-origins=yes --leak-check=full --show-leak-kinds=all ./flark"

# mc: build/flark
# 	LD_PRELOAD=/usr/lib/libc_malloc_debug.so MALLOC_TRACE=trace.txt build/bin/flark
