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

#ifndef SIMPLE_PLANNING_SIMULATOR__UTILS__MECHANICAL_CONTROLLER_HPP_
#define SIMPLE_PLANNING_SIMULATOR__UTILS__MECHANICAL_CONTROLLER_HPP_

#include <deque>
#include <map>
#include <optional>
#include <string>
#include <utility>

namespace autoware::simple_planning_simulator::utils::mechanical_controller
{

using DelayBuffer = std::deque<std::pair<double, double>>;
using DelayOutput = std::pair<std::optional<double>, DelayBuffer>;

DelayOutput delay(
  const double signal, const double delay_time, const DelayBuffer & buffer,
  const double elapsed_time);

double sign(const double x);

double apply_limits(
  const double current_angle, const double previous_angle, const double angle_limit,
  const double rate_limit, const double dt);

double feedforward(const double input_angle, const double ff_gain);

double polynomial_transform(
  const double torque, const double speed, const double a, const double b, const double c,
  const double d, const double e, const double f, const double g, const double h);

struct PIDControllerParams
{
  double kp{0.0};
  double ki{0.0};
  double kd{0.0};
};

struct PIDControllerState
{
  double integral{0.0};
  double error{0.0};
};

class PIDController
{
public:
  PIDController(const double kp, const double ki, const double kd);

  [[nodiscard]] double compute(
    const double error, const double integral, const double prev_error, const double dt) const;

  void update_state(const double error, const double dt);

  void update_state(
    const double k1_error, const double k2_error, const double k3_error, const double k4_error,
    const double dt);

  [[nodiscard]] PIDControllerState get_state() const;

  void clear_state();

private:
  double kp_{0.0}, ki_{0.0}, kd_{0.0};
  PIDControllerState state_;
};

struct SteeringDynamicsParams
{
  double angular_position{0.0};
  double angular_velocity{0.0};
  double inertia{0.0};
  double damping{0.0};
  double stiffness{0.0};
  double friction{0.0};
  double dead_zone_threshold{0.0};
};

struct SteeringDynamicsState
{
  double angular_position{0.0};
  double angular_velocity{0.0};
  bool is_in_dead_zone{false};
};

struct SteeringDynamicsDeltaState
{
  double d_angular_position{0.0};
  double d_angular_velocity{0.0};
};

/**
 * @brief SteeringDynamics class
 * @details This class calculates the dynamics which receives the steering torque and outputs the
 * steering tire angle. The steering system is modeled as a spring-damper system with friction and
 * dead zone.
 */
class SteeringDynamics
{
public:
  SteeringDynamics(
    const double angular_position, const double angular_velocity, const double inertia,
    const double damping, const double stiffness, const double friction,
    const double dead_zone_threshold);

  [[nodiscard]] bool is_in_dead_zone(
    const SteeringDynamicsState & state, const double input_torque) const;

  [[nodiscard]] SteeringDynamicsDeltaState calc_model(
    const SteeringDynamicsState & state, const double input_torque) const;

  void set_state(const SteeringDynamicsState & state);

  [[nodiscard]] SteeringDynamicsState get_state() const;

  void clear_state();

  void set_steer(const double steer);

private:
  SteeringDynamicsState state_;
  const double inertia_;
  const double damping_;
  const double stiffness_;
  const double friction_;
  const double dead_zone_threshold_;
};

struct StepResult
{
  DelayBuffer delay_buffer{};
  double pid_error{0.0};
  SteeringDynamicsDeltaState dynamics_d_state{};
  bool is_in_dead_zone{false};
};

struct MechanicalParams
{
  double kp{0.0};
  double ki{0.0};
  double kd{0.0};
  double ff_gain{0.0};
  double dead_zone_threshold{0.0};
  double poly_a{0.0};
  double poly_b{0.0};
  double poly_c{0.0};
  double poly_d{0.0};
  double poly_e{0.0};
  double poly_f{0.0};
  double poly_g{0.0};
  double poly_h{0.0};
  double inertia{0.0};
  double damping{0.0};
  double stiffness{0.0};
  double friction{0.0};
  double delay_time{0.0};

  // limit
  double angle_limit{0.0};
  double rate_limit{0.0};
  double steering_torque_limit{0.0};
  double torque_delay_time{0.0};
};

class MechanicalController
{
public:
  explicit MechanicalController(const MechanicalParams & mechanical_params);

  [[maybe_unused]] double update_euler(
    const double input_angle, const double speed, const double prev_input_angle, const double dt);

  double update_runge_kutta(
    const double input_angle, const double speed, const double prev_input_angle, const double dt);

  void clear_state();

  void set_steer(const double steer);

private:
  DelayBuffer delay_buffer_;
  PIDController pid_;
  SteeringDynamics steering_dynamics_;
  const MechanicalParams params_;

  [[nodiscard]] StepResult run_one_step(
    const double input_angle, const double speed, const double prev_input_angle, const double dt,
    const DelayBuffer & delay_buffer, const PIDController & pid,
    const SteeringDynamics & dynamics) const;
};

}  // namespace autoware::simple_planning_simulator::utils::mechanical_controller

#endif  // SIMPLE_PLANNING_SIMULATOR__UTILS__MECHANICAL_CONTROLLER_HPP_
