
#include "stdio.h"

#include "raylib.h"
#include "plustypes.h"


#ifndef LEVEL_LOADER_H
#define LEVEL_LOADER_H

typedef enum {
  PLAT_BOUNCY,
  PLAT_BOUYANT,
  PLAT_SOLID,
  PLAT_KILL,
} PlatformType;

typedef struct {
  Rectangle body;
  Color color;
  PlatformType type;
} Platform;

declare_List(Platform)
const Platform Platform_new(float x, float y, float w, float h, Color color, PlatformType type);
int32_t parse_int(char *string, size_t len);


typedef struct {
  Vector2 start_position;
  List_Platform platforms;
  Color background_color;
  float player_gravity;
  float player_jump_velocity;
  float player_acceleration;
  float player_max_speed;
  float abyss_height;
} Level;

void Platform_DBG(Platform *self, FILE *output_stream);
Level parse_level_stream(FILE *stream);

#ifdef TESTS
bool TEST_char_numbers(FILE *logger);
bool TEST_parse_int(FILE *logger);
#endif

#endif

