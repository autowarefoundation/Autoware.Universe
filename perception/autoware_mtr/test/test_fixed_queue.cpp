// Copyright 2024 TIER IV, inc.
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

#include "autoware/mtr/fixed_queue.hpp"

#include <gtest/gtest.h>

#include <cstddef>

TEST(testFixedQueue, test_name)
{
  constexpr size_t n_size = 5;
  autoware::mtr::FixedQueue<double> queue(n_size);
  EXPECT_DOUBLE_EQ(queue.front(), 0.0);
  EXPECT_DOUBLE_EQ(queue.back(), 0.0);
  EXPECT_EQ(queue.size(), n_size);

  for (size_t n = 0; n < n_size; ++n) {
    queue.push_back(n);
  }

  EXPECT_DOUBLE_EQ(queue.front(), 0.0);
  EXPECT_DOUBLE_EQ(queue.back(), 4.0);
  EXPECT_EQ(queue.size(), n_size);
}

int main(int argc, char ** argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
