
#include "stdio.h"
#include "stdlib.h"
#include "errno.h"

#include "SDL3/SDL.h"

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>

#include "pt_error.h"

#include "geometry.h"
#include "text.h"

// global default font
Font font;

Font Font_load_bmp(SDL_Renderer *renderer, char *bmp_path) {
  SDL_Surface *font_raw = SDL_LoadBMP(bmp_path);
  expect((font_raw != NULL), "Failed to create surface from provided path");
  const float glyph_width = 47.5; // TODO fix font rendering
  // float glyph_height = (float)font_raw->h;
  const float glyph_height = 50.0;

  SDL_Surface *font_transparent = SDL_CreateSurface(
    // 1250, 50,
    font_raw->w, font_raw->h,
    SDL_PIXELFORMAT_BGRA8888
  );

  if (font_raw->format == SDL_PIXELFORMAT_BGR24) {
    // Format BGR24 -> BGRA8888 (32)
    SDL_ConvertPixels(
      font_raw->w, font_raw->h, font_raw->format, font_raw->pixels, font_raw->pitch,
      font_transparent->format, font_transparent->pixels, font_transparent->pitch
    );
    size_t pixel_count = font_transparent->w * font_transparent->h;
    for (size_t i = 0; i < pixel_count; i += 1) {
      uint32_t *pixel_raw = &((uint32_t *)font_transparent->pixels)[i];
      if (*pixel_raw == 0xffffffff) { // white -> transparent
        *pixel_raw &= 0xffffff00;
      }else {
        // TODO maybe fix this so that opacity is correct
        // get intensity and make it transparency
        uint8_t bi = 0xff - (*(uint32_t *)pixel_raw >> 3 * 8);
        uint8_t gi = 0xff - (*(uint32_t *)pixel_raw >> 2 * 8);
        uint8_t ri = 0xff - (*(uint32_t *)pixel_raw >> 8);
        // a full pixel is (uint32_t)0x000000xx
        const uint16_t pixel_full = 0xff * 3;
        uint16_t pixel_intensity = bi + gi + ri;
        uint8_t opacity = (uint8_t)(255.0 * (float)pixel_intensity / (float)pixel_full);
        // *pixel_raw |= opacity;
        *pixel_raw = 0xffffff00 | opacity;
      }
    }
  }else {
    fprintf(stderr,
      "ERROR: invalid surface pixel format %s\n",
      SDL_GetPixelFormatName(font_raw->format)
    );
    return (Font){ 0 };
  }
  SDL_Texture *font_texture = SDL_CreateTextureFromSurface(renderer, font_transparent);

  SDL_DestroySurface(font_raw);
  SDL_DestroySurface(font_transparent);

  return (Font) {
    .charmap = font_texture,
    .glyph_width = glyph_width,
    .glyph_height = glyph_height
  };
}

void Font_free(Font *self) {
  SDL_DestroyTexture(self->charmap);
}

const float CHAR_WIDTH = 47.5;
const float CHAR_HEIGHT = 50.0;

void render_text(
  SDL_Renderer *renderer,
  // SDL_Texture *alphabet_texture,
  Font *font,
  const char *string, // null terminated
  float x, float y, float size,
  uint32_t text_color_bgrx
) {
  Rect texture_window = (Rect){ .x = 0.0, .y = 0.0, .w = CHAR_WIDTH, .h = CHAR_HEIGHT };
  Rect render_rect = (Rect){ .x = x, .y = y, .w = size, .h = size };

  SDL_SetTextureColorMod(
    font->charmap,
    text_color_bgrx >> 0x18,
    (text_color_bgrx >> 0x10) & 0x000000ff,
    (text_color_bgrx >> 0x08) & 0x000000ff
  );

  uint32_t i = 0;
  while (string[i] != 0x0) {
    uint8_t letter_number;
    if (string[i] >= 'a' && string[i] <= 'z') {
      letter_number = string[i] - 'a';
    }else if (string[i] >= 'A' && string[i] <= 'Z') {
      letter_number = string[i] - 'A';
    }else { i += 1; render_rect.x += size; continue; }

    texture_window.x = (float)(letter_number) * CHAR_WIDTH;

    SDL_RenderTexture(renderer, font->charmap, &texture_window, &render_rect);

    render_rect.x += size;
    i += 1;
  }
}

float text_width(const char *text, float size) {
  return (float)strlen(text) * size;
  // float width;

  

  // return width;
}



