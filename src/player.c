
#include "player.h"

const float PLAYER_ACCELERATION = 0.2;
const float PLAYER_MAX_MOVE_SPEED = 3.0;
const float PLAYER_TERMINAL_VELOCITY = 6.0;
const float GRAVITY = 0.15;


void Player_accelerate(Player *self, Direction input_direction) {

  if (self->state & PLAYER_TOUCHING_GROUND) {
    const float FRICTION = 1.05;
    self->velocity.x /= FRICTION;

    if (input_direction & DIR_LEFT) { self->velocity.x -= PLAYER_ACCELERATION; }
    if (input_direction & DIR_RIGHT) { self->velocity.x += PLAYER_ACCELERATION; }
  }else {
    if (input_direction & DIR_LEFT) { self->velocity.x -= (PLAYER_ACCELERATION / 2.0); }
    if (input_direction & DIR_RIGHT) { self->velocity.x += (PLAYER_ACCELERATION / 2.0); }
  }


  // NOTE This always clamps, it may be useful later to
  // to only clamp the acceleration and not the speed
  if (self->velocity.x < -PLAYER_MAX_MOVE_SPEED) { self->velocity.x = -PLAYER_MAX_MOVE_SPEED; }
  if (self->velocity.x > PLAYER_MAX_MOVE_SPEED) { self->velocity.x = PLAYER_MAX_MOVE_SPEED; }

  // if (self->velocity.y > PLAYER_TERMINAL_VELOCITY) { self->velocity.y = PLAYER_TERMINAL_VELOCITY; }

  // gravity
  self->velocity.y += GRAVITY;
}

void Player_collide_normal_block(
  Player *self,
  Rect *item,
  Direction dir
) {
  if (dir & DIR_UP) {
    self->body.y = item->y - self->body.h;
    self->velocity.y = 0.0;
    self->state |= PLAYER_TOUCHING_GROUND;
  } if (dir & DIR_DOWN) {
    self->body.y = item->y + item->h;
    self->velocity.y = 0.0;
  } if (dir & DIR_LEFT) {
    self->body.x = item->x - self->body.w;
    self->velocity.x = 0.0;
    self->state |= PLAYER_TOUCHING_WALL_LEFT;
  } if (dir & DIR_RIGHT) {
    self->body.x = item->x + item->w;
    self->velocity.x = 0.0;
    self->state |= PLAYER_TOUCHING_WALL_RIGHT;
  }
}

void Player_collide_spring(
  Player *self,
  Rect *item,
  Direction dir,
  KeyState keystate
) {
  if (dir & DIR_UP) {
    if (keys_down(KEY_SPACE)) { self->velocity.y = PLAYER_TERMINAL_VELOCITY * -1.5; }
    else { self->velocity.y *= -1; }
    self->body.y = item->y - self->body.h;
    consume_keys(KEY_SPACE);
  } if (dir & DIR_DOWN) {
    self->body.y = item->y + item->h;
    self->velocity.y = 0.0;
  } if (dir & DIR_LEFT) {
    self->body.x = item->x - self->body.w;
    self->velocity.x = 0.0;
    self->state |= PLAYER_TOUCHING_WALL_LEFT;
  } if (dir & DIR_RIGHT) {
    self->body.x = item->x + item->w;
    self->velocity.x = 0.0;
    self->state |= PLAYER_TOUCHING_WALL_RIGHT;
  }
}

void Player_collide_slime(
  Player *self,
  Rect *item,
  Direction dir,
  KeyState keystate
) {
  if (dir & DIR_UP ) {
    self->body.y = item->y - self->body.h;
    self->velocity.y *= -1.0;
  }
  if (dir & DIR_DOWN) {
    self->body.y = item->y + item->h;
    self->velocity.y *= -1.0;
  }
  if (dir & DIR_LEFT) {
    self->body.x = item->x - self->body.w;
    self->velocity.x *= -1.0;
  }
  if (dir & DIR_RIGHT) {
    self->body.x = item->x + item->w;
    self->velocity.x *= -1.0;
  }
}


