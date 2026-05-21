const double g = 9.81;

const int kAmmoNameLength = 15;  // length of the longest ammo name + 1 for \0

struct Point {
  double x;
  double y;
};

struct Config {
  bool isValid = false;
  Point pos = {0, 0};
  double zd = 0.;
  Point target = {0, 0};
  double attackSpeed = 0.;
  double accelerationPath = 0.;
  char ammoName[kAmmoNameLength] = "none"; // NOLINT(*-avoid-c-arrays)
  double mass = 0.;
  double drag = 0.;
  double lift = 0.;
};

auto read_config(const char* filename) -> Config;
auto calculate_time(Config config) -> double;
auto calculate_fire_distance(Config config, double flight_time) -> double;
auto calculate_fire_point(Config config, double fire_distance, double flight_time, Point& mid_point, Point& fire_point) -> bool;
auto save_data(const char* filename, bool needs_maneuver, Point mid_point, Point fire_point) -> int;
