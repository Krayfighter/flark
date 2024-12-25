
#include "stdint.h"

#include "raylib.h"

#include "level_loader.h"

#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
  uint8_t input_jump;
  uint8_t input_jump_frames;
  float prev_x_motion;
  bool controller_mode;
} InputBuffer;

typedef enum {
  SLIDING_LEFT,
  SLIDING_RIGHT,
  SLIDING_NONE,
} WallSlide;

typedef struct {
  WallSlide sliding;
  float prev_speed;
} WallslideState;

typedef struct {
  Rectangle body;
  Vector2 velocity;
  bool can_jump;
  bool touching_ground;
  WallslideState slide_state;
  InputBuffer input_state;
  float gravity;
  float acceleration;
  float max_speed;
  float jump_velocity;
} Player;

Player Player_spawn(Level *level);
void Player_step_input_frame(Player *self);
void Player_apply_gravity(Player *self, float terminal_velocity);
void Player_move(Player *self);
void Player_do_friction(Player *self, float friction);
void Player_move_analogue(Player *self, bool touching_ground, float input);

typedef enum {
  DIR_UP,
  DIR_DOWN,
  DIR_RIGHT,
  DIR_LEFT,
  DIR_NONE,
} CardinalDirection;
void player_move_direction(Player *self, CardinalDirection dir, bool touching_ground);
// returns the direction from which the player collides with the rect
CardinalDirection Player_collide_rect(Player *self, Rectangle rect);
// void InputBuffer_step_frame(InputBuffer *self, Player *player, bool controller_mode);

#endif

