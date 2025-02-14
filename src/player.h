
#include "stdint.h"

#include "geometry.h"
#include "input.h"

#ifndef PLAYER_H
#define PLAYER_H

typedef enum: uint8_t {
  PLAYER_TOUCHING_GROUND     = 0x01,
  PLAYER_TOUCHING_WALL_LEFT  = 0x02,
  PLAYER_TOUCHING_WALL_RIGHT = 0x04,
} PlayerFlag;

typedef struct {
  Rect body;
  Vec2 velocity;
  PlayerFlag state;
} Player;

extern const float PLAYER_ACCELERATION;
extern const float PLAYER_MAX_MOVE_SPEED;
extern const float PLAYER_TERMINAL_VELOCITY;
extern const float GRAVITY;

void Player_accelerate(Player *self, Direction input_direction);
void Player_collide_normal_block( Player *self, Rect *item, Direction dir );
void Player_collide_spring( Player *self, Rect *item, Direction dir, KeyState keystate );
void Player_collide_slime( Player *self, Rect *item, Direction dir, KeyState keystate );

#endif

