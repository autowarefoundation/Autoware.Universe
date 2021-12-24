// Copyright 2021 The Autoware Foundation.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SIMPLE_PLANNING_SIMULATOR__VEHICLE_MODEL__SIM_MODEL_DELAY_STEER_VEL_HPP_
#define SIMPLE_PLANNING_SIMULATOR__VEHICLE_MODEL__SIM_MODEL_DELAY_STEER_VEL_HPP_

#include <deque>
#include <iostream>
#include <queue>

#include "eigen3/Eigen/Core"
#include "eigen3/Eigen/LU"

#include "simple_planning_simulator/vehicle_model/sim_model_interface.hpp"
/**
 * @class SimModelDelaySteerVel
 * @brief calculate delay steering dynamics
 */
class SimModelDelaySteerVel : public SimModelInterface
{
public:
  /**
   * @brief constructor
   * @param [in] vx_lim velocity limit [m/s]
   * @param [in] steer_lim steering limit [rad]
   * @param [in] vx_rate_lim acceleration limit [m/ss]
   * @param [in] steer_rate_lim steering angular velocity limit [rad/ss]
   * @param [in] wheelbase vehicle wheelbase length [m]
   * @param [in] dt delta time information to set input buffer for delay
   * @param [in] vx_delay time delay for velocity command [s]
   * @param [in] vx_time_constant time constant for 1D model of velocity dynamics
   * @param [in] steer_delay time delay for steering command [s]
   * @param [in] steer_time_constant time constant for 1D model of steering dynamics
   */
  SimModelDelaySteerVel(
    float64_t vx_lim, float64_t steer_lim, float64_t vx_rate_lim, float64_t steer_rate_lim,
    float64_t wheelbase, float64_t dt, float64_t vx_delay, float64_t vx_time_constant,
    float64_t steer_delay, float64_t steer_time_constant);

  /**
   * @brief destructor
   */
  ~SimModelDelaySteerVel() = default;

private:
  const float64_t MIN_TIME_CONSTANT;  //!< @brief minimum time constant

  enum IDX
  {
    X = 0,
    Y,
    YAW,
    VX,
    STEER,
  };
  enum IDX_U
  {
    VX_DES = 0,
    STEER_DES,
  };

  const float64_t vx_lim_;       //!< @brief velocity limit
  const float64_t vx_rate_lim_;  //!< @brief acceleration limit
  const float64_t steer_lim_;       //!< @brief steering limit [rad]
  const float64_t steer_rate_lim_;  //!< @brief steering angular velocity limit [rad/s]
  const float64_t wheelbase_;       //!< @brief vehicle wheelbase length [m]

  std::deque<float64_t> vx_input_queue_;  //!< @brief buffer for velocity command
  std::deque<float64_t> steer_input_queue_;  //!< @brief buffer for angular velocity command
  const float64_t vx_delay_;              //!< @brief time delay for velocity command [s]
  const float64_t vx_time_constant_;
    //!< @brief time constant for 1D model of velocity dynamics
  const float64_t steer_delay_;              //!< @brief time delay for angular-velocity command [s]
  const float64_t
    steer_time_constant_;  //!< @brief time constant for 1D model of angular-velocity dynamics

  /**
   * @brief set queue buffer for input command
   * @param [in] dt delta time
   */
  void initializeInputQueue(const float_t & dt);

  /**
   * @brief get vehicle position x
   */
  float64_t getX() override;

  /**
   * @brief get vehicle position y
   */
  float64_t getY() override;

  /**
   * @brief get vehicle angle yaw
   */
  float64_t getYaw() override;

  /**
   * @brief get vehicle longitudinal velocity
   */
  float64_t getVx() override;

  /**
   * @brief get vehicle lateral velocity
   */
  float64_t getVy() override;

  /**
   * @brief get vehicle longiudinal acceleration
   */
  float64_t getAx() override;

  /**
   * @brief get vehicle angular-velocity wz
   */
  float64_t getWz() override;

  /**
   * @brief get vehicle steering angle
   */
  float64_t getSteer() override;

  /**
   * @brief update vehicle states
   * @param [in] dt delta time [s]
   */
  void update(const float64_t & dt) override;

  /**
   * @brief calculate derivative of states with delay steering model
   * @param [in] state current model state
   * @param [in] input input vector to model
   */
  Eigen::VectorXd calcModel(const Eigen::VectorXd & state, const Eigen::VectorXd & input) override;
};

#endif  // SIMPLE_PLANNING_SIMULATOR__VEHICLE_MODEL__SIM_MODEL_DELAY_STEER_VEL_HPP_
