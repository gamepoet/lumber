#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum lumber_level_t {
  LUMBER_ERROR,
  LUMBER_WARNING,
  LUMBER_INFO,
  LUMBER_DEBUG,
};

struct lumber_category_t {
  const char* name;
};

struct lumber_config_t {
  void (*log_handler)(const struct lumber_category_t* category, enum lumber_level_t level, const char* msg);
  void (*assert_handler)(const char* file, int line, const char* func, const char* expression, const char* message);

  void* (*alloc)(size_t size, const char* file, int line, const char* func);
  void (*free)(void* ptr, const char* file, int line, const char* func);
};

// Initializes the given config struct to fill it in with the default values.
void lumber_config_init(struct lumber_config_t* config);

// Initializes this library.
void lumber_init(const struct lumber_config_t* config);

// Tears down this library and frees all allocations.
void lumber_shutdown();

// Sets the default logging level when a specific category level hasn't been specified.
void lumber_set_default_level(enum lumber_level_t level);

// Sets the logging level for the given category.
void lumber_set_level(const struct lumber_category_t* category, enum lumber_level_t level);

// Logs a message for a category and level.
void lumber_log(const struct lumber_category_t* category, enum lumber_level_t level, const char* msg);

#define lumber_error(category, msg) lumber_log(category, LUMBER_ERROR, msg)
#define lumber_warning(category, msg) lumber_log(category, LUMBER_WARNING, msg)
#define lumber_info(category, msg) lumber_log(category, LUMBER_INFO, msg)
#define lumber_debug(category, msg) lumber_log(category, LUMBER_DEBUG, msg)

#ifdef __cplusplus
}
#endif
