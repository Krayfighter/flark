
#include "old_malloc.h"

void *__real_malloc(size_t bytes) { return malloc(bytes); }
// void *malloc(size_t bytes) { return __real_malloc(bytes); }

