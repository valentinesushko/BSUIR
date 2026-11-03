cmake_minimum_required(VERSION 3.16)

project(AnimalWorld LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(AW_WARNING_FLAGS -Wall -Wextra -Wpedantic)

add_library(animalworld_lib
    src/Core.cpp
    src/Exceptions.cpp
    src/Inhabitants.cpp
    src/World.cpp
    src/Simulation.cpp
    src/Species.cpp
)
target_include_directories(animalworld_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_compile_features(animalworld_lib PUBLIC cxx_std_17)
target_compile_options(animalworld_lib PRIVATE ${AW_WARNING_FLAGS})

add_executable(animalworld_app
    src/main.cpp
)
target_link_libraries(animalworld_app PRIVATE animalworld_lib)
target_compile_options(animalworld_app PRIVATE ${AW_WARNING_FLAGS})

add_test(NAME animalworld_smoke
    COMMAND animalworld_app
            ${CMAKE_CURRENT_SOURCE_DIR}/data/world.txt
            auto
            2
)
