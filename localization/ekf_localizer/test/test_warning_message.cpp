// Copyright 2023 Autoware Foundation
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

#include "ekf_localizer/warning_message.hpp"

#include <gtest/gtest.h>

TEST(PoseDelayTimeWarningMessage, SmokeTest)
{
  EXPECT_STREQ(
    poseDelayTimeWarningMessage(-1.0).c_str(),
    "Pose time stamp is inappropriate, set delay to 0[s]. delay = -1.000");
  EXPECT_STREQ(
    poseDelayTimeWarningMessage(-0.4).c_str(),
    "Pose time stamp is inappropriate, set delay to 0[s]. delay = -0.400");
}

TEST(TwistDelayTimeWarningMessage, SmokeTest)
{
  EXPECT_STREQ(
    twistDelayTimeWarningMessage(-1.0).c_str(),
    "Twist time stamp is inappropriate, set delay to 0[s]. delay = -1.000");
  EXPECT_STREQ(
    twistDelayTimeWarningMessage(-0.4).c_str(),
    "Twist time stamp is inappropriate, set delay to 0[s]. delay = -0.400");
}

TEST(MahalanobisWarningMessage, SmokeTest)
{
  EXPECT_STREQ(
    mahalanobisWarningMessage(1.0, 0.5).c_str(),
    "The Mahalanobis distance 1.0000 is over the limit 0.5000.");
}
