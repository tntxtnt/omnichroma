add_executable(uniseed
  uniseed.cpp
  ../../src/omnichroma.h
  ../../src/generate.h
  ../../src/pixelsbuffer.h
  ../../src/pixelsbuffer.cpp
  ../../src/color.h
  ../../src/color.cpp
  ../../src/coord.h
  ../../src/coord.cpp
  ../../src/bitmap.h
  ../../src/bitmap.cpp
)
target_include_directories(uniseed PRIVATE ../../src)
find_package(doctest CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS random)
target_link_libraries(uniseed PRIVATE
  doctest::doctest
  Boost::boost Boost::random
  Threads::Threads
  fmt::fmt
  lodepng
)
add_test(NAME uniseed COMMAND uniseed)