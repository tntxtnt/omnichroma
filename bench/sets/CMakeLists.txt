add_executable(bench_sets sets.cpp)
find_package(tsl-hopscotch-map CONFIG REQUIRED)
find_package(tsl-ordered-map CONFIG REQUIRED)
find_package(tsl-sparse-map CONFIG REQUIRED)
find_package(robin-map CONFIG REQUIRED)
target_link_libraries(bench_sets PRIVATE
  omnichroma
  tsl::hopscotch_map
  tsl::ordered_map
  tsl::sparse_map
  tsl::robin_map
)