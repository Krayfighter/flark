
#include "stdio.h"

#include "pt_error.h"

#include "level.h"

const uint8_t LEVEL_V_0_0 = 0x00;

const uint8_t LEVEL_VERSION_CURRENT = LEVEL_V_0_0;

define_List(Rect)

char *level_filename = "main.bin";

void Level_free(Level self) {
  List_Rect_free(&self.normal_block);
  List_Rect_free(&self.spring);
  List_Rect_free(&self.slime);
  List_Rect_free(&self.kill_block);
}

Level Level_new() {
  return (Level){
    .normal_block = List_Rect_new(8),
    .spring = List_Rect_new(8),
    .slime = List_Rect_new(8),
    .kill_block = List_Rect_new(8),
    .home = (Vec2){ 0.0, 0.0 },
    .abyss = 1000.0,
    .version = LEVEL_VERSION_CURRENT
  };
}

bool Level_save_to_file(Level *self, const char *filename) {
  FILE *save_file = fopen(filename, "w");
  if (save_file == NULL) { return false; }

  fwrite(&self->version, 1, 1, save_file);

  fwrite(&self->normal_block.item_count, sizeof(uint32_t), 1, save_file);
  fwrite(self->normal_block.items, sizeof(Rect), self->normal_block.item_count, save_file);

  fwrite(&self->spring.item_count, sizeof(uint32_t), 1, save_file);
  fwrite(self->spring.items, sizeof(Rect), self->spring.item_count, save_file);

  fwrite(&self->slime.item_count, sizeof(uint32_t), 1, save_file);
  fwrite(self->slime.items, sizeof(Rect), self->slime.item_count, save_file);

  fwrite(&self->kill_block.item_count, sizeof(uint32_t), 1, save_file);
  fwrite(self->kill_block.items, sizeof(Rect), self->kill_block.item_count, save_file);

  fwrite(&self->home, sizeof(Vec2), 1, save_file);

  fwrite(&self->abyss, sizeof(float), 1, save_file);

  fclose(save_file);

  return true;
}


#define load_list(list_name, size_var, item_tmp, stream) \
fread(&size_var, sizeof(size_var), 1, stream); \
for(uint32_t i = 0; i < item_count; i += 1) { \
  fread(&item_tmp, sizeof(item_tmp), 1, stream); \
  List_Rect_push(&list_name, item_tmp); \
}

void load_level_v_0_0(Level *level, FILE *source) {
  uint32_t item_count;
  Rect current_rect;

  load_list(level->normal_block, item_count, current_rect, source);
  load_list(level->spring, item_count, current_rect, source);
  load_list(level->slime, item_count, current_rect, source);
  load_list(level->kill_block, item_count, current_rect, source);

  fread(&level->home, sizeof(Vec2), 1, source);
  fread(&level->abyss, sizeof(float), 1, source);
}

Level Level_load_from_file(const char *filename) {
  FILE *level_file = fopen(filename, "r");
  if (level_file == NULL) { return (Level){ 0 }; }

  Level self = Level_new();

  uint8_t version;
  fread(&version, 1, 1, level_file);

  switch (version) {
    case 0x00: { load_level_v_0_0(&self, level_file); }; break;
    default: { panic("Unhandled Level Version", EXIT_FAILURE); };
  }


  return self;
}

