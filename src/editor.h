
#include "stdint.h"

#include "player.h"
#include "camera.h"
#include "text.h"


#ifndef EDITOR_H
#define EDITOR_H

typedef enum: uint8_t {
  PLATFORM_BLOCK,
  PLATFORM_SPRING,
  PLATFORM_SLIME,
  PLATFORM_KILL,
  PLATFORM_MAX,
} PlatformType;

typedef enum: uint8_t {
  SELECT_MOVE,
  SELECT_DELETE,
  SELECT_MAX,
} PlatformSelectAction;

typedef enum: uint8_t {
  TAB_PLATFORM,
  TAB_SELECT,
  TAB_MAX,
} Tab;

typedef enum {
  EDITOR_RESULT_OK,
  EDITOR_RESULT_QUIT,
} EditorResult;


EditorResult run_editor_loop(
  SDL_Window *window,
  SDL_Renderer *renderer,
  Player *player,
  Level *level,
  Camera *cam
);

#endif

