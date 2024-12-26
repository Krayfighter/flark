
#include "pt_error.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include "math.h"

#include "raylib.h"

#include "plustypes.h"
#include "error.h"
#include "level_loader.h"


const Platform Platform_new(float x, float y, float w, float h, Color color, PlatformType type) {
  return (Platform) {
    .body = (Rectangle){ .x = x, .y = y, .width = w, .height = h },
    .color = color,
    .type = type,
  };
}

void Platform_DBG(Platform *self, FILE *output_stream) {
  fprintf(
    output_stream,
    "PLATFORM: x: %f, y: %f, w: %f, h: %f, r: %u, g: %u, b: %u, a: %u\n",
    self->body.x, self->body.y, self->body.width, self->body.height,
    self->color.r, self->color.g, self->color.b, self->color.a
  );
}

define_List(Platform)

bool string_match(char *str1, size_t str1len, char *str2, size_t str2len) {
  if (str1len != str2len) {
    return false;
  }
  for (size_t index = 0; index < str1len; index += 1) {
    if (str1[index] != str2[index]) { return false; }
  }
  return true;
}

bool string_starts(char *string, size_t string_len, char *substring, size_t substring_len) {
  if (substring_len > string_len) { return false; }
  for (size_t index = 0; index < substring_len; index += 1) {
    if (string[index] != substring[index]) { return false; }
  }
  return true;
}

// float parse_float(char *string, size_t len) {
//   size_t dot = 
// }

// returns base-10 number from char or -1 on failure
int8_t char_number(char chr) {
  int8_t number = chr - (int8_t)'0';
  if (number < 0 || number > 9) { return -1; }
  return number;
}

#ifdef TESTS
char char_numbers[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
uint8_t test_numbers[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
char fail_chars[] = { 'a', 'b', 'g', 'A', 'i', '=', '\\', '.', '/', '`', ';' };

bool TEST_char_numbers(FILE *logger) {
  bool failed = false;

  for (size_t index = 0; index < 10; index += 1) {
    if (char_number(char_numbers[index]) != test_numbers[index]) {
      fprintf(logger, "TEST: failed char_number(%c) != %u (expected equality)\n", char_numbers[index], test_numbers[index]);
      failed = true;
    }
  }

  for (size_t index = 0; index < 11; index += 1) {
    if (char_number(fail_chars[index]) != -1) {
      fprintf(logger, "TEST: failed, char_number(%c) should return fail condition\n", fail_chars[index]);
      failed = true;
    }
  }

  return failed;
}
#endif

int32_t parse_int(char *string, size_t len) {
  int8_t positive = 1;
  char *new_string = string;
  size_t new_len = len;
  if (string[0] == '-') {
    positive = -1;
    new_string = string+1;
    new_len = len-1;
  }
  int32_t number = 0;
  // size_t index = 0;
  for (size_t iter = 0; iter < new_len; iter += 1) {
    int32_t chr_num = char_number(new_string[iter]);
    if (chr_num == -1) { errno = EINVAL; }
    number += (powl(10, (new_len-iter - 1))) * chr_num;
    // index += 1;
  }
  return number * positive;
}

#ifdef TESTS
char *number_strings[] = { "10", "1006", "8134", "012562", "54", "-1456", "-87751", "1153" };
int32_t numbers[] = { 10, 1006, 8134, 12562, 54, -1456, -87751, 1153 };

bool TEST_parse_int(FILE *logger) {
  bool failed = false;

  for (size_t index = 0; index < 8; index += 1) {
    int32_t parsed_int = parse_int(number_strings[index], strlen(number_strings[index]));
    if (parsed_int != numbers[index]) {
      fprintf(
        logger,
        "TEST: failure, parse_int(%s) == %i != %i (expected equality)\n",
        number_strings[index], parsed_int, numbers[index]
      );
      failed = true;
    }
  }
  return failed;
}
#endif


Level parse_level_stream(FILE *stream) {
  char line_buffer[512];

  Level self = (Level){
    .start_position = (Vector2){ .x = 0.0, .y = 0.0},
    .platforms = List_Platform_new(8),
    .background_color = (Color){ .r = 0x10, .g = 0x10, .b = 0x10, .a = 0xff },
    .player_gravity = 1.1,
    .player_acceleration = 1.0,
    .player_max_speed = 10.0,
    .player_jump_velocity = 20.0,
    .abyss_height = 300.0,
  };

  size_t line_number = 0;
  while (fgets(line_buffer, 512, stream) != NULL) {
    line_number += 1;
    size_t buffer_len = strlen(line_buffer);
    if (string_match(line_buffer, buffer_len, "\n", 1)) { continue; }
    if (string_starts(line_buffer, buffer_len, "//", 2)) { continue; }
    // parse a platform declaration
    else if (string_starts(line_buffer, buffer_len, "Platform:", 9)) {
      Platform plat = (Platform) {
        .body = (Rectangle){ .x = 0.0, .y = 0.0, .width = 10.0, .height = 10.0 },
        .color = (Color){ .r = 255, .g = 255, .b = 255, .a = 255 },
        .type = PLAT_SOLID
      };
      char *token = strtok(line_buffer+9, ";");

      while (token != NULL) {
        if (token[0] == '\n') { break; }
        size_t item_len = strlen(token);
        if (item_len < 2) { fprintf(stderr, "Error: syntax error on line %lu\n", line_number); }

        int32_t value = parse_int(token+1, item_len-1);
        switch (token[0]) {
          case 'x': plat.body.x = value; break;
          case 'y': plat.body.y = -value; break;
          case 'w': plat.body.width = value; break;
          case 'h': plat.body.height = value; break;
          case 'r': plat.color.r = value; break;
          case 'g': plat.color.g = value; break;
          case 'b': plat.color.b = value; break;
          case 'a': plat.color.a = value; break;
          case 't': {
            char *subtoken = token+1;
            size_t tok_len = strlen(subtoken);
            if (string_match(subtoken, tok_len, "bouyant", 7)) { plat.type = PLAT_BOUYANT; }
            else if (string_match(subtoken, tok_len, "bouncy", 6)) { plat.type = PLAT_BOUNCY; }
            else if (string_match(subtoken, tok_len, "solid", 5)) { plat.type = PLAT_SOLID; }
            else if (string_match(subtoken, tok_len, "kill", 4)) { plat.type = PLAT_KILL; }
          }; break;
          case 'c': {
            char *subtoken = token+1;
            size_t tok_len = strlen(subtoken);
            if (string_match(subtoken, tok_len, "red", 3)) { plat.color = RED; }
            else if (string_match(subtoken, tok_len, "lightgray", 9)) { plat.color = LIGHTGRAY; }
            else if (string_match(subtoken, tok_len, "gray", 4)) { plat.color = GRAY; }
            else if (string_match(subtoken, tok_len, "darkgray", 8)) { plat.color = DARKGRAY; }
            else if (string_match(subtoken, tok_len, "yellow", 6)) { plat.color = YELLOW; }
            else if (string_match(subtoken, tok_len, "gold", 4)) { plat.color = GOLD; }
            else if (string_match(subtoken, tok_len, "orange", 6)) { plat.color = ORANGE; }
            else if (string_match(subtoken, tok_len, "pink", 4)) { plat.color = PINK; }
            else if (string_match(subtoken, tok_len, "maroon", 6)) { plat.color = MAROON; }
            else if (string_match(subtoken, tok_len, "green", 5)) { plat.color = GREEN; }
            else if (string_match(subtoken, tok_len, "lime", 4)) { plat.color = LIME; }
            else if (string_match(subtoken, tok_len, "darkgreen", 9)) { plat.color = DARKGREEN; }
            else if (string_match(subtoken, tok_len, "skyblue", 7)) { plat.color = SKYBLUE; }
            else if (string_match(subtoken, tok_len, "blue", 4)) { plat.color = BLUE; }
            else if (string_match(subtoken, tok_len, "darkblue", 8)) { plat.color = DARKBLUE; }
            else if (string_match(subtoken, tok_len, "purple", 6)) { plat.color = PURPLE; }
            else if (string_match(subtoken, tok_len, "violet", 6)) { plat.color = VIOLET; }
            else if (string_match(subtoken, tok_len, "darkpurple", 10)) { plat.color = DARKPURPLE; }
            else if (string_match(subtoken, tok_len, "beige", 5)) { plat.color = BEIGE; }
            else if (string_match(subtoken, tok_len, "brown", 5)) { plat.color = BROWN; }
            else if (string_match(subtoken, tok_len, "darkbrown", 9)) { plat.color = DARKBROWN; }
            else if (string_match(subtoken, tok_len, "white", 5)) { plat.color = WHITE; }
            else if (string_match(subtoken, tok_len, "black", 5)) { plat.color = BLACK; }
            else if (string_match(subtoken, tok_len, "blank", 5)) { plat.color = BLANK; }
            else if (string_match(subtoken, tok_len, "magenta", 7)) { plat.color = MAGENTA; }
            else { fprintf(stderr, "WARN: unrecognized color %s\n", subtoken); }
          }; break;
          default: fprintf(stderr, "WARN: invalid directive on line %lu (%s)\n", line_number, token);
        }
        // if (errno != 0) { fprintf(stderr, "Error: invalid number parsed on line %lu (defaulting to 0.0)\n", line_number);}
        token = strtok(NULL, ";"); // get next token
      }
      List_Platform_push(&self.platforms, plat);
    }
    // parse a home point declaration
    else if (string_starts(line_buffer, buffer_len, "Home:", 5)) {
      char *token = strtok(line_buffer + 5, ";");
      while (token != NULL) {
        if (token[0] == '\n') { break; }
        int32_t value = parse_int(token+1, strlen(token+1));
        switch (token[0]) {
          case 'x': self.start_position.x = value; break;
          case 'y': self.start_position.y = -value; break;
          default: fprintf(stderr, "WARN: unrecognized directive on line %lu\n", line_number);
        }
        token = strtok(NULL, ";");
      }
    }
    else if (string_starts(line_buffer, buffer_len, "Background:", 11)) {
      char *token = strtok(line_buffer + 11, ";");
      while (token != NULL) {
        if (token[0] == '\n') { break; }
        int32_t value = parse_int(token+1, strlen(token+1));
        switch(token[0]) {
          case 'r': self.background_color.r = value; break;
          case 'g': self.background_color.g = value; break;
          case 'b': self.background_color.b = value; break;
          case 'a': self.background_color.a = value; break;
          default: fprintf(stderr, "WARN: unrecognized directive on line %lu\n", line_number);
        }
        token = strtok(NULL, ";");
      }
    }
    else if (string_starts(line_buffer, buffer_len, "Gravity:", 8)) {
      float gravity = -1.0;
      sscanf(line_buffer, "Gravity:%f;", &gravity);
      if (gravity < 0.0) {
        fprintf(stderr, "WARN: failed to parse player gravity on line %lu\n", line_number);
        continue;
      }
      self.player_gravity = gravity;
    }
    else if (string_starts(line_buffer, buffer_len, "Acceleration:", 13)) {
      float acceleration = -1.0;
      sscanf(line_buffer, "Acceleration:%f;", &acceleration);
      if (acceleration < 0.0) {
        fprintf(stderr, "WARN: failed to parse player acceleration on line %lu\n", line_number);
        continue;
      }
      self.player_acceleration = acceleration;
    }
    else if (string_starts(line_buffer, buffer_len, "MoveSpeed:", 10)) {
      float max_move_speed = -1.0;
      sscanf(line_buffer, "MoveSpeed:%f;", &max_move_speed);
      if (max_move_speed < 0.0) {
        fprintf(stderr, "WARN: failed to parse player max run speed on line %lu\n", line_number);
        continue;
      }
      self.player_max_speed = max_move_speed;
    }
    else if (string_starts(line_buffer, buffer_len, "Jump:", 5)) {
      float jump_velocity = -1.0;
      sscanf(line_buffer, "Jump:%f;", &jump_velocity);
      if (jump_velocity < 0.0) {
        fprintf(stderr, "WARN: failed to parse player jump velocity on line %lu\n", line_number);
        continue;
      }
      self.player_jump_velocity = jump_velocity;
    }
    else if (string_starts(line_buffer, buffer_len, "Abyss:", 6)) {
      float abyss_height = NAN;
      sscanf(line_buffer, "Abyss:%f;", &abyss_height);
      if (abyss_height == NAN) {
        fprintf(stderr, "WARN: failed to parse abyss height on line %lu\n", line_number);
        continue;
      }
      self.abyss_height = -abyss_height;
    }
    else {
      fprintf(stderr, "Error: syntax error on line %lu, line must start with a directive (skipping this line)\n", line_number);
      fprintf(stderr, "INFO: this line contains an error -> |%s|\n", line_buffer);
    }
  }

  return self;
}


