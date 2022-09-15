// Copyright 2022 Tier IV, Inc.
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

#include <detection_class_adapter/detection_class_adapter.hpp>

DetectionClassAdapter::DetectionClassAdapter(const rclcpp::NodeOptions & options)
: Node("detection_class_adaptor", options)
{
  const auto allow_remapping_by_area_matrix =
    this->declare_parameter<std::vector<int64_t>>("allow_remapping_by_area_matrix");
  const auto min_area_matrix = this->declare_parameter<std::vector<double>>("min_area_matrix");
  const auto max_area_matrix = this->declare_parameter<std::vector<double>>("max_area_matrix");

  assert(allow_remapping_by_area_.size() == min_area_matrix.size());
  assert(allow_remapping_by_area_.size() == max_area_matrix.size());
  assert(std::pow(std::sqrt(min_area_matrix.size()), 2) == std::sqrt(min_area_matrix.size()));

  num_labels_ = static_cast<int>(std::sqrt(min_area_matrix.size()));

  Eigen::Map<const Eigen::Matrix<int64_t, Eigen::Dynamic, Eigen::Dynamic>>
    allow_remapping_by_area_matrix_tmp(
      allow_remapping_by_area_matrix.data(), num_labels_, num_labels_);
  allow_remapping_by_area_matrix_ = allow_remapping_by_area_matrix_tmp.transpose()
                                      .cast<bool>();  // Eigen is column major by default

  Eigen::Map<const Eigen::MatrixXd> min_area_matrix_tmp(
    min_area_matrix.data(), num_labels_, num_labels_);
  min_area_matrix_ = min_area_matrix_tmp.transpose();  // Eigen is column major by default

  Eigen::Map<const Eigen::MatrixXd> max_area_matrix_tmp(
    max_area_matrix.data(), num_labels_, num_labels_);
  max_area_matrix_ = max_area_matrix_tmp.transpose();  // Eigen is column major by default

  min_area_matrix_ = min_area_matrix_.unaryExpr(
    [](double v) { return std::isfinite(v) ? v : std::numeric_limits<double>::max(); });
  max_area_matrix_ = max_area_matrix_.unaryExpr(
    [](double v) { return std::isfinite(v) ? v : std::numeric_limits<double>::max(); });

  objects_sub_ = this->create_subscription<autoware_auto_perception_msgs::msg::DetectedObjects>(
    "~/input/objects", rclcpp::QoS{1},
    std::bind(&DetectionClassAdapter::detectionsCallback, this, std::placeholders::_1));

  objects_pub_ = this->create_publisher<autoware_auto_perception_msgs::msg::DetectedObjects>(
    "~/output/objects", rclcpp::QoS{1});
}

void DetectionClassAdapter::detectionsCallback(
  const autoware_auto_perception_msgs::msg::DetectedObjects::SharedPtr msg)
{
  autoware_auto_perception_msgs::msg::DetectedObjects remapped_msg = *msg;

  for (auto & object : remapped_msg.objects) {
    const float bev_area = object.shape.dimensions.x * object.shape.dimensions.y;

    for (auto & classification : object.classification) {
      auto & label = classification.label;

      for (int i = 0; i < num_labels_; ++i) {
        if (
          allow_remapping_by_area_matrix_(label, i) && bev_area >= min_area_matrix_(label, i) &&
          bev_area <= max_area_matrix_(label, i)) {
          label = i;
          break;
        }
      }
    }
  }

  objects_pub_->publish(remapped_msg);
}

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(DetectionClassAdapter)
