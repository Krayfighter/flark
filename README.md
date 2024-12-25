
# Flark

### video game of the century

Flark is a 2D platformer with an emphasis on physics,
movement mechanics, and hackability.

Flark supports custom levels as a first-class feature
base on .lvl files with a custom parser.

note: the flark executable must be in the same directory
as the .lvl files in order to see them, additionally
file selection is not currently working on windows due
to a bug, so windows builds of flark can only access files
named main.lvl

## Building

### Dependencies

[raylib](https://www.raylib.com/)
it is preferred to have raylib source built alongside
Flark. This is done by downloading raylib 5.5's [github release](https://github.com/raysan5/raylib/archive/refs/tags/5.5.zip)
and placing the full contents unziped under \<flark dir\>/raylib.


### Zig (Preferred)

Although Flark is written in C, it uses zig for its preferred
build because of zig's cross-platform and cross-compiling support.

Zig works easily and consistently across platforms and can be downloaded 
on its [website](https://ziglang.org/download/). It should be installed to
system directories or to the flark directory.

#### Step 1 - build raylib

make a build directory
```mkdir build```

change directory into raylib then build (placing contents in the build directory)
```cd raylib && zig build -p ../build```

if this succeeded files should be visible inside the build directory
```cd ..```
```ls build```

if this final command shows ```lib``` and ```include``` then the build of raylib was successful

#### Step 2 - build Flark

build Flark
```zig build -p build```

copy the binary to thew current directory
```cp build/bin/flark.exe ./```

if the final command fails with a message like ```could not copy file: no such file or directory```
then something has gone wrong, and it is likely that the command prior failed

### Makefile (for *nix workflows)

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


### Manual Build (for when things go wrong)

for building raylib, see its [README](https://github.com/raysan5/raylib)

then build flark with something like (windows)
```<C Compiler> src/main.c src/level_loader.c src/player.c -L<raylib lib dir> -I<raylib incl dir> -I./plustypes/src -lm -lopengl32 -lwinmm -lgdi32 -o flark```

(linux)
```<C Compiler> src/main.c src/level_loader.c src/player.c -L<raylib lib dir> -I<raylib incl dir> -I./plustypes/src -lm -o flark```

## Level Files

Flark loads its level from files ending in .lvl (currently the windows version only supports main.lvl)
and will search the current working directory for any files ending
in .lvl.


### Format

Currently the format only allows three specifiers; Platform, Home, and Background.
These can be seen in the example main.lvl in the github repo.

The format is fairly resilient to mistakes and has a `default override` pattern.
What this means is that the attributes of any object all have a default value.

Default platform is x->0, y->0, w->10, h->10, color->white, type->solid.

Default home is x->0, y->0

Default Background is (in hexadecimal) r->0x10, g->0x10, b->0x10, a->0xff

WARNGING the format is NOT tolerant to whitespace and does not contain spaces between tokens


#### Platform Definition

A platform has a few components, namely, shape, color and type. Each
of these component are defined inline and can be in any order. When
defining the same attribute twice, the later difinition either fully
or partially overwrites the previous. This may be useful in the case
of coloring the platform.

(Full platform defualt definition for reference)

```Platform:x0;y0;w10;h10;cwhite;tsolid;```

or

```Platform:x0;y0;w10;h10;r255;g255;b255;a255;tsolid;```


Firstly, the shape of a platform is described by x, y, width and height. It is
Important to note that the y axis grows downward which is normal for screen
coordinates, but may be counterintuitive at first.

\<platform definition\>x\<int\>;y\<int\>;w\<int\>;h\<int\>;

The color of a platform can be described in two different ways. This first
is RGBA (A is alpha or transparency) that is eight bits deep, which means
that each value must be in the range [0,255]. The second is by color names.

Color by RGBA

\<platform definition\>r\<uint\>;g\<uint\>;b\<uint\>;a\<uint\>;

Color by name
\<platform definition\>c\<name\>;

The possible color names are
`red`, `lightgray`, `gray`, `darkgray`, `yellow`, `gold`,
`orange`, `pink`, `maroon`, `green`, `lime`, `darkgreen`,
`skyblue`, `blue`, `darkblue`, `purple`, `violet`, `darkpurple`,
`beige`, `brown`, `darkbrown`, `white`, `black`, `blank`, `magenta`

Finally, the platform type definition which follows a similar pattern
to color names.

\<platform definition\>t\<name\>;

The possible platform types are `solid`, `bouyant`, `bouncy`, `kill`.

Solid platforms collide from all sides and stop a player in the way
one would espect of a solid block. bouyant platforms are solid on top,
but allow a player to jump up through it. Bouyant platforms do not collide
from the sides. Bouncy platforms are like bouyant, but reflect the players
vertical speed (causing a bounce) from the top and launching them upwards
from the bottom. Finally, killer platforms kill the player when collided with,
in a similar way to falling into the abyss.

#### Home Declaration

The Home level attribute defaults to x0;y0;

```Home:x<int>;y<int>;```

#### Background Declaration

The Background level attribute is simply a sapecial case for
color declaration

```Background:r<uint>;g<uint>;b<uint>;a<uint>;```

or

```Background:c<colorname>;```


#### Player Attribute Declarations

The player has a few tunable attributes to control the feel of
the movement in the level. These attributes are gravity, acceleration,
max movement speed, and jump velocity. Each of these attributes, unlike
the previous declaration types, accept floating point values, and do
not require a key like `x-5;`.

```Gravity:<float>;```

```Acceleration:<float>;```

```MoveSpeed:<float>;```

```Jump:<float>;```


#### Abyss Declaration

The Abyss is a globally defined y value that, when passed
kills the player. Its declaration is similar the the player
attributes.

```Abyss:<float>;```



## Contributors

Aiden Kring (Krayfighter)  - programming & design

Julian Kring (luijejmk) - level editing



