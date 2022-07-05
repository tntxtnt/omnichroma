# omnichroma

## What
https://codegolf.stackexchange.com/questions/22144/images-with-all-colors

My C++ implementation of fejesjoco's algorithm.

`main.cpp` generates all 6 image sizes, single thread and multithread (thread pool). Sample output (with `CoordSet = std::unordered_set<Coord>` (msvc)):

![sameple-output-images](img/sample-output-images.png)

![32x16](img/719662361-32x16-16xy8.png) ![32x16-mt](img/719662361-32x16-16xy8-mt.png)

![64x64](img/719662361-64x64-32xy32.png) ![64x64-mt](img/719662361-64x64-32xy32-mt.png)

![256x128](img/719662361-256x128-128xy64.png) ![256x128-mt](img/719662361-256x128-128xy64-mt.png)

Left are generated single-threaded-ly, right are generated multithreaded-ly (they are similar)

Bigger images (512x512 \~ 621 KB, 2048x1024 \~ 5047 KB, 4096x4096 \~ 37562 KB) are in `img` folder.

```
Seed = 719662361
Num threads = 16
32x16 single thread: repeat 100 times, total = 0.150843s, avg = 0.00150843s
32x16 threadpool   : repeat 100 times, total = 0.9971447s, avg = 0.009971447s
64x64 single thread: repeat 10 times, total = 0.5214949s, avg = 0.05214949s
64x64 threadpool   : repeat 10 times, total = 1.2743839s, avg = 0.12743838999999998s
256x128 single thread: repeat 1 time, total = 0.8621473s, avg = 0.8621473s
256x128 threadpool   : repeat 1 time, total = 1.346049s, avg = 1.346049s
512x512 single thread: repeat 1 time, total = 26.5719913s, avg = 26.5719913s
512x512 threadpool   : repeat 1 time, total = 13.577951s, avg = 13.577951s
2048x1024 single thread: repeat 1 time, total = 482.6002738s, avg = 482.6002738s
2048x1024 threadpool   : repeat 1 time, total = 172.1309171s, avg = 172.1309171s
4096x4096 threadpool   : repeat 1 time, total = 4140.7884682s, avg = 4140.7884682s
```

(my CPU is Ryzen 7 4800H)

## What's new
- v0.0.2: Produce reproducable images from `seed` (and starting point). Tested on Windows (msvc and msys2) and Linux (Ubuntu). **Note**: 32-bit version will produce different images with 64-bit version (32-bit version will fail `uniseed` test).
  - Use a shuffle function from cppreference (`cppref_shuffle`) to generate random colors.
  - Use identity function instead of `std::hash<int>{}()`. GCC uses identity function for hashing `int` while MSVC uses  hash.
  - Use `CoordSet = boost::unordered_set<Coord, std::hash<Coord>>`. MSVC/GCC have different `std::unordered_set` implementation. Using `boost::unordered_set` makes `generate()` runs about 30% slower than `std::unordered_set`. I also tried various `tsl` hash sets but nothing beats `std::unordered_set`.

## Benchmark

With identity function for `int` hash: (MSVC 64-bit)
```text
Num threads = 16
Benching image 256x128...
  std::unordered_set: repeat 100 times, total = 154.260918s, avg = 1.542609s
boost::unordered_set: repeat 100 times, total = 153.523095s, avg = 1.535231s
  tsl::hopscotch_set: repeat 100 times, total = 161.403601s, avg = 1.614036s
    tsl::ordered_set: repeat 100 times, total = 158.509282s, avg = 1.585093s
     tsl::sparse_set: repeat 100 times, total = 156.207256s, avg = 1.562073s
      tsl::robin_set: repeat 100 times, total = 162.986392s, avg = 1.629864s
Benching image 512x512...
  std::unordered_set: repeat 10 times, total = 150.168758s, avg = 15.016876s
boost::unordered_set: repeat 10 times, total = 189.231126s, avg = 18.923113s
  tsl::hopscotch_set: repeat 10 times, total = 190.542528s, avg = 19.054253s
    tsl::ordered_set: repeat 10 times, total = 153.893425s, avg = 15.389342s
     tsl::sparse_set: repeat 10 times, total = 159.534610s, avg = 15.953461s
      tsl::robin_set: repeat 10 times, total = 168.410074s, avg = 16.841007s
```

With FNV-1a for `int` hash:
```text
Num threads = 16
Benching image 256x128...
  std::unordered_set: repeat 100 times, total = 155.224097s, avg = 1.552241s
boost::unordered_set: repeat 100 times, total = 150.984269s, avg = 1.509843s
  tsl::hopscotch_set: repeat 100 times, total = 157.286525s, avg = 1.572865s
    tsl::ordered_set: repeat 100 times, total = 158.560118s, avg = 1.585601s
     tsl::sparse_set: repeat 100 times, total = 144.522430s, avg = 1.445224s
      tsl::robin_set: repeat 100 times, total = 150.278421s, avg = 1.502784s
Benching image 512x512...
  std::unordered_set: repeat 10 times, total = 148.812345s, avg = 14.881235s
boost::unordered_set: repeat 10 times, total = 187.575723s, avg = 18.757572s
  tsl::hopscotch_set: repeat 10 times, total = 179.597619s, avg = 17.959762s
    tsl::ordered_set: repeat 10 times, total = 164.498476s, avg = 16.449848s
     tsl::sparse_set: repeat 10 times, total = 170.278597s, avg = 17.027860s
      tsl::robin_set: repeat 10 times, total = 182.980808s, avg = 18.298081s
```

MSVC uses FNV-1a for `std::hash<int>` and their `std::unordered_set` seems to be even faster than identity function int hash, while all other hash sets seem to run slower on 512x512 images. Benchmarks of generating 256x128 images are a little bit weird though.

`tsl`'s sets are supposed to be faster than `std::unordered_set`, especially `tsl::robin_set`. However it is not the case here.

## How to configure with CMake

### Linux
From bash terminal:
```
cmake --preset ninjam-linux
```

### Windows
Available presets:

- `vs2022`

  From powershell terminal:
  ```powershell
  cmake --preset vs2022
  ```
  You should use VS Community 2022 and this preset on Windows.

- `vs2022-32`

  From powershell terminal:
  ```powershell
  cmake --preset vs2022-32
  ```

- `ninjam-msvc`

  From powershell terminal:
  ```powershell
  cmd
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  cmake --preset ninjam-msvc
  ```
  Please correct the path to `vcvars64.bat` or `vcvars32.bat`

- `ninjam-msys2`

  From MSYS2 MinGW 64-bit terminal:
  ```
  cmake --preset ninjam-msys2
  ```
  You need to configure in MSYS2 MinGW terminal to make sure you use the correct MinGW's `cmake`

- `ninjam-msys2-novcpkg`

  Do the same with `ninjam-msys2` except change the name of the preset.

## How to debug with VS Code

You only need Microsoft C/C++ extension (cpptools).

- Intenllisense profiles are stored in `.vscode/c_cpp_properties.json`
- Build commands are store in `.vscode/tasks.json`
- Debug profiles are stored in `.vscode/launch.json`
- Open `.vscode/<project name>.code-workspace` with VS Code

### Linux

- For `ninjam-linux`: it works out of the box, nothing to do.

### Windows

- For `ninjam-msys2` amd `ninjam-msys2-novcpkg`: you need to set `MINGW_ROOT` environment var to `/mingw64` folder (or `/mingw32`, etc.). For example `D:\msys64\mingw64`

- For `ninjam-msvc`: you need to open VS Code with `vcvars64.bat` (or `vcvars32.bat`) enabled. For example from powershell terminal run:

  ```
  cmd
  "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
  code .vscode/omnichroma.code-workspace
  ```

## How to use external libraries

- For `ninjam-msys2-novcpkg`:
  - Open MSYS2 shell, install needed libraries with `pacman -S ...`
  - Add `find_package(...)` and `target_link_libraries(...)` to `CMakeList.txt`

- For other presets that use `vcpkg`:
  - Add the library's names to `vcpkg.json` `"dependencies": [ ... ]"`
  - Run `cmake --preset ...` again. `vcpkg` will build and installed those libraries for you. In addition to that, `vcpkg` will tell you how to find and link those libraries with CMake.
  - Add `find_package(...)` and `target_link_libraries(...)` to `CMakeList.txt`
