
#include "math.h"
#include "stdint.h"

#include "raylib.h"
#include "raymath.h"

#include  "player.h"


void Player_step_input_frame(Player *self) {
  // remove inputs that have outlived the buffer time
  if (self->input_state.input_jump >= self->input_state.input_jump_frames) { self->input_state.input_jump = 0;}
  // take new inputs
  if (!self->input_state.controller_mode) {
    if (IsKeyDown(KEY_D)) { player_move_direction(self, MOVE_RIGHT, self->touching_ground); }
    if (IsKeyDown(KEY_A)) { player_move_direction(self, MOVE_LEFT, self->touching_ground); }
    if (IsKeyPressed(KEY_SPACE) && self->can_jump) {
      self->velocity.y -= 20.0;
      self->can_jump = false;
    }
  } else {
    // TODO fix controller jank
    // if (IsGamepadAvailable(0)) {
    float x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    // float y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    if (fabsf(x) > 0.1) {
      // fprintf(stderr, "DBG: x pad %f\n", x);
      x = Clamp(x * 1.5, -1.0, 1.0);
      player_move_analogue(self, self->touching_ground, x);
    }
    switch (GetGamepadButtonPressed()) {
      case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT: {
        if (self->can_jump) { self->velocity.y -= 20.0; self->can_jump = false; }
      } break;
    }
    // }else {
    //   fprintf(stderr, "DBG: gamepad not available\n");
    // }
  }
}

void Player_apply_gravity(Player *self, float strength, float terminal_velocity) {
  // const float G = 1.1;
  // float G = s
  if (self->velocity.y < 0.0) {
    self->velocity.y = (self->velocity.y / strength) + 0.2;
  }else {
    self->velocity.y = (self->velocity.y * strength) + 0.2;
  }
  // self->velocity.y -= (self->velocity.y + 0.01);
  if (self->velocity.y > terminal_velocity) { self->velocity.y = terminal_velocity; }
}

void Player_move(Player *self) {
  self->body.x += self->velocity.x;
  self->body.y += self->velocity.y;
}

void Player_do_friction(Player *self, float friction) {
  if (self->touching_ground) { self->velocity.x *= friction; }
}

void player_move_analogue(Player *self, bool touching_ground, float input) {
  const float PLAYER_MAX_SPEED = 10.0;
  const float PLAYER_ACCELERATION = 1.3;
  const float PLAYER_AIR_ACCELERATION = 1.05;

  int direction_sign = 1;
  if (input < 0.0) { direction_sign = -1; }
  // if (dir == MOVE_LEFT) { direction_sign = -1; }
  input = input * direction_sign;

  
  self->velocity.x += 0.1 * direction_sign;

  int velocity_sign = 1;
  if (self->velocity.x < 0.0) { velocity_sign = -1;}

  if (velocity_sign == direction_sign) {
    if (touching_ground) { self->velocity.x *= PLAYER_ACCELERATION * input; }
    else { self->velocity.x *= PLAYER_AIR_ACCELERATION * input; }
  }else {
    if (touching_ground) { self->velocity.x /= (PLAYER_ACCELERATION * input); }
    else { self->velocity.x /= (PLAYER_AIR_ACCELERATION * input); }
  }

  if (fabsf(self->velocity.x) > PLAYER_MAX_SPEED) {
    self->velocity.x = PLAYER_MAX_SPEED * velocity_sign;
  }
}

// assumes that the vector is normalized
void player_move_direction(Player *self, MovementDireciton dir, bool touching_ground) {
  if (dir == MOVE_RIGHT) {
    player_move_analogue(self, touching_ground, 1.0);
  }else {
    player_move_analogue(self, touching_ground, -1.0);
  }
}



