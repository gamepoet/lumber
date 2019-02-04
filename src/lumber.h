#pragma once
#include <stddef.h>
#include <stdint.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum lumber_level_t {
  LUMBER_ERROR,
  LUMBER_WARNING,
  LUMBER_INFO,
  LUMBER_DEBUG,
} lumber_level_t;

typedef struct lumber_category_t {
  const char* name;
} lumber_category_t;

typedef void (*lumber_log_handler_t)(const lumber_category_t* category,
                                     lumber_level_t level,
                                     time_t timestamp,
                                     const char* msg);
typedef void (*lumber_assert_handler_t)(const char* file, int line, const char* func, const char* expression, const char* message);
typedef void* (*lumber_alloc_handler_t)(size_t size, void* user_data, const char* file, int line, const char* func);
typedef void (*lumber_free_handler_t)(void* ptr, void* user_data, const char* file, int line, const char* func);

typedef struct lumber_config_t {
  // The handler to use when a log message would be printed.
  lumber_log_handler_t log_handler;

  // The handler to use when an assertion fails.
  lumber_assert_handler_t assert_handler;

  // The handler to use when allocating memory.
  lumber_alloc_handler_t alloc_handler;

  // The handler to use when freeing memory.
  lumber_free_handler_t free_handler;

  // Opaque user data provided te tho alloc and free handlers
  void* alloc_user_data;
} lumber_config_t;

// Initializes the given config struct to fill it in with the default values.
void lumber_config_init(lumber_config_t* config);

// Initializes this library.
void lumber_lib_init(const lumber_config_t* config);

// Tears down this library and frees all allocations.
void lumber_lib_shutdown();

// Sets the default logging level when a specific category level hasn't been specified.
void lumber_set_default_level(lumber_level_t level);

// Sets the logging level for the given category.
void lumber_set_level(const lumber_category_t* category, lumber_level_t level);

// Gets the logging level for the given category.
lumber_level_t lumber_get_level(const lumber_category_t* category);

// Resets the logging level for all categories back to their defaults.
void lumber_reset_levels();

// Logs a message for a category and level.
void lumber_log(const lumber_category_t* category, lumber_level_t level, const char* msg);

// Logs a formatted message for a category and level.
void lumber_logf(const lumber_category_t* category, lumber_level_t level, const char* format, ...);

#define LUMBER_JOIN_IMPL(a, b) a##b
#define LUMBER_JOIN(a, b) LUMBER_JOIN_IMPL(a, b)
#define LUMBER_VA_ARGC_IMPL2(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, N, ...) N
#define LUMBER_VA_ARGC_IMPL(args) LUMBER_VA_ARGC_IMPL2 args
#define LUMBER_VA_ARGC(...) LUMBER_VA_ARGC_IMPL((__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))
#define LUMBER_ARG_TEST_EVAL_1(one_arg, many_args) one_arg
#define LUMBER_ARG_TEST_EVAL_2(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_3(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_4(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_5(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_6(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_7(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_8(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_9(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_10(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_11(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_12(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_13(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_14(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_15(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_16(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_17(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_18(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_19(one_arg, many_args) many_args
#define LUMBER_ARG_TEST_EVAL_20(one_arg, many_args) many_args
#define LUMBER_ARG_TEST(argc, one_arg, many_args) \
  LUMBER_JOIN(LUMBER_ARG_TEST_EVAL_, argc)        \
  (one_arg, many_args)

// Logs a message at the error level.
#define lumber_error(category, ...)                                \
  LUMBER_ARG_TEST(LUMBER_VA_ARGC(__VA_ARGS__),                     \
                  lumber_log(category, LUMBER_ERROR, __VA_ARGS__), \
                  lumber_logf(category, LUMBER_ERROR, __VA_ARGS__))

// Logs a message at the warning level.
#define lumber_warning(category, ...)                                \
  LUMBER_ARG_TEST(LUMBER_VA_ARGC(__VA_ARGS__),                       \
                  lumber_log(category, LUMBER_WARNING, __VA_ARGS__), \
                  lumber_logf(category, LUMBER_WARNING, __VA_ARGS__))

// Logs a message at the info level.
#define lumber_info(category, ...)                                \
  LUMBER_ARG_TEST(LUMBER_VA_ARGC(__VA_ARGS__),                    \
                  lumber_log(category, LUMBER_INFO, __VA_ARGS__), \
                  lumber_logf(category, LUMBER_INFO, __VA_ARGS__))

// Logs a message at the debug level.
#define lumber_debug(category, ...)                                \
  LUMBER_ARG_TEST(LUMBER_VA_ARGC(__VA_ARGS__),                     \
                  lumber_log(category, LUMBER_DEBUG, __VA_ARGS__), \
                  lumber_logf(category, LUMBER_DEBUG, __VA_ARGS__))

#ifdef __cplusplus
}
#endif
