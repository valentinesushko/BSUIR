#include "sorting/Sorting.h"
#include "sorting/Student.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

namespace {

int failures = 0;

void expect(bool condition, const std::string& message) {
    if (!condition) {
        ++failures;
        std::cerr << "FAILED: " << message << '\n';
    }
}

template <typename Container>
std::string join(const Container& container) {
    std::string result;
    for (const auto& value : container) {
        result += std::to_string(value);
        result += ' ';
    }
    return result;
}

void testInPlaceMergeSortArray() {
    int values[] = {5, 1, 4, 2, 3};
    sorting::inPlaceMergeSort(values);
    expect(std::is_sorted(std::begin(values), std::end(values)),
           "merge sort orders a C array");
    expect(join(std::vector<int>(std::begin(values), std::end(values))) ==
               "1 2 3 4 5 ",
           "merge sort produces the expected array order");
}

void testInPlaceMergeSortVector() {
    std::vector<std::string> words{"pear", "apple", "fig", "banana"};
    sorting::inPlaceMergeSort(words);
    expect(std::is_sorted(words.begin(), words.end()),
           "merge sort orders a vector of strings");
    expect(words.front() == "apple" && words.back() == "pear",
           "merge sort uses the default comparator");

    std::vector<double> numbers{1.5, -2.0, 7.25, 0.0};
    sorting::inPlaceMergeSort(numbers, std::greater<double>{});
    expect(std::is_sorted(numbers.begin(), numbers.end(),
                          std::greater<double>{}),
           "merge sort accepts a custom comparator");
}

void testInPlaceMergeSortEdgeCases() {
    std::vector<int> empty;
    sorting::inPlaceMergeSort(empty.begin(), empty.end());
    expect(empty.empty(), "merge sort handles an empty range");

    std::vector<int> single{42};
    sorting::inPlaceMergeSort(single);
    expect(single.size() == 1 && single.front() == 42,
           "merge sort handles a single element");

    std::vector<int> duplicates{3, 1, 3, 2, 1, 3};
    sorting::inPlaceMergeSort(duplicates);
    expect(duplicates == std::vector<int>({1, 1, 2, 3, 3, 3}),
           "merge sort keeps duplicate values");

    std::vector<int> sorted{1, 2, 3, 4};
    sorting::inPlaceMergeSort(sorted);
    expect(sorted == std::vector<int>({1, 2, 3, 4}),
           "merge sort handles an already sorted range");

    std::vector<int> reversed{5, 4, 3, 2, 1};
    sorting::inPlaceMergeSort(reversed);
    expect(reversed == std::vector<int>({1, 2, 3, 4, 5}),
           "merge sort handles a reversed range");
}

void testInPlaceMergeSortRandomized() {
    std::mt19937 generator(20240517U);
    std::uniform_int_distribution<int> distribution(-1000, 1000);

    for (int trial = 0; trial < 50; ++trial) {
        std::vector<int> data(static_cast<std::size_t>(trial) * 3U + 1U);
        for (int& value : data) {
            value = distribution(generator);
        }

        std::vector<int> expected = data;
        std::sort(expected.begin(), expected.end());
        sorting::inPlaceMergeSort(data);
        expect(data == expected,
               "merge sort matches std::sort on random data");
    }
}

void testStability() {
    struct Entry {
        int key;
        int order;
        bool operator<(const Entry& other) const { return key < other.key; }
    };

    std::vector<Entry> entries{{2, 0}, {1, 1}, {2, 2}, {1, 3}, {2, 4}};
    sorting::inPlaceMergeSort(entries);
    bool stable = true;
    for (std::size_t index = 1; index < entries.size(); ++index) {
        if (entries[index - 1].key == entries[index].key &&
            entries[index - 1].order > entries[index].order) {
            stable = false;
        }
    }
    expect(stable, "in-place merge sort is stable");
}

void testStudentSorting() {
    std::vector<sorting::Student> students{
        {"Alice", 2, 8.7}, {"Bob", 1, 6.4},  {"Carol", 2, 9.1},
        {"Dave", 3, 7.2},  {"Eve", 1, 8.7},   {"Frank", 3, 5.9}};

    sorting::inPlaceMergeSort(students);
    expect(std::is_sorted(students.begin(), students.end()),
           "merge sort orders user-defined objects");
    expect(students.front().name() == "Frank" &&
               students.back().name() == "Carol",
           "user-defined objects use their operator<");

    sorting::inPlaceMergeSort(
        students, [](const sorting::Student& left,
                     const sorting::Student& right) {
            return left.name() < right.name();
        });
    expect(students.front().name() == "Alice" &&
               students.back().name() == "Frank",
           "user-defined objects accept a custom comparator");

    std::array<sorting::Student, 2> pair{
        sorting::Student("Zoe", 1, 7.0), sorting::Student("Adam", 1, 7.0)};
    sorting::inPlaceMergeSort(pair.begin(), pair.end());
    expect(pair.front().name() == "Adam",
           "merge sort works with std::array iterators");
}

void testBogosort() {
    std::mt19937 generator(12345U);

    std::vector<int> values{3, 1, 4, 0, 2};
    sorting::bogosort(values, std::less<int>{}, generator);
    expect(values == std::vector<int>({0, 1, 2, 3, 4}),
           "bogosort orders a vector");

    int numbers[] = {2, 1, 0};
    sorting::bogosort(numbers);
    expect(std::is_sorted(std::begin(numbers), std::end(numbers)),
           "bogosort orders a C array");

    std::vector<int> empty;
    sorting::bogosort(empty, std::less<int>{}, generator);
    expect(empty.empty(), "bogosort handles an empty range");

    std::vector<int> single{7};
    sorting::bogosort(single, std::less<int>{}, generator);
    expect(single.size() == 1 && single.front() == 7,
           "bogosort handles a single element");
}

void testIsSorted() {
    std::vector<int> ascending{1, 2, 2, 3};
    std::vector<int> descending{3, 2, 1};
    expect(sorting::isSorted(ascending.begin(), ascending.end()),
           "isSorted accepts ascending data");
    expect(!sorting::isSorted(descending.begin(), descending.end()),
           "isSorted rejects descending data");
    expect(sorting::isSorted(descending.begin(), descending.end(),
                             std::greater<int>{}),
           "isSorted accepts a custom comparator");
}

}  // namespace

int main() {
    testInPlaceMergeSortArray();
    testInPlaceMergeSortVector();
    testInPlaceMergeSortEdgeCases();
    testInPlaceMergeSortRandomized();
    testStability();
    testStudentSorting();
    testBogosort();
    testIsSorted();

    if (failures != 0) {
        std::cerr << failures << " test(s) failed\n";
        return 1;
    }

    std::cout << "All tests passed\n";
    return 0;
}
