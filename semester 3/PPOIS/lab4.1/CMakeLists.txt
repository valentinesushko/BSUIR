cmake_minimum_required(VERSION 3.16)

project(SortingLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(SORTING_WARNING_FLAGS -Wall -Wextra -Wpedantic)

add_library(sorting_lib
    src/Student.cpp
)
target_include_directories(sorting_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(sorting_lib PUBLIC cxx_std_17)
target_compile_options(sorting_lib PRIVATE ${SORTING_WARNING_FLAGS})

add_executable(sorting_app
    src/main.cpp
)
target_link_libraries(sorting_app PRIVATE sorting_lib)
target_compile_options(sorting_app PRIVATE ${SORTING_WARNING_FLAGS})

add_executable(sorting_tests
    tests/test_main.cpp
)
target_link_libraries(sorting_tests PRIVATE sorting_lib)
target_compile_options(sorting_tests PRIVATE ${SORTING_WARNING_FLAGS})

add_test(NAME sorting_tests COMMAND sorting_tests)
