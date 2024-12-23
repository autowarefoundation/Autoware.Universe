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

#include "autoware/trajectory/detail/utils.hpp"

#include <gtest/gtest.h>

TEST(TestUtils, fill_bases)
{
  using autoware::trajectory::detail::fill_bases;

  std::vector<double> x = {0.0, 1.0, 2.0, 3.0};
  size_t min_points = 9;
  std::vector<double> expected = {0.0, 1.0 / 3, 2.0 / 3, 1.0, 4.0 / 3, 5.0 / 3, 2.0, 2.5, 3.0};

  auto result = fill_bases(x, min_points);

  EXPECT_EQ(result.size(), min_points);

  for (size_t i = 0; i < min_points; ++i) {
    EXPECT_NEAR(result[i], expected[i], 1e-6);
  }
}
