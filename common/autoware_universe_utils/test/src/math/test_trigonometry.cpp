// Copyright 2023 TIER IV, Inc.
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

#include "autoware/universe_utils/math/constants.hpp"
#include "autoware/universe_utils/math/trigonometry.hpp"

#include <gtest/gtest.h>

#include <cmath>
#include <random>

TEST(trigonometry, sin)
{
  float x = 4.f * autoware::universe_utils::pi / 128.f;
  for (int i = 0; i < 128; i++) {
    EXPECT_TRUE(
      std::abs(
        std::sin(x * static_cast<float>(i)) -
        autoware::universe_utils::sin(x * static_cast<float>(i))) < 10e-5);
  }
}

TEST(trigonometry, cos)
{
  float x = 4.f * autoware::universe_utils::pi / 128.f;
  for (int i = 0; i < 128; i++) {
    EXPECT_TRUE(
      std::abs(
        std::cos(x * static_cast<float>(i)) -
        autoware::universe_utils::cos(x * static_cast<float>(i))) < 10e-5);
  }
}

TEST(trigonometry, sin_and_cos)
{
  float x = 4.f * autoware::universe_utils::pi / 128.f;
  for (int i = 0; i < 128; i++) {
    const auto sin_and_cos = autoware::universe_utils::sin_and_cos(x * static_cast<float>(i));
    EXPECT_TRUE(std::abs(std::sin(x * static_cast<float>(i)) - sin_and_cos.first) < 10e-7);
    EXPECT_TRUE(std::abs(std::cos(x * static_cast<float>(i)) - sin_and_cos.second) < 10e-7);
  }
}

float normalize_angle(double angle)
{
  const double tau = 2 * autoware::universe_utils::pi;
  double factor = std::floor(angle / tau);
  return static_cast<float>(angle - (factor * tau));
}

TEST(trigonometry, opencv_fast_atan2)
{
  // Default initialization ensures a constant seed needed for reproducible tests
  std::mt19937 gen{};

  // Generate random x and y between -10.0 and 10.0 as float
  std::uniform_real_distribution<float> dis(-10.0f, 10.0f);

  for (int i = 0; i < 100; ++i) {
    const float x = dis(gen);
    const float y = dis(gen);

    float fast_atan = autoware::universe_utils::opencv_fast_atan2(y, x);
    float std_atan = normalize_angle(std::atan2(y, x));

    // 0.3 degree accuracy
    ASSERT_NEAR(fast_atan, std_atan, 6e-3)
      << "Test failed for input (" << y << ", " << x << "): "
      << "fast atan2 = " << fast_atan << ", std::atan2 = " << std_atan;
  }
}
