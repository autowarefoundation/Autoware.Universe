// Copyright 2024 AutoCore, Inc.
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

#pragma once
#include "autoware/kalman_filter/kalman_filter.hpp"
#include "math.h"

using autoware::kalman_filter::KalmanFilter;

class yKalmanFilter : public KalmanFilter
{
public:
  void setX(Eigen::MatrixXd state);

  float normalize_theta(float theta);

  float yaw_correction(float pre_yaw, float obs_yaw);
};
