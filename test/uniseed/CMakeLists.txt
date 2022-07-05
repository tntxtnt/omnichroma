add_executable(uniseed uniseed.cpp)
find_package(doctest CONFIG REQUIRED)
target_link_libraries(uniseed PRIVATE
  doctest::doctest
  omnichroma
)
add_test(NAME uniseed COMMAND uniseed)