# Rubik's Cube

A C++17 implementation of a 3x3x3 Rubik's Cube. The project contains the
`RubiksCube` model, a separate console front end with an interactive menu, and
a UnitTest++ test suite.

## Requirements

- CMake 3.16 or newer
- A C++17 compatible compiler
- UnitTest++ (fetched automatically by CMake when it is not installed)
- Doxygen (optional, only for generating documentation)

## Build

From the project root:

```sh
cmake -S . -B build
cmake --build build
```

This builds the `rubiks_cube_lib` library, the `rubiks_cube_app` executable,
and the `rubiks_cube_tests` executable.

## Run the application

```sh
./build/rubiks_cube_app
```

The menu provides these actions:

1. Show cube
2. Randomize
3. Load from file
4. Rotate a face
5. Check whether solved
6. Save to file
0. Exit

Faces are entered as `U`, `D`, `L`, `R`, `F`, or `B`. Turn directions are
entered as `c` for clockwise or `a` for counter-clockwise. Example states are
available in `data/solved_cube.txt` and `data/scrambled_cube.txt`.

## Run the tests

After building:

```sh
ctest --test-dir build --output-on-failure
```

The UnitTest++ suite covers at least 30 test cases. If your CMake version is
older than 3.20, run CTest from the build directory instead:

```sh
cd build
ctest --output-on-failure
```

## Generate documentation

The Doxygen configuration is stored in `Doxyfile` at the project root. Run:

```sh
doxygen Doxyfile
```

The generated HTML documentation is written to
`docs/generated/html/index.html`. Open that file in a browser. Generated
documentation and build output are ignored by Git.

## Sticker, file, and stream format

A cube state is exactly 54 color characters, written as six lines of nine
characters. Face order is Up, Down, Left, Right, Front, Back, and each line is
row-major, from the top-left sticker to the bottom-right sticker as seen from
outside that face.

Colors are encoded as:

| Character | Color  |
|-----------|--------|
| `W`       | White  |
| `Y`       | Yellow |
| `G`       | Green  |
| `B`       | Blue   |
| `R`       | Red    |
| `O`       | Orange |

A solved cube is written as:

```
WWWWWWWWW
YYYYYYYYY
OOOOOOOOO
RRRRRRRRR
GGGGGGGGG
BBBBBBBBB
```

Stream input skips whitespace and accepts uppercase or lowercase color letters.
A cube is modified only when all 54 characters are valid. File input and output
use the same format through `loadFromFile` and `saveToFile`.

## Project layout

- `include/RubiksCube.h` — model declaration and API documentation
- `include/ConsoleUI.h` — console front-end declaration
- `src/RubiksCube.cpp` — cube storage, turns, I/O, and solved check
- `src/ConsoleUI.cpp` — interactive menu implementation
- `src/main.cpp` — program entry point
- `tests/` — UnitTest++ test programs
- `data/` — example solved and scrambled cube states
- `docs/mainpage.dox` — documentation main page
- `Doxyfile` — Doxygen configuration
- `CMakeLists.txt` — CMake build and test configuration
