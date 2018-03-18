#include "catch.hpp"
#include "lumber.h"

using namespace Catch;

static std::function<void(const lumber_category_t* category, lumber_level_t level, const char* msg)> s_log_handler;
static bool s_log_handler_called;

static void test_log_handler(const lumber_category_t* category, lumber_level_t level, const char* msg) {
  s_log_handler_called = true;
  if (s_log_handler) {
    s_log_handler(category, level, msg);
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
    lumber_init(config);
  }
  ~init_t() {
    lumber_shutdown();
  }
};

static void
with_handler(std::function<void(const lumber_category_t* category, lumber_level_t level, const char* msg)> handler,
             std::function<void()> block) {
  s_log_handler_called = false;
  s_log_handler = handler;
  block();
  REQUIRE(s_log_handler_called);
  s_log_handler = nullptr;
}

TEST_CASE("basic logging") {
  init_t init(nullptr);
  lumber_category_t cats{"cats"};

  SECTION("it logs at level debug") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_DEBUG);
      CHECK_THAT(msg, Equals("sleep"));
    };
    with_handler(handler, [&]() { lumber_debug(&cats, "sleep"); });
  }

  SECTION("it logs at level info") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_INFO);
      CHECK_THAT(msg, Equals("stretch"));
    };
    with_handler(handler, [&]() { lumber_info(&cats, "stretch"); });
  }

  SECTION("it logs at level warning") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_WARNING);
      CHECK_THAT(msg, Equals("hungry"));
    };
    with_handler(handler, [&]() { lumber_warning(&cats, "hungry"); });
  }

  SECTION("it logs at level error") {
    auto handler = [&](const lumber_category_t* category, lumber_level_t level, const char* msg) {
      CHECK(category == &cats);
      CHECK(level == LUMBER_ERROR);
      CHECK_THAT(msg, Equals("hunting"));
    };
    with_handler(handler, [&]() { lumber_error(&cats, "hunting"); });
  }
}