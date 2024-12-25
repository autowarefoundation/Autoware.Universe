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

#include "autoware/universe_utils/math/trigonometry.hpp"

#include "autoware/universe_utils/math/constants.hpp"
#include "autoware/universe_utils/math/sin_table.hpp"

#include <cmath>
#include <utility>

namespace autoware::universe_utils
{

float sin(float radian)
{
  float degree = radian * (180.f / static_cast<float>(autoware::universe_utils::pi)) *
                 (discrete_arcs_num_360 / 360.f);
  size_t idx =
    (static_cast<int>(std::round(degree)) % discrete_arcs_num_360 + discrete_arcs_num_360) %
    discrete_arcs_num_360;

  float mul = 1.f;
  if (discrete_arcs_num_90 <= idx && idx < 2 * discrete_arcs_num_90) {
    idx = 2 * discrete_arcs_num_90 - idx;
  } else if (2 * discrete_arcs_num_90 <= idx && idx < 3 * discrete_arcs_num_90) {
    mul = -1.f;
    idx = idx - 2 * discrete_arcs_num_90;
  } else if (3 * discrete_arcs_num_90 <= idx && idx < 4 * discrete_arcs_num_90) {
    mul = -1.f;
    idx = 4 * discrete_arcs_num_90 - idx;
  }

  return mul * g_sin_table[idx];
}

float cos(float radian)
{
  return sin(radian + static_cast<float>(autoware::universe_utils::pi) / 2.f);
}

std::pair<float, float> sin_and_cos(float radian)
{
  constexpr float tmp =
    (180.f / static_cast<float>(autoware::universe_utils::pi)) * (discrete_arcs_num_360 / 360.f);
  const float degree = radian * tmp;
  size_t idx =
    (static_cast<int>(std::round(degree)) % discrete_arcs_num_360 + discrete_arcs_num_360) %
    discrete_arcs_num_360;

  if (idx < discrete_arcs_num_90) {
    return {g_sin_table[idx], g_sin_table[discrete_arcs_num_90 - idx]};
  } else if (discrete_arcs_num_90 <= idx && idx < 2 * discrete_arcs_num_90) {
    idx = 2 * discrete_arcs_num_90 - idx;
    return {g_sin_table[idx], -g_sin_table[discrete_arcs_num_90 - idx]};
  } else if (2 * discrete_arcs_num_90 <= idx && idx < 3 * discrete_arcs_num_90) {
    idx = idx - 2 * discrete_arcs_num_90;
    return {-g_sin_table[idx], -g_sin_table[discrete_arcs_num_90 - idx]};
  } else {  // 3 * discrete_arcs_num_90 <= idx && idx < 4 * discrete_arcs_num_90
    idx = 4 * discrete_arcs_num_90 - idx;
    return {-g_sin_table[idx], g_sin_table[discrete_arcs_num_90 - idx]};
  }
}

// This code is modified from a part of the OpenCV project
// (https://github.com/opencv/opencv/blob/4.x/modules/core/src/mathfuncs_core.simd.hpp). It is
// subject to the license terms in the LICENSE file found in the top-level directory of this
// distribution and at http://opencv.org/license.html.
// The license can be found in
// common/autoware_universe_utils/third_party_licenses/opencv-license.md
// and https://github.com/opencv/opencv/blob/master/LICENSE

// Modification:
// 1. use autoware defined PI
// 2. output of the function is changed from degrees to radians.
namespace detail_fast_atan2
{
static const float atan2_p1 =
  0.9997878412794807f * static_cast<float>(180) / autoware::universe_utils::pi;
static const float atan2_p3 =
  -0.3258083974640975f * static_cast<float>(180) / autoware::universe_utils::pi;
static const float atan2_p5 =
  0.1555786518463281f * static_cast<float>(180) / autoware::universe_utils::pi;
static const float atan2_p7 =
  -0.04432655554792128f * static_cast<float>(180) / autoware::universe_utils::pi;
static const float atan2_DBL_EPSILON = 2.2204460492503131e-016f;
}  // namespace detail_fast_atan2

float opencv_fast_atan2(float dy, float dx)
{
  float ax = std::abs(dx);
  float ay = std::abs(dy);
  float a, c, c2;
  if (ax >= ay) {
    c = ay / (ax + detail_fast_atan2::atan2_DBL_EPSILON);
    c2 = c * c;
    a = (((detail_fast_atan2::atan2_p7 * c2 + detail_fast_atan2::atan2_p5) * c2 +
          detail_fast_atan2::atan2_p3) *
           c2 +
         detail_fast_atan2::atan2_p1) *
        c;
  } else {
    c = ax / (ay + detail_fast_atan2::atan2_DBL_EPSILON);
    c2 = c * c;
    a = 90.f - (((detail_fast_atan2::atan2_p7 * c2 + detail_fast_atan2::atan2_p5) * c2 +
                 detail_fast_atan2::atan2_p3) *
                  c2 +
                detail_fast_atan2::atan2_p1) *
                 c;
  }
  if (dx < 0) a = 180.f - a;
  if (dy < 0) a = 360.f - a;

  a = a * autoware::universe_utils::pi / 180.f;
  return a;
}

}  // namespace autoware::universe_utils
