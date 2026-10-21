cmake_minimum_required(VERSION 3.16)

project(RubiksCube LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(RUBIKS_DATA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/data")
set(RUBIKS_WARNING_FLAGS -Wall -Wextra -Wpedantic)

add_library(rubiks_cube_lib
    src/RubiksCube.cpp
)
target_include_directories(rubiks_cube_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(rubiks_cube_lib PUBLIC cxx_std_17)
target_compile_options(rubiks_cube_lib PRIVATE ${RUBIKS_WARNING_FLAGS})

add_executable(rubiks_cube_app
    src/main.cpp
    src/ConsoleUI.cpp
)
target_link_libraries(rubiks_cube_app PRIVATE rubiks_cube_lib)
target_compile_definitions(rubiks_cube_app PRIVATE
    RUBIKS_DATA_DIR="${RUBIKS_DATA_DIR}"
)
target_compile_options(rubiks_cube_app PRIVATE ${RUBIKS_WARNING_FLAGS})

find_package(UnitTest++ QUIET)

set(RUBIKS_UNITTEST_TARGET "")
set(RUBIKS_UNITTEST_INCLUDE_DIRS "")

if(NOT TARGET UnitTest++ AND NOT TARGET UnitTest++::UnitTest++)
    include(FetchContent)

    if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    endif()

    FetchContent_Declare(
        unittestcpp
        GIT_REPOSITORY https://github.com/unittest-cpp/unittest-cpp.git
        GIT_TAG v2.0.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(unittestcpp)

    if(TARGET UnitTest++)
        set(RUBIKS_UNITTEST_INCLUDE_DIRS ${unittestcpp_SOURCE_DIR})
    endif()
endif()

if(TARGET UnitTest++::UnitTest++)
    set(RUBIKS_UNITTEST_TARGET UnitTest++::UnitTest++)
elseif(TARGET UnitTest++)
    set(RUBIKS_UNITTEST_TARGET UnitTest++)
else()
    message(FATAL_ERROR "UnitTest++ target not found (expected UnitTest++ or UnitTest++::UnitTest++)")
endif()

add_executable(rubiks_cube_tests
    tests/test_main.cpp
    tests/test_cube.cpp
)
target_link_libraries(rubiks_cube_tests PRIVATE
    rubiks_cube_lib
    ${RUBIKS_UNITTEST_TARGET}
)
if(RUBIKS_UNITTEST_INCLUDE_DIRS)
    target_include_directories(rubiks_cube_tests PRIVATE
        ${RUBIKS_UNITTEST_INCLUDE_DIRS}
    )
endif()
target_compile_definitions(rubiks_cube_tests PRIVATE
    RUBIKS_DATA_DIR="${RUBIKS_DATA_DIR}"
)
target_compile_options(rubiks_cube_tests PRIVATE ${RUBIKS_WARNING_FLAGS})

add_test(NAME rubiks_cube_tests
    COMMAND rubiks_cube_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )
endif()
