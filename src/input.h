
#include "stdint.h"

#include "SDL3/SDL.h"

#ifndef INPUT_H
#define INPUT_H

typedef enum: uint32_t {
  KEY_MOVE_LEFT   = 1,
  KEY_MOVE_RIGHT  = 1 << 1,
  KEY_MOVE_UP     = 1 << 2,
  KEY_MOVE_DOWN   = 1 << 3,
  KEY_SHIFT       = 1 << 4,
  KEY_SPACE       = 1 << 5,
  KEY_TAB         = 1 << 6,
  KEY_SWITCH_MODE = 1 << 7,
  KEY_ZOOM_IN     = 1 << 8,
  KEY_ZOOM_OUT    = 1 << 9,
  KEY_LOAD_FILE   = 1 << 10,
} KeyFlag;

typedef struct {
  uint32_t keys_pressed;
  uint32_t keys_consumed;
} KeyState;

extern KeyState keystate;

#define KeyState_get_keyflag(self, flag) (self.key_flags & flag) != 0

#define keys_down(flags) (keystate.keys_pressed & flags)
#define keys_pressed(flags) (keystate.keys_pressed & flags && !(keystate.keys_consumed & flags))
#define consume_keys(flags) keystate.keys_consumed |= flags

#define set_keys_pressed(keyflags) (keystate.keys_pressed |= keyflags)
#define set_keys_released(keyflags) { \
  keystate.keys_pressed &= ~keyflags; \
  keystate.keys_consumed &= ~keyflags; \
}

void register_sdl_keydown(SDL_KeyboardEvent key);
void register_sdl_keyup(SDL_KeyboardEvent key);

#endif
