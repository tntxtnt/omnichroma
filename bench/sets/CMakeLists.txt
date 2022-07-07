add_executable(bench_sets sets.cpp)

find_package(tsl-hopscotch-map CONFIG REQUIRED)
find_package(tsl-ordered-map CONFIG REQUIRED)
find_package(tsl-sparse-map CONFIG REQUIRED)
find_package(tsl-robin-map CONFIG REQUIRED)
find_path(BOOST_UNORDERED_SET_INCLUDE_DIRS "boost/unordered_set.hpp" REQUIRED)

target_include_directories(bench_sets PRIVATE ${BOOST_UNORDERED_SET_INCLUDE_DIRS})

target_link_libraries(bench_sets PRIVATE
  omnichroma
  tsl::hopscotch_map
  tsl::ordered_map
  tsl::sparse_map
  tsl::robin_map
)