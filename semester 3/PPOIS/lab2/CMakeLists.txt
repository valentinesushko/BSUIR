cmake_minimum_required(VERSION 3.16)

project(ClothingModel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(CLOTHING_WARNING_FLAGS -Wall -Wextra -Wpedantic)

add_library(clothing_model
    src/Exceptions.cpp
    src/Core.cpp
    src/Materials.cpp
    src/Garments.cpp
    src/Sportswear.cpp
    src/Retail.cpp
)
target_include_directories(clothing_model PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(clothing_model PUBLIC cxx_std_17)
target_compile_options(clothing_model PRIVATE ${CLOTHING_WARNING_FLAGS})

add_executable(clothing_app src/main.cpp)
target_link_libraries(clothing_app PRIVATE clothing_model)
target_compile_options(clothing_app PRIVATE ${CLOTHING_WARNING_FLAGS})

add_executable(clothing_tests tests/test_main.cpp)
target_link_libraries(clothing_tests PRIVATE clothing_model)
target_compile_options(clothing_tests PRIVATE ${CLOTHING_WARNING_FLAGS})

add_test(NAME clothing_tests COMMAND clothing_tests)
