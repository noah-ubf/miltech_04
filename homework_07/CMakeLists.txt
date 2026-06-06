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

add_library(simulation src/simulation.cpp)
target_include_directories(simulation PUBLIC include)

add_executable(calc_ballistics_hw7 src/main.cpp)

target_link_libraries(calc_ballistics_hw7
  PRIVATE solver
  PRIVATE fabric
  PRIVATE loader
  PRIVATE provider
  PRIVATE processor
  PRIVATE simulation
)

target_compile_options(calc_ballistics_hw7 PRIVATE -Wall -Wextra -pedantic)
