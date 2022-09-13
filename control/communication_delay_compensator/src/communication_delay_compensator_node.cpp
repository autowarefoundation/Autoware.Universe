
// Copyright 2022 The Autoware Foundation
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

#include "communication_delay_compensator_node.hpp"

namespace observers
{
CommunicationDelayCompensatorNode::CommunicationDelayCompensatorNode(const rclcpp::NodeOptions &node_options)
  : Node("communication_delay_compensator", node_options)
{
  using std::placeholders::_1;

  /* get parameter updates */
  observers::sLyapMatrixVecs lyap_mat_vec;  // for state observer Lyapunov mats.
  readAndLoadParameters(lyap_mat_vec);

  params_node_.wheel_base = vehicle_info_util::VehicleInfoUtil(*this).getVehicleInfo().wheel_base_m;
  initTimer(params_node_.cdob_ctrl_period);

  // Create Publishers
  pub_delay_compensator_ =
    create_publisher<DelayCompensatatorMsg>("~/output/communication_delay_compensation_refs", 1);

  pub_delay_compensator_debug_ =
    create_publisher<DelayCompensatorDebugMsg>("~/output/communication_delay_compensation_debug", 1);

  // Create subscriptions
  sub_control_cmds_ =
    create_subscription<ControlCommand>("~/input/control_cmd", rclcpp::QoS{1},
                                        std::bind(
                                          &observers::CommunicationDelayCompensatorNode::onControlCommands,
                                          this, std::placeholders::_1));

  sub_current_velocity_ptr_ =
    create_subscription<VelocityMsg>("~/input/current_odometry", rclcpp::QoS{1},
                                     std::bind(
                                       &observers::CommunicationDelayCompensatorNode::onCurrentVelocity,
                                       this, std::placeholders::_1));

  sub_current_steering_ptr_ =
    create_subscription<SteeringReport>("~/input/steering_state", rclcpp::QoS{1},
                                        std::bind(
                                          &observers::CommunicationDelayCompensatorNode::onCurrentSteering,
                                          this, std::placeholders::_1));

  sub_current_long_error_ptr_ =
    create_subscription<ControllerErrorReportMsg>("~/input/long_errors", rclcpp::QoS{1},
                                                  std::bind(
                                                    &observers::CommunicationDelayCompensatorNode::onCurrentLongitudinalError,
                                                    this,
                                                    std::placeholders::_1));

  sub_current_lat_errors_ptr_ =
    create_subscription<ControllerErrorReportMsg>("~/input/lat_errors", rclcpp::QoS{1},
                                                  std::bind(
                                                    &observers::CommunicationDelayCompensatorNode::onCurrentLateralErrors,
                                                    this,
                                                    std::placeholders::_1));

  //  sub_control_perf_errors_ptr_ = create_subscription<ErrorStampedControlPerfMsg>(
  //    "~/input/cp_errors", rclcpp::QoS{1},
  //    std::bind(
  //      &observers::CommunicationDelayCompensatorNode::onControlPerfErrors, this,
  //      std::placeholders::_1));

  // Dynamic Parameter Update.
  is_parameters_set_res_ = this->add_on_set_parameters_callback(
    std::bind(&CommunicationDelayCompensatorNode::onParameterUpdate, this, _1));

  // set the vehicle models.
  vehicle_model_ptr_ = std::make_shared<observers::linear_vehicle_model_t>(params_node_.wheel_base,
                                                                           params_node_.steering_tau,
                                                                           params_node_.cdob_ctrl_period);

  dist_td_obs_vehicle_model_ptr_ = std::make_shared<observers::linear_state_observer_model_t>(
    params_node_.wheel_base,
    params_node_.steering_tau,
    params_node_.cdob_ctrl_period);

  /**
   * @brief read the Lyapunov matrices and pass them to the delay compensator.
   * */
  setLateralCDOB_DOBs(lyap_mat_vec);
}

void CommunicationDelayCompensatorNode::initTimer(float64_t period_s)
{
  const auto
    period_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<float64_t>(period_s));

  timer_ =
    rclcpp::create_timer(this, get_clock(), period_ns,
                         std::bind(&CommunicationDelayCompensatorNode::onTimer, this));
}

void CommunicationDelayCompensatorNode::onTimer()
{
  // Create compensator messages: For breaking cyclic dependency (controllers wait this package vice
  // versa.).

  current_delay_ref_msg_ptr_ = std::make_shared<DelayCompensatatorMsg>();

  current_delay_debug_msg_ = std::make_shared<DelayCompensatorDebugMsg>();

  if (!isDataReady())
  {
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 1000, "Not enough data to compute delay compensation");

    publishCompensationReferences();
    return;
  }

  if (!previous_control_cmd_ptr_)
  {
    previous_control_cmd_ptr_ = std::make_shared<ControlCommand>();
    publishCompensationReferences();
  }

  // Update vehicle model.
  updateVehicleModelsWithPreviousTargets();
  // updateVehicleModelsWithCurrentTargets();

  is_vehicle_stopped_ = isVehicleStopping();
  computeLateralCDOB();

  // Publish delay compensation reference.
  publishCompensationReferences();

  // Debug
  // cdob_lateral_ptr_->printQfilterTFs();
  // cdob_lateral_ptr_->printQfilterSSs();
  // cdob_lateral_ptr_->printLyapMatrices();
  // end of debug

}

void CommunicationDelayCompensatorNode::onControlCommands(const ControlCommand::SharedPtr msg)
{
  previous_control_cmd_ptr_ = current_control_cmd_ptr_;
  current_control_cmd_ptr_ = std::make_shared<ControlCommand>(*msg);

  // Debug
  // ns_utils::print("ACT On control method ");
  // ns_utils::print("Read parameter control period :", params_node_.cdob_ctrl_period);
  // RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 1000 /*ms*/, "onControlCommands");
  // end of debug
}

void CommunicationDelayCompensatorNode::onCurrentVelocity(const VelocityMsg::SharedPtr msg)
{
  if (current_velocity_ptr_)
  {
    previous_velocity_ = current_velocity_ptr_->twist.twist.linear.x;
  }

  current_velocity_ptr_ = std::make_shared<VelocityMsg>(*msg);
  current_velocity_ = current_velocity_ptr_->twist.twist.linear.x;

  // ns_utils::print("ACT On velocity method ");
  // ns_utils::print("Read parameter control period :", params_node_.cdob_ctrl_period);
  // RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 1000 /*ms*/, "On Velocity");
}

void CommunicationDelayCompensatorNode::onCurrentLongitudinalError(ControllerErrorReportMsg::SharedPtr const msg)
{
  prev_long_errors_ptr_ = current_long_errors_ptr_;
  current_long_errors_ptr_ = std::make_shared<ControllerErrorReportMsg>(*msg);

  if (prev_long_errors_ptr_)
  {
    // Compute current steering error.
    previous_target_velocity_ = static_cast<float64_t>(prev_long_errors_ptr_->target_velocity_read);
  }

  current_target_velocity_ = static_cast<float64_t>(current_long_errors_ptr_->target_velocity_read);
}

void CommunicationDelayCompensatorNode::onCurrentLateralErrors(ControllerErrorReportMsg::SharedPtr const msg)
{
  prev_lat_errors_ptr_ = current_lat_errors_ptr_;
  current_lat_errors_ptr_ = std::make_shared<ControllerErrorReportMsg>(*msg);

  // Compute current steering error.
  current_curvature_ = static_cast<float64_t>(current_lat_errors_ptr_->curvature_read);

  // Ackerman Ideal Steering
  current_ideal_steering_ = std::atan(current_curvature_ * params_node_.wheel_base);

  if (prev_lat_errors_ptr_)
  {
    // Compute current steering error.
    prev_curvature_ = static_cast<float64_t>(prev_lat_errors_ptr_->curvature_read);

    // Ackerman Ideal Steering
    prev_ideal_steering_ = std::atan(prev_curvature_ * params_node_.wheel_base);
  }

}

void CommunicationDelayCompensatorNode::publishCompensationReferences()
{
  current_delay_ref_msg_ptr_->stamp = this->now();
  current_delay_debug_msg_->stamp = this->now();

  // new_msg.lateral_deviation_error_compensation_ref = 1.0;
  pub_delay_compensator_->publish(*current_delay_ref_msg_ptr_);
  pub_delay_compensator_debug_->publish(*current_delay_debug_msg_);
}

void CommunicationDelayCompensatorNode::onCurrentSteering(const SteeringReport::SharedPtr msg)
{
  prev_steering_ptr_ = current_steering_ptr_;
  current_steering_ptr_ = std::make_shared<SteeringReport>(*msg);

  if (prev_steering_ptr_)
  {
    previous_steering_angle_ = static_cast<float64_t>(prev_steering_ptr_->steering_tire_angle);
  }

  current_steering_angle_ = static_cast<float64_t>(current_steering_ptr_->steering_tire_angle);

  // Debug
  RCLCPP_WARN_SKIPFIRST_THROTTLE(
    get_logger(), *get_clock(), (1000ms).count(), "[communication_delay] On Steering  ...");
  // ns_utils::print("ACT On steering method ");
  // end of debug
}

bool8_t CommunicationDelayCompensatorNode::isDataReady()
{
  if (!current_velocity_ptr_)
  {
    RCLCPP_WARN_SKIPFIRST_THROTTLE(get_logger(),
                                   *get_clock(),
                                   (1000ms).count(),
                                   "[communication_delay] Waiting for the velocity measurement ...");
    return false;
  }

  if (!current_steering_ptr_)
  {
    RCLCPP_WARN_SKIPFIRST_THROTTLE(get_logger(),
                                   *get_clock(),
                                   (1000ms).count(),
                                   "[communication_delay] Waiting for the steering measurement ...");
    return false;
  }

  if (!current_control_cmd_ptr_)
  {
    RCLCPP_WARN_SKIPFIRST_THROTTLE(get_logger(),
                                   *get_clock(),
                                   (1000ms).count(),
                                   "[communication_delay] Waiting for the control command ...");
    return false;
  }

  if (!previous_control_cmd_ptr_)
  {
    RCLCPP_WARN_SKIPFIRST_THROTTLE(get_logger(),
                                   *get_clock(),
                                   (1000ms).count(),
                                   "[communication_delay] Waiting for the control command ...");
    return false;
  }

  if (!current_lat_errors_ptr_)
  {
    RCLCPP_WARN_SKIPFIRST_THROTTLE(get_logger(),
                                   *get_clock(),
                                   (1000ms).count(),
                                   "[communication_delay] Waiting for the current lateral error report ...");
    return false;
  }

  if (!current_long_errors_ptr_)
  {
    RCLCPP_WARN_SKIPFIRST_THROTTLE(get_logger(),
                                   *get_clock(),
                                   (1000ms).count(),
                                   "[communication_delay] Waiting for the current longitudinal error report ...");
    return false;
  }

  return true;
}

void CommunicationDelayCompensatorNode::readAndLoadParameters(observers::sLyapMatrixVecs &lyap_mats)
{

  // Read the filter orders.
  params_node_.cdob_ctrl_period = declare_parameter<float64_t>("cdob_ctrl_period");  // reads sec.

  params_node_.qfilter_lateral_error_cdob_order = declare_parameter<int>("qfilter_lateral_error_cdob_order");

  params_node_.qfilter_lateral_dob_order = declare_parameter<int>("qfilter_lateral_dob_order");

  params_node_.qfilter_longitudinal_error_order = declare_parameter<int>("qfilter_longitudinal_error_order");

  // Read the filter cut-oof frequencies.
  params_node_.qfilter_lateral_error_cdob_freq = declare_parameter<float64_t>("qfilter_lateral_error_cdob_freq");

  params_node_.qfilter_lateral_dob_freq = declare_parameter<float64_t>("qfilter_lateral_dob_freq");

  params_node_.qfilter_longitudinal_error_freq = declare_parameter<float64_t>("qfilter_longitudinal_error_freq");

  // Damping
  params_node_.qfilter_lateral_cdob_damping = declare_parameter<float64_t>("qfilter_lateral_cdob_damping");
  params_node_.qfilter_lateral_dob_damping = declare_parameter<float64_t>("qfilter_lateral_dob_damping");

  // First order state dynamics parameters.
  params_node_.steering_tau = declare_parameter<float64_t>("steering_time_constant_");
  params_node_.velocity_tau = declare_parameter<float64_t>("velocity_time_constant_");
  params_node_.acc_tau = declare_parameter<float64_t>("acc_time_constant_");

  // Load the state observer Lyapunov matrices.
  auto labelX_tag = "Xn";  // No delay nodel for steering and longitudinal speed
  auto labelY_tag = "Yn";

  for (size_t k = 0; k < observers::cx_NUMBER_OF_LYAP_MATS; k++)
  {
    auto labelX = labelX_tag + std::to_string(k + 1);
    auto tempvX = declare_parameter<std::vector<float64_t>>(labelX);
    auto tempX = state_matrix_observer_t::Map(tempvX.data());

    lyap_mats.vXs.emplace_back(tempX);

    auto labelY = labelY_tag + std::to_string(k + 1);
    auto tempvY = declare_parameter<std::vector<float64_t>>(labelY);
    auto tempY = measurement_matrix_observer_t::Map(tempvY.data());

    lyap_mats.vYs.emplace_back(tempY);
  }
}

rcl_interfaces::msg::SetParametersResult
CommunicationDelayCompensatorNode::onParameterUpdate(const std::vector<rclcpp::Parameter> &parameters)
{
  rcl_interfaces::msg::SetParametersResult result;
  result.successful = true;
  result.reason = "success";

  try
  {
    update_param(parameters, "cdob_ctrl_period", params_node_.cdob_ctrl_period);

    update_param(parameters, "qfilter_lateral_error_order", params_node_.qfilter_lateral_error_cdob_order);

    update_param(parameters, "qfilter_longitudinal_error_order",
                 params_node_.qfilter_longitudinal_error_order);

    update_param(parameters, "qfilter_lateral_error_freq", params_node_.qfilter_lateral_error_cdob_freq);

    update_param(parameters, "qfilter_longitudinal_error_freq", params_node_.qfilter_longitudinal_error_freq);

    update_param(parameters, "steering_time_constant_", params_node_.steering_tau);
    update_param(parameters, "velocity_time_constant_", params_node_.velocity_tau);
    update_param(parameters, "acc_time_constant_", params_node_.acc_tau);

  }

    // transaction succeeds, now assign values
  catch (const rclcpp::exceptions::InvalidParameterTypeException &e)
  {
    result.successful = false;
    result.reason = e.what();
  }

  for (const auto &param : parameters)
  {
    RCLCPP_INFO(this->get_logger(), "%s", param.get_name().c_str());
    RCLCPP_INFO(this->get_logger(), "%s", param.get_type_name().c_str());
    RCLCPP_INFO(this->get_logger(), "%s", param.value_to_string().c_str());
  }

  return result;
}

bool8_t CommunicationDelayCompensatorNode::isVehicleStopping()
{
  auto current_vel = current_velocity_ptr_->twist.twist.linear.x;
  return std::fabs(current_vel) <= 0.1;
}

/**
 * @brief Parallel vehicle model that operates on the past states [k-1] of the actual vehicle [k].
 * */
void CommunicationDelayCompensatorNode::updateVehicleModelsWithPreviousTargets()
{
  // auto vref = current_velocity_ptr_->twist.twist.linear.x;
  // auto & u_prev = previous_control_cmd_ptr_->lateral.steering_tire_angle;

  // Update the matrices
  dist_td_obs_vehicle_model_ptr_->updateStateSpace(previous_velocity_, previous_steering_angle_);
  vehicle_model_ptr_->updateStateSpace(previous_velocity_, previous_steering_angle_);

  // Update the initial state.

  if (prev_lat_errors_ptr_ && prev_steering_ptr_ && prev_long_errors_ptr_)
  {
    float64_t ey{static_cast<float64_t>(current_lat_errors_ptr_->lateral_deviation_read)};
    float64_t eyaw{static_cast<float64_t>(current_lat_errors_ptr_->heading_angle_error_read)};
    float64_t steering_angle{static_cast<float64_t>(current_lat_errors_ptr_->steering_read)};
    float64_t current_vx{current_velocity_};

    dist_td_obs_vehicle_model_ptr_->updateInitialStates(ey, eyaw, steering_angle, current_vx,
                                                        current_curvature_);
    vehicle_model_ptr_->updateInitialStates(ey, eyaw, steering_angle, current_vx, current_curvature_);
  }

}

/**
 * @brief Parallel vehicle model that operates on the past states [k-1] of the actual vehicle [k].
 * */
void CommunicationDelayCompensatorNode::updateVehicleModelsWithCurrentTargets()
{

  // Update the matrices
  dist_td_obs_vehicle_model_ptr_->updateStateSpace(current_target_velocity_, current_steering_angle_);
  vehicle_model_ptr_->updateStateSpace(current_target_velocity_, current_steering_angle_);

  // Update the initial state.

  if (prev_lat_errors_ptr_ && prev_steering_ptr_ && prev_long_errors_ptr_)
  {
    float64_t ey{static_cast<float64_t>(current_lat_errors_ptr_->lateral_deviation_read)};
    float64_t eyaw{static_cast<float64_t>(current_lat_errors_ptr_->heading_angle_error_read)};
    float64_t vx{current_velocity_};

    dist_td_obs_vehicle_model_ptr_->updateInitialStates(ey, eyaw, current_steering_angle_, vx,
                                                        current_curvature_);
    vehicle_model_ptr_->updateInitialStates(ey, eyaw, current_steering_angle_, vx, current_curvature_);
  }

}

void CommunicationDelayCompensatorNode::setLateralCDOB_DOBs(sLyapMatrixVecs const &lyap_matsXY)
{
  /**
   * Create qfilter for lateral controller.  It filters both control input and estimated
   * disturbance in the observer.
   */

  // --------------- Qfilter Construction for lateral error state -------------------------
  auto const &order_lat_error_cdob = params_node_.qfilter_lateral_error_cdob_order;
  auto const &wc_lat_error_cdob = params_node_.qfilter_lateral_error_cdob_freq;


  // Create nth order qfilter transfer function for the steering system. 1 /( tau*s + 1)&^n
  ns_control_toolbox::tf qfilter_lat_error_cdob;
  if (order_lat_error_cdob > 1)
  {
    auto const &remaining_order_lat_error_cdob = order_lat_error_cdob - 2;
    auto const &damping_val_cdob = params_node_.qfilter_lateral_cdob_damping;

    qfilter_lat_error_cdob = get_nthOrderTFwithDampedPoles(wc_lat_error_cdob,
                                                           remaining_order_lat_error_cdob,
                                                           damping_val_cdob);
  } else
  {

    // Create nth order qfilter transfer function for the steering system. 1 /( tau*s + 1)&^n
    qfilter_lat_error_cdob = get_nthOrderTF(wc_lat_error_cdob, order_lat_error_cdob);
  }

  cdob_lateral_ptr_ = std::make_unique<LateralCommunicationDelayCompensator>(dist_td_obs_vehicle_model_ptr_,
                                                                             vehicle_model_ptr_,
                                                                             qfilter_lat_error_cdob,
                                                                             lyap_matsXY,
                                                                             params_node_.cdob_ctrl_period);

  // Set the DOB
  auto const &order_lat_error_dob = params_node_.qfilter_lateral_dob_order;
  auto const &wc_lat_error_dob = params_node_.qfilter_lateral_dob_freq;

  ns_control_toolbox::tf qfilter_lat_error_dob;
  if (order_lat_error_dob > 1)
  {
    auto const &remaining_order_lat_error_dob = order_lat_error_dob - 2;
    auto const &damping_val_dob = params_node_.qfilter_lateral_dob_damping;

    qfilter_lat_error_dob = get_nthOrderTFwithDampedPoles(wc_lat_error_dob,
                                                          remaining_order_lat_error_dob,
                                                          damping_val_dob);
  } else
  {

    // Create nth order qfilter transfer function for the steering system. 1 /( tau*s + 1)&^n
    qfilter_lat_error_dob = get_nthOrderTF(wc_lat_error_dob, order_lat_error_dob);
  }

  dob_lateral_ptr_ = std::make_unique<LateralDisturbanceCompensator>(dist_td_obs_vehicle_model_ptr_,
                                                                     qfilter_lat_error_dob,
                                                                     lyap_matsXY,
                                                                     params_node_.cdob_ctrl_period);
}

void CommunicationDelayCompensatorNode::computeLateralCDOB()
{
  // get the current outputs observed y=[ey, eyaw, steering] for qfilters.
  auto const &current_lat_error = current_lat_errors_ptr_->lateral_deviation_read;
  auto const &current_heading_error = current_lat_errors_ptr_->heading_angle_error_read;
  auto const &current_steering = current_lat_errors_ptr_->steering_read;

  current_lat_measurements_ << current_lat_error, current_heading_error, current_steering;

  // Set messages
  current_delay_ref_msg_ptr_->lateral_deviation_read = current_lat_error;
  current_delay_ref_msg_ptr_->heading_angle_error_read = current_heading_error;
  current_delay_ref_msg_ptr_->steering_read = current_steering;


  /**
   * get the previous inputs and parameter that are used and sent to the vehicle.
   * previous: ideal steering and target velocities to linearize the model, and previous
   * curvature as an input to the steering.
   * */

  auto const
    &prev_steering_control_cmd = static_cast<float64_t>(previous_control_cmd_ptr_->lateral.steering_tire_angle);

  auto const
    &current_steering_control_cmd = static_cast<float64_t>(current_control_cmd_ptr_->lateral.steering_tire_angle);

  cdob_lateral_ptr_->simulateOneStep(current_lat_measurements_,
                                     prev_steering_control_cmd,
                                     current_steering_control_cmd,
                                     current_delay_ref_msg_ptr_,
                                     current_delay_debug_msg_);

  dob_lateral_ptr_->simulateOneStep(current_lat_measurements_,
                                    prev_steering_control_cmd,
                                    current_steering_control_cmd,
                                    current_delay_ref_msg_ptr_);

}

}  // namespace observers
#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(observers::CommunicationDelayCompensatorNode)