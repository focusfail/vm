cmake_minimum_required(VERSION 3.25)

project(
  vm
  LANGUAGES C
  DESCRIPTION "small bytecode vm"
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(vm)

file(GLOB_RECURSE sources "src/*.c")
target_sources(
  vm
  PRIVATE
  ${sources}
)

target_include_directories(
  vm
  PRIVATE
  "src/"
)

target_link_libraries(
  vm
  PRIVATE
)

target_compile_options(
  vm
  PRIVATE
  -ggdb
  -Wall
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(vm PRIVATE -fsanitize=address,undefined,leak -ggdb -Wall -fno-omit-frame-pointer)
    target_link_options(vm PRIVATE -fsanitize=address,undefined,leak)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(vm PRIVATE -ggdb -02)
    target_link_options(vm PRIVATE)
endif()
