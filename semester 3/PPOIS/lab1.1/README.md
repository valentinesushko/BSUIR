# Polynomial

A C++17 implementation of a polynomial in one variable. The project contains
the `Polynomial` class, a console menu application, and UnitTest++ based tests.

## Requirements

- CMake 3.16 or newer
- A C++17 compatible compiler
- UnitTest++ (optional; CMake fetches it automatically if it is not installed)
- Doxygen (optional, only for generating documentation)

## Build

From the project root:

```sh
cmake -S . -B build
cmake --build build
```

This builds the `polynomial_lib` library, the `polynomial_app` executable, and
the test executables.

## Run the application

The console application stores two polynomials, `A` and `B`, and provides a
menu for reading them, displaying them, evaluating them, performing arithmetic,
and comparing them.

```sh
./build/polynomial_app
```

Polynomials are entered in the format `degree c0 c1 ... cdegree`, for example
`2 1 2 3` for `1 + 2x + 3x^2`.

## Run the tests

After building:

```sh
ctest --test-dir build --output-on-failure
```

If your CMake version is older than 3.20, run CTest from the build directory:

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
`docs/generated/html/index.html`. Open that file in a browser. The generated
documentation directory is ignored by Git.

## Project layout

- `include/Polynomial.h` — class declaration and API documentation
- `src/polynomial_core.cpp` — constructors, accessors, evaluation, I/O
- `src/polynomial_arithmetic.cpp` — arithmetic operators
- `src/main.cpp` — console menu application
- `tests/` — UnitTest++ test programs
- `docs/mainpage.dox` — documentation main page
- `Doxyfile` — Doxygen configuration
