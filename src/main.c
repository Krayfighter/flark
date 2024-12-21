

#include "stdint.h"
#include "stdio.h"
// #include "math.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "dirent.h"

#ifndef WIN32
#include "sys/fcntl.h"
#include "unistd.h"
#define sleep_millis(count) usleep(count * 1000)
#else
// #include "windows.h"
#define sleep_millis(count) WaitTime((double)count * 0.001)
#endif

#include "raylib.h"
// #include "raymath.h"
#include "external/glfw/include/GLFW/glfw3.h"

#include "plustypes.h"
#include "level_loader.h"
#include "player.h"


typedef struct { char *string; } CharString;

declare_List(CharString)
define_List(CharString)

bool str_endswith(char *string, size_t len, char *postfix, size_t plen) {
  if (plen > len) { return false; }
  bool not_endswith = false;
  for (ssize_t n_index = 0; n_index < plen; n_index += 1) {
    not_endswith |= (string[len-n_index] != postfix[plen-n_index]);
  }
  return !not_endswith;
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
int main(int argc, char **argv) {

  FILE *level_file = NULL;
  if (argc > 1) {
    if (argc > 2) { fprintf(stderr, "WARN: only one level file is supported at a time\n"); }
    level_file = fopen(argv[1], "r");
    if (level_file == NULL) {
      fprintf(stderr, "Error: unable to open file -> %s\n", strerror(errno));
      exit(-1);
    }
  }else {
    DIR *current_dir = opendir(".");
    List_CharString level_names = List_CharString_new(4);

    struct dirent *entry = NULL;
    while((entry = readdir(current_dir)) != NULL) {
      size_t name_len = strlen(entry->d_name);
      if (str_endswith(entry->d_name, name_len, ".lvl", 4)) {
        char *buffer = malloc(name_len);
        memcpy(buffer, entry->d_name, name_len);
        List_CharString_push(&level_names, (CharString){ .string = buffer });
      }
    }
    closedir(current_dir);

    if (level_names.item_count == 0) {
      fprintf(stderr, "Error: no level files found in current directory, make a .lvl file or download the example from https://github.com/Krayfighter/flark/blob/master/main.lvl\n");
      exit(-1);
    }
    for (size_t i = 0; i < level_names.item_count; i += 1) {
      printf("(%lu): %s\n", i, List_CharString_get(&level_names, i)->string);
    }
    printf("Enter a level number: ");
    fflush(stdout);

    char input_buffer[4];
    fgets(input_buffer, 4, stdin);
    size_t len = strlen(input_buffer);
    input_buffer[len-2] = '\0';
    int32_t index = parse_int(input_buffer, strlen(input_buffer));
    if (index < 0) { fprintf(stderr, "WARN: ignoring sign of negative number: %i index\n", index); }
    index = abs(index);
    if (index > level_names.item_count) {
      fprintf(stderr, "Error: index out of range: %i\n", index);
      exit(-1);
    }
    level_file = fopen(List_CharString_get(&level_names, index)->string, "r");
    if (level_file == NULL) {
      fprintf(stderr, "Error: unable to open file -> %s\n", strerror(errno));
      exit(-1);
    }
  }

  SetConfigFlags(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
  InitWindow(0, 0, "flark");
  MaximizeWindow();

  // raylib has its own functions for getting window width and height
  // but they aren't working correctly, so I use the glfw functions instead
  int window_width;
  int window_height;
  glfwGetWindowSize(GetWindowHandle(), &window_width, &window_height);


  printf("\n\n\n"); // put some space between raylib init logging
  
  // FILE *level_file = fopen("main.lvl", "r");
  Level level = parse_level_stream(level_file);

  Player player = (Player) {
    .body = (Rectangle){ .x = level.start_position.x, .y = level.start_position.y, .width = 10.0, .height = 20.0 },
    .velocity = (Vector2){ .x = 0.0, .y = 0.0 }
  };

  Camera2D camera;
  camera.target = (Vector2){ 0.0, 0.0 };
  camera.offset = (Vector2){ (float)window_width / 2.0, (float)window_height / 2.0 };
  camera.rotation = 0.0;
  camera.zoom = 1.0;

  bool frame_mode = false;

  SetTargetFPS(60);

  // bool touched_ground_last_frame = false;
  // bool controller_mode = false;


  fprintf(stderr, "DBG: gamepad name %s\n", GetGamepadName(0));

  while(!WindowShouldClose()) {
    // if (IsGamepadAvailable(0)) { controller_mode = true; }
    player.input_state.controller_mode = IsGamepadAvailable(0);
    Player_step_input_frame(&player);
    // if (!controller_mode) {
    //   // if (IsKeyDown(KEY_D)) { player_move_direction(&player, MOVE_RIGHT, touched_ground_last_frame); }
    //   // if (IsKeyDown(KEY_A)) { player_move_direction(&player, MOVE_LEFT, touched_ground_last_frame); }
    //   // if (IsKeyPressed(KEY_SPACE) && player.can_jump) {
    //   //   player.velocity.y -= 20.0;
    //   //   player.can_jump = false;
    //   // }
    // }else {
    //   // if (IsGamepadAvailable(0)) {
    //   float x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    //   // float y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
    //   if (fabsf(x) > 0.1) {
    //     fprintf(stderr, "DBG: x pad %f\n", x);
    //     x = Clamp(x * 1.5, -1.0, 1.0);
    //     // player_move_analogue(&player, touched_ground_last_frame, x);
    //   }
    //   switch (GetGamepadButtonPressed()) {
    //     case GAMEPAD_BUTTON_RIGHT_FACE_RIGHT: {
    //       if (player.can_jump) { player.velocity.y -= 20.0; player.can_jump = false; }
    //     } break;
    //   }
    //   // }else {
    //   //   fprintf(stderr, "DBG: gamepad not available\n");
    //   // }
    // }
    if (IsKeyPressed(KEY_P)) { frame_mode = !frame_mode; }

    Player_do_friction(&player, 0.85);
    if (IsKeyDown(KEY_S)) {
      Player_apply_gravity(&player, 1.2, 15.0);
    }else {
      Player_apply_gravity(&player, 1.1, 10.0);
    }
    // touched_ground_last_frame = false;
    player.touching_ground = false;

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
          // touched_ground_last_frame = true;
          player.touching_ground = true;
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
          player.touching_ground = true;
        }
      }else if(item->type == PLAT_SOLID) {
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

          if (will_collide_from_top) {
            // do collision
            player.velocity.y = 0.0;
            player.body.y = item->body.y - player.body.height;
            player.can_jump = true;
          player.touching_ground = true;
          }else if (will_collide_from_bottom) {
            // do collision
            player.velocity.y = 0.0;
            player.body.y = item->body.y + item->body.height;
            player.can_jump = true;
          }
        }
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
    // #ifndef WIN32
    if (frame_mode) {
      printf("INFO: press p then Enter into terminal to unpause\n");

      while (1) {
        // char buf = 0x0;
        char buffer[128];
        // ssize_t read_size = read(fileno(stdin), &buf, 1);
        char *result = fgets(buffer, 128, stdin);
        if (result != NULL) {
          if (!strcmp(buffer, "p\n")) { frame_mode = false; }
          break;
        }
        sleep_millis(1);
        // usleep(1000);
      }
    }
    // #endif
  }

  CloseWindow();
  
  return 0;
}
#endif



