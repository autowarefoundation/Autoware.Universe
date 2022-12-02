//
//  Copyright 2020 Tier IV, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include "autoware_state_panel.hpp"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <rviz_common/display_context.hpp>

#include <memory>
#include <string>

inline std::string Bool2String(const bool var) { return var ? "True" : "False"; }

using std::placeholders::_1;

namespace rviz_plugins
{
AutowareStatePanel::AutowareStatePanel(QWidget * parent) : rviz_common::Panel(parent)
{
  // Gear
  auto * gear_prefix_label_ptr = new QLabel("GEAR: ");
  gear_prefix_label_ptr->setAlignment(Qt::AlignRight);
  gear_label_ptr_ = new QLabel("INIT");
  gear_label_ptr_->setAlignment(Qt::AlignCenter);
  auto * gear_layout = new QHBoxLayout;
  gear_layout->addWidget(gear_prefix_label_ptr);
  gear_layout->addWidget(gear_label_ptr_);

  // Velocity Limit
  velocity_limit_button_ptr_ = new QPushButton("Send Velocity Limit");
  pub_velocity_limit_input_ = new QSpinBox();
  pub_velocity_limit_input_->setRange(-100.0, 100.0);
  pub_velocity_limit_input_->setValue(0.0);
  pub_velocity_limit_input_->setSingleStep(5.0);
  connect(velocity_limit_button_ptr_, SIGNAL(clicked()), this, SLOT(onClickVelocityLimit()));

  // Emergency Button
  emergency_button_ptr_ = new QPushButton("Set Emergency");
  connect(emergency_button_ptr_, SIGNAL(clicked()), this, SLOT(onClickEmergencyButton()));

  // Layout
  auto * v_layout = new QVBoxLayout;
  auto * velocity_limit_layout = new QHBoxLayout();
  v_layout->addWidget(makeOperationModeGroup());
  v_layout->addWidget(makeControlModeGroup());
  {
    auto * h_layout = new QHBoxLayout();
    h_layout->addWidget(makeRoutingGroup());
    h_layout->addWidget(makeLocalizationGroup());
    h_layout->addWidget(makeMotionGroup());
    v_layout->addLayout(h_layout);
  }

  v_layout->addLayout(gear_layout);
  velocity_limit_layout->addWidget(velocity_limit_button_ptr_);
  velocity_limit_layout->addWidget(pub_velocity_limit_input_);
  velocity_limit_layout->addWidget(new QLabel("  [km/h]"));
  velocity_limit_layout->addWidget(emergency_button_ptr_);
  v_layout->addLayout(velocity_limit_layout);
  setLayout(v_layout);
}

QGroupBox * AutowareStatePanel::makeOperationModeGroup()
{
  auto * group = new QGroupBox("OperationMode");
  auto * grid = new QGridLayout;

  operation_mode_label_ptr_ = new QLabel("INIT");
  operation_mode_label_ptr_->setAlignment(Qt::AlignCenter);
  operation_mode_label_ptr_->setStyleSheet("border:1px solid black;");
  grid->addWidget(operation_mode_label_ptr_, 0, 0, 0, 1);

  auto_button_ptr_ = new QPushButton("AUTO");
  auto_button_ptr_->setCheckable(true);
  connect(auto_button_ptr_, SIGNAL(clicked()), SLOT(onClickAutonomous()));
  grid->addWidget(auto_button_ptr_, 0, 1);

  stop_button_ptr_ = new QPushButton("STOP");
  stop_button_ptr_->setCheckable(true);
  connect(stop_button_ptr_, SIGNAL(clicked()), SLOT(onClickStop()));
  grid->addWidget(stop_button_ptr_, 0, 2);

  local_button_ptr_ = new QPushButton("LOCAL");
  local_button_ptr_->setCheckable(true);
  connect(local_button_ptr_, SIGNAL(clicked()), SLOT(onClickLocal()));
  grid->addWidget(local_button_ptr_, 1, 1);

  remote_button_ptr_ = new QPushButton("REMOTE");
  remote_button_ptr_->setCheckable(true);
  connect(remote_button_ptr_, SIGNAL(clicked()), SLOT(onClickRemote()));
  grid->addWidget(remote_button_ptr_, 1, 2);

  group->setLayout(grid);
  return group;
}

QGroupBox * AutowareStatePanel::makeControlModeGroup()
{
  auto * group = new QGroupBox("AutowareControl");
  auto * grid = new QGridLayout;

  control_mode_label_ptr_ = new QLabel("INIT");
  control_mode_label_ptr_->setAlignment(Qt::AlignCenter);
  control_mode_label_ptr_->setStyleSheet("border:1px solid black;");
  grid->addWidget(control_mode_label_ptr_, 0, 0);

  enable_button_ptr_ = new QPushButton("Enable");
  enable_button_ptr_->setCheckable(true);
  connect(enable_button_ptr_, SIGNAL(clicked()), SLOT(onClickAutowareControl()));
  grid->addWidget(enable_button_ptr_, 0, 1);

  disable_button_ptr_ = new QPushButton("Disable");
  disable_button_ptr_->setCheckable(true);
  connect(disable_button_ptr_, SIGNAL(clicked()), SLOT(onClickDirectControl()));
  grid->addWidget(disable_button_ptr_, 0, 2);

  group->setLayout(grid);
  return group;
}

QGroupBox * AutowareStatePanel::makeRoutingGroup()
{
  auto * group = new QGroupBox("Routing");
  auto * grid = new QGridLayout;


  routing_label_ptr_ = new QLabel("INIT");
  routing_label_ptr_->setAlignment(Qt::AlignCenter);
  routing_label_ptr_->setStyleSheet("border:1px solid black;");
  grid->addWidget(routing_label_ptr_, 0, 0);

  clear_route_button_ptr_ = new QPushButton("Clear Route");
  clear_route_button_ptr_->setCheckable(true);
  connect(clear_route_button_ptr_, SIGNAL(clicked()), SLOT(onClickClearRoute()));
  grid->addWidget(clear_route_button_ptr_, 1, 0);

  group->setLayout(grid);
  return group;
}

QGroupBox * AutowareStatePanel::makeLocalizationGroup()
{
  auto * group = new QGroupBox("Localization");
  auto * grid = new QGridLayout;


  localization_label_ptr_ = new QLabel("INIT");
  localization_label_ptr_->setAlignment(Qt::AlignCenter);
  localization_label_ptr_->setStyleSheet("border:1px solid black;");
  grid->addWidget(localization_label_ptr_, 0, 0);

  group->setLayout(grid);
  return group;
}

QGroupBox * AutowareStatePanel::makeMotionGroup()
{
  auto * group = new QGroupBox("Motion");
  auto * grid = new QGridLayout;


  motion_label_ptr_ = new QLabel("INIT");
  motion_label_ptr_->setAlignment(Qt::AlignCenter);
  motion_label_ptr_->setStyleSheet("border:1px solid black;");
  grid->addWidget(motion_label_ptr_, 0, 0);

  accept_start_button_ptr_ = new QPushButton("Accept Start");
  accept_start_button_ptr_->setCheckable(true);
  connect(accept_start_button_ptr_, SIGNAL(clicked()), SLOT(onClickAcceptStart()));
  grid->addWidget(accept_start_button_ptr_, 1, 0);

  group->setLayout(grid);
  return group;
}

void AutowareStatePanel::onInitialize()
{
  raw_node_ = this->getDisplayContext()->getRosNodeAbstraction().lock()->get_raw_node();

  // Operation Mode
  sub_operation_mode_ = raw_node_->create_subscription<OperationModeState>(
    "/api/operation_mode/state", rclcpp::QoS{1}.transient_local(),
    std::bind(&AutowareStatePanel::onOperationMode, this, _1));

  client_change_to_autonomous_ = raw_node_->create_client<ChangeOperationMode>(
    "/api/operation_mode/change_to_autonomous", rmw_qos_profile_services_default);

  client_change_to_stop_ = raw_node_->create_client<ChangeOperationMode>(
    "/api/operation_mode/change_to_stop", rmw_qos_profile_services_default);

  client_change_to_local_ = raw_node_->create_client<ChangeOperationMode>(
    "/api/operation_mode/change_to_local", rmw_qos_profile_services_default);

  client_change_to_remote_ = raw_node_->create_client<ChangeOperationMode>(
    "/api/operation_mode/change_to_remote", rmw_qos_profile_services_default);

  client_enable_autoware_control_ = raw_node_->create_client<ChangeOperationMode>(
    "/api/operation_mode/enable_autoware_control", rmw_qos_profile_services_default);

  client_enable_direct_control_ = raw_node_->create_client<ChangeOperationMode>(
    "/api/operation_mode/disable_autoware_control", rmw_qos_profile_services_default);

  // Routing
  sub_route_ = raw_node_->create_subscription<RouteState>(
    "/api/routing/state", rclcpp::QoS{1}.transient_local(),
    std::bind(&AutowareStatePanel::onRoute, this, _1));

  client_clear_route_ = raw_node_->create_client<ClearRoute>(
    "/api/routing/clear_route", rmw_qos_profile_services_default);

  // Localization
  sub_localization_ = raw_node_->create_subscription<LocalizationInitializationState>(
    "/api/localization/initialization_state", rclcpp::QoS{1}.transient_local(),
    std::bind(&AutowareStatePanel::onLocalization, this, _1));

  // Motion
  sub_motion_ = raw_node_->create_subscription<MotionState>(
    "/api/motion/state", rclcpp::QoS{1}.transient_local(),
    std::bind(&AutowareStatePanel::onMotion, this, _1));

  client_accept_start_ = raw_node_->create_client<AcceptStart>(
    "/api/motion/accept_start", rmw_qos_profile_services_default);

  sub_gear_ = raw_node_->create_subscription<autoware_auto_vehicle_msgs::msg::GearReport>(
    "/vehicle/status/gear_status", 10, std::bind(&AutowareStatePanel::onShift, this, _1));

  sub_emergency_ = raw_node_->create_subscription<tier4_external_api_msgs::msg::Emergency>(
    "/api/autoware/get/emergency", 10, std::bind(&AutowareStatePanel::onEmergencyStatus, this, _1));

  client_emergency_stop_ = raw_node_->create_client<tier4_external_api_msgs::srv::SetEmergency>(
    "/api/autoware/set/emergency", rmw_qos_profile_services_default);

  pub_velocity_limit_ = raw_node_->create_publisher<tier4_planning_msgs::msg::VelocityLimit>(
    "/planning/scenario_planning/max_velocity_default", rclcpp::QoS{1}.transient_local());
}

void AutowareStatePanel::onOperationMode(const OperationModeState::ConstSharedPtr msg)
{
  auto changeButtonState = [](
                             QPushButton * button, const bool is_desired_mode_available,
                             const uint8_t current_mode = OperationModeState::UNKNOWN,
                             const uint8_t desired_mode = OperationModeState::STOP) {
    if (is_desired_mode_available && current_mode != desired_mode) {
      button->setChecked(false);
      button->setEnabled(true);
    } else {
      button->setChecked(true);
      button->setEnabled(false);
    }
  };

  // Operation Mode
  switch (msg->mode) {
    case OperationModeState::AUTONOMOUS:
      operation_mode_label_ptr_->setText("AUTONOMOUS");
      operation_mode_label_ptr_->setStyleSheet("background-color: #00FF00;");
      break;

    case OperationModeState::LOCAL:
      operation_mode_label_ptr_->setText("LOCAL");
      operation_mode_label_ptr_->setStyleSheet("background-color: #FFFF00;");
      break;

    case OperationModeState::REMOTE:
      operation_mode_label_ptr_->setText("REMOTE");
      operation_mode_label_ptr_->setStyleSheet("background-color: #FFFF00;");
      break;

    case OperationModeState::STOP:
      operation_mode_label_ptr_->setText("STOP");
      operation_mode_label_ptr_->setStyleSheet("background-color: #FFA500;");
      break;

    default:
      operation_mode_label_ptr_->setText("UNKNOWN");
      operation_mode_label_ptr_->setStyleSheet("background-color: #FF0000;");
      break;
  }

  if (msg->is_in_transition) {
    operation_mode_label_ptr_->setText(operation_mode_label_ptr_->text() + "\n(TRANSITION)");
  }

  // Control Mode
  if (msg->is_autoware_control_enabled) {
    control_mode_label_ptr_->setText("Enable");
    control_mode_label_ptr_->setStyleSheet("background-color: #00FF00;");
  } else {
    control_mode_label_ptr_->setText("Disable");
    control_mode_label_ptr_->setStyleSheet("background-color: #FFFF00;");
  }

  // Button
  changeButtonState(
    auto_button_ptr_, msg->is_autonomous_mode_available, msg->mode, OperationModeState::AUTONOMOUS);
  changeButtonState(
    stop_button_ptr_, msg->is_stop_mode_available, msg->mode, OperationModeState::STOP);
  changeButtonState(
    local_button_ptr_, msg->is_local_mode_available, msg->mode, OperationModeState::LOCAL);
  changeButtonState(
    remote_button_ptr_, msg->is_remote_mode_available, msg->mode, OperationModeState::REMOTE);

  changeButtonState(enable_button_ptr_, !msg->is_autoware_control_enabled);
  changeButtonState(disable_button_ptr_, msg->is_autoware_control_enabled);
}

void AutowareStatePanel::onRoute(const RouteState::ConstSharedPtr msg)
{
  switch (msg->state) {
    case RouteState::UNSET:
      routing_label_ptr_->setText("UNSET");
      routing_label_ptr_->setStyleSheet("background-color: #FFFF00;");
      break;

    case RouteState::SET:
      routing_label_ptr_->setText("SET");
      routing_label_ptr_->setStyleSheet("background-color: #00FF00;");
      break;

    case RouteState::ARRIVED:
      routing_label_ptr_->setText("ARRIVED");
      routing_label_ptr_->setStyleSheet("background-color: #FFA500;");
      break;

    case RouteState::CHANGING:
      routing_label_ptr_->setText("CHANGING");
      routing_label_ptr_->setStyleSheet("background-color: #FFFF00;");
      break;

    default:
      routing_label_ptr_->setText("UNKNOWN");
      routing_label_ptr_->setStyleSheet("background-color: #FF0000;");
      break;
  }

  if (msg->state == RouteState::SET)
  {
    clear_route_button_ptr_->setChecked(false);
    clear_route_button_ptr_->setEnabled(true);
  } else {
    clear_route_button_ptr_->setChecked(true);
    clear_route_button_ptr_->setEnabled(false);
  }
}

void AutowareStatePanel::onLocalization(const LocalizationInitializationState::ConstSharedPtr msg)
{
  QString text = "";
  QString style_sheet = "";
  switch (msg->state) {
    case LocalizationInitializationState::UNINITIALIZED:
      text = "UNINITIALIZED";
      style_sheet = "background-color: #FFFF00;";  // yellow
      break;

    case LocalizationInitializationState::INITIALIZING:
      text = "INITIALIZING";
      style_sheet = "background-color: #FFA500;";  // orange
      break;

    case LocalizationInitializationState::INITIALIZED:
      text = "INITIALIZED";
      style_sheet = "background-color: #00FF00;";  // green
      break;

    default:
      text = "UNKNOWN";
      style_sheet = "background-color: #FF0000;";  // red
      break;
  }

  localization_label_ptr_->setText(text);
  localization_label_ptr_->setStyleSheet(style_sheet);
}

void AutowareStatePanel::onMotion(const MotionState::ConstSharedPtr msg)
{
  QString text = "";
  QString style_sheet = "";
  switch (msg->state) {
    case MotionState::STARTING:
      text = "STARTING";
      style_sheet = "background-color: #FFFF00;";  // yellow
      break;

    case MotionState::STOPPED:
      text = "STOPPED";
      style_sheet = "background-color: #FFA500;";  // orange
      break;

    case MotionState::MOVING:
      text = "MOVING";
      style_sheet = "background-color: #00FF00;";  // green
      break;

    default:
      text = "UNKNOWN";
      style_sheet = "background-color: #FF0000;";  // red
      break;
  }

  motion_label_ptr_->setText(text);
  motion_label_ptr_->setStyleSheet(style_sheet);

  if (msg->state == MotionState::STARTING) {
    accept_start_button_ptr_->setChecked(false);
    accept_start_button_ptr_->setEnabled(true);
  } else {
    accept_start_button_ptr_->setChecked(true);
    accept_start_button_ptr_->setEnabled(false);
  }
}

void AutowareStatePanel::onShift(
  const autoware_auto_vehicle_msgs::msg::GearReport::ConstSharedPtr msg)
{
  switch (msg->report) {
    case autoware_auto_vehicle_msgs::msg::GearReport::PARK:
      gear_label_ptr_->setText("PARKING");
      break;
    case autoware_auto_vehicle_msgs::msg::GearReport::REVERSE:
      gear_label_ptr_->setText("REVERSE");
      break;
    case autoware_auto_vehicle_msgs::msg::GearReport::DRIVE:
      gear_label_ptr_->setText("DRIVE");
      break;
    case autoware_auto_vehicle_msgs::msg::GearReport::LOW:
      gear_label_ptr_->setText("LOW");
      break;
  }
}

void AutowareStatePanel::onEmergencyStatus(
  const tier4_external_api_msgs::msg::Emergency::ConstSharedPtr msg)
{
  current_emergency_ = msg->emergency;
  if (msg->emergency) {
    emergency_button_ptr_->setText(QString::fromStdString("Clear Emergency"));
    emergency_button_ptr_->setStyleSheet("background-color: #FF0000;");
  } else {
    emergency_button_ptr_->setText(QString::fromStdString("Set Emergency"));
    emergency_button_ptr_->setStyleSheet("background-color: #00FF00;");
  }
}

void AutowareStatePanel::onClickVelocityLimit()
{
  auto velocity_limit = std::make_shared<tier4_planning_msgs::msg::VelocityLimit>();
  velocity_limit->max_velocity = pub_velocity_limit_input_->value() / 3.6;
  pub_velocity_limit_->publish(*velocity_limit);
}

void AutowareStatePanel::onClickAutonomous() { changeOperationMode(client_change_to_autonomous_); }
void AutowareStatePanel::onClickStop() { changeOperationMode(client_change_to_stop_); }
void AutowareStatePanel::onClickLocal() { changeOperationMode(client_change_to_local_); }
void AutowareStatePanel::onClickRemote() { changeOperationMode(client_change_to_remote_); }
void AutowareStatePanel::onClickAutowareControl()
{
  changeOperationMode(client_enable_autoware_control_);
}
void AutowareStatePanel::onClickDirectControl()
{
  changeOperationMode(client_enable_direct_control_);
}

void AutowareStatePanel::changeOperationMode(
  const rclcpp::Client<ChangeOperationMode>::SharedPtr client)
{
  auto req = std::make_shared<ChangeOperationMode::Request>();

  RCLCPP_INFO(raw_node_->get_logger(), "client request");

  if (!client->service_is_ready()) {
    RCLCPP_INFO(raw_node_->get_logger(), "client is unavailable");
    return;
  }

  client->async_send_request(req, [this](rclcpp::Client<ChangeOperationMode>::SharedFuture result) {
    RCLCPP_INFO(
      raw_node_->get_logger(), "Status: %d, %s", result.get()->status.code,
      result.get()->status.message.c_str());
  });
}

void AutowareStatePanel::onClickClearRoute()
{
  auto req = std::make_shared<ClearRoute::Request>();

  RCLCPP_INFO(raw_node_->get_logger(), "client request");

  if (!client_clear_route_->service_is_ready()) {
    RCLCPP_INFO(raw_node_->get_logger(), "client is unavailable");
    return;
  }

  client_clear_route_->async_send_request(
    req, [this](rclcpp::Client<ClearRoute>::SharedFuture result) {
      RCLCPP_INFO(
        raw_node_->get_logger(), "Status: %d, %s", result.get()->status.code,
        result.get()->status.message.c_str());
    });
}

void AutowareStatePanel::onClickAcceptStart()
{
  auto req = std::make_shared<AcceptStart::Request>();

  RCLCPP_INFO(raw_node_->get_logger(), "client request");

  if (!client_accept_start_->service_is_ready()) {
    RCLCPP_INFO(raw_node_->get_logger(), "client is unavailable");
    return;
  }

  client_accept_start_->async_send_request(
    req, [this](rclcpp::Client<AcceptStart>::SharedFuture result) {
      RCLCPP_INFO(
        raw_node_->get_logger(), "Status: %d, %s", result.get()->status.code,
        result.get()->status.message.c_str());
    });
}


void AutowareStatePanel::onClickEmergencyButton()
{
  using tier4_external_api_msgs::msg::ResponseStatus;
  using tier4_external_api_msgs::srv::SetEmergency;

  auto request = std::make_shared<SetEmergency::Request>();
  request->emergency = !current_emergency_;

  RCLCPP_INFO(raw_node_->get_logger(), request->emergency ? "Set Emergency" : "Clear Emergency");

  client_emergency_stop_->async_send_request(
    request, [this](rclcpp::Client<SetEmergency>::SharedFuture result) {
      const auto & response = result.get();
      if (response->status.code == ResponseStatus::SUCCESS) {
        RCLCPP_INFO(raw_node_->get_logger(), "service succeeded");
      } else {
        RCLCPP_WARN(
          raw_node_->get_logger(), "service failed: %s", response->status.message.c_str());
      }
    });
}

}  // namespace rviz_plugins

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rviz_plugins::AutowareStatePanel, rviz_common::Panel)
