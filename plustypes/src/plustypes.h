
#include "stdint.h"
#include "stddef.h"
#include "stdlib.h"

#ifndef PTYPES_H
#define PTYPES_H

#define declare_List_struct(ItemT) \
typedef struct { \
  ItemT *items; \
  size_t item_count; \
  size_t buffer_size; \
} List_ ## ItemT;

#define declare_List_new(ItemT) \
List_ ## ItemT List_ ## ItemT ## _new(size_t initial_size);

#define define_List_new(ItemT) \
List_ ## ItemT List_ ## ItemT ## _new(size_t initial_size) { \
  ItemT *buffer = malloc(initial_size * sizeof(ItemT)); \
  return (List_ ## ItemT) { \
    .items = buffer, \
    .item_count = 0, \
    .buffer_size = initial_size, \
  }; \
}

#define declare_List_get(ItemT) \
ItemT *List_ ## ItemT ## _get(List_ ## ItemT *self, size_t index);

// returns a NULL pointer if index is out of bounds
#define define_List_get(ItemT) \
ItemT *List_ ## ItemT ## _get(List_ ## ItemT *self, size_t index) { \
  if (index >= self->item_count) { return NULL; } \
  return &self->items[index]; \
}


#define declare_List_free(ItemT) \
void List_ ## ItemT ## _free(List_ ## ItemT *self);

#define define_List_free(ItemT) \
void List_ ## ItemT ## _free(List_ ## ItemT *self) { \
  free(self->items); \
}


#define declare_List_push(ItemT) \
void List_ ## ItemT ## _push(List_ ## ItemT *self, ItemT item);

#define define_List_push(ItemT) \
void List_ ## ItemT ## _push(List_ ## ItemT *self, ItemT item) { \
  if (self->item_count == self->buffer_size) { \
    self->buffer_size *= 2; \
    self->items = realloc(self->items, self->buffer_size * sizeof(ItemT)); \
  } \
  self->items[self->item_count] = item; \
  self->item_count += 1; \
}


#define declare_List_pushall(ItemT) \
void List_ ## ItemT ## _pushall(List_ ## ItemT *self, List_ ## ItemT *other);

#define define_List_pushall(ItemT) \
void List_ ## ItemT ## _pushall(List_ ## ItemT *self, List_ ## ItemT *other) { \
  for (size_t i = 0; i < other->item_count; i += 1) { \
    List_ ## ItemT ## _push(self, other->items[i]); \
  } \
}


#define declare_List_UNINIT(ItemT) \
static const List_ ## ItemT List_ ## ItemT ## _UNINIT = { .items = NULL, .item_count = 0, .buffer_size = 0 };


// iterates over items in a list running CodeBlock each iteration
// with access to a pointer to the iteration item
#define List_foreach(ItemT, list, CodeBlock) \
for (size_t i = 0; i < list.item_count; i += 1) { \
  ItemT *item = &list.items[i]; \
  CodeBlock \
} \


// declare a dynamically allocated growable list of ItemT typed items
#define declare_List(ItemT) \
declare_List_struct(ItemT) \
declare_List_new(ItemT) \
declare_List_get(ItemT) \
declare_List_free(ItemT) \
declare_List_push(ItemT) \
declare_List_UNINIT(ItemT) \
declare_List_pushall(ItemT)

#define define_List(ItemT) \
define_List_new(ItemT) \
define_List_get(ItemT) \
define_List_free(ItemT) \
define_List_push(ItemT) \
define_List_pushall(ItemT)
#endif


