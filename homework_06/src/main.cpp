#include <cstring>
#include <cmath>
#include "../include/ballistics.hpp";

using namespace std;

/*
  Fire point and maneuver calculation for a drone.
*/
int main()
{
    Config config = readConfig("input.txt");
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