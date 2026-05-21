#include <iostream>

#include "../include/ballistics.hpp"

/*
  Fire point and maneuver calculation for a drone.
*/
int main(int argc, char** argv)
{
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <input_file>" << std::endl;
    return 1;
  }

  Config config = readConfig(argv[1]);
  if (!config.isValid) {
    return 1;
  }

  double flightTime = calculateTime(config);
  if (flightTime < 0) {
    return 1;
  }

  double fireDistance = calculateFireDistance(config, flightTime);
  if (fireDistance < 0) {
    return 1;
  }

  Point midPoint, firePoint;
  bool needsManeuver = calculateFirePoint(config, fireDistance, flightTime, midPoint, firePoint);

  if (saveData("output.txt", needsManeuver, midPoint, firePoint) != 0) {
    return 1;
  }

  return 0;
}