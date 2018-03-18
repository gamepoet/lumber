#include "lumber.h"
#include <stdio.h>

// the configuration used to initialize this library
static struct lumber_config_t s_config;

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

void lumber_config_init(struct lumber_config_t* config) {
  if (config == NULL) {
    return;
  }

  config->log_handler = &default_log_handler;
}

void lumber_init(const struct lumber_config_t* config) {
  if (config != NULL) {
    s_config = *config;
  }
  else {
    lumber_config_init(&s_config);
  }
}

void lumber_shutdown() {
}

void lumber_set_level(const struct lumber_category_t* category, enum lumber_level_t level) {
  printf("lumber_set_level: NOT IMPLEMENTED!\n");
}

void lumber_log(const struct lumber_category_t* category, enum lumber_level_t level, const char* msg) {
  if (s_config.log_handler != NULL)
  {
    s_config.log_handler(category, level, msg);
  }
}
