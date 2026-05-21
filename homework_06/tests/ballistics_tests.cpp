#include <gtest/gtest.h>
#include <filesystem>
#include <string>

#include "ballistics.hpp"

// NOLINTBEGIN(*-magic-numbers)
Config config = {.isValid = true,
                 .pos = {0, 0},
                 .zd = 100,
                 .target = {200, 200},
                 .attackSpeed = 10,
                 .accelerationPath = 10,
                 .ammoName = "VOG-17",
                 .mass = 0.35,
                 .drag = 0.07,
                 .lift = 0.0};

TEST(BallisticsSummary, ReadConfig)
{
  std::string testConfigPath = std::filesystem::current_path().append("build/debug/homework_06/data/test_config.txt").string();
  Config config = read_config(testConfigPath.c_str());
  EXPECT_TRUE(config.isValid);
  EXPECT_EQ(config.pos.x, 180);
  EXPECT_EQ(config.pos.y, 180);
  EXPECT_EQ(config.zd, 100);
  EXPECT_EQ(config.target.x, 200);
  EXPECT_EQ(config.target.y, 200);
  EXPECT_EQ(config.attackSpeed, 10);
  EXPECT_EQ(config.accelerationPath, 10);
  EXPECT_STREQ(config.ammoName, "VOG-17");
  EXPECT_EQ(config.mass, 0.35);
  EXPECT_EQ(config.drag, 0.07);
  EXPECT_EQ(config.lift, 0.0);
}

TEST(BallisticsSummary, ReadConfigIncorrectAmmo)
{
  std::string incorrectAmmoPath = std::filesystem::current_path().append("build/debug/homework_06/data/incorrect_ammo.txt").string();
  Config config = read_config(incorrectAmmoPath.c_str());
  EXPECT_FALSE(config.isValid);
}

TEST(BallisticsSummary, CalculateTimeForValidConfig)
{
  double time = calculate_time(config);
  double expectedTime = 5.74975819;
  EXPECT_NEAR(time, expectedTime, 1e-7);
}

TEST(BallisticsSummary, CalculateTimeForNegativeZD)
{
  Config configWithNegativeZD = config;
  configWithNegativeZD.zd = -100;

  double time = calculate_time(configWithNegativeZD);
  EXPECT_LT(time, 0.);
}

TEST(BallisticsSummary, CalculateFireDistanceForValidData)
{
  double fireDistance = calculate_fire_distance(config, 5.74975819);
  double expectedFireDistance = 37.110222;
  EXPECT_NEAR(fireDistance, expectedFireDistance, 1e-7);
}

TEST(BallisticsSummary, CalculateFirePointWithoutMidPoint)
{
  double fireDistance = 37.110222;
  double flightTime = 5.74975819;
  Point midPoint = {0., 0.};
  Point firePoint = {0., 0.};

  bool needMidPoint = calculate_fire_point(config, fireDistance, flightTime, midPoint, firePoint);

  double expectedFirePointX = 173.75911029;
  double expectedFirePointY = 173.75911029;
  EXPECT_FALSE(needMidPoint);
  EXPECT_NEAR(firePoint.x, expectedFirePointX, 1e-7);
  EXPECT_NEAR(firePoint.y, expectedFirePointY, 1e-7);
}

TEST(BallisticsSummary, CalculateFirePointWithMidPoint)
{
  Config configWithMidPoint = config;
  configWithMidPoint.pos = {190, 190};
  double fireDistance = 37.110222;
  double flightTime = 5.74975819;
  Point midPoint = {0., 0.};
  Point firePoint = {0., 0.};

  bool needMidPoint = calculate_fire_point(configWithMidPoint, fireDistance, flightTime, midPoint, firePoint);

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


// NOLINTEND(*-magic-numbers)