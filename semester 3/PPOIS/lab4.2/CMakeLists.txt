cmake_minimum_required(VERSION 3.16)

project(GraphLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(GRAPH_WARNING_FLAGS -Wall -Wextra -Wpedantic)

option(ENABLE_COVERAGE "Build with coverage instrumentation" OFF)
if(ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_library(graph_lib INTERFACE)
target_include_directories(graph_lib INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(graph_lib INTERFACE cxx_std_17)

add_executable(graph_app src/main.cpp)
target_link_libraries(graph_app PRIVATE graph_lib)
target_compile_options(graph_app PRIVATE ${GRAPH_WARNING_FLAGS})

add_executable(graph_tests tests/test_main.cpp)
target_link_libraries(graph_tests PRIVATE graph_lib)
target_compile_options(graph_tests PRIVATE ${GRAPH_WARNING_FLAGS})

add_test(NAME graph_tests COMMAND graph_tests)
