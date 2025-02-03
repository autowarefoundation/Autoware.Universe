/*
 * MIT License
 *
 * Copyright (c) 2021 Yifu Zhang
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

#pragma once

#include <autoware/kalman_filter/kalman_filter.hpp>
#include <opencv2/opencv.hpp>

#include <boost/uuid/uuid.hpp>

#include <string>
#include <vector>

enum TrackState { New = 0, Tracked, Lost, Removed };
using autoware::kalman_filter::KalmanFilter;

/** manage one tracklet*/
class STrack
{
public:
  STrack(std::vector<float> in_pose, std::vector<float> in_lwh, float score, int label);
  ~STrack();

  static void multi_predict(std::vector<STrack *> & stracks);
  // void static_pose();
  void mark_lost();
  void mark_removed();
  int next_id();
  int end_frame();
  void init_kalman_filter();
  void update_kalman_filter(const Eigen::MatrixXd & measurement);
  void reflect_state();

  void activate(int frame_id);
  void re_activate(STrack & new_track, int frame_id, bool new_id = false);
  void update(STrack & new_track, int frame_id);
  // void predict(const int frame_id);
  void predict();

  void load_parameters(const std::string & filename);

  void init_kf_params();

  float normalize_theta(float theta);

  float yaw_correction(float pre_yaw, float obs_yaw);

public:
  bool is_activated;
  int track_id;
  boost::uuids::uuid unique_id;
  int state;

  // std::vector<float> original_pose;  // x,y,z,yaw
  std::vector<float> pose;      // x,y,z,yaw
  std::vector<float> lwh;       // l,w,h
  std::vector<float> velocity;  // vx,xy,vz
  float time_elapsed;
  // 构造时不需要赋值，只在最后将速度读出来

  int frame_id;
  int tracklet_len;
  int start_frame;

  float score;
  int label;

private:
  KalmanFilter kalman_filter_;
  struct KfParams
  {
    // dimension
    char dim_x = 11;
    char dim_z = 7;
    // system noise
    float q_cov_p;
    float q_cov_yaw;
    float q_cov_d;
    float q_cov_v;
    float q_cov_vyaw;
    // measurement noise
    float r_cov_p;
    float r_cov_yaw;
    float r_cov_d;
    // initial state covariance
    float p0_cov_p;
    float p0_cov_yaw;
    float p0_cov_d;
    float p0_cov_v;
    float p0_cov_vyaw;
    // other parameters
    float dt;  // sampling time
  };
  static KfParams _kf_parameters;
  static bool _parameters_loaded;
  enum IDX { X = 0, Y = 1, Z = 2, Yaw = 3, L = 4, W = 5, H = 6, VX = 7, VY = 8, VZ = 9, VYaw = 10 };

  Eigen::MatrixXd A;
  Eigen::MatrixXd u;
  Eigen::MatrixXd B;
  Eigen::MatrixXd C;
  Eigen::MatrixXd R;
  Eigen::MatrixXd Q;
  Eigen::MatrixXd P0;
};
