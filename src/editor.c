
#include "stdint.h"
#include "stdio.h"
#include "errno.h"
#include "math.h"

#include <SDL3/SDL_events.h>

#define __USE_POSIX199309
#include "time.h"

#include "pt_error.h"

#include "editor.h"


PlatformType platform_type;
PlatformSelectAction select_type;
Tab current_tab;

const char *normal_platform_text = "Normal Block";
const char *spring_platform_text = "Spring";
const char *slime_platform_text = "Bouncy Slime";
const char *killer_platform_text = "Killer Block";

const char *select_move_text = "Move";
const char *select_delete_text = "Delete";

const char *platform_tab_text = "Platforms";
const char *select_tab_text = "Select";

#define point_in_rect(point, rect) \
point.x >= rect->x && point.x <= (rect->x + rect->w) && \
point.y >= rect->y && point.y <= (rect->y + rect->h)

typedef enum: uint8_t {
  SELECT_NONE = 0,
  SELECT_PLATFORM,
  SELECT_ABYSS,
  SELECT_HOME,
} SelectType;

SelectType select_level_item(
  Vec2 coords, // world woords
  Camera *cam, Level *level,
  uint8_t *type, uint32_t *index
) {
  *type = PLATFORM_BLOCK;
  Rect *item;
  for (uint16_t i = 0; i < level->normal_block.item_count; i += 1) {
    item = level->normal_block.items + i;
    if (point_in_rect(coords, item)) { *index = i; return SELECT_PLATFORM; }
  }
  *type = PLATFORM_SPRING;
  for (uint16_t i = 0; i < level->spring.item_count; i += 1) {
    item = level->spring.items + i;
    if (point_in_rect(coords, item)) { *index = i; return SELECT_PLATFORM; }
  }
  *type = PLATFORM_SLIME;
  for (uint16_t i = 0; i < level->slime.item_count; i += 1) {
    item = level->slime.items + i;
    if (point_in_rect(coords, item)) { *index = i; return SELECT_PLATFORM; }
  }
  *type = PLATFORM_KILL;
  for (uint16_t i = 0; i < level->kill_block.item_count; i += 1) {
    item = level->kill_block.items + i;
    if (point_in_rect(coords, item)) { *index = i; return SELECT_PLATFORM; }
  }
  if (fabsf(coords.y - level->abyss) < 5.0) {
    return SELECT_ABYSS;
  }
  float dx, dy;
  dx = coords.x - level->home.x;
  dy = coords.y - level->home.y;
  // no square root needed after squaring both sides :)
  if ( (dx*dx) + (dy*dy) < 25.0 ) {
    return SELECT_HOME;
  }
  return SELECT_NONE;
}

Vec2 mouse_world_coords(Camera *cam) {
  float mouse_x, mouse_y;
  SDL_GetMouseState(&mouse_x, &mouse_y);
  return Camera_to_world_coords(cam, mouse_x, mouse_y);
}

void run_menu_save_level_to_file(
  SDL_Renderer *renderer,
  Camera *cam,
  Level *level
) {
  SDL_Event event;
  while (true) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_KEY_DOWN: register_sdl_keydown(event.key); break;
        case SDL_EVENT_KEY_UP: register_sdl_keyup(event.key); break;
        default: {};
      }
    }

    if (keys_pressed(KEY_CONFIRM)) {
      consume_keys(KEY_CONFIRM);
      Level_save_to_file(level, level_filename);
      return;
    }
    if (keys_pressed(KEY_CANCEL)) {
      consume_keys(KEY_CANCEL);
      return;
    }

    float s_width = (float)cam->render_width;
    float s_height = (float)cam->render_height;

    float menu_width = s_width * 0.8;
    float menu_height = s_height * 0.8;
    float offset_x = (s_width - menu_width) / 2.0;
    float offset_y = (s_height - menu_height) / 2.0;

    Rect menu_rect = (Rect){ .x = offset_x, .y = offset_y, .w = menu_width, .h = menu_height };

    SDL_SetRenderDrawColor(renderer, 0x20, 0x20, 0x20, 0xff);
    SDL_RenderFillRect(renderer, &menu_rect);
    SDL_SetRenderDrawColor(renderer, 0x40, 0x40, 0x40, 0xff);
    SDL_RenderRect(renderer, &menu_rect);

    const char *prompt_text = "Would you like to save over file";
    const char *instruction_text = "Enter to confirm   Escape to cancel";

    float font_size = 15.0;

    float prompt_text_size = text_width(prompt_text, font_size);
    float instruction_text_size = text_width(instruction_text, font_size);

    float prompt_text_offset_x = (s_width - prompt_text_size) / 2.0;
    float instruction_text_offset_x = (s_width - instruction_text_size) / 2.0;
    
    float prompt_text_offset_y = (s_height / 2.0) - font_size;
    float instruction_text_offset_y = prompt_text_offset_y + font_size + 5.0;

    render_text(renderer, &font, prompt_text, prompt_text_offset_x, prompt_text_offset_y, font_size, 0xffffff00);
    render_text(
      renderer, &font,
      instruction_text, instruction_text_offset_x, instruction_text_offset_y,
      font_size, 0xaaaaaa00
    );

    SDL_RenderPresent(renderer);

    nanosleep(
      &(struct timespec){ .tv_sec = 0, .tv_nsec = 1000 * 1000 * 20 },
      NULL
    );
  }
}

EditorResult run_editor_loop(
  SDL_Window *window,
  SDL_Renderer *renderer,
  Player *player,
  Level *level,
  Camera *cam
) {
  Tab selected_tab = 0x00;
  PlatformSelectAction selected_action = 0x00;
  PlatformType selected_platform = 0x00;

  bool entering_platform = false;
  Vec2 new_platform_start = (Vec2){ 0 };

  SelectType selection_type = SELECT_NONE;
  void *selected_item = NULL;


  while (true) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_EVENT_QUIT: return EDITOR_RESULT_QUIT;
        case SDL_EVENT_MOUSE_BUTTON_DOWN: {
          if (selected_tab == TAB_PLATFORM) {
            Vec2 world_coords = Camera_to_world_coords(cam, event.button.x, event.button.y);
            if (!entering_platform) {
              new_platform_start = world_coords;
              entering_platform = true;
            }else {
              Rect new_rect = Rect_from_points(new_platform_start, world_coords);
              switch (selected_platform) {
                case PLATFORM_BLOCK: List_Rect_push(&level->normal_block, new_rect); break;
                case PLATFORM_SPRING: List_Rect_push(&level->spring, new_rect); break;
                case PLATFORM_SLIME: List_Rect_push(&level->slime, new_rect); break;
                case PLATFORM_KILL: List_Rect_push(&level->kill_block, new_rect); break;
                case PLATFORM_MAX: {};
              }
              entering_platform = false;
            }
          }
          else if (selected_tab == TAB_SELECT) {
            Vec2 coords = mouse_world_coords(cam);
            if (selected_item == NULL) {
              PlatformType type;
              uint32_t index;
              selection_type = select_level_item(coords, cam, level, &type, &index);
              if (selection_type == SELECT_PLATFORM) {
                switch (selected_action) {
                  case SELECT_MOVE: {
                    switch (type) {
                      case PLATFORM_BLOCK: selected_item = level->normal_block.items + index; break;
                      case PLATFORM_SPRING: selected_item = level->spring.items + index; break;
                      case PLATFORM_SLIME: selected_item = level->slime.items + index; break;
                      case PLATFORM_KILL: selected_item = level->kill_block.items + index; break;
                      case PLATFORM_MAX: {};
                    }
                  }; break;
                  case SELECT_DELETE: {
                    switch (type) {
                      case PLATFORM_BLOCK: List_Rect_swapback_delete(&level->normal_block, index); break;
                      case PLATFORM_SPRING: List_Rect_swapback_delete(&level->spring, index); break;
                      case PLATFORM_SLIME: List_Rect_swapback_delete(&level->slime, index); break;
                      case PLATFORM_KILL: List_Rect_swapback_delete(&level->kill_block, index); break;
                      case PLATFORM_MAX: {};
                    }
                    // List_Rect_swapback_delete(&level->normal_block, index);
                  }; break;
                  case SELECT_MAX: {};
                }
              }else if (selection_type == SELECT_ABYSS) {
                selected_item = &level->abyss;
              }else if (selection_type == SELECT_HOME) {
                selected_item = &level->home;
              }
            }else { selected_item = NULL; }
          }
        } break;
        case SDL_EVENT_KEY_DOWN: register_sdl_keydown(event.key); break;
        case SDL_EVENT_KEY_UP: register_sdl_keyup(event.key); break;
      }
    }

    if (keys_pressed(KEY_SWITCH_MODE)) {
      consume_keys(KEY_SWITCH_MODE);
      player->velocity = (Vec2){ 0.0, 0.0 };
      return EDITOR_RESULT_OK;
    }
    if (keys_pressed(KEY_CANCEL)) {
      consume_keys(KEY_CANCEL);
      if (entering_platform) {
        entering_platform = false;
      }else if (selected_item != NULL) {
        selected_item = NULL;
      }else {
        return EDITOR_RESULT_QUIT;
      }
    }
    if (keys_pressed(KEY_LOAD_FILE)) {
      consume_keys(KEY_LOAD_FILE);
      if (keys_down(KEY_SHIFT)) {
        // Level_free(*level);
        Level new_level = Level_load_from_file(level_filename);
        if (new_level.normal_block.items == NULL) {
          // TODO add error screen
          fprintf(stderr, "WARN: failed to load level, doing nothing\n");
        }else {
          Level_free(*level);
          *level = new_level;
        }
        // expect((level->normal_block.items != NULL), "Failed to load level from file");
      } else {
        run_menu_save_level_to_file(renderer, cam, level);
        // Level_save_to_file(level, level_filename);
      }
    }
    // switch selection
    if (keys_pressed(KEY_SPACE)) {
      consume_keys(KEY_SPACE);
      if (selected_tab == TAB_PLATFORM) {
        if (keys_down(KEY_SHIFT)) {
          if (selected_platform > 0) { selected_platform -= 1; }
          else { selected_platform = PLATFORM_MAX - 1; }
        }else {
          selected_platform += 1;
          if (selected_platform == PLATFORM_MAX) { selected_platform = 0; }
          // if (selected_platform < PLATFORM_MAX) { selected_platform += 1; }
          // else { selected_action = 0; }
        }
      }else if (selected_tab == TAB_SELECT) {
        selected_action += 1;
        if (selected_action == SELECT_MAX) { selected_action = 0x00; }
      }
      else { panic("Failed to handle tab variant", EXIT_FAILURE); }
    }
    zoom_camera(cam);

    //  Movement
    Direction move_dir = KeyState_get_input_direction();
    float move_speed = 4.0;
    if (keys_down(KEY_SHIFT)) { move_speed *= 2.0; }
    if (keys_pressed(KEY_TAB)) {
      consume_keys(KEY_TAB);
      if (selected_tab == TAB_SELECT) { selected_tab = TAB_PLATFORM; }
      else if (selected_tab == TAB_PLATFORM) { selected_tab = TAB_SELECT; }
      selected_action = 0x00;
    }
    if (move_dir & DIR_UP)    { player->body.y -= move_speed; }
    if (move_dir & DIR_DOWN)  { player->body.y += move_speed; }
    if (move_dir & DIR_LEFT)  { player->body.x -= move_speed; }
    if (move_dir & DIR_RIGHT) { player->body.x += move_speed; }

    if (selected_item != NULL) {
      Vec2 coords = mouse_world_coords(cam);
      switch (selection_type) {
        case SELECT_PLATFORM: {
          ((Rect *)selected_item)->x = coords.x;
          ((Rect *)selected_item)->y = coords.y;
        }; break;
        case SELECT_ABYSS: {
          *((float *)selected_item) = coords.y;
        }; break;
        case SELECT_HOME: {
          *((Vec2 *)selected_item) = coords;
        }; break;
        default: panic("Invalid selection type while item is selected", EXIT_FAILURE);
      }
      // moving_platform = NULL;
    }

    // Render
    Camera_update_focus_and_window_size(
      cam, window, (Vec2){ .x = player->body.x, .y = player->body.y }
    );

    SetRenderDrawColor_uint32(renderer, CLEAR_COLOR);
    SDL_RenderClear(renderer);

    Camera_render_level(cam, renderer, level);

    SDL_SetRenderDrawColor(renderer, 0xcc, 0x00, 0x88, 0xff);
    Camera_render_rects(cam, renderer, NULL, NULL, &player->body, 1, false);

    render_text(renderer, &font, "Edit Mode   Press E to play", 5.0, 5.0, 15.0, 0xffffff00);

    const uint32_t normal_text_color = 0x88888800;
    const uint32_t selected_text_color = 0xcccccc00;

    uint32_t text_color;

    // #define text_color(plat_type) (selected_action == plat_type) ? selected_text_color : normal_text_color
    #define print_text(text, y) render_text(renderer, &font, text, 5.0, y, 10.0, text_color)

    if (selected_tab == TAB_PLATFORM) {
      // text_color = text_color(PLATFORM_BLOCK);
      text_color = (selected_platform == PLATFORM_BLOCK)
        ? selected_text_color
        : normal_text_color;
      print_text(normal_platform_text, 125.0);

      // text_color = text_color(PLATFORM_SPRING);
      text_color = (selected_platform == PLATFORM_SPRING)
        ? selected_text_color
        : normal_text_color;
      print_text(spring_platform_text, 140.0);

      // text_color = text_color(PLATFORM_SLIME);
      text_color = (selected_platform == PLATFORM_SLIME)
        ? selected_text_color
        : normal_text_color;
      print_text(slime_platform_text, 155.0);

      // text_color = text_color(PLATFORM_KILL);
      text_color = (selected_platform == PLATFORM_KILL)
        ? selected_text_color
        : normal_text_color;
      print_text(killer_platform_text, 170.0);

    } else if (selected_tab == TAB_SELECT) {
      // text_color = text_color(SELECT_MOVE);
      text_color = (selected_action == SELECT_MOVE)
        ? selected_text_color
        : normal_text_color;
      print_text(select_move_text, 125.0);

      // text_color = text_color(SELECT_DELETE);
      text_color = (selected_action == SELECT_DELETE)
        ? selected_text_color
        : normal_text_color;
      print_text(select_delete_text, 140.0);
    } else { panic("Unhandled tab variant", EXIT_FAILURE); }

    float win_height = (float)cam->render_height;
    SDL_SetRenderDrawColor( renderer, 0x66, 0x66, 0x66, 0xff );
    SDL_RenderFillRect(renderer, &(Rect) {
      .x = 0.0, .y = win_height - 20.0,
      .w = (float)cam->render_width, .h = 20.0
    });

    SDL_SetRenderDrawColor(renderer, 0x22, 0x22, 0x22, 0xff);

    float offset = 5.0;
    // text_color = (selected_edit_tab == TAB_PLATFORM) ? selected_text_color : normal_text_color;
    if (selected_tab == TAB_PLATFORM) {
      SDL_RenderFillRect(renderer, &(Rect){
        .x = offset, .y = win_height - 20.0,
        .w = text_width(platform_tab_text, 10.0), .h = 20.0
      } );
    }
    render_text(renderer, &font, platform_tab_text, offset, win_height - 15.0, 10.0, 0xffffff00);

    offset += text_width(platform_tab_text, 10.0) + 5.0;

    if (selected_tab == TAB_SELECT) {
      SDL_RenderFillRect(renderer, &(Rect){
        .x = offset, .y = win_height - 20.0,
        .w = text_width(select_tab_text, 10.0), .h = 20.0
      });
    }
    render_text(renderer, &font, select_tab_text, offset, win_height - 15.0, 10.0, 0xffffff00);

    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

    Rect trect = Camera_convert_rect(cam, (Rect){ .x = level->home.x, .y = level->home.y, .w = 0.0, .h = 0.0 });
    SDL_RenderLine(renderer, trect.x - 10.0, trect.y, trect.x + 10.0, trect.y);
    SDL_RenderLine(renderer, trect.x, trect.y - 10.0, trect.x, trect.y + 10.0);

    trect = Camera_convert_rect(cam, (Rect){ .x = 0.0, .y = level->abyss, .w = 0.0, .h = 0.0 });
    SDL_RenderLine(renderer, 0.0, trect.y, (float)cam->render_width, trect.y);
    // SDL_RenderFillRect(renderer, &trect);

    if (entering_platform) {
      float mouse_x, mouse_y;
      SDL_GetMouseState(&mouse_x, &mouse_y);

      SDL_SetRenderDrawColor(renderer, 0xe0, 0xe0, 0xff, 0xff);
      Vec2 mouse_world_coords = Camera_to_world_coords(cam, mouse_x, mouse_y);
      Rect tmp_rect = Rect_from_points(new_platform_start, mouse_world_coords);

      Camera_render_rects(cam ,renderer, NULL, NULL, &tmp_rect, 1, false);
    }

    // SDL_SetRenderDrawColor(renderer, 0x88, 0x00, 0xff, 0xff);
    SDL_RenderPresent(renderer);

    nanosleep( &(struct timespec){ .tv_sec = 0, .tv_nsec = 1000000 * 12}, NULL );
  }
  // uint32_t win_w, win_h = SDL_GetWindowSize(window, (int *)&win_w, (int *)&win_h);
  // cam->render_width = win_w;
  // cam->render_height = win_h;
  // cam->focus = (Vec2){ .x = player->body.x, .y = player->body.y };

  // SDL_SetRenderDrawColor
}


