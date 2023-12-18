// Copyright 2021 The Autoware Foundation.
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

#ifndef SIMPLE_PLANNING_SIMULATOR__VEHICLE_MODEL__SIM_MODEL_PYMODELS_HPP_
#define SIMPLE_PLANNING_SIMULATOR__VEHICLE_MODEL__SIM_MODEL_PYMODELS_HPP_

#include "simple_planning_simulator/vehicle_model/sim_model_interface.hpp"

#include <Eigen/Core>
#include <Eigen/LU>

#include <deque>
#include <iostream>
#include <queue>

// #include <pybind11/embed.h>
// namespace py = pybind11;

/**
 * @class SimModelPymodels
 * @brief calculate delay steering dynamics
 */
class SimModelPymodels : public SimModelInterface /*delay steer velocity*/
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
   * @param [in] steer_dead_band dead band for steering angle [rad]
   * @param [in] HELLO_FROM_CODING_GOD PRAISE THE MESSIAH!!!!! <3
   */
  SimModelPymodels(
    double vx_lim, double steer_lim, double vx_rate_lim, double steer_rate_lim, double wheelbase,
    double dt, double vx_delay, double vx_time_constant, double steer_delay,
    double steer_time_constant, double steer_dead_band);

  /**
   * @brief destructor
   */
  ~SimModelPymodels() = default;

private:
  const double MIN_TIME_CONSTANT;  //!< @brief minimum time constant   --> TODO what is this?

  /* 
  Specify string names for states and inputs. So we can automatically map states and 
  inputs of this model to states and inputs of the python submodels.
  In order not to compare strings each iteration, we compute mappings between the states 
  and inputs of this model and python submodels.
  */
  // const char* STATE_NAMES[5] = {"POS_X", "POS_Y", "YAW", "VX", "STEER"};
  // const char* INPUT_NAMES[2] = {"VX_DES", "STEER_DES"};

  /* 
  Index means state or input in this model. First 5 indexes are states, last 2 indexes are inputs.
  ----------------------------  {      STATES      }{INPUTS} */
  // int MAP_TO_PYSTEER_INPUT[7] = {-1, -1, -1, -1, -1, -1, -1};  // <<- Value means index of the input in python steer model. -1 means not used.
  /* 
  Index means state in this model.
  -------------------------------  {      STATES      }*/
  // int MAP_FROM_PYSTEER_OUTPUT[5] = {-1, -1, -1, -1, -1};  // <<- Value means index of the output in python steer model. -1 means not used.

  enum IDX {
    X = 0,
    Y,
    YAW,
    VX,
    STEER,
  };
  enum IDX_U {
    VX_DES = 0,
    STEER_DES,
  };

  // py::scoped_interpreter guard{};
  

  const double vx_lim_;          //!< @brief velocity limit
  const double vx_rate_lim_;     //!< @brief acceleration limit
  const double steer_lim_;       //!< @brief steering limit [rad]
  const double steer_rate_lim_;  //!< @brief steering angular velocity limit [rad/s]
  const double wheelbase_;       //!< @brief vehicle wheelbase length [m]
  double prev_vx_ = 0.0;
  double current_ax_ = 0.0;

  std::deque<double> vx_input_queue_;     //!< @brief buffer for velocity command
  std::deque<double> steer_input_queue_;  //!< @brief buffer for angular velocity command
  const double vx_delay_;                 //!< @brief time delay for velocity command [s]
  const double vx_time_constant_;
  //!< @brief time constant for 1D model of velocity dynamics
  const double steer_delay_;  //!< @brief time delay for angular-velocity command [s]
  const double
    steer_time_constant_;  //!< @brief time constant for 1D model of angular-velocity dynamics
  const double steer_dead_band_;  //!< @brief dead band for steering angle [rad]


  //PyObject *steer_base_model;

  // int num_states_steering;
  // int num_inputs_steering;

  // Create vectors to store system input and state (C++)
  // Eigen::VectorXd state_steering;
  // Eigen::VectorXd input_steering;

  // Create vectors to store system input and state (python)
  // PyObject *py_state_steering, *py_input_steering;

  // std::vector<double> state_steering;
  // std::vector<double> input_steering;

  // py::object steering_module_class;
  

  /**
   * @brief set queue buffer for input command
   * @param [in] dt delta time
   */
  void initializeInputQueue(const double & dt);

  /**
   * @brief get vehicle position x
   */
  double getX() override;

  /**
   * @brief get vehicle position y
   */
  double getY() override;

  /**
   * @brief get vehicle angle yaw
   */
  double getYaw() override;

  /**
   * @brief get vehicle longitudinal velocity
   */
  double getVx() override;

  /**
   * @brief get vehicle lateral velocity
   */
  double getVy() override;

  /**
   * @brief get vehicle longitudinal acceleration
   */
  double getAx() override;

  /**
   * @brief get vehicle angular-velocity wz
   */
  double getWz() override;

  /**
   * @brief get vehicle steering angle
   */
  double getSteer() override;

  /**
   * @brief update vehicle states
   * @param [in] dt delta time [s]
   */
  void update(const double & dt) override;

  /**
   * @brief calculate derivative of states with delay steering model
   * @param [in] state current model state
   * @param [in] input input vector to model
   */
  Eigen::VectorXd calcModel(const Eigen::VectorXd & state, const Eigen::VectorXd & input) override;
};

#endif  // SIMPLE_PLANNING_SIMULATOR__VEHICLE_MODEL__SIM_MODEL_PYMODELS_HPP_
