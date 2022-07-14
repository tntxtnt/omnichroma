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

find_package(fmt CONFIG REQUIRED)
# saveToPng
find_package(lodepng CONFIG REQUIRED)          
# boost::random::uniform_int_distribution
find_package(Boost REQUIRED COMPONENTS random)
# Thread pool
find_package(Threads REQUIRED)
find_path(BSHOSHANY_THREAD_POOL_INCLUDE_DIRS "BS_thread_pool.hpp" REQUIRED)

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
find_path(BOOST_UNORDERED_SET_INCLUDE_DIRS "boost/unordered_set.hpp" REQUIRED)
find_package(CLI11 CONFIG REQUIRED)
target_include_directories(main PRIVATE ${BOOST_UNORDERED_SET_INCLUDE_DIRS})
target_link_libraries(main
  PRIVATE
    omnichroma
    CLI11::CLI11
)
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT main)

add_subdirectory(bench/sets)

enable_testing()
add_subdirectory(test/uniseed)

