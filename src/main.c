

#include "stdint.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"

#ifndef WIN32
#include "sys/fcntl.h"
#include "unistd.h"
#endif

#include "raylib.h"

#include "plustypes.h"
#include "level_loader.h"


typedef struct {
  Rectangle body;
  Vector2 velocity;
  bool can_jump;
} Player;

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

void Player_do_friction(Player *self, float friction, bool grounded) {
  if (grounded) { self->velocity.x *= friction; }
}

typedef enum {
  MOVE_RIGHT,
  MOVE_LEFT,
} MovementDireciton;

// assumes that the vector is normalized
void player_move_direction(Player *self, MovementDireciton dir, bool touching_ground) {
  const float PLAYER_MAX_SPEED = 10.0;
  const float PLAYER_ACCELERATION = 1.3;
  const float PLAYER_AIR_ACCELERATION = 1.05;

  int direction_sign = 1;
  if (dir == MOVE_LEFT) { direction_sign = -1; }

  
  self->velocity.x += 0.1 * direction_sign;

  int velocity_sign = 1;
  if (self->velocity.x < 0.0) { velocity_sign = -1;}

  if (velocity_sign == direction_sign) {
    if (touching_ground) { self->velocity.x *= PLAYER_ACCELERATION; }
    else { self->velocity.x *= PLAYER_AIR_ACCELERATION; }
  }else {
    if (touching_ground) { self->velocity.x /= PLAYER_ACCELERATION; }
    else { self->velocity.x /= PLAYER_AIR_ACCELERATION; }
  }

  if (fabsf(self->velocity.x) > PLAYER_MAX_SPEED) {
    self->velocity.x = PLAYER_MAX_SPEED * velocity_sign;
  }
}

#ifdef TESTS
int main() {
  bool char_number_test_failed = TEST_char_numbers(stderr);
  if (char_number_test_failed) { fprintf(stderr, "TEST: failed char_number test\n"); }
  else { fprintf(stderr, "SUCCESS: char_number has passed its test unit\n"); }

  bool parse_int_failed = TEST_parse_int(stderr);
  if (parse_int_failed) { fprintf(stderr, "TEST: failed parse_int test unit\n"); }
  else { fprintf(stderr, "SUCCESS: parse_int hase passed its test unit\n"); }
}
#else
int main() {

  FILE *level_file = fopen("main.lvl", "r");
  // char buffer[512];
  // while (fgets(buffer, 512, level_file)) {
  //   fprintf(stderr, "DBG: file line %s", buffer);
  // }
  Level level = parse_level_stream(level_file);

  #ifndef WIN32
  fcntl(fileno(stdin), F_SETFL, O_NONBLOCK);
  #endif

  const uint32_t WINDOW_WIDTH = 600;
  const uint32_t WINDOW_HEIGHT = 400;

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "flark");
  SetWindowPosition(30, 30);
  
  // Vector2 start_position = (Vector2){ .x = 20.0, .y = 0.0 };

  Player player = (Player) {
    .body = (Rectangle){ .x = level.start_position.x, .y = level.start_position.y, .width = 10.0, .height = 20.0 },
    .velocity = (Vector2){ .x = 0.0, .y = 0.0 }
  };

  Camera2D camera;
  camera.target = (Vector2){ 20.0, 20.0 };
  camera.offset = (Vector2){ WINDOW_WIDTH / 2.0, WINDOW_HEIGHT / 2.0 };
  camera.rotation = 0.0;
  camera.zoom = 1.0;

  bool frame_mode = false;

  SetTargetFPS(60);

  bool touched_ground_last_frame = false;

  while(!WindowShouldClose()) {
    if (IsKeyDown(KEY_D)) { player_move_direction(&player, MOVE_RIGHT, touched_ground_last_frame); }
    if (IsKeyDown(KEY_A)) { player_move_direction(&player, MOVE_LEFT, touched_ground_last_frame); }
    if (IsKeyPressed(KEY_SPACE) && player.can_jump) {
      player.velocity.y -= 20.0;
      player.can_jump = false;
    }
    if (IsKeyPressed(KEY_P)) { frame_mode = !frame_mode; }

    // if (player.can_jump == true) {
    Player_do_friction(&player, 0.85, touched_ground_last_frame);
    // }
    if (IsKeyDown(KEY_S)) {
      Player_apply_gravity(&player, 1.2, 15.0);
    }else {
      Player_apply_gravity(&player, 1.1, 10.0);
    }
    touched_ground_last_frame = false;

    if (player.body.y > 300.0) {
      player.body.x = level.start_position.x;
      player.body.y = level.start_position.y;
      player.velocity = (Vector2){ .x = 0.0, .y = 0.0 };
    }

    List_foreach(Platform, level.platforms, {
      Rectangle overlap = GetCollisionRec(player.body, item->body);
      // if (overlap.height != 0.0) {
      if (item->type == PLAT_BOUYANT) {
        if (overlap.height != 0.0) {
          if (player.velocity.y > 0.0) { player.velocity.y = 0.0; player.body.y = item->body.y - player.body.height; }
          else{ player.velocity.y -= overlap.height; }
          player.can_jump = true;
          touched_ground_last_frame = true;
        }
      }
      // reverse bounce
      // if (player.velocity.y > 0.0) { player.velocity.y = 0.0; }
      // player.velocity.y *= -2.0;
      // booster bounce
      // if (player.velocity.y > 0.0) { player.velocity.y = 0.0; }
      // player.velocity.y -= overlap.height;
      else if (item->type == PLAT_BOUNCY) {
        if (overlap.height != 0.0) {
          if (player.velocity.y > 0.0) { player.velocity.y *= -1.0; }
          else { player.velocity.y -= 10.0; }
          player.can_jump = true;
          touched_ground_last_frame = true;
        }
      }else if(item->type == PLAT_SOLID) {
        // This is supposed to do vector collision physics but I don't think it works right
        // bool player_collides = CheckCollisionLines(
        //   // player.body,
        //   (Vector2){ .x = player.body.x, .y = player.body.y },
        //   (Vector2){ .x = player.body.x + player.velocity.x, .y = player.body.y + player.velocity.y },
        //   (Vector2){ .x = item->body.x, .y = item->body.y },
        //   (Vector2){ .x = item->body.x+item->body.width, .y = item->body.y },
        //   NULL
        // );
        bool in_collision_range = (
          player.body.x + player.body.width + player.velocity.x > item->body.x &&
          player.body.x + player.velocity.x < item->body.x + item->body.width
        );
        if (in_collision_range) {
        // if (true) {
          bool player_above = player.body.y < item->body.y - item->body.height;
          bool next_frame_player_above = player.body.y + player.body.height + player.velocity.y <= item->body.y - item->body.height;
          bool will_collide_from_top = player_above && !next_frame_player_above;

          bool player_below = player.body.y > item->body.y;
          bool next_frame_player_below = player.body.y + player.velocity.y > item->body.y;
          bool will_collide_from_bottom = player_below && !next_frame_player_below;

          // fprintf(stderr,
          //   "DBG pa %u na %u, ct %u | pb %u, nb %u, cb %u\n",
          //   player_above, next_frame_player_above, will_collide_from_top,
          //   player_below, next_frame_player_below, will_collide_from_bottom
          // );

          if (will_collide_from_top) {
            // fprintf(stderr, "DBG: colliding from top\n");
            // do collision
            player.velocity.y = 0.0;
            player.body.y = item->body.y - player.body.height;
            player.can_jump = true;
            touched_ground_last_frame = true;
          }else {
            // bool player_below = player.body.y - player.body.height > item->body.y;
            // bool next_frame_player_below = player.body.y + player.velocity.y - player.body.height > item->body.y;
            // bool will_collide_from_bottom = player_below && !next_frame_player_below;

            // fprintf(stderr,
            //   "DBG pa %u na %u, ct %u | pb %u, nb %u, cb %u\n",
            //   player_above, next_frame_player_above, will_collide_from_top,
            //   player_below, next_frame_player_below, will_collide_from_bottom
            // );

            if (will_collide_from_bottom) {
              fprintf(stderr, "DBG: colliding from below\n");
              // do collision
              player.velocity.y = 0.0;
              player.body.y = item->body.y + item->body.height;
              player.can_jump = true;
              // touched_ground_last_frame = false;
            }
          }
        }
        // player_collides |= CheckCollisionLines(
        //   (Vector2){ .x = player.body.x, .y = player.body.y + player.body.height },
        //   (Vector2){ .x = player.body.x}, , , )
        // ) || CheckCollisionLines(
        //   // player.body,
        //   (Vector2){ .x = player.body.x, .y = player.body.y - player.body.height },
        //   (Vector2){ .x = player.body.x + player.velocity.x, .y = player.body.y - player.body.height + player.velocity.y },
        //   (Vector2){ .x = item->body.x, .y = item->body.y },
        //   (Vector2){ .x = item->body.x+item->body.width, .y = item->body.y },
        //   NULL
        // );
        // if (player_collides) { 
        //   player.body.y = item->body.y - player.body.height -1;
        //   player.velocity.y = 0.0;
        // }
        // player.is_grounded = true;
        // else if (CheckCollisionRecs(player.body, item->body)) {
        //   if (player.velocity.y < 0.0) {
        //     player.body.y = item->body.y + item->body.height;
        //     player.velocity.y = 0.0;
        //     player.is_grounded = false;
        //   }else {
        //     player.body.y = item->body.y - player.body.height;
        //     player.velocity.y = 0.0;
        //     player.is_grounded = true;
        //   }
        // }
        // }else {
        //   fprintf(stderr, "Error: platform type invalid or not implemented\n");
        // }
        // player.is_grounded = true;
      }
    });

    Player_move(&player);
    
    camera.target = (Vector2){ player.body.x, player.body.y };

    BeginDrawing();
    ClearBackground(level.background_color);

    BeginMode2D(camera);
    DrawRectangleRec(player.body, RED);
    List_foreach(Platform, level.platforms, {
      if (item->body.height < 1.0) {
        DrawLine(item->body.x, item->body.y, item->body.x+item->body.width, item->body.y, item->color);
      }else {
        DrawRectangleRec(item->body, item->color); }
      }
    )
    EndMode2D();

    EndDrawing();

    // halt at the end of each frame when operating in frame-by-frame mode
    // also only works with POSIX APIs
    #ifndef WIN32
    if (frame_mode) {

      while (1) {
        char buf = 0x0;
        ssize_t read_size = read(fileno(stdin), &buf, 1);
        if (read_size != -1 && read_size != 0) {
          if (buf == 'p') { frame_mode = false; }
          break;
        }
        usleep(1000);
      }
    }
    #endif
  }

  CloseWindow();
  
  return 0;
}
#endif



