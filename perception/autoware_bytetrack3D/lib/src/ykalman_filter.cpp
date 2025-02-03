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

#include "ykalman_filter.h"

#include "math.h"

void yKalmanFilter::setX(Eigen::MatrixXd state)
{
  this->x_ = state;
}

float yKalmanFilter::normalize_theta(float theta)
{
  if (theta >= M_PI) theta -= M_PI * 2;
  if (theta < -M_PI) theta += M_PI * 2;

  return theta;
}

float yKalmanFilter::yaw_correction(float pre_yaw, float obs_yaw)
{
  obs_yaw = normalize_theta(obs_yaw);
  pre_yaw = normalize_theta(pre_yaw);

  if (std::abs(obs_yaw - pre_yaw) > M_PI / 2.0 && std::abs(obs_yaw - pre_yaw) < M_PI * 3 / 2.0) {
    obs_yaw += M_PI;
    obs_yaw = normalize_theta(obs_yaw);
  }

  if (std::abs(obs_yaw - pre_yaw) >= M_PI * 3 / 2.0) {
    obs_yaw = -obs_yaw;
    obs_yaw = normalize_theta(obs_yaw);
  }

  return obs_yaw;
}
