
#include "stdint.h"

#include "SDL3/SDL.h"

#ifndef GEOMETRY_H
#define GEOMETRY_H

typedef struct { float x, y; } Vec2;

typedef struct SDL_FRect Rect;


void Rect_translate(Rect *self, Vec2 translation);
Rect Rect_from_points(Vec2 first, Vec2 second);
void Rect_dilate(Rect *self, Vec2 dilation);


typedef uint8_t Direction;

#define DIR_NONE  0b00000000
#define DIR_UP    0b00000001
#define DIR_DOWN  0b00000010
#define DIR_LEFT  0b00000100
#define DIR_RIGHT 0b00001000

// returns true if the range of [x, x+y] overlaps the range [a, a+b]
// which is useful for collision physics
//                                 left side       right side
#define ranges_overlap(x, y, a, b) (x + y >= a) && (x <= a + b)

Direction Rect_vector_collision(Rect self, Vec2 velocity, Rect other);
Direction KeyState_get_input_direction();

#endif
