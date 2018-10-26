#include "catch.hpp"
#include "lumber.h"

using Catch::Equals;

static std::function<void(const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg)> s_log_handler;
static bool s_log_handler_called;

static void test_log_handler(const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg) {
  s_log_handler_called = true;
  if (s_log_handler) {
    s_log_handler(category, level, timestamp, msg);
  }
}

// init/shutdown helper if an exception gets thrown
struct init_t {
  init_t(const lumber_config_t* config) {
    s_log_handler = nullptr;

    lumber_config_t config_default;
    if (!config) {
      lumber_config_init(&config_default);
      config_default.log_handler = &test_log_handler;
      config = &config_default;
    }
    lumber_lib_init(config);
    lumber_set_default_level(LUMBER_DEBUG);
  }
  ~init_t() {
    lumber_lib_shutdown();
  }
};

static void
with_handler(std::function<void(const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg)> handler,
             std::function<void()> block) {
  s_log_handler_called = false;
  s_log_handler = handler;
  block();
  REQUIRE(s_log_handler_called);
  s_log_handler = nullptr;
}

static void with_handler_not_called(
    std::function<void(const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg)> handler,
    std::function<void()> block) {
  s_log_handler_called = false;
  s_log_handler = handler;
  block();
  REQUIRE(!s_log_handler_called);
  s_log_handler = nullptr;
}

TEST_CASE("basic logging") {
  init_t init(nullptr);
  lumber_category_t cats{"cats"};

  SECTION("it logs at level debug") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_DEBUG);
      CHECK_THAT(msg, Equals("sleep"));
    };
    with_handler(handler, [&]() { lumber_debug(&cats, "sleep"); });
  }

  SECTION("it logs at level info") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_INFO);
      CHECK_THAT(msg, Equals("stretch"));
    };
    with_handler(handler, [&]() { lumber_info(&cats, "stretch"); });
  }

  SECTION("it logs at level warning") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_WARNING);
      CHECK_THAT(msg, Equals("hungry"));
    };
    with_handler(handler, [&]() { lumber_warning(&cats, "hungry"); });
  }

  SECTION("it logs at level error") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_ERROR);
      CHECK_THAT(msg, Equals("hunting"));
    };
    with_handler(handler, [&]() { lumber_error(&cats, "hunting"); });
  }
}

TEST_CASE("overriding the default level") {
  init_t init(nullptr);
  lumber_category_t cats{"cats"};

  SECTION("it won't log when the level is below the default") {
    lumber_set_default_level(LUMBER_ERROR);
    with_handler_not_called(nullptr, [&]() { lumber_info(&cats, "stretch"); });
  }

  SECTION("it logs when the level equal") {
    lumber_set_default_level(LUMBER_INFO);
    with_handler(nullptr, [&]() { lumber_info(&cats, "stretch"); });
  }
}

TEST_CASE("set specific category levels") {
  init_t init(nullptr);
  lumber_category_t cats{"cats"};
  lumber_category_t dogs{"dogs"};

  SECTION("it doesn't log when the category level is set too high") {
    lumber_set_level(&dogs, LUMBER_ERROR);
    with_handler(nullptr, [&]() { lumber_info(&cats, "stretch"); });
    with_handler_not_called(nullptr, [&]() { lumber_info(&dogs, "stretch"); });
  }

  SECTION("it logs when the level is equal") {
    lumber_set_level(&dogs, LUMBER_ERROR);
    with_handler(nullptr, [&]() { lumber_info(&cats, "stretch"); });
    with_handler(nullptr, [&]() { lumber_error(&dogs, "stretch"); });
  }
}

TEST_CASE("formating logs") {
  init_t init(nullptr);
  lumber_category_t cats{"cats"};

  SECTION("it won't format if only a single string is given") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_ERROR);
      CHECK_THAT(msg, Equals("hunting %s"));
    };
    with_handler(handler, [&]() { lumber_error(&cats, "hunting %s"); });
  }

  SECTION("it will format if multiple args are given") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, time_t timestamp, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_ERROR);
      CHECK_THAT(msg, Equals("hunting 2 mice"));
    };
    with_handler(handler, [&]() { lumber_error(&cats, "hunting %d %s", 2, "mice"); });
  }
}
