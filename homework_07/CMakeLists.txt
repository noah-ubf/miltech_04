cmake_minimum_required(VERSION 3.20)
project(homework_06 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(util src/util.cpp)
target_include_directories(util PUBLIC include)

add_library(solver src/analytical_solver.cpp)
target_include_directories(solver PUBLIC include)
target_link_libraries(solver PRIVATE util)

add_library(loader src/file_config_loader.cpp)
target_include_directories(loader PUBLIC include)

add_library(provider src/json_target_provider.cpp)
target_include_directories(provider PUBLIC include)

add_library(fabric src/fabric.cpp)
target_include_directories(fabric PUBLIC include)
target_link_libraries(fabric PRIVATE loader)
target_link_libraries(fabric PRIVATE solver)
target_link_libraries(fabric PRIVATE provider)

add_library(processor src/mission_processor.cpp)
target_include_directories(processor PUBLIC include)

add_executable(calc_ballistics_hw7 src/main.cpp)

target_link_libraries(calc_ballistics_hw7
  PRIVATE solver
  PRIVATE fabric
  PRIVATE loader
  PRIVATE provider
  PRIVATE processor
)
target_compile_options(calc_ballistics_hw7 PRIVATE -Wall -Wextra -pedantic)

# configure_file(data/ammo.json ${CMAKE_CURRENT_BINARY_DIR}/data/ammo.json COPYONLY)
# configure_file(data/config.json ${CMAKE_CURRENT_BINARY_DIR}/data/config.json COPYONLY)
# configure_file(data/targets.json ${CMAKE_CURRENT_BINARY_DIR}/data/targets.json COPYONLY)


#include(GoogleTest)
# тест
#add_executable(ballistics_tests tests/ballistics_tests.cpp)
#target_link_libraries(ballistics_tests ballistics GTest::gtest_main)
#configure_file(tests/data/test_config.txt ${CMAKE_CURRENT_BINARY_DIR}/data/test_config.txt COPYONLY)
#configure_file(tests/data/incorrect_ammo.txt ${CMAKE_CURRENT_BINARY_DIR}/data/incorrect_ammo.txt COPYONLY)