

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
#include "pt_error.h"
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
  char *level_filename = NULL;
  if (argc > 1) {
    if (argc > 2) { fprintf(stderr, "WARN: only one level file is supported at a time\n"); }
    level_filename = argv[1];
  }
  #ifndef WIN32 // NOTE this is part of a workaround (see #else below)
  else {
    DIR *current_dir = opendir(".");
    List_CharString level_names = List_CharString_new(4);

    struct dirent *entry = NULL;
    while((entry = readdir(current_dir)) != NULL) {
      size_t name_len = strlen(entry->d_name);
      // size_t name_len = entry->d_name;
      if (str_endswith(entry->d_name, name_len, ".lvl", 4)) {
        char *buffer = malloc(name_len+1);
        memcpy(buffer, entry->d_name, name_len);
        List_CharString_push(&level_names, (CharString){ .string = buffer });
      }
    }
    closedir(current_dir);

    expect(
      (level_names.item_count != 0),
      "no level files found in current directory, make a .lvl file or download the example from https://github.com/Krayfighter/flark/blob/master/main.lvl"
    );
    for (size_t i = 0; i < level_names.item_count; i += 1) {
      printf("(%lu): |%s|\n", i, List_CharString_get(&level_names, i)->string);
    }
    printf("Enter a level number: ");
    fflush(stdout);

    char input_buffer[4];
    fgets(input_buffer, 4, stdin);
    size_t len = strlen(input_buffer);
    input_buffer[len-1] = '\0';
    fprintf(stderr, "DBG: Input buffer, |%s|", input_buffer);
    int32_t index = parse_int(input_buffer, strlen(input_buffer));
    if (index < 0) { fprintf(stderr, "WARN: ignoring sign of negative number: %i index\n", index); }
    index = abs(index);
    expect(
      (index < level_names.item_count),
      "index out of range, (please select a file by the numbner displayed to its left)"
    );
    level_filename = List_CharString_get(&level_names, index)->string;
  }
  #else
  // NOTE this is workaround code for otherwise broken functionality on windows
  else {
    fprintf(stderr, "WARN: level selection currently fails on windows, this is known bug\n");
    level_filename = "main.lvl";
  }
  #endif
  expect((level_filename != NULL), "a file must be selected");

  SetConfigFlags(FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
  InitWindow(0, 0, "flark");
  MaximizeWindow();

  int window_width = 1240;
  int window_height = 720;
  #ifndef WIN32 // NOTE this is part of another workaround, there seem to be glfw linking issues for windows
  // raylib has its own functions for getting window width and height
  // but they aren't working correctly, so I use the glfw functions instead
  glfwGetWindowSize(GetWindowHandle(), &window_width, &window_height);
  #endif


  printf("\n\n\n"); // put some space between raylib init logging

  load_file:
  errno = 0;
  level_file = fopen(level_filename, "r");
  expect_errno("unable to open file");
  expect((level_file != NULL), "unable to open file");

  // FILE *level_file = fopen("main.lvl", "r");
  Level level = parse_level_stream(level_file);
  fclose(level_file);

  Player player = Player_spawn(&level);

  Camera2D camera;
  camera.target = (Vector2){ 0.0, 0.0 };
  camera.offset = (Vector2){ (float)window_width / 2.0, (float)window_height / 2.0 };
  camera.rotation = 0.0;
  camera.zoom = 1.0;

  bool frame_mode = false;
  bool first_frame = true;

  SetTargetFPS(60);

  fprintf(stderr, "DBG: gamepad name %s\n", GetGamepadName(0));

  char position_debug_string[128];
  Font font = LoadFont("./GeistMono-Medium.otf");


  while(!WindowShouldClose()) {
    player.input_state.controller_mode = IsGamepadAvailable(0);
    Player_do_friction(&player, 0.85);
    Player_step_input_frame(&player); // handle input for player

    if (IsKeyPressed(KEY_P)) { frame_mode = !frame_mode; }
    if (IsKeyPressed(KEY_R) && !first_frame) {
      goto load_file;
    }

    // TODO move this into Player_step_input_frame
    if (IsKeyDown(KEY_S)) {
      // TODO fix ground pound and make it better
      // Player_apply_gravity(&player, 1.2, 15.0);
      Player_apply_gravity(&player, 15.0);
    }else {
      // Player_apply_gravity(&player, 1.1, 10.0);
      Player_apply_gravity(&player, 10.0);
    }
    player.touching_ground = false; // reset whether the player has touched the ground this frame
    // reset whether the player is sliding this frame, but do not remove prev_speed
    // because the player may still be sliding on a wall
    player.slide_state.sliding = SLIDING_NONE;

    if (player.body.y > level.abyss_height) {
      player.body.x = level.start_position.x;
      player.body.y = level.start_position.y;
      player.velocity = (Vector2){ .x = 0.0, .y = 0.0 };
    }

    List_foreach(Platform, level.platforms, {
      Rectangle overlap = GetCollisionRec(player.body, item->body);
      // if (overlap.height != 0.0) {
      if (item->type == PLAT_BOUYANT) {
        CardinalDirection collision = Player_collide_rect(&player, item->body);
        switch (collision) {
          case DIR_UP: {
            player.velocity.y = 0.0;
            player.body.y = item->body.y - player.body.height;
            player.touching_ground = true;
            player.can_jump = true;
          }; break;
          case DIR_DOWN: {}; break;
          case DIR_LEFT: {}; break;
          case DIR_RIGHT: {}; break;
          case DIR_NONE: {}; break;
          default: { fprintf(stderr, "ERROR: INVALID CONDITION, invalid direction in main.c"); };
        }
        if (overlap.height != 0.0) {
          player.velocity.y -= 5.0;
        }
        // if (overlap.height != 0.0) {
        //   if (player.velocity.y > 0.0) { player.velocity.y = 0.0; player.body.y = item->body.y - player.body.height; }
        //   else{ player.velocity.y -= overlap.height; }
        //   player.can_jump = true;
        //   // touched_ground_last_frame = true;
        //   player.touching_ground = true;
        // }
      }
      // reverse bounce
      // if (player.velocity.y > 0.0) { player.velocity.y = 0.0; }
      // player.velocity.y *= -2.0;
      // booster bounce
      // if (player.velocity.y > 0.0) { player.velocity.y = 0.0; }
      // player.velocity.y -= overlap.height;
      else if (item->type == PLAT_BOUNCY) {
        CardinalDirection collision = Player_collide_rect(&player, item->body);
        switch (collision) {
          case DIR_UP: {
            if (IsKeyDown(KEY_SPACE)) {
              // float collision_distance = player.body.y - item->body.y - player.body.height;
              player.body.y = item->body.y - player.body.height;
              // player.velocity.y = -player.jump_velocity - (player.jump_velocity * (1.0/5.0));
              player.velocity.y = (-6.0 / 5.0) * player.jump_velocity;
            }else { player.velocity.y *= -1.0; }
          }; break;
          case DIR_DOWN: { player.velocity.y = -20.0; }; break;
          case DIR_LEFT: {}; break;
          case DIR_RIGHT: {}; break;
          case DIR_NONE: {}; break;
          default: {fprintf(stderr, "ERROR: INVALID CONDITION, unmathed cardinal direction in main.c\n");}
        }
        if (overlap.height != 0.0) {
          player.velocity.y -= 5.0;
        }
        // if (overlap.height != 0.0) {
        //   if (player.velocity.y > 0.0) { player.velocity.y *= -1.0; }
        //   else { player.velocity.y -= 10.0; }
        //   player.can_jump = true;
        //   player.touching_ground = true;
        // }
      }else if(item->type == PLAT_SOLID) {
        CardinalDirection collision = Player_collide_rect(&player, item->body);
        switch (collision) {
          case DIR_UP: {
            player.body.y = item->body.y - player.body.height;
            player.velocity.y = 0.0;
            player.can_jump = true;
            player.touching_ground = true;
          }; break;
          case DIR_DOWN: {
            player.body.y = item->body.y + item->body.height;
            player.velocity.y = 0.0;
            player.can_jump = true;
            player.touching_ground = false;
          }; break;
          case DIR_LEFT: {
            if (player.slide_state.prev_speed == 0.0) {
              player.body.x = item->body.x - player.body.width;
              player.slide_state.prev_speed = player.velocity.x;
            }
            player.slide_state.sliding = SLIDING_LEFT;
            player.velocity.x = 0.0;
            // player.can_jump = false;
          }; break;
          case DIR_RIGHT: {
            if (player.slide_state.prev_speed == 0.0) {
              player.body.x = item->body.x + item->body.width;
              player.slide_state.prev_speed = player.velocity.x;
            }
            player.slide_state.sliding = SLIDING_RIGHT;
            player.velocity.x = 0.0;
            // player.can_jump = false;
          }; break;
          case DIR_NONE: {}; break;
          default: {
            fprintf(stderr, "ERROR: INVALID CONDITION -> unmatched CardinalDirection in main.c\n");
          }; break;
        }
      }else if (item->type == PLAT_KILL) {
        CardinalDirection collision = Player_collide_rect(&player, item->body);
        if (collision != DIR_NONE) {
          player = Player_spawn(&level);
        }
      }
      else { fprintf(stderr, "WARN: unimplemented Platform type -> %u", item->type); }
    });

    // if the player has not collided with a wall this frame, then reset
    // the stored speed in the wall slide indicating that the player is truly
    // no longer sliding
    if (player.slide_state.sliding == SLIDING_NONE) {
      player.slide_state.prev_speed = 0.0;
    }

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
        DrawRectangleRec(item->body, item->color);
        if (item->type == PLAT_KILL) {
          DrawRectangleLinesEx(item->body, 3.0, (Color){ .r = 0xe0, .g = 0x70, .b = 0x05, .a = 0xff });
        }
      }
    } )
    EndMode2D();

    snprintf(position_debug_string, 128, "Character Position: {%f},{%f}", player.body.x, player.body.y);
    // DrawText(position_debug_string, 5, 5, 15, RED);
    DrawTextEx(font, position_debug_string, (Vector2){ .x = 5, .y = 5 }, 15, 1, RED);

    EndDrawing();

    // halt at the end of each frame when operating in frame-by-frame mode
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
      }
    }
    // #endif
    first_frame = false;
  }

  CloseWindow();
  
  return 0;
}
#endif



