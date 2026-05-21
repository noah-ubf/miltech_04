#include <gtest/gtest.h>

#include "ballistics.hpp"

Config config = {
  .isValid = true,
  .pos = {0, 0},
  .zd = 100,
  .target = {200, 200},
  .attackSpeed = 10,
  .accelerationPath = 10,
  .ammo_name = "VOG-17",
  .mass = 0.35,
  .drag = 0.07,
  .lift = 0.0
};

TEST(BallisticsSummary, CalculateTimeForValidConfig) {
    double time = calculateTime(config);
    double expectedTime = 5.74975819;
    EXPECT_NEAR(time, expectedTime, 1e-7);
}

TEST(BallisticsSummary, CalculateFireDistanceForValidData) {
    double fireDistance = calculateFireDistance(config, 5.74975819);
    double expectedFireDistance = 37.110222;
    EXPECT_NEAR(fireDistance, expectedFireDistance, 1e-7);
}

TEST(BallisticsSummary, CalculateFirePointWithoutMidPoint) {
    double fireDistance = 37.110222;
    double flightTime = 5.74975819;
    Point midPoint = {0., 0.};
    Point firePoint = {0., 0.};

    bool needMidPoint = calculateFirePoint(config, fireDistance, flightTime, midPoint, firePoint);

    double expectedFirePointX = 173.75911029;
    double expectedFirePointY = 173.75911029;
    EXPECT_FALSE(needMidPoint);
    EXPECT_NEAR(firePoint.x, expectedFirePointX, 1e-7);
    EXPECT_NEAR(firePoint.y, expectedFirePointY, 1e-7);
}

TEST(BallisticsSummary, CalculateFirePointWithMidPoint) {
    Config configWithMidPoint = config;
    configWithMidPoint.pos = {190, 190};
    double fireDistance = 37.110222;
    double flightTime = 5.74975819;
    Point midPoint = {0., 0.};
    Point firePoint = {0., 0.};

    bool needMidPoint = calculateFirePoint(configWithMidPoint, fireDistance, flightTime, midPoint, firePoint);

    double expectedFirePointX = 173.75911029;
    double expectedFirePointY = 173.75911029;
    double expectedMidPointX = 166.68804247;
    double expectedMidPointY = 166.68804247;
    EXPECT_TRUE(needMidPoint);
    EXPECT_NEAR(midPoint.x, expectedMidPointX, 1e-7);
    EXPECT_NEAR(midPoint.y, expectedMidPointY, 1e-7);
    EXPECT_NEAR(firePoint.x, expectedFirePointX, 1e-7);
    EXPECT_NEAR(firePoint.y, expectedFirePointY, 1e-7);
}

