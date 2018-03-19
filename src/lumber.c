#include "lumber.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// the configuration used to initialize this library
static struct lumber_config_t s_config;

// the default level when a category doesn't have a specific override
static enum lumber_level_t s_default_level;

// category-specific configuration. NOTE: this implementation requires a linear search through config names (and also
// doing string compares on each one). if this shows up in a profiler, we should first hash the names to avoid the
// string compares, and then look into a proper hashtable
static int s_category_config_capacity;
static int s_category_config_count;
static const char** s_category_config_names;
static enum lumber_level_t* s_category_config_levels;

static const char* get_level_name(enum lumber_level_t level) {
  switch (level) {
    case LUMBER_ERROR:
      return "ERRO";
    case LUMBER_WARNING:
      return "WARN";
    case LUMBER_INFO:
      return "INFO";
    case LUMBER_DEBUG:
      return "DBUG";
    default:
      return "UNKN";
  }
}

static void default_log_handler(const struct lumber_category_t* category, enum lumber_level_t level, const char* msg) {
  printf("[%s] %s %s\n", category->name, get_level_name(level), msg);
}

static void* default_alloc(size_t size, const char* file, int line, const char* func) {
  return malloc(size);
}

static void default_free(void* ptr, const char* file, int line, const char* func) {
  free(ptr);
}

#define lumber_assert(expr, message)                                                                                   \
  ((expr) ? true : (lumber_assert_ex(__FILE__, __LINE__, __func__, #expr, message), false))

static void
lumber_assert_ex(const char* file, int line, const char* func, const char* expression, const char* message) {
  s_config.assert_handler(file, line, func, expression, message);
}

static void* lumber_realloc(void* ptr, size_t size, const char* file, int line, const char* func) {
  void* new_ptr = s_config.alloc(size, file, line, func);
  lumber_assert(new_ptr != NULL, "allocation failed");
  if (ptr != NULL) {
    memcpy(new_ptr, ptr, size);
    s_config.free(ptr, __FILE__, __LINE__, __func__);
  }
  return new_ptr;
}

static void category_config_reserve(int new_capacity) {
  s_category_config_names = (const char**)lumber_realloc(
      s_category_config_names, new_capacity * sizeof(const char*), __FILE__, __LINE__, __func__);
  s_category_config_levels = (enum lumber_level_t*)lumber_realloc(
      s_category_config_levels, new_capacity * sizeof(enum lumber_level_t), __FILE__, __LINE__, __func__);
  s_category_config_capacity = new_capacity;
}

void lumber_config_init(struct lumber_config_t* config) {
  if (config == NULL) {
    return;
  }

  config->log_handler = &default_log_handler;
  config->alloc = &default_alloc;
  config->free = &default_free;
}

void lumber_init(const struct lumber_config_t* config) {
  if (config != NULL) {
    s_config = *config;
  }
  else {
    lumber_config_init(&s_config);
  }

  s_default_level = LUMBER_INFO;

  s_category_config_count = 0;
  s_category_config_capacity = 0;
  s_category_config_names = NULL;
  s_category_config_levels = NULL;
  category_config_reserve(128);
}

void lumber_shutdown() {
  if (s_category_config_capacity > 0) {
    s_config.free(s_category_config_levels, __FILE__, __LINE__, __func__);
    s_config.free(s_category_config_names, __FILE__, __LINE__, __func__);
    s_category_config_capacity = 0;
    s_category_config_count = 0;
    s_category_config_names = NULL;
    s_category_config_levels = NULL;
  }
}

void lumber_set_default_level(enum lumber_level_t level) {
  s_default_level = level;
}

void lumber_set_level(const struct lumber_category_t* category, enum lumber_level_t level) {
  if (s_category_config_count >= s_category_config_capacity) {
    category_config_reserve(s_category_config_capacity + 128);
  }
  s_category_config_names[s_category_config_count] = category->name;
  s_category_config_levels[s_category_config_count] = level;
  ++s_category_config_count;
}

void lumber_log(const struct lumber_category_t* category, enum lumber_level_t level, const char* msg) {
  if (s_config.log_handler != NULL) {
    // get the currently configured log level for the category
    enum lumber_level_t enabled_level = s_default_level;
    for (int index = 0; index < s_category_config_count; ++index) {
      if (0 == strcmp(s_category_config_names[index], category->name)) {
        enabled_level = s_category_config_levels[index];
        break;
      }
    }

    if (level <= enabled_level) {
      s_config.log_handler(category, level, msg);
    }
  }
}
