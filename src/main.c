

#include "stdio.h"
#include "stdlib.h"
#include "errno.h"

#define __USE_POSIX199309
#include "time.h"

// #include "unistd.h"

#include "SDL3/SDL.h"

// For lsp
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_scancode.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>

#include "pt_error.h"
#include "plustypes.h"

#include "geometry.h"
#include "text.h"
#include "level.h"
#include "editor.h"
#include "input.h"
#include "player.h"
#include "camera.h"



int main() {

  expect(SDL_Init(SDL_INIT_VIDEO) == true, "Failed to initialize SDL3")

  SDL_Window *window;
  SDL_Renderer *renderer;
  expect(
    SDL_CreateWindowAndRenderer(
      "New Flark", 0, 0,
      SDL_WINDOW_RESIZABLE | SDL_WINDOW_TRANSPARENT,
      &window, &renderer
    ) == true,
    "Failed to create SDL window and/or renderer"
  )
  SDL_MaximizeWindow(window);

  font = Font_load_bmp(renderer, "alphabet_caps.bmp");
  expect((font.charmap != NULL), "Failed to load font");

  SDL_Surface *plat_surface = SDL_LoadBMP("platform.bmp");
  solid_block_texture = SDL_CreateTextureFromSurface(renderer, plat_surface);
  SDL_DestroySurface(plat_surface);

  SDL_Surface *spring_surface = SDL_LoadBMP("spring.bmp");
  spring_texture = SDL_CreateTextureFromSurface(renderer, spring_surface);
  SDL_DestroySurface(spring_surface);

  Level level = Level_new();

  Player player = (Player) {
    .body = (Rect) { .x = level.home.x, .y = level.home.y, .w = 10.0, .h = 20.0 },
    .velocity = (Vec2) { .x = 0.0, .y = 0.0 },
    .state = 0x00
  };

  Camera cam = (Camera){ 0 };
  cam.view_width = 1000.0;


  SDL_Event event = { 0 };

  EditorResult result = run_editor_loop(window, renderer, &player, &level, &cam);
  if (result == EDITOR_RESULT_QUIT) { goto APP_QUIT; }
  
  LOOP_START: while (true) {
    // Handle Events
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) { goto APP_QUIT; }
      else if (event.type == SDL_EVENT_KEY_DOWN) { register_sdl_keydown(event.key); }
      else if (event.type == SDL_EVENT_KEY_UP) { register_sdl_keyup(event.key); }
    }

    if (keys_pressed(KEY_SWITCH_MODE)) {
      consume_keys(KEY_SWITCH_MODE);
      EditorResult result = run_editor_loop( window, renderer, &player, &level, &cam );
      switch (result) {
        case EDITOR_RESULT_OK: {} break;
        case EDITOR_RESULT_QUIT: goto APP_QUIT;
      }
    }
    zoom_camera(&cam);

    // Physics / Movement
    Direction move_dir = KeyState_get_input_direction();
    Player_accelerate(&player, move_dir);

    player.state &= ~PLAYER_TOUCHING_GROUND;
    player.state &= ~PLAYER_TOUCHING_WALL_LEFT;
    player.state &= ~PLAYER_TOUCHING_WALL_RIGHT;

    for (uint16_t i = 0; i < level.normal_block.item_count; i += 1) {
      Direction collision_dir = Rect_vector_collision(player.body, player.velocity, level.normal_block.items[i]);
      if (collision_dir != DIR_NONE) {
        Player_collide_normal_block(&player, level.normal_block.items + i, collision_dir);
      }
    }

    for (uint16_t i = 0; i < level.spring.item_count; i += 1) {
      Direction collision_dir = Rect_vector_collision(player.body, player.velocity, level.spring.items[i]);
      if (collision_dir != DIR_NONE) {
        Player_collide_spring(&player, level.spring.items + i, collision_dir, keystate);
      }
    }

    for (uint16_t i = 0; i < level.slime.item_count; i += 1) {
      Direction collision_dir = Rect_vector_collision(player.body, player.velocity, level.slime.items[i]);
      Player_collide_slime(&player, level.slime.items + i, collision_dir, keystate);
    }

    for (uint16_t i = 0; i < level.kill_block.item_count; i += 1) {
      Direction collision_dir = Rect_vector_collision(player.body, player.velocity, level.kill_block.items[i]);
      if (collision_dir != DIR_NONE) {
        player.body.x = level.home.x;
        player.body.y = level.home.y;
        player.velocity = (Vec2){ .x = 0.0, .y = 0.0 };
        goto LOOP_START;
      }
    }


    player.body.x += player.velocity.x;
    player.body.y += player.velocity.y;

    // kill the player if the pass the abyss
    if (player.body.y + player.body.h > level.abyss) {
      player.velocity = (Vec2){ 0.0, 0.0 };
      player.state = 0x00;
      player.body.x = level.home.x;
      player.body.y = level.home.y;
      goto LOOP_START;
    }

    // jumping / wall jumping
    const float JUMP_VELOCITY = 5.0;
    if (
      (player.state & PLAYER_TOUCHING_GROUND) &&
      keys_pressed(KEY_SPACE)
    ) {
      player.velocity.y -= JUMP_VELOCITY;
      consume_keys(KEY_SPACE);
    }else if (
      player.state & PLAYER_TOUCHING_WALL_LEFT &&
      keys_pressed(KEY_SPACE)
    ) {
      player.velocity.y = -JUMP_VELOCITY;
      player.velocity.x = -PLAYER_MAX_MOVE_SPEED;
      consume_keys(KEY_SPACE);
    }else if (
      player.state & PLAYER_TOUCHING_WALL_RIGHT &&
      keys_pressed(KEY_SPACE)
    ) {
      player.velocity.y = -JUMP_VELOCITY;
      player.velocity.x = PLAYER_MAX_MOVE_SPEED;
      consume_keys(KEY_SPACE);
    }


    // Render
    Camera_update_focus_and_window_size(&cam, window, (Vec2){ .x = player.body.x, .y = player.body.y });

    SetRenderDrawColor_uint32(renderer, CLEAR_COLOR);
    SDL_RenderClear(renderer);

    Camera_render_level( &cam, renderer, &level );

    Camera_render_rects(&cam, renderer, NULL, NULL, &player.body, 1, false);

    render_text(
      renderer, &font,
      "Game Mode   Press E to edit",
      5.0, 5.0, 15.0,
      0xffffff00
    );

    SDL_RenderPresent(renderer);

    #define MICROS(n) 1000 * n
    #define MILLIS(n) 1000 * MICROS(n)

    nanosleep(
      &(struct timespec){ .tv_sec = 0, .tv_nsec = MILLIS(12) },
      NULL
    );
  }
  
  APP_QUIT: {};

  if (level.normal_block.items != NULL) {
    Level_free(level);
  }

  SDL_DestroyTexture(solid_block_texture);
  SDL_DestroyTexture(spring_texture);

  SDL_Quit();
  
  return 0;
}

