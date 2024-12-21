
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
void player_move_analogue(Player *self, bool touching_ground, float input);

typedef enum {
  MOVE_RIGHT,
  MOVE_LEFT,
} MovementDireciton;
void player_move_direction(Player *self, MovementDireciton dir, bool touching_ground);

// void InputBuffer_step_frame(InputBuffer *self, Player *player, bool controller_mode);


