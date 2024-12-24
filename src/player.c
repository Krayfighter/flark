
#include "math.h"
#include "stdint.h"

#include "raylib.h"
#include "raymath.h"

#include  "player.h"

#include "stdio.h"


Player Player_spawn(Vector2 start_pos) {
  return (Player) {
    .body = (Rectangle){ .x = start_pos.x, .y = start_pos.y, .width = 10, .height = 20 },
    .velocity = (Vector2){ .x = 0.0, .y = 0.0 },
    .can_jump = false,
    .touching_ground = false,
    .input_state = (InputBuffer){ }, // TODO implement input buffer
    .slide_state = (WallslideState) {
      .sliding = SLIDING_NONE,
      .prev_speed = 0.0,
    },
  };
}

void Player_step_input_frame(Player *self) {
  // remove inputs that have outlived the buffer time
  if (self->input_state.input_jump >= self->input_state.input_jump_frames) { self->input_state.input_jump = 0;}
  // take new inputs
  if (!self->input_state.controller_mode) {
    if (IsKeyDown(KEY_D)) { player_move_direction(self, DIR_RIGHT, self->touching_ground); }
    if (IsKeyDown(KEY_A)) { player_move_direction(self, DIR_LEFT, self->touching_ground); }
    if (IsKeyPressed(KEY_SPACE)) {
      if (self->can_jump) {
        self->velocity.y -= 20.0;
        self->can_jump = false;
      } else if (self->slide_state.sliding != SLIDING_NONE) {
        self->velocity.x = self->slide_state.prev_speed * -1.0;
        if (fabsf(self->velocity.x) < 10.0) {
          self->velocity.x = 10.0;
          if (self->slide_state.sliding == SLIDING_LEFT) { self->velocity.x *= -1.0; }
        }
        self->slide_state.prev_speed = 0.0; // This also indicates that the player is no longer sliding
        self->velocity.y -= 20.0;
        self->can_jump = false;
      }
    }
  } else {
    // TODO fix controller jank
    // if (IsGamepadAvailable(0)) {
    float x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    // float y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    if (fabsf(x) > 0.1) {
      // fprintf(stderr, "DBG: x pad %f\n", x);
      x = Clamp(x * 1.5, -1.0, 1.0);
      Player_move_analogue(self, self->touching_ground, x);
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

void Player_apply_gravity(Player *self, float acceleration, float terminal_velocity) {
  // const float G = 1.1;
  // float G = s
  if (self->velocity.y < 0.0) {
    self->velocity.y = (self->velocity.y / acceleration) + 0.2;
  }else {
    self->velocity.y = (self->velocity.y * acceleration) + 0.2;
  }
  // self->velocity.y -= (self->velocity.y + 0.01);
  // if (self->velocity.y > terminal_velocity) { self->velocity.y = terminal_velocity; }
  if (self->velocity.y + acceleration > terminal_velocity) { self->velocity.y = terminal_velocity; }
  // self->velocity.y += acceleration;
  // if (self->velocity.y > terminal_velocity) {
  //   self->velocity.y = terminal_velocity;
  // }
}

void Player_move(Player *self) {
  self->body.x += self->velocity.x;
  self->body.y += self->velocity.y;
}

void Player_do_friction(Player *self, float friction) {
  if (self->touching_ground) { self->velocity.x *= friction; }
}

void Player_move_analogue(Player *self, bool touching_ground, float input) {
  const float PLAYER_MAX_SPEED = 10.0;
  const float PLAYER_ACCELERATION = 1.0;
  // const float PLAYER_ACCELERATION = 0.3;
  const float PLAYER_AIR_ACCELERATION = 0.3;


  int move_sign = 1;
  if (self->velocity.x < 0.0) { move_sign = -1; }

  if (fabsf(self->velocity.x) > PLAYER_MAX_SPEED) { self->velocity.x = PLAYER_MAX_SPEED * move_sign; }
  else {
    if (self->touching_ground) {
      self->velocity.x += input * PLAYER_ACCELERATION;
    }else {
      self->velocity.x += input * PLAYER_AIR_ACCELERATION;
    }
  }

  // int direction_sign = 1;
  // if (input < 0.0) { direction_sign = -1; }
  // // if (dir == MOVE_LEFT) { direction_sign = -1; }
  // input = input * direction_sign;

  
  // self->velocity.x += 0.1 * direction_sign;

  // int velocity_sign = 1;
  // if (self->velocity.x < 0.0) { velocity_sign = -1;}

  // if (velocity_sign == direction_sign) {
  //   if (touching_ground) { self->velocity.x *= PLAYER_ACCELERATION * input; }
  //   else { self->velocity.x *= PLAYER_AIR_ACCELERATION * input; }
  // }else {
  //   if (touching_ground) { self->velocity.x /= (PLAYER_ACCELERATION * input); }
  //   else { self->velocity.x /= (PLAYER_AIR_ACCELERATION * input); }
  // }

  // if (fabsf(self->velocity.x) > PLAYER_MAX_SPEED) {
  //   self->velocity.x = PLAYER_MAX_SPEED * velocity_sign;
  // }
}

// assumes that the vector is normalized
void player_move_direction(Player *self, CardinalDirection dir, bool touching_ground) {
  if (dir == DIR_RIGHT) {
    Player_move_analogue(self, touching_ground, 1.0);
  }else {
    Player_move_analogue(self, touching_ground, -1.0);
  }
}

CardinalDirection Player_collide_rect(Player *self, Rectangle rect) {
  bool in_horizontal_collision_range = (
    self->body.x + self->body.width + self->velocity.x > rect.x &&
    self->body.x + self->velocity.x < rect.x + rect.width
  );
  bool in_vertical_collision_range = (
    self->body.y + self->body.height > rect.y &&
    self->body.y + self->velocity.y < rect.y + rect.height
  );
  if (in_horizontal_collision_range ) { // VERTICAL collision detection
    // bool self_above = self->body.y < rect.y - rect.height;
    // bool next_frame_self_above = self->body.y + self->body.height + self->velocity.y <= rect.y - rect.height;
    // bool will_collide_from_top = self_above && !next_frame_self_above;

    // bool self_below = self->body.y > rect.y;
    // bool next_frame_self_below = self->body.y + self->velocity.y > rect.y;
    // bool will_collide_from_bottom = self_below && !next_frame_self_below;
    bool self_above = self->body.y + self->body.height <= rect.y;
    bool next_frame_self_above = self->body.y + self->body.height + self->velocity.y < rect.y;
    bool will_collide_from_top = self_above && !next_frame_self_above;

    bool self_below = self->body.y >= rect.y + rect.height;
    bool next_frame_self_below = self->body.y + self->velocity.y > rect.y + rect.height;
    bool will_collide_from_bottom = self_below && !next_frame_self_below;

    if (will_collide_from_top) {
      // // do collision
      // self->velocity.y = 0.0;
      // self->body.y = rect.y - self->body.height;
      // self->can_jump = true;
      // self->touching_ground = true;
      return DIR_UP;
    }else if (will_collide_from_bottom) {
      // // do collision
      // self->velocity.y = 0.0;
      // self->body.y = rect.y + rect.height;
      // self->can_jump = true;
      return DIR_DOWN;
    }
  }
  // fprintf(stderr, "DBG: running collision horizontally\n");
  if (in_vertical_collision_range) { // HORIZONTAL collistion detection
    bool self_left = self->body.x + self->body.width <= rect.x;
    bool next_frame_self_left = self->body.x + self->body.width + self->velocity.x <= rect.x;
    bool will_collide_from_left = self_left && !next_frame_self_left;

    bool self_right = self->body.x >= rect.x + rect.width;
    bool next_frame_self_right = self->body.x + self->velocity.x > rect.x + rect.width;
    bool will_collide_from_right = self_right && !next_frame_self_right;

    if (will_collide_from_left) {
      // fprintf(stderr, "DBG: collision from left\n");
      return DIR_LEFT;
      // self->velocity.x = 0.0;
      // self->body.x = rect.y - self->body.width;
      // self->can_jump = true; // ??? TODO implement wall jump ???
      // // NOTE not setting touching_ground to true
    }else if (will_collide_from_right) {
      return DIR_RIGHT;
    }
  }
  return DIR_NONE;
}




