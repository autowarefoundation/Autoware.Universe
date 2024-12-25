// Copyright 2024 The Autoware Foundation.
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

#include "simple_planning_simulator/utils/mechanical_controller.hpp"

#include <algorithm>
#include <cmath>
#include <deque>
#include <iostream>
#include <map>
#include <numeric>
#include <optional>
#include <utility>
#include <vector>

namespace autoware::simple_planning_simulator::utils::mechanical_controller
{

using DelayBuffer = std::deque<std::pair<double, double>>;
using DelayOutput = std::pair<std::optional<double>, DelayBuffer>;

DelayOutput delay(
  const double signal, const double delay_time, const DelayBuffer & buffer,
  const double elapsed_time)
{
  DelayBuffer new_buffer = buffer;

  new_buffer.push_back(std::make_pair(signal, elapsed_time));

  if (!buffer.empty() && (elapsed_time - new_buffer.front().second) >= delay_time) {
    const double delayed_signal = new_buffer.front().first;
    new_buffer.pop_front();
    return {delayed_signal, new_buffer};
  } else {
    return {std::nullopt, new_buffer};
  }
}

double sign(const double x)
{
  return (x >= 0.0) ? 1.0 : -1.0;
}

double apply_limits(
  const double current_angle, const double previous_angle, const double angle_limit,
  const double rate_limit, const double dt)
{
  const double angle_diff = std::clamp(current_angle, -angle_limit, angle_limit) - previous_angle;
  const double rate_limited_diff = std::clamp(angle_diff, -rate_limit * dt, rate_limit * dt);
  return std::clamp(previous_angle + rate_limited_diff, -angle_limit, angle_limit);
}

double feedforward(const double input_angle, const double ff_gain)
{
  return ff_gain * input_angle;
}

double polynomial_transform(
  const double torque, const double speed, const double a, const double b, const double c,
  const double d, const double e, const double f, const double g, const double h)
{
  return a * torque * torque * torque + b * torque * torque + c * torque +
         d * speed * speed * speed + e * speed * speed + f * speed + g * torque * speed + h;
}

PIDController::PIDController(const double kp, const double ki, const double kd)
: kp_(kp), ki_(ki), kd_(kd), state_{0.0, 0.0}
{
}

double PIDController::compute(
  const double error, const double integral, const double prev_error, const double dt) const
{
  const double p_term = kp_ * error;
  const double i_term = ki_ * integral;
  const double d_term = dt < 1e-6 ? 0.0 : kd_ * (error - prev_error) / dt;

  return p_term + i_term + d_term;
}

void PIDController::update_state(const double error, const double dt)
{
  state_.integral += error * dt;
  state_.error = error;
};

void PIDController::update_state(
  const double k1_error, const double k2_error, const double k3_error, const double k4_error,
  const double dt)
{
  state_.error = (k1_error + 2 * k2_error + 2 * k3_error + k4_error) / 6.0;
  state_.integral += state_.error * dt;
};

PIDControllerState PIDController::get_state() const
{
  return state_;
}

void PIDController::clear_state()
{
  state_ = {0.0, 0.0};
}

SteeringDynamics::SteeringDynamics(
  const double angular_position, const double angular_velocity, const double inertia,
  const double damping, const double stiffness, const double friction,
  const double dead_zone_threshold)
: state_{angular_position, angular_velocity, false},
  inertia_(inertia),
  damping_(damping),
  stiffness_(stiffness),
  friction_(friction),
  dead_zone_threshold_(dead_zone_threshold)
{
}

bool SteeringDynamics::is_in_dead_zone(
  const SteeringDynamicsState & state, const double input_torque) const
{
  bool is_in_dead_zone = state.is_in_dead_zone;
  const int rotation_direction = sign(state.angular_velocity);
  const int input_direction = sign(input_torque);

  if (input_direction != rotation_direction && std::abs(input_torque) < dead_zone_threshold_) {
    return true;
  }

  if (is_in_dead_zone) {
    return !(dead_zone_threshold_ <= std::abs(input_torque));
  }

  return false;
}

SteeringDynamicsDeltaState SteeringDynamics::calc_model(
  const SteeringDynamicsState & state, const double input_torque) const
{
  const double friction_force = friction_ * sign(state.angular_velocity);
  const double angular_acceleration = (input_torque - damping_ * state.angular_velocity -
                                       stiffness_ * state.angular_position - friction_force) /
                                      inertia_;

  const double d_angular_velocity = angular_acceleration;
  const double d_angular_position = state.angular_velocity;

  return {d_angular_position, d_angular_velocity};
}

void SteeringDynamics::set_state(const SteeringDynamicsState & state)
{
  state_ = state;
}

SteeringDynamicsState SteeringDynamics::get_state() const
{
  return state_;
}

void SteeringDynamics::set_steer(const double steer)
{
  state_.angular_position = steer;
}

void SteeringDynamics::clear_state()
{
  state_ = {0.0, 0.0, false};
}

MechanicalController::MechanicalController(const MechanicalParams & params)
: pid_(params.kp, params.ki, params.kd),
  steering_dynamics_(
    0.0, 0.0, params.inertia, params.damping, params.stiffness, params.friction,
    params.dead_zone_threshold),
  params_(params)
{
}

void MechanicalController::clear_state()
{
  delay_buffer_.clear();
  pid_.clear_state();
  steering_dynamics_.clear_state();
}

void MechanicalController::set_steer(const double steer)
{
  steering_dynamics_.set_steer(steer);
}

[[maybe_unused]] double MechanicalController::update_euler(
  const double input_angle, const double speed, const double prev_input_angle, const double dt)
{
  const auto dynamics_state = steering_dynamics_.get_state();

  const auto d_state =
    run_one_step(input_angle, speed, prev_input_angle, dt, delay_buffer_, pid_, steering_dynamics_);

  const double d_angular_position = d_state.dynamics_d_state.d_angular_position;
  const double d_angular_velocity = d_state.dynamics_d_state.d_angular_velocity;

  auto dynamics_state_new = dynamics_state;
  dynamics_state_new.angular_position = std::clamp(
    dynamics_state.angular_position + d_angular_position * dt, -params_.angle_limit,
    params_.angle_limit);
  dynamics_state_new.angular_velocity = std::clamp(
    dynamics_state.angular_velocity + d_angular_velocity * dt, -params_.rate_limit,
    params_.rate_limit);
  dynamics_state_new.is_in_dead_zone = d_state.is_in_dead_zone;
  steering_dynamics_.set_state(dynamics_state_new);

  pid_.update_state(d_state.pid_error, dt);
  delay_buffer_ = d_state.delay_buffer;

  return dynamics_state_new.angular_position;
}

double MechanicalController::update_runge_kutta(
  const double input_angle, const double speed, const double prev_input_angle, const double dt)
{
  const auto dynamics_state = steering_dynamics_.get_state();

  // NOTE: k1, k2, k3, k4 suffix denote the intermediate points of RK4
  const auto k1 =
    run_one_step(input_angle, speed, prev_input_angle, dt, delay_buffer_, pid_, steering_dynamics_);

  auto dynamics_for_k2 = steering_dynamics_;
  auto dynamics_state_for_k2 = steering_dynamics_.get_state();
  dynamics_state_for_k2.angular_position =
    dynamics_state.angular_position + k1.dynamics_d_state.d_angular_position * 0.5 * dt;
  dynamics_state_for_k2.angular_velocity =
    dynamics_state.angular_velocity + k1.dynamics_d_state.d_angular_velocity * 0.5 * dt;
  dynamics_for_k2.set_state(dynamics_state_for_k2);
  const auto k2 =
    run_one_step(input_angle, speed, prev_input_angle, dt, delay_buffer_, pid_, dynamics_for_k2);

  auto dynamics_for_k3 = steering_dynamics_;
  auto dynamics_state_for_k3 = steering_dynamics_.get_state();
  dynamics_state_for_k3.angular_position =
    dynamics_state.angular_position + k2.dynamics_d_state.d_angular_position * 0.5 * dt;
  dynamics_state_for_k3.angular_velocity =
    dynamics_state.angular_velocity + k2.dynamics_d_state.d_angular_velocity * 0.5 * dt;
  dynamics_for_k3.set_state(dynamics_state_for_k3);
  const auto k3 =
    run_one_step(input_angle, speed, prev_input_angle, dt, delay_buffer_, pid_, dynamics_for_k3);

  auto dynamics_for_k4 = steering_dynamics_;
  auto dynamics_state_for_k4 = steering_dynamics_.get_state();
  dynamics_state_for_k4.angular_position =
    dynamics_state.angular_position + k3.dynamics_d_state.d_angular_position * dt;
  dynamics_state_for_k4.angular_velocity =
    dynamics_state.angular_velocity + k3.dynamics_d_state.d_angular_velocity * dt;
  dynamics_for_k4.set_state(dynamics_state_for_k4);
  const auto k4 =
    run_one_step(input_angle, speed, prev_input_angle, dt, delay_buffer_, pid_, dynamics_for_k4);

  const double d_angular_position =
    (k1.dynamics_d_state.d_angular_position + 2.0 * k2.dynamics_d_state.d_angular_position +
     2.0 * k3.dynamics_d_state.d_angular_position + k4.dynamics_d_state.d_angular_position) /
    6.0;
  const double d_angular_velocity =
    (k1.dynamics_d_state.d_angular_velocity + 2.0 * k2.dynamics_d_state.d_angular_velocity +
     2.0 * k3.dynamics_d_state.d_angular_velocity + k4.dynamics_d_state.d_angular_velocity) /
    6.0;

  // update steering dynamics/controller internal state
  auto dynamics_state_new = dynamics_state;
  dynamics_state_new.angular_position = std::clamp(
    dynamics_state.angular_position + d_angular_position * dt, -params_.angle_limit,
    params_.angle_limit);
  dynamics_state_new.angular_velocity = std::clamp(
    dynamics_state.angular_velocity + d_angular_velocity * dt, -params_.rate_limit,
    params_.rate_limit);
  pid_.update_state(k1.pid_error, k2.pid_error, k3.pid_error, k4.pid_error, dt);
  if (
    k1.delay_buffer.empty() || k2.delay_buffer.empty() || k3.delay_buffer.empty() ||
    k4.delay_buffer.empty()) {
    // This condition is assumed to never be met because it is always pushed by
    // the delay() function.
    return dynamics_state.angular_position;
  }
  const double delayed_signal =
    (k1.delay_buffer.back().first + 2.0 * k2.delay_buffer.back().first +
     2.0 * k3.delay_buffer.back().first + k4.delay_buffer.back().first) /
    6.0;
  const double elapsed_time = delay_buffer_.empty() ? dt : delay_buffer_.back().second + dt;
  delay_buffer_ =
    delay(delayed_signal, params_.torque_delay_time, delay_buffer_, elapsed_time).second;
  dynamics_state_new.is_in_dead_zone =
    steering_dynamics_.is_in_dead_zone(dynamics_state_new, delayed_signal);
  steering_dynamics_.set_state(dynamics_state_new);

  return dynamics_state_new.angular_position;
}

StepResult MechanicalController::run_one_step(
  const double input_angle, const double speed, const double prev_input_angle, const double dt,
  const DelayBuffer & delay_buffer, const PIDController & pid,
  const SteeringDynamics & dynamics) const
{
  const auto dynamics_state = dynamics.get_state();
  const auto pid_state = pid.get_state();

  const double limited_input_angle =
    apply_limits(input_angle, prev_input_angle, params_.angle_limit, params_.rate_limit, dt);

  const double ff_torque = feedforward(limited_input_angle, params_.ff_gain);

  const double pid_error = limited_input_angle - dynamics_state.angular_position;

  const double pid_torque =
    pid.compute(pid_error, pid_state.integral + pid_error * dt, pid_state.error, dt);

  const double total_torque = ff_torque + pid_torque;

  // NOTE: do not distinguish between forward and backward driving
  const double steering_torque = std::clamp(
    polynomial_transform(
      total_torque, std::abs(speed), params_.poly_a, params_.poly_b, params_.poly_c, params_.poly_d,
      params_.poly_e, params_.poly_f, params_.poly_g, params_.poly_h),
    -params_.steering_torque_limit, params_.steering_torque_limit);

  const double elapsed_time = delay_buffer.empty() ? dt : delay_buffer.back().second + dt;
  const auto [delayed_torque_opt, delay_buffer_new] =
    delay(steering_torque, params_.torque_delay_time, delay_buffer, elapsed_time);

  if (!delayed_torque_opt.has_value()) {
    return {delay_buffer_new, pid_error, {0.0, 0.0}, dynamics_state.is_in_dead_zone};
  }

  const bool is_in_dead_zone = dynamics.is_in_dead_zone(dynamics_state, steering_torque);
  if (is_in_dead_zone) {
    return {delay_buffer_new, pid_error, {0.0, 0.0}, true};
  }

  const auto d_state = dynamics.calc_model(dynamics.get_state(), steering_torque);

  return {delay_buffer_new, pid_error, d_state, false};
}

}  // namespace autoware::simple_planning_simulator::utils::mechanical_controller
