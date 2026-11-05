#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <random>
#include <type_traits>
#include <utility>

namespace sorting {

template <typename RandomIt, typename Compare>
void mergeInPlace(RandomIt first, RandomIt middle, RandomIt last, Compare comp) {
    while (first != middle && middle != last) {
        if (comp(*middle, *first)) {
            RandomIt next = std::next(middle);
            std::rotate(first, middle, next);
            ++first;
            ++middle;
        } else {
            ++first;
        }
    }
}

template <typename RandomIt, typename Compare>
void inPlaceMergeSort(RandomIt first, RandomIt last, Compare comp) {
    const auto size = std::distance(first, last);
    if (size < 2) {
        return;
    }

    RandomIt middle = first + size / 2;
    inPlaceMergeSort(first, middle, comp);
    inPlaceMergeSort(middle, last, comp);
    mergeInPlace(first, middle, last, comp);
}

template <typename RandomIt>
void inPlaceMergeSort(RandomIt first, RandomIt last) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    inPlaceMergeSort(first, last, std::less<Value>{});
}

template <typename T, std::size_t N, typename Compare>
void inPlaceMergeSort(T (&array)[N], Compare comp) {
    inPlaceMergeSort(array, array + N, comp);
}

template <typename T, std::size_t N>
void inPlaceMergeSort(T (&array)[N]) {
    T* first = array;
    inPlaceMergeSort(first, first + N);
}

template <typename Container,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void inPlaceMergeSort(Container& container) {
    inPlaceMergeSort(std::begin(container), std::end(container));
}

template <typename Container, typename Compare,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void inPlaceMergeSort(Container& container, Compare comp) {
    inPlaceMergeSort(std::begin(container), std::end(container), comp);
}

template <typename RandomIt, typename Compare>
bool isSorted(RandomIt first, RandomIt last, Compare comp) {
    if (first == last) {
        return true;
    }

    RandomIt previous = first;
    for (RandomIt current = std::next(first); current != last;
         ++previous, ++current) {
        if (comp(*current, *previous)) {
            return false;
        }
    }
    return true;
}

template <typename RandomIt>
bool isSorted(RandomIt first, RandomIt last) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    return isSorted(first, last, std::less<Value>{});
}

template <typename RandomIt, typename Compare, typename Generator>
void bogosort(RandomIt first, RandomIt last, Compare comp,
              Generator& generator) {
    if (std::distance(first, last) < 2) {
        return;
    }

    while (!isSorted(first, last, comp)) {
        std::shuffle(first, last, generator);
    }
}

template <typename RandomIt, typename Compare>
void bogosort(RandomIt first, RandomIt last, Compare comp) {
    std::random_device device;
    std::mt19937 generator(device());
    bogosort(first, last, comp, generator);
}

template <typename RandomIt>
void bogosort(RandomIt first, RandomIt last) {
    using Value = typename std::iterator_traits<RandomIt>::value_type;
    bogosort(first, last, std::less<Value>{});
}

template <typename T, std::size_t N, typename Compare>
void bogosort(T (&array)[N], Compare comp) {
    bogosort(array, array + N, comp);
}

template <typename T, std::size_t N>
void bogosort(T (&array)[N]) {
    T* first = array;
    bogosort(first, first + N);
}

template <typename Container,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void bogosort(Container& container) {
    bogosort(std::begin(container), std::end(container));
}

template <typename Container, typename Compare,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void bogosort(Container& container, Compare comp) {
    bogosort(std::begin(container), std::end(container), comp);
}

template <typename Container, typename Compare, typename Generator,
          typename = std::enable_if_t<!std::is_array_v<Container>>,
          typename = decltype(std::begin(std::declval<Container&>())),
          typename = decltype(std::end(std::declval<Container&>()))>
void bogosort(Container& container, Compare comp, Generator& generator) {
    bogosort(std::begin(container), std::end(container), comp, generator);
}

}  // namespace sorting
