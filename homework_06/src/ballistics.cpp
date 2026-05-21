#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

#include "../include/ballistics.hpp"

using namespace std;

Config readConfig(const char* filename)
{  // Input and setting initial data
  Config config;

  ifstream input(filename);

  if (!input.is_open()) {
    cout << "Error: Unable to open input file" << endl;
    config.isValid = false;
    return config;
  }

  input >> config.pos.x >> config.pos.y >> config.zd >> config.target.x >> config.target.y >> config.attackSpeed >>
    config.accelerationPath >> config.ammo_name;
  input.close();

  if (strcmp(config.ammo_name, "none") == 0) {
    // the simplest check if all data is read (all I could figure out from the lecture)
    cout << "Error reading data from file" << endl;
    config.isValid = false;
    return config;
  }
  else if (strcmp(config.ammo_name, "VOG-17") == 0) {
    config.mass = 0.35;
    config.drag = 0.07;
    config.lift = 0.0;
  }
  else if (strcmp(config.ammo_name, "M67") == 0) {
    config.mass = 0.6;
    config.drag = 0.10;
    config.lift = 0.0;
  }
  else if (strcmp(config.ammo_name, "RKG-3") == 0) {
    config.mass = 1.2;
    config.drag = 0.10;
    config.lift = 0.0;
  }
  else if (strcmp(config.ammo_name, "GLIDING-VOG") == 0) {
    config.mass = 0.45;
    config.drag = 0.10;
    config.lift = 1.0;
  }
  else if (strcmp(config.ammo_name, "GLIDING-RKG") == 0) {
    config.mass = 1.4;
    config.drag = 0.10;
    config.lift = 1.0;
  }
  else {
    cout << "Error: Unknown ammo name \"" << config.ammo_name << "\"" << endl;
    config.isValid = false;
  }
  config.isValid = true;
  return config;
}

double calculateTime(const Config config)
{
  if (!config.isValid) {
    cout << "Error: Invalid config" << endl;
    return -1.;
  }

  double a = config.drag * 9.81 * config.mass - 2 * config.drag * config.drag * config.lift * config.attackSpeed;
  if (a == 0.f) {  // a appears in denominator, so it cannot be zero
    cout << "Error: Incorrect a; no solution" << endl;
    return -1.;
  }
  double b = -3 * 9.81 * config.mass * config.mass + 3 * config.drag * config.lift * config.mass * config.attackSpeed;
  double c = 6 * config.mass * config.mass * config.zd;
  double p = -b * b / (3 * a * a);
  if (p >= 0) {  // inversed p appears as sqrt parameter, so p cannot be positive; p appears in denominator so it cannot be zero
    cout << "Error: Incorrect p; no solution" << endl;
    return -1.;
  }
  double q = 2 * b * b * b / (27 * a * a * a) + c / a;
  double sqrtP3 = sqrt(-p / 3);
  double temp = 3 * q / (2 * p * sqrtP3);
  if (temp > 1 || temp < -1) {
    cout << "Error: Argument of acos out of bounds [-1;1]" << endl;
    return -1.;
  }
  double fi = acos(temp);
  double flightTime = 2 * sqrtP3 * cos((fi + 4 * M_PI) / 3) - b / (3 * a);
  if (flightTime <= 0) {
    cout << "Error: negative flight time" << endl;
    return -1.;
  }
  return flightTime;
}

double calculateFireDistance(const Config config, const double flightTime)
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
  double ft = flightTime;
  double ft2 = ft * ft;
  double ft3 = ft2 * ft;
  double ft4 = ft3 * ft;
  double ft5 = ft4 * ft;
  double dglm = d * l * g * m;
  double v = config.attackSpeed;

  double fireDistance =
    ft * v - ft2 * d * v / (2 * m) + ft3 * (6 * dglm - 6 * d2 * (l2 - 1) * v) / (36 * m2) +
    ft4 * (-6 * d * dglm * (1 + l2 + l4) + 3 * d3 * l2 * (1 + l2) * v + 6 * d3 * l4 * (1 + l2) * v) / (36 * (1 + l2) * (1 + l2) * m3) +
    ft5 * (3 * d2 * dglm * l2 - 3 * d4 * l2 * (1 + l2) * v) / (36 * (1 + l2) * m4);

  if (fireDistance <= 0) {
    cout << "Error: negative fire distance" << endl;
  }

  return fireDistance;
}

bool calculateFirePoint(const Config config, const double fireDistance, const double flightTime, Point& midPoint, Point& firePoint)
{
  double distance(0.);
  bool needsManeuver(false);
  // Calculating distances, intermediate point and fire point:
  double minDistance = fireDistance + config.accelerationPath;
  if (config.target.x == config.pos.x && config.target.y == config.pos.y) {
    cout << "Initial distance is zero - maneuvering to an arbitrary direction, e.g. to negative x" << endl;
    midPoint.x = config.target.x - minDistance;
    midPoint.y = config.target.y;
    needsManeuver = true;
    firePoint.x = config.target.x - fireDistance;
    firePoint.y = config.target.y;
  }
  else {
    // non-zero distance between the drone and the target
    // - moving along the line that connects these two points
    double deltaX = config.target.x - config.pos.x, deltaY = config.target.y - config.pos.y;
    distance = sqrt(deltaX * deltaX + deltaY * deltaY);
    // unit vector:
    double unitX = deltaX / distance, unitY = deltaY / distance;

    if (distance < minDistance) {
      cout << "Too close - need a maneuver" << endl;
      needsManeuver = true;
      midPoint.x = config.target.x - minDistance * unitX;
      midPoint.y = config.target.y - minDistance * unitY;
    }

    // calculating the fire point coordinates:
    firePoint.x = config.target.x - fireDistance * unitX;
    firePoint.y = config.target.y - fireDistance * unitY;
  }

  return needsManeuver;
}

int saveData(const char* filename, const bool needsManeuver, const Point midPoint, const Point firePoint)
{
  ofstream output(filename);

  if (!output.is_open()) {
    cout << "Unable to open output file" << endl;
    return 1;
  }

  if (needsManeuver) {
    output << midPoint.x << " " << midPoint.y << " ";
  }

  output << firePoint.x << " " << firePoint.y << endl;
  output.close();
  return 0;
}
