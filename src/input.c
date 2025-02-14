
#include "input.h"

KeyState keystate;

void register_sdl_keydown(SDL_KeyboardEvent key) {
  switch (key.scancode) {
    case SDL_SCANCODE_W: set_keys_pressed(KEY_MOVE_UP); break;
    case SDL_SCANCODE_S: set_keys_pressed(KEY_MOVE_DOWN); break;
    case SDL_SCANCODE_A: set_keys_pressed(KEY_MOVE_LEFT); break;
    case SDL_SCANCODE_D: set_keys_pressed(KEY_MOVE_RIGHT); break;
    case SDL_SCANCODE_E: set_keys_pressed(KEY_SWITCH_MODE); break;
    case SDL_SCANCODE_BACKSLASH: set_keys_pressed(KEY_LOAD_FILE); break;
    case SDL_SCANCODE_SPACE: set_keys_pressed(KEY_SPACE); break;
    case SDL_SCANCODE_TAB: set_keys_pressed(KEY_TAB); break;
    case SDL_SCANCODE_LSHIFT: set_keys_pressed(KEY_SHIFT); break;
    case SDL_SCANCODE_PAGEDOWN: set_keys_pressed(KEY_ZOOM_IN); break;
    case SDL_SCANCODE_PAGEUP: set_keys_pressed(KEY_ZOOM_OUT); break;
    default: {}
  }
}

void register_sdl_keyup(SDL_KeyboardEvent key) {
  switch (key.scancode) {
    case SDL_SCANCODE_W: set_keys_released(KEY_MOVE_UP); break;
    case SDL_SCANCODE_S: set_keys_released(KEY_MOVE_DOWN); break;
    case SDL_SCANCODE_A: set_keys_released(KEY_MOVE_LEFT); break;
    case SDL_SCANCODE_D: set_keys_released(KEY_MOVE_RIGHT); break;
    case SDL_SCANCODE_E: set_keys_released(KEY_SWITCH_MODE); break;
    case SDL_SCANCODE_BACKSLASH: set_keys_released(KEY_LOAD_FILE); break;
    case SDL_SCANCODE_SPACE: set_keys_released(KEY_SPACE); break;
    case SDL_SCANCODE_TAB: set_keys_released(KEY_TAB); break;
    case SDL_SCANCODE_LSHIFT: set_keys_released(KEY_SHIFT); break;
    case SDL_SCANCODE_PAGEDOWN: set_keys_released(KEY_ZOOM_IN); break;
    case SDL_SCANCODE_PAGEUP: set_keys_released(KEY_ZOOM_OUT); break;
    default: {}
  }
}

