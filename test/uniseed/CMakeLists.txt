add_executable(uniseed uniseed.cpp)

find_package(doctest CONFIG REQUIRED)
find_path(BOOST_UNORDERED_SET_INCLUDE_DIRS "boost/unordered_set.hpp" REQUIRED)

target_include_directories(bench_sets PRIVATE ${BOOST_UNORDERED_SET_INCLUDE_DIRS})

target_link_libraries(uniseed PRIVATE
  doctest::doctest
  omnichroma
)

add_test(NAME uniseed COMMAND uniseed)