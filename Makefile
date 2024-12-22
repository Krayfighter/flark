

raylib_flags = $(shell pkg-config --libs raylib)

run: flark
	./flark

test: test_bin
	./test_bin

test_bin: src/*
	gcc src/level_loader.c src/main.c -I./plustypes -lm -lraylib/raylib_out/lib/raylib \
		-D TESTS -Wall -Werror -Wpedantic -o test_bin

vg:
	valgrind ./flark

raylib:
	cd raylib && zig build -p raylib_out

raylib_win32:
	cd raylib && zig build -Dtarget=x86_64-windows -p raylib_win32

flark: raylib src/*
	gcc src/level_loader.c src/main.c src/player.c \
		-I./plustypes/src -Iraylib/raylib_out/include -Iraylib/src \
		-Lraylib/raylib_out/lib \
		-lraylib -lm \
		-Wall -Wpedantic -o flark

flark_w32: raylib_win32 src/*
	zig cc src/main.c src/level_loader.c src/player.c \
		-target x86_64-windows \
		-I./plustypes/src -Iraylib/raylib_win32/include -Iraylib/src/ \
		-Lraylib/raylib_win32/lib \
		-lraylib -lm -lwinmm -lgdi32 -lopengl32 -o flark_w32

	# zig cc src/main.c -c \
	# 	-I../plustypes/src -Iraylib/build/raylib/include \
	# 	-Wall -Wpedantic \
	# 	-target x86_64-windows -lc
	# zig cc src/level_loader.c -c \
	# 	-I../plustypes/src -Iraylib/build/raylib/include \
	# 	-Wall -Wpedantic \
	# 	-target x86_64-windows -lc
	# zig cc -v -target x86_64-windows main.obj level_loader.obj raylib/src/*.obj \
	# 	-lm -lwinmm -lgdi32 -lopengl32 -o flark_w32
