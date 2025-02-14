
#include "stdint.h"

#include "geometry.h"
#include "level.h"

#ifndef CAMERA_H
#define CAMERA_H

extern const uint32_t CLEAR_COLOR;

extern const float CAMERA_VIEW_SIZE_MIN;
extern const float CAMERA_VIEW_SIZE_MAX;

extern SDL_Texture *solid_block_texture;
extern SDL_Texture *spring_texture;

#define SetRenderDrawColor_uint32(renderer, color) \
SDL_SetRenderDrawColor( \
  renderer, \
  color >> 24, \
  (color >> 16) & 0x000000ff, \
  (color >> 8) & 0x000000ff, \
  color & 0x000000ff \
)


typedef struct {
  float view_width;
  uint32_t render_width;
  uint32_t render_height;
  Vec2 focus;
} Camera;

Vec2 Camera_to_world_coords(Camera *self, float screen_x, float screen_y);
void Camera_render_rects(
  Camera *self,
  SDL_Renderer *renderer,
  SDL_Texture *texture, // may be null
  Rect *texture_view, // only matters if texture is not null
  Rect *rects, uint32_t rect_count,
  bool tiled
);
Rect Camera_convert_rect( Camera *self, Rect source );
void Camera_update_focus_and_window_size(
  Camera *self,
  SDL_Window *window,
  Vec2 world_focus
);
void Camera_render_level( Camera *self, SDL_Renderer *renderer, Level *level );
void zoom_camera(Camera *cam);

#endif

