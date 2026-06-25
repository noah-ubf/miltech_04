cmake_minimum_required(VERSION 3.20)
project(homework_06 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(util src/basics/util.cpp)
target_include_directories(util PUBLIC include)

add_library(coord src/basics/coord.cpp)
target_include_directories(coord PUBLIC include)

add_library(ballistic_table src/config/ballistic_table.cpp)
target_include_directories(ballistic_table PUBLIC include)

add_library(drone src/basics/drone_context.cpp)
target_include_directories(drone PUBLIC include)
target_link_libraries(drone PUBLIC coord)

add_library(solver src/solvers/analytical_solver.cpp)
target_include_directories(solver PUBLIC include)
target_link_libraries(solver PRIVATE util)
target_link_libraries(solver PRIVATE drone)

add_library(table_solver src/solvers/ballistic_table_solver.cpp)
target_include_directories(table_solver PUBLIC include)
target_link_libraries(table_solver PRIVATE util)
target_link_libraries(table_solver PRIVATE drone)
target_link_libraries(table_solver PRIVATE ballistic_table)

add_library(loader src/config/file_config_loader.cpp)
target_include_directories(loader PUBLIC include)

add_library(provider src/providers/json_target_provider.cpp)
target_include_directories(provider PUBLIC include)
target_link_libraries(provider PUBLIC coord)

add_library(factory src/config/factory.cpp)
target_include_directories(factory PUBLIC include)
target_link_libraries(factory PRIVATE loader)
target_link_libraries(factory PRIVATE solver)
target_link_libraries(factory PRIVATE table_solver)
target_link_libraries(factory PRIVATE provider)

add_library(processor src/mission_processor.cpp)
target_include_directories(processor PUBLIC include)
target_link_libraries(processor PRIVATE drone)

add_library(simulation src/basics/simulation.cpp)
target_include_directories(simulation PUBLIC include)

add_library(drone_states
  src/drone_states/drone_state_accelerating.cpp
  src/drone_states/drone_state_decelerating.cpp
  src/drone_states/drone_state_moving.cpp
  src/drone_states/drone_state_stopped.cpp
  src/drone_states/drone_state_turning.cpp)
target_include_directories(drone_states PUBLIC include)
target_link_libraries(drone_states PUBLIC coord)

add_executable(calc_ballistics_hw7 src/main.cpp)

target_link_libraries(calc_ballistics_hw7
  PRIVATE solver
  PRIVATE table_solver
  PRIVATE processor
  PRIVATE drone
  PRIVATE factory
  PRIVATE loader
  PRIVATE provider
  PRIVATE simulation
  PRIVATE drone_states
)

target_compile_options(calc_ballistics_hw7 PRIVATE -Wall -Wextra -pedantic)
