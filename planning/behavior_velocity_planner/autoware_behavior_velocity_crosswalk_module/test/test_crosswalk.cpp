// Copyright 2024 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "../src/scene_crosswalk.hpp"

#include <gtest/gtest.h>

#include <vector>

TEST(CrosswalkTest, CheckInterpolateEgoPassMargin)
{
  namespace bvp = autoware::behavior_velocity_planner;

  std::vector<double> x_vec{0.0, 1.0, 10.0};
  std::vector<double> y_vec{0.0, -1.0, 8.0};

  EXPECT_EQ(0.0, bvp::interpolateEgoPassMargin(x_vec, y_vec, -100.0));
  EXPECT_EQ(0.0, bvp::interpolateEgoPassMargin(x_vec, y_vec, 0.0));
  EXPECT_EQ(-0.5, bvp::interpolateEgoPassMargin(x_vec, y_vec, 0.5));
  EXPECT_EQ(-1.0, bvp::interpolateEgoPassMargin(x_vec, y_vec, 1.0));
  EXPECT_EQ(-0.5, bvp::interpolateEgoPassMargin(x_vec, y_vec, 1.5));
  EXPECT_EQ(8.0, bvp::interpolateEgoPassMargin(x_vec, y_vec, 10.0));
  EXPECT_EQ(8.0, bvp::interpolateEgoPassMargin(x_vec, y_vec, 100.0));
}

TEST(CrosswalkTest, CheckInterpolateMap)
{
  namespace bvp = autoware::behavior_velocity_planner;

  std::vector<double> x_vec{0.0, 1.0, 10.0};
  std::vector<double> y_vec{0.0, -1.0, 8.0};

  EXPECT_EQ(0.0, bvp::InterpolateMap(x_vec, y_vec, -100.0));
  EXPECT_EQ(0.0, bvp::InterpolateMap(x_vec, y_vec, 0.0));
  EXPECT_EQ(-0.5, bvp::InterpolateMap(x_vec, y_vec, 0.5));
  EXPECT_EQ(-1.0, bvp::InterpolateMap(x_vec, y_vec, 1.0));
  EXPECT_EQ(-0.5, bvp::InterpolateMap(x_vec, y_vec, 1.5));
  EXPECT_EQ(8.0, bvp::InterpolateMap(x_vec, y_vec, 10.0));
  EXPECT_EQ(8.0, bvp::InterpolateMap(x_vec, y_vec, 100.0));
}
