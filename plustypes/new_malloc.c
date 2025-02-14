
// #include "new_malloc.h"

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "pthread.h"
#include "string.h"
#include "stdbool.h"

#include "old_malloc.h"

// typedef struct {
//   void *base;
//   uint32_t size;
// } MemoryPool;

void *base = NULL;
uint32_t size = 0;
uint32_t length = 0;
// stack grows down from the top
uint32_t records_end = 0;
// uint32_t 
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

// void *new_malloc(size_t bytes) {
//   // todo
//   return NULL;
// }
typedef struct {
  uint32_t item_index;
  uint32_t item_length;
} Record;

void push_record(Record item) {
  records_end -= sizeof(Record);
  void *top_record = base + records_end;
  *(Record *)top_record = item;
}

// // TODO maybe there is a self-modifying solution so that
// // this check is run only once
// void *malloc(size_t bytes) {
//   const uint32_t DEFAULT_SIZE = 4 * 4096;
//   pthread_mutex_lock(&pool_mutex);
//   if (base == NULL) {
//     base = aligned_alloc(4096, DEFAULT_SIZE);
//     size = DEFAULT_SIZE;
//     length = size;
//     records_end = size;
//     // records_end = size - sizeof(Record);
//     memset(base, 0x0, size);
//   }
//   bool space_is_free = false;
//   uint32_t search_start = 0;
//   for (uint32_t i = records_end; i < size; i += sizeof(Record)) {
    
//   }
//   pthread_mutex_unlock(&pool_mutex);
// }


// static void *(*malloc_ptr)(size_t bytes) = __real_malloc;

void *__wrap_malloc(size_t bytes) {
  fprintf(stderr, "DBG: wrapped malloc call - %lu bytes", bytes);
  // return malloc_ptr(bytes);
  return __real_malloc(bytes);
}

typedef struct {
  void *heap;
  uint32_t capacity;
  uint32_t filled;
} LinearAllocator;

LinearAllocator LinearAllocator_new(uint32_t size) {
  
}


