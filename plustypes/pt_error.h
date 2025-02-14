

#ifndef PT_ERROR_H
#define PT_ERROR_H

#define panic(err_msg, exit_code) \
fprintf(stderr, "Error: %s, in file %s on line %i\n", err_msg, __FILE__, __LINE__); \
exit(exit_code);

#define expect_errno(err_msg) \
if (errno != 0) { \
  fprintf(stderr, "Error: %s -> %s (occured in file %s, on line %i)\n", err_msg, strerror(errno), __FILE__, __LINE__); \
  exit(-1); \
}

#define expect(expr, err_msg) \
if (!expr) { \
  fprintf(stderr, "Error: %s, (occured in file %s, on line %i)\n", err_msg, __FILE__, __LINE__); \
  if (errno != 0) { \
    fprintf(stderr, "NOTE: non-zero errno at time of exception -> %s - %u", strerror(errno), errno); \
  } \
  exit(-1); \
}

// print a message to stderr, but only if DEBUG_BUILD is defined
// this may be set as a compile flag -DDEBUG_BUILD (preferred)
// but could be #define'ed in a file if that is useful
#ifdef DEBUG_BUILD
#define debug(message, ...) \
fprintf(stderr, message, __VA_ARGS__)
#else
#define debug(message, ...)
#endif

// fprintf(stderr, "Error: %s, in file %s on line %s\n")

#endif


