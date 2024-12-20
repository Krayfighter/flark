

raylib_flags = $(shell pkg-config --libs raylib)

run: flark
	./flark

test: test_bin
	./test_bin

test_bin: src/*
	gcc src/level_loader.c src/main.c -I../plustypes/src -lm $(raylib_flags) -D TESTS -Wall -Werror -Wpedantic -o test_bin

vg:
	valgrind ./flark

flark: src/*
	gcc src/level_loader.c src/main.c -I../plustypes/src -lm $(raylib_flags) -Wall -Werror -Wpedantic -o flark

flark_w32: src/*
	zig cc src/main.c -c \
		-I../plustypes/src -Iraylib/build/raylib/include \
		-Wall -Wpedantic \
		-target x86_64-windows -lc
	zig cc src/level_loader.c -c \
		-I../plustypes/src -Iraylib/build/raylib/include \
		-Wall -Wpedantic \
		-target x86_64-windows -lc
	zig cc -v -target x86_64-windows main.obj level_loader.obj raylib/src/*.obj \
		-lm -lwinmm -lgdi32 -lopengl32 -o flark_w32
		# -L/home/aiden/code/flark/raylib/zig-out/lib -lraylib -lm -o flark_w32
		# -L/home/aiden/.wine/drive_c/windows/system32 -lm -o flark_w32
	# gcc main.o level_loader.- raylib/build/raylib/libraylib.a -l -o flark_w32

	# zig cc src/level_loader.c -c \
	# 	-I../plustypes/src -Iraylib/build/raylib/include \
	# 	-Wall -Wpedantic \
	# 	-target x86_64-windows \
	# 	-o flark_w32.o
	# zig cc -v -target x86_64-windows main.o level_loader.o raylib/build/raylib/libraylib.a -lm -o flark_w32
		# -Lraylib/build/raylib -lraylib -lm \
		# -o flark_w32.exe
