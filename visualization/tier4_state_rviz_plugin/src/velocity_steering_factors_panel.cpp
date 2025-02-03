//
//  Copyright 2023 TIER IV, Inc. All rights reserved.
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

#include "include/velocity_steering_factors_panel.hpp"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QString>
#include <QVBoxLayout>
#include <autoware/motion_utils/distance/distance.hpp>
#include <rviz_common/display_context.hpp>

#include <algorithm>
#include <memory>
#include <string>

namespace rviz_plugins
{
VelocitySteeringFactorsPanel::VelocitySteeringFactorsPanel(QWidget * parent)
: rviz_common::Panel(parent)
{
  // Layout
  auto * v_layout = new QVBoxLayout;
  v_layout->addWidget(makeVelocityFactorsGroup());
  v_layout->addWidget(makeSteeringFactorsGroup());
  setLayout(v_layout);
}

QGroupBox * VelocitySteeringFactorsPanel::makeVelocityFactorsGroup()
{
  auto * group = new QGroupBox("VelocityFactors");
  auto * grid = new QGridLayout;

  auto vertical_header = new QHeaderView(Qt::Vertical);
  vertical_header->hide();
  auto horizontal_header = new QHeaderView(Qt::Horizontal);
  horizontal_header->setSectionResizeMode(QHeaderView::ResizeToContents);
  horizontal_header->setStretchLastSection(true);

  auto header_labels = QStringList({"Type", "Status", "Distance [m]", "Detail"});
  velocity_factors_table_ = new QTableWidget();
  velocity_factors_table_->setColumnCount(header_labels.size());
  velocity_factors_table_->setHorizontalHeaderLabels(header_labels);
  velocity_factors_table_->setVerticalHeader(vertical_header);
  velocity_factors_table_->setHorizontalHeader(horizontal_header);
  grid->addWidget(velocity_factors_table_, 0, 0, 4, 1);

  auto * jerk_label = new QLabel("Jerk");
  grid->addWidget(jerk_label, 0, 1);

  jerk_input_ = new QDoubleSpinBox;
  jerk_input_->setMinimum(0.0);
  jerk_input_->setValue(JERK_DEFAULT);
  jerk_input_->setSingleStep(0.1);
  jerk_input_->setSuffix(" [m/s\u00B3]");
  grid->addWidget(jerk_input_, 1, 1);

  auto * decel_limit_label = new QLabel("Decel limit");
  grid->addWidget(decel_limit_label, 2, 1);

  decel_limit_input_ = new QDoubleSpinBox;
  decel_limit_input_->setMinimum(0.0);
  decel_limit_input_->setValue(DECEL_LIMIT_DEFAULT);
  decel_limit_input_->setSingleStep(0.1);
  decel_limit_input_->setSuffix(" [m/s\u00B2]");
  grid->addWidget(decel_limit_input_, 3, 1);

  group->setLayout(grid);
  return group;
}

QGroupBox * VelocitySteeringFactorsPanel::makeSteeringFactorsGroup()
{
  auto * group = new QGroupBox("SteeringFactors");
  auto * grid = new QGridLayout;

  auto vertical_header = new QHeaderView(Qt::Vertical);
  vertical_header->hide();
  auto horizontal_header = new QHeaderView(Qt::Horizontal);
  horizontal_header->setSectionResizeMode(QHeaderView::Stretch);

  auto header_labels =
    QStringList({"Type", "Status", "Distance.1 [m]", "Distance.2 [m]", "Direction", "Detail"});
  steering_factors_table_ = new QTableWidget();
  steering_factors_table_->setColumnCount(header_labels.size());
  steering_factors_table_->setHorizontalHeaderLabels(header_labels);
  steering_factors_table_->setVerticalHeader(vertical_header);
  steering_factors_table_->setHorizontalHeader(horizontal_header);
  grid->addWidget(steering_factors_table_, 1, 0);

  group->setLayout(grid);
  return group;
}

void VelocitySteeringFactorsPanel::onInitialize()
{
  using std::placeholders::_1;

  raw_node_ = this->getDisplayContext()->getRosNodeAbstraction().lock()->get_raw_node();

  // Planning
  sub_kinematic_state_ = raw_node_->create_subscription<nav_msgs::msg::Odometry>(
    "/localization/kinematic_state", 10,
    [this](const nav_msgs::msg::Odometry::ConstSharedPtr msg) { kinematic_state_ = msg; });

  sub_acceleration_ =
    raw_node_->create_subscription<geometry_msgs::msg::AccelWithCovarianceStamped>(
      "/localization/acceleration", 10,
      [this](const geometry_msgs::msg::AccelWithCovarianceStamped::ConstSharedPtr msg) {
        acceleration_ = msg;
      });

  sub_velocity_factors_ = raw_node_->create_subscription<VelocityFactorArray>(
    "/api/planning/velocity_factors", 10,
    std::bind(&VelocitySteeringFactorsPanel::onVelocityFactors, this, _1));

  sub_steering_factors_ = raw_node_->create_subscription<SteeringFactorArray>(
    "/api/planning/steering_factors", 10,
    std::bind(&VelocitySteeringFactorsPanel::onSteeringFactors, this, _1));
}

void VelocitySteeringFactorsPanel::onVelocityFactors(const VelocityFactorArray::ConstSharedPtr msg)
{
  velocity_factors_table_->clearContents();
  velocity_factors_table_->setRowCount(msg->factors.size());

  auto sorted = *msg;

  // sort by distance to the decel/stop point.
  std::sort(sorted.factors.begin(), sorted.factors.end(), [](const auto & a, const auto & b) {
    return a.distance < b.distance;
  });

  for (std::size_t i = 0; i < sorted.factors.size(); i++) {
    const auto & e = sorted.factors.at(i);

    // behavior
    {
      auto label = new QLabel(e.behavior.empty() ? "UNKNOWN" : e.behavior.c_str());
      label->setAlignment(Qt::AlignCenter);
      velocity_factors_table_->setCellWidget(i, 0, label);
    }

    // status
    {
      auto label = new QLabel();
      switch (e.status) {
        case VelocityFactor::APPROACHING:
          label->setText("APPROACHING");
          break;
        case VelocityFactor::STOPPED:
          label->setText("STOPPED");
          break;
        default:
          label->setText("UNKNOWN");
          break;
      }
      label->setAlignment(Qt::AlignCenter);
      velocity_factors_table_->setCellWidget(i, 1, label);
    }

    // distance
    {
      auto label = new QLabel();
      std::stringstream ss;
      ss << std::fixed << std::setprecision(2) << e.distance;
      label->setText(QString::fromStdString(ss.str()));
      label->setAlignment(Qt::AlignCenter);
      velocity_factors_table_->setCellWidget(i, 2, label);
    }

    // detail
    {
      auto label = new QLabel(QString::fromStdString(e.detail));
      label->setAlignment(Qt::AlignCenter);
      velocity_factors_table_->setCellWidget(i, 3, label);
    }

    const auto row_background = [&]() -> QBrush {
      if (!kinematic_state_ || !acceleration_) {
        return {};
      }
      const auto & current_vel = kinematic_state_->twist.twist.linear.x;
      const auto & current_acc = acceleration_->accel.accel.linear.x;
      const auto acc_min = -decel_limit_input_->value();
      const auto jerk_acc = jerk_input_->value();
      const auto decel_dist = autoware::motion_utils::calcDecelDistWithJerkAndAccConstraints(
        current_vel, 0., current_acc, acc_min, jerk_acc, -jerk_acc);
      if (decel_dist > e.distance && e.distance >= 0 && e.status == VelocityFactor::APPROACHING) {
        return COLOR_FREAK_PINK;
      }
      return {};
    }();
    for (int j = 0; j < velocity_factors_table_->columnCount(); j++) {
      velocity_factors_table_->setItem(i, j, new QTableWidgetItem);
      velocity_factors_table_->item(i, j)->setBackground(row_background);
    }
  }
  velocity_factors_table_->update();
}

void VelocitySteeringFactorsPanel::onSteeringFactors(const SteeringFactorArray::ConstSharedPtr msg)
{
  steering_factors_table_->clearContents();
  steering_factors_table_->setRowCount(msg->factors.size());

  auto sorted = *msg;

  // sort by distance to the point where it starts moving the steering.
  std::sort(sorted.factors.begin(), sorted.factors.end(), [](const auto & a, const auto & b) {
    return a.distance.front() < b.distance.front();
  });

  for (std::size_t i = 0; i < sorted.factors.size(); i++) {
    const auto & e = sorted.factors.at(i);

    // behavior
    {
      auto label = new QLabel(e.behavior.empty() ? "UNKNOWN" : e.behavior.c_str());
      label->setAlignment(Qt::AlignCenter);
      steering_factors_table_->setCellWidget(i, 0, label);
    }

    // status
    {
      auto label = new QLabel();
      switch (e.status) {
        case SteeringFactor::APPROACHING:
          label->setText("APPROACHING");
          break;
        case SteeringFactor::TURNING:
          label->setText("TURNING");
          break;
        default:
          label->setText("UNKNOWN");
          break;
      }
      label->setAlignment(Qt::AlignCenter);
      steering_factors_table_->setCellWidget(i, 1, label);
    }

    // distance.1
    {
      auto label = new QLabel();
      std::stringstream ss;
      ss << std::fixed << std::setprecision(2) << e.distance.front();
      label->setText(QString::fromStdString(ss.str()));
      label->setAlignment(Qt::AlignCenter);
      steering_factors_table_->setCellWidget(i, 2, label);
    }

    // distance.2
    {
      auto label = new QLabel();
      std::stringstream ss;
      ss << std::fixed << std::setprecision(2) << e.distance.back();
      label->setText(QString::fromStdString(ss.str()));
      label->setAlignment(Qt::AlignCenter);
      steering_factors_table_->setCellWidget(i, 3, label);
    }

    // Direction
    {
      auto label = new QLabel();
      switch (e.direction) {
        case SteeringFactor::LEFT:
          label->setText("LEFT");
          break;
        case SteeringFactor::RIGHT:
          label->setText("RIGHT");
          break;
        case SteeringFactor::STRAIGHT:
          label->setText("STRAIGHT");
          break;
        default:
          label->setText("UNKNOWN");
          break;
      }
      label->setAlignment(Qt::AlignCenter);
      steering_factors_table_->setCellWidget(i, 4, label);
    }

    // detail
    {
      auto label = new QLabel(QString::fromStdString(e.detail));
      label->setAlignment(Qt::AlignCenter);
      steering_factors_table_->setCellWidget(i, 5, label);
    }
  }
  steering_factors_table_->update();
}

void VelocitySteeringFactorsPanel::save(rviz_common::Config config) const
{
  rviz_common::Panel::save(config);
  config.mapSetValue("jerk", jerk_input_->value());
  config.mapSetValue("decel_limit", decel_limit_input_->value());
}

void VelocitySteeringFactorsPanel::load(const rviz_common::Config & config)
{
  rviz_common::Panel::load(config);
  if (float jerk; config.mapGetFloat("jerk", &jerk)) {
    jerk_input_->setValue(static_cast<double>(jerk));
  }
  if (float decel_limit; config.mapGetFloat("decel_limit", &decel_limit)) {
    decel_limit_input_->setValue(static_cast<double>(decel_limit));
  }
}
}  // namespace rviz_plugins

#include <pluginlib/class_list_macros.hpp>
PLUGINLIB_EXPORT_CLASS(rviz_plugins::VelocitySteeringFactorsPanel, rviz_common::Panel)
