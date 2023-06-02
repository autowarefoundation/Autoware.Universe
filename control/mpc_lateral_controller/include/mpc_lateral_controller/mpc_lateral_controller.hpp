// Copyright 2021 The Autoware Foundation
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

#ifndef MPC_LATERAL_CONTROLLER__MPC_LATERAL_CONTROLLER_HPP_
#define MPC_LATERAL_CONTROLLER__MPC_LATERAL_CONTROLLER_HPP_

#include "mpc_lateral_controller/interpolate.hpp"
#include "mpc_lateral_controller/lowpass_filter.hpp"
#include "mpc_lateral_controller/mpc.hpp"
#include "mpc_lateral_controller/mpc_trajectory.hpp"
#include "mpc_lateral_controller/mpc_utils.hpp"
#include "mpc_lateral_controller/qp_solver/qp_solver_osqp.hpp"
#include "mpc_lateral_controller/qp_solver/qp_solver_unconstr_fast.hpp"
#include "mpc_lateral_controller/steering_offset/steering_offset.hpp"
#include "mpc_lateral_controller/vehicle_model/vehicle_model_bicycle_dynamics.hpp"
#include "mpc_lateral_controller/vehicle_model/vehicle_model_bicycle_kinematics.hpp"
#include "mpc_lateral_controller/vehicle_model/vehicle_model_bicycle_kinematics_no_delay.hpp"
#include "osqp_interface/osqp_interface.hpp"
#include "rclcpp/rclcpp.hpp"
#include "trajectory_follower_base/lateral_controller_base.hpp"
#include "vehicle_info_util/vehicle_info_util.hpp"

#include "autoware_auto_control_msgs/msg/ackermann_lateral_command.hpp"
#include "autoware_auto_planning_msgs/msg/trajectory.hpp"
#include "autoware_auto_vehicle_msgs/msg/steering_report.hpp"
#include "autoware_auto_vehicle_msgs/msg/vehicle_odometry.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "tier4_debug_msgs/msg/float32_multi_array_stamped.hpp"

#include <deque>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace autoware::motion::control::mpc_lateral_controller
{

namespace trajectory_follower = ::autoware::motion::control::trajectory_follower;
using autoware_auto_control_msgs::msg::AckermannLateralCommand;
using autoware_auto_planning_msgs::msg::Trajectory;
using tier4_debug_msgs::msg::Float32MultiArrayStamped;
using tier4_debug_msgs::msg::Float32Stamped;

class MpcLateralController : public trajectory_follower::LateralControllerBase
{
public:
  explicit MpcLateralController(rclcpp::Node & node);
  virtual ~MpcLateralController();

private:
  rclcpp::Node * node_;

  rclcpp::Publisher<Trajectory>::SharedPtr m_pub_predicted_traj;
  rclcpp::Publisher<Float32MultiArrayStamped>::SharedPtr m_pub_debug_values;
  rclcpp::Publisher<Float32Stamped>::SharedPtr m_pub_steer_offset;

  /* parameters for path smoothing */
  TrajectoryFilteringParam m_trajectory_filtering_param;

  /* parameters for stop state */
  double m_stop_state_entry_ego_speed;
  double m_stop_state_entry_target_speed;
  double m_converged_steer_rad;
  double m_mpc_converged_threshold_rps;  // max mpc output change threshold for 1 sec
  double m_new_traj_duration_time;       // check trajectory shape change
  double m_new_traj_end_dist;            // check trajectory shape change
  bool m_keep_steer_control_until_converged;

  // trajectory buffer for detecting new trajectory
  std::deque<Trajectory> m_trajectory_buffer;

  // MPC object
  MPC m_mpc;

  // Check is mpc output converged
  bool m_is_mpc_history_filled{false};

  //!< @brief store the last mpc outputs for 1 sec
  std::vector<std::pair<autoware_auto_control_msgs::msg::AckermannLateralCommand, rclcpp::Time>>
    m_mpc_steering_history{};
  //!< @brief set the mpc steering output to history
  void setSteeringToHistory(
    const autoware_auto_control_msgs::msg::AckermannLateralCommand & steering);
  //!< @brief check if the mpc steering output is converged
  bool isMpcConverged();

  //!< @brief measured kinematic state
  nav_msgs::msg::Odometry m_current_kinematic_state;

  //!< @brief measured steering
  autoware_auto_vehicle_msgs::msg::SteeringReport m_current_steering;

  //!< @brief reference trajectory
  Trajectory m_current_trajectory;

  //!< @brief mpc filtered output in previous period
  double m_steer_cmd_prev = 0.0;

  //!< @brief flag of m_ctrl_cmd_prev initialization
  bool m_is_ctrl_cmd_prev_initialized = false;

  //!< @brief previous control command
  AckermannLateralCommand m_ctrl_cmd_prev;

  //!< @brief flag whether the first trajectory has been received
  bool m_has_received_first_trajectory = false;

  // ego nearest index search
  double m_ego_nearest_dist_threshold;
  double m_ego_nearest_yaw_threshold;

  // for steering offset compensation
  bool enable_auto_steering_offset_removal_;
  std::shared_ptr<SteeringOffsetEstimator> steering_offset_;

  //!< initialize timer to work in real, simulation, and replay
  void initTimer(double period_s);

  bool isReady(const trajectory_follower::InputData & input_data) override;

  /**
   * @brief compute control command for path follow with a constant control period
   */
  trajectory_follower::LateralOutput run(
    trajectory_follower::InputData const & input_data) override;

  /**
   * @brief set m_current_trajectory with received message
   */
  void setTrajectory(const Trajectory & msg);

  /**
   * @brief check if the received data is valid.
   */
  bool checkData() const;

  /**
   * @brief create control command
   * @param [in] ctrl_cmd published control command
   */
  AckermannLateralCommand createCtrlCmdMsg(AckermannLateralCommand ctrl_cmd);

  /**
   * @brief publish predicted future trajectory
   * @param [in] predicted_traj published predicted trajectory
   */
  void publishPredictedTraj(Trajectory & predicted_traj) const;

  /**
   * @brief publish diagnostic message
   * @param [in] diagnostic published diagnostic
   */
  void publishDebugValues(Float32MultiArrayStamped & diagnostic) const;

  /**
   * @brief get stop command
   */
  AckermannLateralCommand getStopControlCommand() const;

  /**
   * @brief get initial command
   */
  AckermannLateralCommand getInitialControlCommand() const;

  /**
   * @brief check ego car is in stopped state
   */
  bool isStoppedState() const;

  /**
   * @brief check if the trajectory has valid value
   */
  bool isValidTrajectory(const Trajectory & traj) const;

  bool isTrajectoryShapeChanged() const;

  bool isSteerConverged(const AckermannLateralCommand & cmd) const;

  rclcpp::Node::OnSetParametersCallbackHandle::SharedPtr m_set_param_res;

  /**
   * @brief Declare MPC parameters as ROS parameters to allow tuning on the fly
   */
  void declareMPCparameters();

  /**
   * @brief Called when parameters are changed outside of node
   */
  rcl_interfaces::msg::SetParametersResult paramCallback(
    const std::vector<rclcpp::Parameter> & parameters);

  inline void info_throttle(const std::string & s, const uint duration_ms = 5000)
  {
    RCLCPP_INFO_THROTTLE(node_->get_logger(), *node_->get_clock(), duration_ms, "%s", s.c_str());
  }

  inline void warn_throttle(const std::string & s, const uint duration_ms = 5000)
  {
    RCLCPP_WARN_THROTTLE(node_->get_logger(), *node_->get_clock(), duration_ms, "%s", s.c_str());
  }
};
}  // namespace autoware::motion::control::mpc_lateral_controller

#endif  // MPC_LATERAL_CONTROLLER__MPC_LATERAL_CONTROLLER_HPP_
