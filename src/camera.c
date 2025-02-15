
#include "SDL3/SDL.h"

#include "camera.h"
#include "input.h"

const uint32_t CLEAR_COLOR = 0x101010cc;

const float CAMERA_VIEW_SIZE_MIN = 300.0;
const float CAMERA_VIEW_SIZE_MAX = 30000.0;

SDL_Texture *solid_block_texture;
SDL_Texture *spring_texture;

void precompute_camera_vectors(
  Camera *cam,
  float *camera_scale,
  float *height_ratio,
  Vec2 *camera_offset
) {
  *camera_scale = (float)(cam->render_width) / cam->view_width;
  *height_ratio = (float)(cam->render_height) / (float)(cam->render_width);
  *camera_offset = (Vec2){
    .x = -cam->focus.x + (cam->view_width / 2.0),
    .y = -cam->focus.y + ((cam->view_width * *height_ratio) / 2.0)
  };
}


// convert rect with precalculated values
// convert world rect to camera rect
Rect Camera_convert_rect_precalc(
  Camera *self,
  Rect source,
  float camera_scale,
  float height_ratio,
  Vec2 camera_offset
) {
  Rect_translate(&source, camera_offset);
  Rect_dilate(&source, (Vec2){ .x = camera_scale, .y = camera_scale });
  return source;
}


Rect Camera_convert_rect( Camera *self, Rect source ) {
  float camera_scale, height_ratio;
  Vec2 camera_offset;
  precompute_camera_vectors(self, &camera_scale, &height_ratio, &camera_offset);
  return Camera_convert_rect_precalc(self, source, camera_scale, height_ratio, camera_offset);
}


void Camera_render_rects(
  Camera *self,
  SDL_Renderer *renderer,
  SDL_Texture *texture, // may be null
  Rect *texture_view, // only matters if texture is not null
  Rect *rects, uint32_t rect_count,
  bool tiled
) {
  float camera_scale, height_ratio;
  Vec2 camera_offset;
  precompute_camera_vectors(self, &camera_scale, &height_ratio, &camera_offset);
  
  for (uint32_t i = 0; i < rect_count; i += 1) {
    Rect trect = Camera_convert_rect_precalc(self, rects[i], camera_scale, height_ratio, camera_offset);

    if (texture != NULL) {
      const float TEXTURE_WIDTH = 100.0;
      const float TEXTURE_HEIGHT = 100.0;
      if (tiled) {
        if (trect.w > trect.h) {
          SDL_RenderTextureTiled(renderer, texture, texture_view, trect.w / (TEXTURE_WIDTH * 3.0), &trect);
        }else {
          SDL_RenderTextureTiled(renderer, texture, texture_view, trect.h / (TEXTURE_HEIGHT * 3.0), &trect);
        }
      }else {
        SDL_RenderTexture(renderer, texture, texture_view, &trect);
      }
    }else {
      SDL_RenderRect(renderer, &trect);
    }
  }
}

// This does some vector math to take the click vector and transform it into a
// vector pointing from the world origin (0, 0) to the equivelant place on the screen
Vec2 Camera_to_world_coords(Camera *self, float screen_x, float screen_y) {
  float world_scale = self->view_width / (float)(self->render_width);
  float view_height = self->view_width * (float)(self->render_height) / (float)(self->render_width);

  float world_x = self->focus.x + (screen_x * world_scale) - (self->view_width / 2.0);
  float world_y = self->focus.y + (screen_y * world_scale) - (view_height / 2.0);

  return (Vec2){ .x = world_x, .y = world_y };
}

void Camera_update_focus_and_window_size(
  Camera *self,
  SDL_Window *window,
  Vec2 world_focus
) {
  SDL_GetWindowSize(window, (int *)&self->render_width, (int *)&self->render_height);
  self->focus = (Vec2){ .x = world_focus.x, world_focus.y };
}

void Camera_render_level(
  Camera *self,
  SDL_Renderer *renderer,
  Level *level
) {
  
  Rect view_rect = (Rect){ .x = 0.0, .y = 0.0, .w = solid_block_texture->w, .h = solid_block_texture->h };
  Camera_render_rects(self, renderer, solid_block_texture, &view_rect, level->normal_block.items, level->normal_block.item_count, true);
  Camera_render_rects(self, renderer, spring_texture, &view_rect, level->spring.items, level->spring.item_count, false);
  SDL_SetRenderDrawColor(renderer, 0x44, 0xff, 0x88, 0x88);
  Camera_render_rects(self, renderer, NULL, NULL, level->slime.items, level->slime.item_count, false);
  SDL_SetRenderDrawColor(renderer, 0xcc, 0x22, 0x44, 0xff);
  Camera_render_rects(self, renderer, NULL, NULL, level->kill_block.items, level->kill_block.item_count, false);
}

void zoom_camera(Camera *cam) {
  if (keys_down(KEY_ZOOM_OUT)) {
    cam->view_width /= 1.025;
    cam->view_width = (cam->view_width >= CAMERA_VIEW_SIZE_MIN)
      ? cam->view_width : CAMERA_VIEW_SIZE_MIN;
  }
  if (keys_down(KEY_ZOOM_IN)) {
    cam->view_width *= 1.025;
    cam->view_width = (cam->view_width <= CAMERA_VIEW_SIZE_MAX)
      ? cam->view_width : CAMERA_VIEW_SIZE_MAX;
  }
}

