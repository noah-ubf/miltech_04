const double g = 9.81;

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
  char ammo_name[15] = "none";  // size = longest name + 1 for \0 + some extra space just in case
  double mass = 0.;
  double drag = 0.;
  double lift = 0.;
};

Config readConfig(const char* filename);
double calculateTime(Config config);
double calculateFireDistance(Config config, double flightTime);
bool calculateFirePoint(Config config, double fireDistance, double flightTime, Point& midPoint, Point& firePoint);
int saveData(const char* filename, bool needsManeuver, Point midPoint, Point firePoint);
