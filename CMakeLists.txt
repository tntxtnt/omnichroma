cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(omnichroma CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
  src/main.cpp
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
target_include_directories(${PROJECT_NAME} PRIVATE ${BSHOSHANY_THREAD_POOL_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE
  Threads::Threads
  fmt::fmt
  lodepng
  Boost::boost Boost::random
)

add_subdirectory(test/uniseed)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
