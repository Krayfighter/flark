
#include "SDL3/SDL.h"
#include <SDL3/SDL_rect.h>

#include "plustypes.h"

#include "geometry.h"


#ifndef LEVEL_H
#define LEVEL_H

declare_List(Rect)

extern char *level_filename;

typedef struct {
  List_Rect normal_block;
  List_Rect spring;
  List_Rect slime;
  List_Rect kill_block;
  Vec2 home;
  float abyss;
  uint8_t version;
} Level;

void Level_free(Level self);
Level Level_new();
bool Level_save_to_file(Level *self, const char *filename);
// WARN will return (Level){ 0 } on failure
Level Level_load_from_file(const char *filename);

#endif
