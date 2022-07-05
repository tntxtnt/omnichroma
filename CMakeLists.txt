cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(omnichroma CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(omnichroma STATIC
  src/omnichroma.h
  src/generate.h
  src/pixelsbuffer.h
  src/pixelsbuffer.cpp
  src/color.h
  src/color.cpp
  src/coord.h
  src/coord.cpp
  src/bitmap.h
  src/bitmap.cpp
)
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_path(BSHOSHANY_THREAD_POOL_INCLUDE_DIRS "BS_thread_pool.hpp")
find_package(lodepng CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS random)
target_include_directories(omnichroma
  PUBLIC
    ${BSHOSHANY_THREAD_POOL_INCLUDE_DIRS}
    src
)
target_link_libraries(omnichroma PUBLIC
  Threads::Threads
  fmt::fmt
  lodepng
  Boost::boost Boost::random
)

add_executable(main src/main.cpp)
target_link_libraries(main PRIVATE omnichroma)

enable_testing()
add_subdirectory(test/uniseed)
add_subdirectory(bench/sets)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT main)
