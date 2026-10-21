cmake_minimum_required(VERSION 3.16)

project(Polynomial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_library(polynomial_lib
    src/polynomial_core.cpp
    src/polynomial_arithmetic.cpp
)
target_include_directories(polynomial_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(polynomial_lib PUBLIC cxx_std_17)

add_executable(polynomial_app
    src/main.cpp
)
target_link_libraries(polynomial_app PRIVATE polynomial_lib)

find_package(UnitTest++ QUIET)

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
        set(POLYNOMIAL_UNITTEST_INCLUDE_DIRS ${unittestcpp_SOURCE_DIR})
    endif()
endif()

if(TARGET UnitTest++::UnitTest++)
    set(POLYNOMIAL_UNITTEST_TARGET UnitTest++::UnitTest++)
elseif(TARGET UnitTest++)
    set(POLYNOMIAL_UNITTEST_TARGET UnitTest++)
else()
    message(FATAL_ERROR "UnitTest++ target not found (expected UnitTest++ or UnitTest++::UnitTest++)")
endif()

add_executable(polynomial_tests_core
    tests/test_core.cpp
    tests/test_main.cpp
)
target_link_libraries(polynomial_tests_core PRIVATE
    polynomial_lib
    ${POLYNOMIAL_UNITTEST_TARGET}
)
if(POLYNOMIAL_UNITTEST_INCLUDE_DIRS)
    target_include_directories(polynomial_tests_core PRIVATE
        ${POLYNOMIAL_UNITTEST_INCLUDE_DIRS}
    )
endif()

add_executable(polynomial_tests_arithmetic
    tests/test_arithmetic.cpp
    tests/test_main.cpp
)
target_link_libraries(polynomial_tests_arithmetic PRIVATE
    polynomial_lib
    ${POLYNOMIAL_UNITTEST_TARGET}
)
if(POLYNOMIAL_UNITTEST_INCLUDE_DIRS)
    target_include_directories(polynomial_tests_arithmetic PRIVATE
        ${POLYNOMIAL_UNITTEST_INCLUDE_DIRS}
    )
endif()

add_test(NAME polynomial_tests_core COMMAND polynomial_tests_core)
add_test(NAME polynomial_tests_arithmetic COMMAND polynomial_tests_arithmetic)

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    set(POLYNOMIAL_DOXYFILE ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    file(WRITE ${POLYNOMIAL_DOXYFILE}
        "PROJECT_NAME = \"Polynomial\"\n"
        "OUTPUT_DIRECTORY = ${CMAKE_CURRENT_SOURCE_DIR}/docs/generated\n"
        "INPUT = ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/src\n"
        "RECURSIVE = YES\n"
        "EXTRACT_ALL = YES\n"
        "GENERATE_LATEX = NO\n"
        "QUIET = YES\n"
    )

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${POLYNOMIAL_DOXYFILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM
    )
endif()
