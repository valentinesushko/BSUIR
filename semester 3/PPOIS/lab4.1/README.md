# Generic Sorting Algorithms

A C++17 project implementing two generic sorting algorithms required by the
laboratory variant:

- **In-place merge sort** — a stable comparison sort that merges subranges by
  rotating elements of the same range, so no auxiliary buffer is allocated.
- **Bogosort** — a deliberately inefficient randomized sort that repeatedly
  shuffles the range until it becomes ordered.

Both algorithms are templates. They accept random-access iterators, C-style
arrays, `std::array`, and standard dynamic collections such as `std::vector`.
Elements may be built-in types or user-defined classes; ordering is controlled
either by the element's `operator<` or by a custom comparator.

## Requirements

- CMake 3.16 or newer
- A C++17 compatible compiler
- No third-party libraries are required

## Build

From this directory:

```sh
cmake -S . -B build
cmake --build build
```

This builds:

- `sorting_lib` — the library with the demonstration `Student` class
- `sorting_app` — a demonstration program
- `sorting_tests` — the test executable

## Run the demonstration

```sh
./build/sorting_app
```

The program sorts a C-style array of integers, a `std::vector` of doubles in
descending order, a `std::array` of strings, a `std::vector<Student>` by the
natural order and by a custom comparator, and a small array with bogosort.

## Run the tests

```sh
ctest --test-dir build --output-on-failure
```

## API

All declarations are in `include/sorting/Sorting.h`, namespace `sorting`.

```cpp
sorting::inPlaceMergeSort(first, last);
sorting::inPlaceMergeSort(first, last, comparator);
sorting::inPlaceMergeSort(array);              // C-style array
sorting::inPlaceMergeSort(array, comparator);
sorting::inPlaceMergeSort(container);          // vector, array, etc.
sorting::inPlaceMergeSort(container, comparator);

sorting::bogosort(first, last);
sorting::bogosort(first, last, comparator);
sorting::bogosort(first, last, comparator, generator);
sorting::bogosort(array);
sorting::bogosort(array, comparator);
sorting::bogosort(container);
sorting::bogosort(container, comparator);
sorting::bogosort(container, comparator, generator);

sorting::isSorted(first, last);
sorting::isSorted(first, last, comparator);
```

The `generator` overload of `bogosort` accepts any uniform random bit generator
and is useful for reproducible tests.

## Usage example

```cpp
#include "sorting/Sorting.h"

#include <vector>

int main() {
    int values[] = {5, 1, 4, 2, 3};
    sorting::inPlaceMergeSort(values);

    std::vector<int> numbers{5, 1, 4, 2, 3};
    sorting::inPlaceMergeSort(numbers, std::greater<int>{});

    return 0;
}
```

## User-defined type

`sorting::Student` demonstrates sorting objects of a custom class:

- fields: `name_`, `group_`, `gpa_`
- `operator<` orders by GPA, then by name, then by group
- `operator==` compares all fields
- `describe()` and `operator<<` provide a textual representation

The demonstration also sorts students by name with a lambda comparator.

## Complexity

| Algorithm | Time | Auxiliary memory | Stable |
|---|---|---|---|
| In-place merge sort | `O(n log n)` best case, `O(n^2)` worst case due to rotations | `O(log n)` recursion stack | yes |
| Bogosort | expected `O(n · n!)`, unbounded worst case | `O(1)` beyond the range | no |

Bogosort is intended only for tiny ranges because its running time grows
factorially.

## Project layout

- `include/sorting/Sorting.h` — generic sorting templates
- `include/sorting/Student.h` — user-defined demonstration class
- `src/Student.cpp` — `Student` implementation
- `src/main.cpp` — demonstration program
- `tests/test_main.cpp` — tests covering arrays, vectors, strings, custom
  objects, custom comparators, edge cases, stability, and bogosort
- `task.txt` — laboratory task statement
