// Copyright 2024 Tier IV, Inc.
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

#include "autoware/trajectory/interpolator/akima_spline.hpp"
#include "autoware/trajectory/interpolator/cubic_spline.hpp"
#include "autoware/trajectory/interpolator/linear.hpp"
#include "autoware/trajectory/interpolator/nearest_neighbor.hpp"
#include "autoware/trajectory/interpolator/spherical_linear.hpp"
#include "autoware/trajectory/interpolator/stairstep.hpp"

#include <geometry_msgs/msg/quaternion.hpp>

#include <gtest/gtest.h>

#include <optional>
#include <random>
#include <vector>

template <class Interpolator>
class TestInterpolator : public ::testing::Test
{
public:
  std::optional<Interpolator> interpolator;
  std::vector<double> bases;
  std::vector<double> values;

  void SetUp() override
  {
    // generate random values -1 to 1
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1, 1);
    bases.resize(10);
    values.resize(10);
    for (size_t i = 0; i < bases.size(); ++i) {
      bases[i] = static_cast<double>(i);
      values[i] = dis(gen);
    }
  }
};

using Interpolators = testing::Types<
  autoware::trajectory::interpolator::CubicSpline, autoware::trajectory::interpolator::AkimaSpline,
  autoware::trajectory::interpolator::Linear,
  autoware::trajectory::interpolator::NearestNeighbor<double>,
  autoware::trajectory::interpolator::Stairstep<double>>;

TYPED_TEST_SUITE(TestInterpolator, Interpolators, );

TYPED_TEST(TestInterpolator, compute)
{
  this->interpolator =
    typename TypeParam::Builder().set_bases(this->bases).set_values(this->values).build();
  for (size_t i = 0; i < this->bases.size(); ++i) {
    EXPECT_NEAR(this->values[i], this->interpolator->compute(this->bases[i]), 1e-6);
  }
}

// Instantiate test cases for all interpolators
template class TestInterpolator<autoware::trajectory::interpolator::CubicSpline>;
template class TestInterpolator<autoware::trajectory::interpolator::AkimaSpline>;
template class TestInterpolator<autoware::trajectory::interpolator::Linear>;
template class TestInterpolator<autoware::trajectory::interpolator::NearestNeighbor<double>>;
template class TestInterpolator<autoware::trajectory::interpolator::Stairstep<double>>;

/*
 * Test SphericalLinear interpolator
 */

geometry_msgs::msg::Quaternion create_quaternion(double w, double x, double y, double z)
{
  geometry_msgs::msg::Quaternion q;
  q.w = w;
  q.x = x;
  q.y = y;
  q.z = z;
  return q;
}

TEST(TestSphericalLinearInterpolator, compute)
{
  using autoware::trajectory::interpolator::SphericalLinear;

  std::vector<double> bases = {0.0, 1.0};
  std::vector<geometry_msgs::msg::Quaternion> quaternions = {
    create_quaternion(1.0, 0.0, 0.0, 0.0), create_quaternion(0.0, 1.0, 0.0, 0.0)};

  auto interpolator = autoware::trajectory::interpolator::SphericalLinear::Builder()
                        .set_bases(bases)
                        .set_values(quaternions)
                        .build();

  if (!interpolator) {
    FAIL();
  }

  double s = 0.5;
  geometry_msgs::msg::Quaternion result = interpolator->compute(s);

  // Expected values (from SLERP calculation)
  double expected_w = std::sqrt(2.0) / 2.0;
  double expected_x = std::sqrt(2.0) / 2.0;
  double expected_y = 0.0;
  double expected_z = 0.0;

  EXPECT_NEAR(result.w, expected_w, 1e-6);
  EXPECT_NEAR(result.x, expected_x, 1e-6);
  EXPECT_NEAR(result.y, expected_y, 1e-6);
  EXPECT_NEAR(result.z, expected_z, 1e-6);
}
