cmake_minimum_required(VERSION 3.20)
project(homework_06 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(ballistics src/ballistics.cpp)
target_include_directories(ballistics PUBLIC include)

add_executable(calc_ballistics src/main.cpp)

target_link_libraries(calc_ballistics PRIVATE ballistics)
target_compile_options(calc_ballistics PRIVATE -Wall -Wextra -pedantic)

include(GoogleTest)
# тест
add_executable(ballistics_tests tests/ballistics_tests.cpp)
target_compile_definitions(ballistics_tests PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_BINARY_DIR}/data")
target_link_libraries(ballistics_tests ballistics GTest::gtest_main)
gtest_discover_tests(ballistics_tests WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
configure_file(tests/data/test_config.txt ${CMAKE_CURRENT_BINARY_DIR}/data/test_config.txt COPYONLY)
configure_file(tests/data/incorrect_ammo.txt ${CMAKE_CURRENT_BINARY_DIR}/data/incorrect_ammo.txt COPYONLY)
