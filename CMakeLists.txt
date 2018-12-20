cmake_minimum_required(VERSION 3.11)
project(lumber LANGUAGES C CXX)

option(LUMBER_BUILD_TESTS "Build tests" OFF)

# max out the warning settings for the compilers (why isn't there a generic way to do this?)
if (MSVC)
  string(REPLACE "/W3" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  string(REPLACE "/W3" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif()

add_library(
  lumber
  STATIC
  src/lumber.h
  src/lumber.c
)
target_compile_features(
  lumber
  PRIVATE
  cxx_std_11
  cxx_thread_local
  PUBLIC
  cxx_variadic_macros
)
target_include_directories(
  lumber
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:>
)
target_compile_options(
  lumber
  PRIVATE
  $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-gnu-zero-variadic-macro-arguments>
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100>
)

if (LUMBER_BUILD_TESTS)
  include(FetchContent)
  set(FETCHCONTENT_QUIET OFF)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v2.5.0
  )
  FetchContent_GetProperties(catch2)
  if (NOT catch2_POPULATED)
    FetchContent_Populate(catch2)
  endif()

  # test app
  add_executable(
    test_runner
    spec/main.cpp
    spec/lumber_spec.cpp
  )
  target_include_directories(test_runner PRIVATE ${catch2_SOURCE_DIR}/single_include/catch2)
  target_compile_features(test_runner PRIVATE cxx_std_11)
  target_link_libraries(test_runner lumber)
  target_compile_options(
    lumber
    PRIVATE
    $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-gnu-zero-variadic-macro-arguments>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4100>
  )

  # test suite
  enable_testing()
  add_test(NAME spec COMMAND test_runner)
endif()
