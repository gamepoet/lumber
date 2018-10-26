#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lumber.h"

// the configuration used to initialize this library
static lumber_config_t s_config;

// the default level when a category doesn't have a specific override
static lumber_level_t s_default_level;

// category-specific configuration. NOTE: this implementation requires a linear search through config names (and also
// doing string compares on each one). if this shows up in a profiler, we should first hash the names to avoid the
// string compares, and then look into a proper hashtable
static int s_category_config_capacity;
static int s_category_config_count;
static const char** s_category_config_names;
static lumber_level_t* s_category_config_levels;

static _Thread_local char s_format_buf[2048];

static const char* get_level_name(lumber_level_t level) {
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

static void default_log_handler(const lumber_category_t* category,
                                lumber_level_t level,
                                time_t timestamp,
                                const char* msg) {
  // format the time
  struct tm time_local;
  localtime_r(&timestamp, &time_local);
  char time_buf[sizeof("2001-01-01T01:01:01+100") + 1];
  strftime(time_buf, sizeof(time_buf), "%FT%T%z", &time_local);
  time_buf[sizeof(time_buf) - 1] = 0;

  printf("%s %s [%s] %s\n", time_buf, get_level_name(level), category->name, msg);
}

static void default_assert_handler(const char* file, int line, const char* func, const char* expression, const char* message) {
  fprintf(stderr, "ASSERT FAILURE: %s\n%s\nfile: %s\nline: %d\nfunc: %s\n", expression, message, file, line, func);
  exit(EXIT_FAILURE);
}

static void* default_alloc_handler(size_t size, const char* file, int line, const char* func) {
  return malloc(size);
}

static void default_free_handler(void* ptr, const char* file, int line, const char* func) {
  free(ptr);
}

#define lumber_assert(expr, message) ((expr) ? true : (lumber_assert_ex(__FILE__, __LINE__, __func__, #expr, message), false))

static void lumber_assert_ex(const char* file, int line, const char* func, const char* expression, const char* message) {
  s_config.assert_handler(file, line, func, expression, message);
}

static void* lumber_realloc(void* ptr, size_t size, const char* file, int line, const char* func) {
  void* new_ptr = s_config.alloc_handler(size, file, line, func);
  lumber_assert(new_ptr != NULL, "allocation failed");
  if (ptr != NULL) {
    memcpy(new_ptr, ptr, size);
    s_config.free_handler(ptr, __FILE__, __LINE__, __func__);
  }
  return new_ptr;
}

static void category_config_reserve(int new_capacity) {
  s_category_config_names = (const char**)lumber_realloc(
      s_category_config_names, new_capacity * sizeof(const char*), __FILE__, __LINE__, __func__);
  s_category_config_levels = (lumber_level_t*)lumber_realloc(
      s_category_config_levels, new_capacity * sizeof(lumber_level_t), __FILE__, __LINE__, __func__);
  s_category_config_capacity = new_capacity;
}

static lumber_level_t resolve_enabled_level(const lumber_category_t* category) {
  for (int index = 0; index < s_category_config_count; ++index) {
    if (0 == strcmp(s_category_config_names[index], category->name)) {
      return s_category_config_levels[index];
    }
  }

  return s_default_level;
}

void lumber_config_init(lumber_config_t* config) {
  if (config == NULL) {
    return;
  }

  config->log_handler = &default_log_handler;
  config->assert_handler = &default_assert_handler;
  config->alloc_handler = &default_alloc_handler;
  config->free_handler = &default_free_handler;
}

void lumber_lib_init(const lumber_config_t* config) {
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

void lumber_lib_shutdown() {
  if (s_category_config_capacity > 0) {
    s_config.free_handler(s_category_config_levels, __FILE__, __LINE__, __func__);
    s_config.free_handler(s_category_config_names, __FILE__, __LINE__, __func__);
    s_category_config_capacity = 0;
    s_category_config_count = 0;
    s_category_config_names = NULL;
    s_category_config_levels = NULL;
  }
}

void lumber_set_default_level(lumber_level_t level) {
  s_default_level = level;
}

void lumber_set_level(const lumber_category_t* category, lumber_level_t level) {
  lumber_assert(category, "category cannot be null");
  if (s_category_config_count >= s_category_config_capacity) {
    category_config_reserve(s_category_config_capacity + 128);
  }
  s_category_config_names[s_category_config_count] = category->name;
  s_category_config_levels[s_category_config_count] = level;
  ++s_category_config_count;
}

void lumber_log(const lumber_category_t* category, lumber_level_t level, const char* msg) {
  lumber_assert(category, "category cannot be null");
  if (s_config.log_handler != NULL) {
    // get the currently configured log level for the category
    const lumber_level_t enabled_level = resolve_enabled_level(category);
    if (level <= enabled_level) {
      time_t now = time(NULL);
      s_config.log_handler(category, level, now, msg);
    }
  }
}

void lumber_logf(const lumber_category_t* category, lumber_level_t level, const char* format, ...) {
  lumber_assert(category, "category cannot be null");
  if (s_config.log_handler != NULL) {
    // get the currently configured log level for the category
    const lumber_level_t enabled_level = resolve_enabled_level(category);
    if (level <= enabled_level) {
      // format the message
      va_list args;
      va_start(args, format);
      vsnprintf(s_format_buf, sizeof(s_format_buf), format, args);
      va_end(args);

      time_t now = time(NULL);
      s_config.log_handler(category, level, now, s_format_buf);
    }
  }
}
