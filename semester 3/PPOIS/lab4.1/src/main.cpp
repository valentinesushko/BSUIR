#include "sorting/Sorting.h"
#include "sorting/Student.h"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

namespace {

template <typename Container>
void print(const std::string& title, const Container& container) {
    std::cout << title << ":";
    for (const auto& value : container) {
        std::cout << " " << value;
    }
    std::cout << '\n';
}

void printArray(const std::string& title, const int* values, std::size_t size) {
    std::cout << title << ":";
    for (std::size_t index = 0; index < size; ++index) {
        std::cout << " " << values[index];
    }
    std::cout << '\n';
}

}  // namespace

int main() {
    int numbers[] = {9, 1, 5, 3, 7, 3, 8, 2, 6, 4, 0};
    printArray("array before", numbers, std::size(numbers));
    sorting::inPlaceMergeSort(numbers);
    printArray("array after ", numbers, std::size(numbers));

    std::vector<double> values{3.5, -1.25, 8.0, 0.0, 2.75, -4.5};
    print("vector before        ", values);
    sorting::inPlaceMergeSort(values, std::greater<double>{});
    print("vector descending    ", values);

    std::array<std::string, 5> words{"pear", "apple", "fig", "banana", "kiwi"};
    print("strings before       ", words);
    sorting::inPlaceMergeSort(words.begin(), words.end());
    print("strings after        ", words);

    std::vector<sorting::Student> students{
        {"Alice", 2, 8.7}, {"Bob", 1, 6.4},  {"Carol", 2, 9.1},
        {"Dave", 3, 7.2},  {"Eve", 1, 8.7},   {"Frank", 3, 5.9}};
    print("students before      ", students);
    sorting::inPlaceMergeSort(students);
    print("students by GPA      ", students);
    sorting::inPlaceMergeSort(
        students, [](const sorting::Student& left,
                     const sorting::Student& right) {
            return left.name() > right.name();
        });
    print("students by name desc", students);

    int shuffled[] = {3, 1, 2, 0};
    printArray("bogosort before", shuffled, std::size(shuffled));
    sorting::bogosort(shuffled);
    printArray("bogosort after ", shuffled, std::size(shuffled));

    std::cout << "array sorted: "
              << std::boolalpha
              << sorting::isSorted(std::begin(numbers), std::end(numbers))
              << '\n';
    return 0;
}
