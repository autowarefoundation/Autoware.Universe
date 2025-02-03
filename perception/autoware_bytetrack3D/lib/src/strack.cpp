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

#include "strack.h"

#include <ament_index_cpp/get_package_share_directory.hpp>

#include <boost/uuid/uuid_generators.hpp>

#include <yaml-cpp/yaml.h>

// init static variable
bool STrack::_parameters_loaded = false;
STrack::KfParams STrack::_kf_parameters;

STrack::STrack(std::vector<float> in_pose, std::vector<float> in_lwh, float score, int label)
{
  this->pose = std::move(in_pose);
  this->lwh = std::move(in_lwh);
  velocity.resize(4, 0);

  is_activated = false;
  track_id = 0;
  state = TrackState::New;

  frame_id = 0;
  tracklet_len = 0;
  this->score = score;
  start_frame = 0;
  this->label = label;

  // load static kf parameters: initialized once in program
  const std::string package_share_directory =
    ament_index_cpp::get_package_share_directory("autoware_bytetrack3d");
  const std::string default_config_path =
    package_share_directory + "/config/kalman_filter.param.yaml";
  if (!_parameters_loaded) {
    load_parameters(default_config_path);
    _parameters_loaded = true;
  }

  init_kf_params();

  init_kalman_filter();
}

STrack::~STrack()
{
}

void STrack::init_kalman_filter()
{
  // assert parameter is loaded
  assert(_parameters_loaded);

  // init kalman filter state
  Eigen::MatrixXd X0 = Eigen::MatrixXd::Zero(_kf_parameters.dim_x, 1);

  X0(IDX::X) = this->pose[0];
  X0(IDX::Y) = this->pose[1];
  X0(IDX::Z) = this->pose[2];
  X0(IDX::Yaw) = this->pose[3];
  X0(IDX::L) = this->lwh[0];
  X0(IDX::W) = this->lwh[1];
  X0(IDX::H) = this->lwh[2];
  X0(IDX::VX) = 0.0;
  X0(IDX::VY) = 0.0;
  X0(IDX::VZ) = 0.0;
  X0(IDX::VYaw) = 0.0;

  this->kalman_filter_.init(X0, P0);
}

/** init a tracklet */
void STrack::activate(int frame_id)
{
  this->track_id = this->next_id();
  this->unique_id = boost::uuids::random_generator()();

  this->tracklet_len = 1;
  this->state = TrackState::Tracked;
  this->is_activated = true;
  this->frame_id = frame_id;
  this->start_frame = frame_id;
}

void STrack::re_activate(STrack & new_track, int frame_id, bool new_id)
{
  Eigen::MatrixXd measurement = Eigen::MatrixXd::Zero(_kf_parameters.dim_z, 1);
  measurement << new_track.pose[0], new_track.pose[1], new_track.pose[2], new_track.pose[3],
    new_track.lwh[0], new_track.lwh[1], new_track.lwh[2];

  measurement(3) = yaw_correction(kalman_filter_.getXelement(IDX::Yaw), new_track.pose[3]);

  update_kalman_filter(measurement);

  if (std::abs(measurement(3) - this->pose[3]) > M_PI * 0.1) {
    this->pose[3] = measurement(3);
  }

  reflect_state();

  this->tracklet_len = 1;
  this->state = TrackState::Tracked;
  this->is_activated = true;
  this->frame_id = frame_id;
  this->score = new_track.score;
  if (new_id) {
    this->track_id = next_id();
    this->unique_id = boost::uuids::random_generator()();
  }
}

void STrack::mark_lost()
{
  state = TrackState::Lost;
}

void STrack::mark_removed()
{
  state = TrackState::Removed;
}

int STrack::next_id()
{
  static int _count = 0;
  _count++;
  return _count;
}

int STrack::end_frame()
{
  return this->frame_id;
}

void STrack::update(STrack & new_track, int frame_id)
{
  this->frame_id = frame_id;
  this->tracklet_len++;

  // update

  Eigen::MatrixXd measurement = Eigen::MatrixXd::Zero(_kf_parameters.dim_z, 1);
  measurement << new_track.pose[0], new_track.pose[1], new_track.pose[2], new_track.pose[3],
    new_track.lwh[0], new_track.lwh[1], new_track.lwh[2];

  measurement(3) = yaw_correction(kalman_filter_.getXelement(IDX::Yaw), new_track.pose[3]);

  update_kalman_filter(measurement);

  Eigen::MatrixXd state = Eigen::MatrixXd::Zero(_kf_parameters.dim_x, 1);

  reflect_state();

  if (std::abs(measurement(3) - this->pose[3]) > M_PI * 0.1) {
    this->pose[3] = measurement(3);
  }

  this->state = TrackState::Tracked;
  this->is_activated = true;

  this->score = new_track.score;
}

/** reflect kalman filter state to current object variables*/
void STrack::reflect_state()
{
  Eigen::MatrixXd state = Eigen::MatrixXd::Zero(_kf_parameters.dim_x, 1);
  this->kalman_filter_.getX(state);
  this->pose[0] = state(IDX::X);
  this->pose[1] = state(IDX::Y);
  this->pose[2] = state(IDX::Z);

  this->pose[3] = state(IDX::Yaw);

  this->lwh[0] = state(IDX::L);
  this->lwh[1] = state(IDX::W);
  this->lwh[2] = state(IDX::H);
  this->velocity[0] = state(IDX::VX);
  this->velocity[1] = state(IDX::VY);
  this->velocity[2] = state(IDX::VZ);
}

void STrack::multi_predict(std::vector<STrack *> & stracks)
{
  for (size_t i = 0; i < stracks.size(); i++) {
    stracks[i]->predict();
  }
}

void STrack::update_kalman_filter(const Eigen::MatrixXd & measurement)
{
  // assert parameter is loaded
  assert(_parameters_loaded);

  // update
  if (!this->kalman_filter_.update(measurement, C, R)) {
    std::cerr << "Cannot update" << std::endl;
  }
}

void STrack::predict()
{
  // prediction
  if (!this->kalman_filter_.predict(u, A, B, Q)) {
    std::cerr << "Cannot predict" << std::endl;
  }
  reflect_state();
}

void STrack::load_parameters(const std::string & path)
{
  YAML::Node config = YAML::LoadFile(path);
  // initialize ekf params
  _kf_parameters.dim_x = config["dim_x"].as<int>();
  _kf_parameters.dim_z = config["dim_z"].as<int>();
  _kf_parameters.q_cov_p = config["q_cov_p"].as<float>();
  _kf_parameters.q_cov_yaw = config["q_cov_yaw"].as<float>();
  _kf_parameters.q_cov_d = config["q_cov_d"].as<float>();
  _kf_parameters.q_cov_v = config["q_cov_v"].as<float>();
  _kf_parameters.q_cov_vyaw = config["q_cov_vyaw"].as<float>();
  _kf_parameters.r_cov_p = config["r_cov_p"].as<float>();
  _kf_parameters.r_cov_yaw = config["r_cov_yaw"].as<float>();
  _kf_parameters.r_cov_d = config["r_cov_d"].as<float>();
  _kf_parameters.p0_cov_p = config["p0_cov_p"].as<float>();
  _kf_parameters.p0_cov_yaw = config["p0_cov_yaw"].as<float>();
  _kf_parameters.p0_cov_d = config["p0_cov_d"].as<float>();
  _kf_parameters.p0_cov_v = config["p0_cov_v"].as<float>();
  _kf_parameters.p0_cov_vyaw = config["p0_cov_vyaw"].as<float>();

  _kf_parameters.dt = config["dt"].as<float>();
}

void STrack::init_kf_params()
{
  A = Eigen::MatrixXd::Identity(_kf_parameters.dim_x, _kf_parameters.dim_x);

  A(IDX::X, IDX::VX) = _kf_parameters.dt;
  A(IDX::Y, IDX::VY) = _kf_parameters.dt;
  A(IDX::Z, IDX::VZ) = _kf_parameters.dt;
  A(IDX::Yaw, IDX::VYaw) = _kf_parameters.dt;

  u = Eigen::MatrixXd::Zero(_kf_parameters.dim_x, 1);
  B = Eigen::MatrixXd::Zero(_kf_parameters.dim_x, _kf_parameters.dim_x);

  C = Eigen::MatrixXd::Zero(_kf_parameters.dim_z, _kf_parameters.dim_x);
  C(IDX::X, IDX::X) = 1;
  C(IDX::Y, IDX::Y) = 1;
  C(IDX::Z, IDX::Z) = 1;
  C(IDX::Yaw, IDX::Yaw) = 1;
  C(IDX::L, IDX::L) = 1;
  C(IDX::W, IDX::W) = 1;
  C(IDX::H, IDX::H) = 1;

  R = Eigen::MatrixXd::Zero(_kf_parameters.dim_z, _kf_parameters.dim_z);
  R(IDX::X, IDX::X) = _kf_parameters.r_cov_p;
  R(IDX::Y, IDX::Y) = _kf_parameters.r_cov_p;
  R(IDX::Z, IDX::Z) = _kf_parameters.r_cov_p;
  R(IDX::Yaw, IDX::Yaw) = _kf_parameters.r_cov_yaw;
  R(IDX::L, IDX::L) = _kf_parameters.r_cov_d;
  R(IDX::W, IDX::W) = _kf_parameters.r_cov_d;
  R(IDX::H, IDX::H) = _kf_parameters.r_cov_d;

  Q = Eigen::MatrixXd::Zero(_kf_parameters.dim_x, _kf_parameters.dim_x);
  Q(IDX::X, IDX::X) = _kf_parameters.q_cov_p;
  Q(IDX::Y, IDX::Y) = _kf_parameters.q_cov_p;
  Q(IDX::Z, IDX::Z) = _kf_parameters.q_cov_p;
  Q(IDX::Yaw, IDX::Yaw) = _kf_parameters.q_cov_yaw;
  Q(IDX::L, IDX::L) = _kf_parameters.q_cov_d;
  Q(IDX::W, IDX::W) = _kf_parameters.q_cov_d;
  Q(IDX::H, IDX::H) = _kf_parameters.q_cov_d;
  Q(IDX::VX, IDX::VX) = _kf_parameters.q_cov_v;
  Q(IDX::VY, IDX::VY) = _kf_parameters.q_cov_v;
  Q(IDX::VZ, IDX::VZ) = _kf_parameters.q_cov_v;
  Q(IDX::VYaw, IDX::VYaw) = _kf_parameters.q_cov_vyaw;

  P0 = Eigen::MatrixXd::Zero(_kf_parameters.dim_x, _kf_parameters.dim_x);
  P0(IDX::X, IDX::X) = _kf_parameters.p0_cov_p;
  P0(IDX::Y, IDX::Y) = _kf_parameters.p0_cov_p;
  P0(IDX::Z, IDX::Z) = _kf_parameters.p0_cov_p;
  P0(IDX::Yaw, IDX::Yaw) = _kf_parameters.p0_cov_yaw;
  P0(IDX::L, IDX::L) = _kf_parameters.p0_cov_d;
  P0(IDX::W, IDX::W) = _kf_parameters.p0_cov_d;
  P0(IDX::H, IDX::H) = _kf_parameters.p0_cov_d;
  P0(IDX::VX, IDX::VX) = _kf_parameters.p0_cov_v;
  P0(IDX::VY, IDX::VY) = _kf_parameters.p0_cov_v;
  P0(IDX::VZ, IDX::VZ) = _kf_parameters.p0_cov_v;
  P0(IDX::VYaw, IDX::VYaw) = _kf_parameters.p0_cov_vyaw;
}

float STrack::normalize_theta(float theta)
{
  if (theta >= M_PI) theta -= M_PI * 2;
  if (theta < -M_PI) theta += M_PI * 2;

  return theta;
}

float STrack::yaw_correction(float pre_yaw, float obs_yaw)
{
  obs_yaw = normalize_theta(obs_yaw);
  pre_yaw = normalize_theta(pre_yaw);

  if (std::abs(obs_yaw - pre_yaw) >= M_PI / 2) {
    float ref_vel;
    bool wrong_obs = true;
    if (std::abs(obs_yaw - M_PI / 2) <= M_PI * 2 / 3) {
      ref_vel = this->velocity[0];
      if (std::abs(ref_vel) > 0.2) {
        wrong_obs = (std::abs(obs_yaw) - M_PI / 2) > 0.0;
      }

    } else {
      ref_vel = this->velocity[1];
      if (std::abs(ref_vel) > 1.0) {
        wrong_obs = (std::abs(obs_yaw - M_PI / 2) - M_PI / 2.0) > 0.0;
      }
    }
    if (wrong_obs) {
      if (
        std::abs(obs_yaw - pre_yaw) > M_PI / 2.0 && std::abs(obs_yaw - pre_yaw) < M_PI * 3 / 2.0) {
        if (obs_yaw < pre_yaw) {
          obs_yaw += M_PI;
        } else {
          obs_yaw -= M_PI;
        }
      }

      if (std::abs(obs_yaw - pre_yaw) >= M_PI * 3 / 2.0) {
        obs_yaw = -obs_yaw;
      }
    }
    obs_yaw = normalize_theta(obs_yaw);
  }
  return obs_yaw;
}
