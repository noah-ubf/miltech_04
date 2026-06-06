#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

#include "../include/ballistics.hpp"

using namespace std;

auto read_config(const char* filename) -> Config
{  // Input and setting initial data
  Config config;

  ifstream input(filename);

  if (!input.is_open()) {
    cerr << "Error: Unable to open input file" << endl;
    config.isValid = false;
    return config;
  }

  input >> config.pos.x >> config.pos.y >> config.zd >> config.target.x >> config.target.y >> config.attackSpeed >>
    config.accelerationPath >> config.ammoName;
  input.close();

  if (config.zd <= 0 || config.attackSpeed <= 0 || config.accelerationPath < 0) {
    cerr << "Error: Incorrect numeric parameters" << endl;
    config.isValid = false;
    return config;
  }

  // NOLINTBEGIN(*-array-to-pointer-decay, *-magic-numbers)
  if (strcmp(config.ammoName, "VOG-17") == 0) {
    config.mass = 0.35;
    config.drag = 0.07;
    config.lift = 0.0;
  }
  else if (strcmp(config.ammoName, "M67") == 0) {
    config.mass = 0.6;
    config.drag = 0.10;
    config.lift = 0.0;
  }
  else if (strcmp(config.ammoName, "RKG-3") == 0) {
    config.mass = 1.2;
    config.drag = 0.10;
    config.lift = 0.0;
  }
  else if (strcmp(config.ammoName, "GLIDING-VOG") == 0) {
    config.mass = 0.45;
    config.drag = 0.10;
    config.lift = 1.0;
  }
  else if (strcmp(config.ammoName, "GLIDING-RKG") == 0) {
    config.mass = 1.4;
    config.drag = 0.10;
    config.lift = 1.0;
  }
  else {
    cerr << "Error: Unknown ammo name \"" << config.ammoName << "\"" << endl;
    config.isValid = false;
    return config;
  }
  // NOLINTEND(*-array-to-pointer-decay, *-magic-numbers)
  config.isValid = true;
  return config;
}

auto calculate_time(const Config config) -> double
{
  if (!config.isValid) {
    cerr << "Error: Invalid config" << endl;
    return -1.;
  }

  double a = config.drag * g * config.mass - 2 * config.drag * config.drag * config.lift * config.attackSpeed;
  if (a == 0.f) {  // a appears in denominator, so it cannot be zero
    cerr << "Error: Incorrect a; no solution" << endl;
    return -1.;
  }
  double b = -3 * g * config.mass * config.mass + 3 * config.drag * config.lift * config.mass * config.attackSpeed;
  double c = 6 * config.mass * config.mass * config.zd;  // NOLINT - prefering to keep constants inline
  double p = -b * b / (3 * a * a);
  if (p >= 0) {  // inversed p appears as sqrt parameter, so p cannot be positive; p appears in denominator so it cannot be zero
    cerr << "Error: Incorrect p; no solution" << endl;
    return -1.;
  }
  double q = 2 * b * b * b / (27 * a * a * a) + c / a;  // NOLINT - prefering to keep constants inline
  double sqrtP3 = sqrt(-p / 3);
  double temp = 3 * q / (2 * p * sqrtP3);
  if (temp > 1 || temp < -1) {
    cerr << "Error: Argument of acos out of bounds [-1;1]" << endl;
    return -1.;
  }
  double fi = acos(temp);
  double flight_time = 2 * sqrtP3 * cos((fi + 4 * M_PI) / 3) - b / (3 * a);
  if (flight_time <= 0) {
    cerr << "Error: negative flight time" << endl;
    return -1.;
  }
  return flight_time;
}

auto calculate_fire_distance(const Config config, const double flight_time) -> double
{
  // helper variables to reduce repeated multiplications
  // and make the final formula more concise:
  double m = config.mass;
  double m2 = m * m;
  double m3 = m2 * m;
  double m4 = m3 * m;
  double l = config.lift;
  double l2 = l * l;
  double l3 = l2 * l;
  double l4 = l3 * l;
  double d = config.drag;
  double d2 = d * d;
  double d3 = d2 * d;
  double d4 = d3 * d;
  double ft = flight_time;
  double ft2 = ft * ft;
  double ft3 = ft2 * ft;
  double ft4 = ft3 * ft;
  double ft5 = ft4 * ft;
  double dglm = d * l * g * m;
  double v = config.attackSpeed;

  // NOLINTBEGIN(*-magic-numbers)
  double fire_distance =
    ft * v - ft2 * d * v / (2 * m) + ft3 * (6 * dglm - 6 * d2 * (l2 - 1) * v) / (36 * m2) +
    ft4 * (-6 * d * dglm * (1 + l2 + l4) + 3 * d3 * l2 * (1 + l2) * v + 6 * d3 * l4 * (1 + l2) * v) / (36 * (1 + l2) * (1 + l2) * m3) +
    ft5 * (3 * d2 * dglm * l2 - 3 * d4 * l2 * (1 + l2) * v) / (36 * (1 + l2) * m4);
  // NOLINTEND(*-magic-numbers)

  if (fire_distance <= 0) {
    cerr << "Error: negative fire distance" << endl;
  }

  return fire_distance;
}

auto calculate_fire_point(const Config config, const double fire_distance, const double flight_time, Point& mid_point, Point& fire_point)
  -> bool
{
  double distance(0.);
  bool needs_maneuver(false);
  // Calculating distances, intermediate point and fire point:
  double min_distance = fire_distance + config.accelerationPath;
  if (config.target.x == config.pos.x && config.target.y == config.pos.y) {
    cout << "Initial distance is zero - maneuvering to an arbitrary direction, e.g. to negative x" << endl;
    mid_point.x = config.target.x - min_distance;
    mid_point.y = config.target.y;
    needs_maneuver = true;
    fire_point.x = config.target.x - fire_distance;
    fire_point.y = config.target.y;
  }
  else {
    // non-zero distance between the drone and the target
    // - moving along the line that connects these two points
    double delta_x = config.target.x - config.pos.x;
    double delta_y = config.target.y - config.pos.y;
    distance = sqrt(delta_x * delta_x + delta_y * delta_y);
    // unit vector:
    double unit_x = delta_x / distance;
    double unit_y = delta_y / distance;

    if (distance < min_distance) {
      cout << "Too close - need a maneuver" << endl;
      needs_maneuver = true;
      mid_point.x = config.target.x - min_distance * unit_x;
      mid_point.y = config.target.y - min_distance * unit_y;
    }

    // calculating the fire point coordinates:
    fire_point.x = config.target.x - fire_distance * unit_x;
    fire_point.y = config.target.y - fire_distance * unit_y;
  }

  return needs_maneuver;
}

auto save_data(const char* filename, bool needs_maneuver, Point mid_point, Point fire_point) -> int
{
  ofstream output(filename);

  if (!output.is_open()) {
    cerr << "Unable to open output file" << endl;
    return 1;
  }

  if (needs_maneuver) {
    output << mid_point.x << " " << mid_point.y << " ";
  }

  output << fire_point.x << " " << fire_point.y << endl;
  output.close();
  return 0;
}
