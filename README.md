
## Flark

Flark is a video game written in SDL(3), and is a platformer
with a built-in level editor

### Building

#### Dependencies

GNU Make

SDL3

Any recent C Compiler

#### Steps to Build

```git clone https://github.com/Krayfighter/flark.git```

```cd flark```

```make flark``` or ```make``` to build and run

afterwards flark can be run with ```./flark``` inside the
directory containing it

`NOTE` flark currently must be run within the directory of the git
repository because the assets  are there, and are fetched as relative
file paths

### Controls

#### Game Mode

A to move left

D to move right

SPACE to jump / wall jump / spring jump

E to enter edit mode

#### Edit Mode

E to enter game mode

SHIFT + \ to read current level from `main.bin`

\ to write current evel to `main.bin`

SPACE to increment selection

SHIFT + SPACE to decrement selection

TAB to increment tab

CLICK to edit level

