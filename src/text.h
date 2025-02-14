
#include "SDL3/SDL.h"


#ifndef FONT_H
#define FONT_H

typedef struct {
  SDL_Texture *charmap;
  float glyph_width;
  float glyph_height;
} Font;

extern Font font;

void render_text(
  SDL_Renderer *renderer,
  // SDL_Texture *alphabet_texture,
  Font *font,
  const char *string, // null terminated
  float x, float y, float size,
  uint32_t text_color_bgrx
);
float text_width( const char *text, float size );

Font Font_load_bmp(SDL_Renderer *renderer, char *bmp_path);
void Font_free(Font *self);

#endif

