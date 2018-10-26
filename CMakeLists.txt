cmake_minimum_required(VERSION 3.8)
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
  # test app
  add_executable(
    test_runner
    spec/catch.hpp
    spec/main.cpp
    spec/lumber_spec.cpp
  )
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
