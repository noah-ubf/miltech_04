#include <iostream>

#include "../include/ballistics.hpp"

/*
  Fire point and maneuver calculation for a drone.
*/
auto main(int argc, char** argv) -> int
{
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl; // NOLINT(*-pointer-arithmetic)
    return 1;
  }

  Config config = read_config(argv[1]); // NOLINT(*-pointer-arithmetic) - No reason to use std::string
  if (!config.isValid) {
    return 1;
  }

  double flight_time = calculate_time(config);
  if (flight_time < 0) {
    return 1;
  }

  double fire_distance = calculate_fire_distance(config, flight_time);
  if (fire_distance < 0) {
    return 1;
  }

  Point mid_point = {0., 0.};
  Point fire_point = {0., 0.};
  bool needs_maneuver = calculate_fire_point(config, fire_distance, flight_time, mid_point, fire_point);

  if (save_data("output.txt", needs_maneuver, mid_point, fire_point) != 0) {
    return 1;
  }

  return 0;
}