cmake_minimum_required(VERSION 3.8)
project(lumber)

set(
  SRCS
  src/lumber.h
  src/lumber.c
)

# TODO: max out the warning settings for the compilers (why isn't there a generic way to do this?)
#if(MSVC)
#  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
#endif()

add_library(lumber STATIC ${SRCS})
target_compile_features(lumber PRIVATE cxx_std_11 cxx_thread_local)
target_compile_features(lumber PUBLIC cxx_variadic_macros)
target_include_directories(
  lumber
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:>
)

set(
  TEST_SRCS
  spec/catch.hpp
  spec/main.cpp
  spec/lumber_spec.cpp
)

add_executable(test_runner ${TEST_SRCS})
target_compile_features(test_runner PRIVATE cxx_std_11)
target_link_libraries(test_runner lumber)

enable_testing()
add_test(NAME spec COMMAND test_runner)
