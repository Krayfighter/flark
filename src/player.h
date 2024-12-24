
#include "stdint.h"

#include "raylib.h"



typedef struct {
  uint8_t input_jump;
  uint8_t input_jump_frames;
  float prev_x_motion;
  bool controller_mode;
} InputBuffer;

typedef struct {
  Rectangle body;
  Vector2 velocity;
  bool can_jump;
  bool touching_ground;
  InputBuffer input_state;
} Player;

void Player_step_input_frame(Player *self);
void Player_apply_gravity(Player *self, float strength, float terminal_velocity);
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


