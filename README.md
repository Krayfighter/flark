
## Flark

#### video game of the century

Flark is a 2D platformer with an emphasis on physics,
movement mechanics, and hackability.

Flark supports custom levels as a first-class feature
base on .lvl files with a custom parser.

note: the flark executable must be in the same directory
as the .lvl files in order to see them, additionally
file selection is not currently working on windows due
to a bug, so windows builds of flark can only access files
named main.lvl

### Building

#### Dependencies

(raylib)[https://www.raylib.com/]
it is preferred to have raylib source built alongside
Flark. This is done by downloading raylib 5.5's (github release)[https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip]
and placing the full contents unziped under <flark dir>/raylib.


#### Zig (Preferred)

Although Flark is written in C, it uses zig for its preferred
build because of zig's cross-platform and cross-compiling support.

Zig works easily and consistently across platforms and can be downloaded 
on its (website)[https://ziglang.org/download/]. It should be installed to
system directories or to the flark directory.

##### Step 1 - build raylib

make a build directory
```mkdir build```

change directory into raylib then build (placing contents in the build directory)
```cd raylib && zig build -p ../build```

if this succeeded files should be visible inside the build directory
```cd ..```
```ls build```

if this final command shows ```lib``` and ```include``` then the build of raylib was successful

##### Step 2 - build Flark

build Flark
```zig build -p build```

copy the binary to thew current directory
```cp build/bin/flark.exe ./```

if the final command fails with a message like ```could not copy file: no such file or directory```
then something has gone wrong, and it is likely that the command prior failed

#### Makefile (for *nix workflows)

While this may be easier, it is less widely supported and
still requires zig for the windows build (zig cc for flark_w32, gcc for flark).

build flark (*nix)
```make flark```

build flark and run (*nix)
```make```

build flark for windows
```make flark_w32```

For *nix platforms the executable is install in the base directory
as ```flark``` and for windows ```flark_w32``` (the .exe may need to be added manually)


#### Manual Build (for when things go wrong)

for building raylib, see its (README)[https://github.com/raysan5/raylib]

then build flark with something like (windows)
```<C Compiler> src/main.c src/level_loader.c src/player.c -L<raylib lib dir> -I<raylib incl dir> -I./plustypes/src -lm -lopengl32 -lwinmm -lgdi32 -o flark```

(linux)
```<C Compiler> src/main.c src/level_loader.c src/player.c -L<raylib lib dir> -I<raylib incl dir> -I./plustypes/src -lm -o flark```

### Level Files

Flark loads its level from files ending in .lvl (currently the windows version only supports main.lvl)
and will search the current working directory for any files ending
in .lvl.


#### Format

Currently the format only allows three specifiers; Platform, Home, and Background.
These can be seen in the example main.lvl in the github repo.

The format is fairly resilient to mistakes and has a `default override` pattern.
What this means is that the attributes of any object all have a default value.

Default platform is x->0, y->0, w->10, h->10, color->white, type->solid.

Default home is x->0, y->0

Default Background is (in hexadecimal) r->0x10, g->0x10, b->0x10, a->0xff

WARNGING the format is NOT tolerant to whitespace and does not contain spaces between tokens


##### Platform Definition

example platforms (NOTE: positive y is down on the screen because they are screen coordinates)

Platform:x-10;y30;w60;h5;corange;tbouyant; // fully defined platform using color specifyer

Platform:x-200;h3;cblue;tsolid; // partially define platform, all undefined attributes are default

Platform:x80;y60;w25;h1;r128;g64;b0;a255;tbouncy; // rgba colored platform

These show the possible configurations for a platform


x -> x coord

y -> y coord

w -> width

h -> height

r -> red pixel (8bit)

g -> green pixel (8bit)

b -> blue pixel (8bit)

alpha -> transparency (8bit)

c -> predefined color {red, lightgray, gray, darkgray, yellow, gold, orange, pink, maroon, green, lime, darkgreen, skyblue, blue, darkblue, purple, violet, darkpurple", beige, brown, darkbrown, white, black, blank, magenta }

t -> platform type { solid, bouyant, bouncy }




