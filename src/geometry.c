
#include "SDL3/SDL.h"

#include "geometry.h"
#include "input.h"

void Rect_translate(Rect *self, Vec2 translation) {
  self->x += translation.x;
  self->y += translation.y;
}

Rect Rect_from_points(Vec2 first, Vec2 second) {
  float x, w;
  if (first.x < second.x) {
    x = first.x;
    w = second.x - x;
  }else {
    x = second.x;
    w = first.x - x;
  }
  float y, h;
  if (first.y < second.y) {
    y = first.y;
    h = second.y - y;
  }else {
    y = second.y;
    h = first.y - y;
  }

  return (Rect) { .x = x, .y = y, .w = w, .h = h };
}
void Rect_dilate(Rect *self, Vec2 dilation) {
  self->x *= dilation.x;
  self->w *= dilation.x;
  self->y *= dilation.y;
  self->h *= dilation.y;
}


Direction Rect_vector_collision(Rect self, Vec2 velocity, Rect other) {
  bool in_horizontal_collision_range = (
    self.x + self.w + velocity.x > other.x &&
    self.x + velocity.x < other.x + other.w
  );
  bool in_vertical_collision_range = (
    self.y + self.h + velocity.y > other.y &&
    self.y + velocity.y < other.y + other.h
  );
  if (in_horizontal_collision_range ) { // VERTICAL collision detection
    bool self_above = self.y + self.h <= other.y;
    bool next_frame_self_above = self.y + self.h+ velocity.y < other.y;
    bool will_collide_from_top = self_above && !next_frame_self_above;

    bool self_below = self.y >= other.y + other.h;
    bool next_frame_self_below = self.y + velocity.y > other.y + other.h;
    bool will_collide_from_bottom = self_below && !next_frame_self_below;

    if (will_collide_from_top) {
      return DIR_UP;
    }else if (will_collide_from_bottom) {
      return DIR_DOWN;
    }
  }
  if (in_vertical_collision_range) { // HORIZONTAL collistion detection
    bool self_left = self.x + self.w <= other.x;
    bool next_frame_self_left = self.x + self.w + velocity.x <= other.x;
    bool will_collide_from_left = self_left && !next_frame_self_left;

    bool self_right = self.x >= other.x + other.w;
    bool next_frame_self_right = self.x + velocity.x > other.x + other.w;
    bool will_collide_from_right = self_right && !next_frame_self_right;

    if (will_collide_from_left) {
      return DIR_LEFT;
    }else if (will_collide_from_right) {
      return DIR_RIGHT;
    }
  }
  return DIR_NONE;
}
Direction KeyState_get_input_direction() {
  Direction dir = DIR_NONE;
  if (keys_down(KEY_MOVE_LEFT))  { dir |= DIR_LEFT; }
  if (keys_down(KEY_MOVE_RIGHT)) { dir |= DIR_RIGHT; }
  if (keys_down(KEY_MOVE_UP))    { dir |= DIR_UP; }
  if (keys_down(KEY_MOVE_DOWN))  { dir |= DIR_DOWN; }

  return dir;
}
