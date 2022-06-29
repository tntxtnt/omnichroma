cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(omnichroma CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/bitmap.h
  src/bitmap.cpp
  src/omnichroma.h
  src/omnichroma.cpp
  src/color.h
  src/color.cpp
  src/coord.h
  src/coord.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt)
find_path(BSHOSHANY_THREAD_POOL_INCLUDE_DIRS "BS_thread_pool.hpp")
target_include_directories(${PROJECT_NAME} PRIVATE ${BSHOSHANY_THREAD_POOL_INCLUDE_DIRS})
find_package(lodepng CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE lodepng)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
