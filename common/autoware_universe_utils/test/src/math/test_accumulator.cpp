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

#include "autoware/universe_utils/math/accumulator.hpp"

#include <gtest/gtest.h>

TEST(accumulator, empty)
{
  autoware::universe_utils::Accumulator<double> acc;

  EXPECT_DOUBLE_EQ(acc.mean(), 0.0);
  EXPECT_DOUBLE_EQ(acc.min(), std::numeric_limits<double>::max());
  EXPECT_DOUBLE_EQ(acc.max(), std::numeric_limits<double>::lowest());
  EXPECT_EQ(acc.count(), 0);
}

TEST(accumulator, addValues)
{
  autoware::universe_utils::Accumulator<double> acc;
  acc.add(100.0);

  EXPECT_DOUBLE_EQ(acc.mean(), 100.0);
  EXPECT_DOUBLE_EQ(acc.min(), 100.0);
  EXPECT_DOUBLE_EQ(acc.max(), 100.0);
  EXPECT_EQ(acc.count(), 1);
}

TEST(accumulator, positiveValues)
{
  autoware::universe_utils::Accumulator<double> acc;
  acc.add(10.0);
  acc.add(40.0);
  acc.add(10.0);

  EXPECT_DOUBLE_EQ(acc.mean(), 20.0);
  EXPECT_DOUBLE_EQ(acc.min(), 10.0);
  EXPECT_DOUBLE_EQ(acc.max(), 40.0);
  EXPECT_EQ(acc.count(), 3);
}

TEST(accumulator, negativeValues)
{
  autoware::universe_utils::Accumulator<double> acc;
  acc.add(-10.0);
  acc.add(-40.0);
  acc.add(-10.0);

  EXPECT_DOUBLE_EQ(acc.mean(), -20.0);
  EXPECT_DOUBLE_EQ(acc.min(), -40.0);
  EXPECT_DOUBLE_EQ(acc.max(), -10.0);
  EXPECT_EQ(acc.count(), 3);
}
