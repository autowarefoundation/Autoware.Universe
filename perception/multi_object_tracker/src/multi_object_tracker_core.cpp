// Copyright 2020 Tier IV, Inc.
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
//
//

#include <boost/optional.hpp>

#include <glog/logging.h>
#include <tf2_ros/create_timer_interface.h>
#include <tf2_ros/create_timer_ros.h>

#include <iterator>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define EIGEN_MPL2_ONLY
#include "multi_object_tracker/multi_object_tracker_core.hpp"
#include "multi_object_tracker/utils/utils.hpp"

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <rclcpp_components/register_node_macro.hpp>

namespace
{
// Function to get the transform between two frames
boost::optional<geometry_msgs::msg::Transform> getTransformAnonymous(
  const tf2_ros::Buffer & tf_buffer, const std::string & source_frame_id,
  const std::string & target_frame_id, const rclcpp::Time & time)
{
  try {
    // Check if the frames are ready
    std::string errstr;  // This argument prevents error msg from being displayed in the terminal.
    if (!tf_buffer.canTransform(
          target_frame_id, source_frame_id, tf2::TimePointZero, tf2::Duration::zero(), &errstr)) {
      return boost::none;
    }

    // Lookup the transform
    geometry_msgs::msg::TransformStamped self_transform_stamped;
    self_transform_stamped = tf_buffer.lookupTransform(
      /*target*/ target_frame_id, /*src*/ source_frame_id, time,
      rclcpp::Duration::from_seconds(0.5));
    return self_transform_stamped.transform;
  } catch (tf2::TransformException & ex) {
    RCLCPP_WARN_STREAM(rclcpp::get_logger("multi_object_tracker"), ex.what());
    return boost::none;
  }
}

}  // namespace

namespace multi_object_tracker
{
using Label = autoware_auto_perception_msgs::msg::ObjectClassification;

MultiObjectTracker::MultiObjectTracker(const rclcpp::NodeOptions & node_options)
: rclcpp::Node("multi_object_tracker", node_options),
  tf_buffer_(this->get_clock()),
  tf_listener_(tf_buffer_),
  last_published_time_(this->now())
{
  // Get parameters
  double publish_rate = declare_parameter<double>("publish_rate");  // [hz]
  world_frame_id_ = declare_parameter<std::string>("world_frame_id");
  bool enable_delay_compensation{declare_parameter<bool>("enable_delay_compensation")};
  declare_parameter("input_topic_names", std::vector<std::string>());
  input_topic_names_ = get_parameter("input_topic_names").as_string_array();

  // Initialize the last measurement time
  last_measurement_time_ = this->now();

  // ROS interface - Publisher
  tracked_objects_pub_ = create_publisher<TrackedObjects>("output", rclcpp::QoS{1});

  // ROS interface - Subscribers
  if (input_topic_names_.empty()) {
    RCLCPP_ERROR(get_logger(), "Need a 'input_topic_names' parameter to be set before continuing");
    return;
  }
  input_topic_size_ = input_topic_names_.size();
  sub_objects_array_.resize(input_topic_size_);

  for (size_t i = 0; i < input_topic_size_; i++) {
    RCLCPP_INFO(get_logger(), "Subscribing to %s", input_topic_names_.at(i).c_str());

    std::function<void(const DetectedObjects::ConstSharedPtr msg)> func =
      std::bind(&MultiObjectTracker::onData, this, std::placeholders::_1, i);
    sub_objects_array_.at(i) =
      create_subscription<DetectedObjects>(input_topic_names_.at(i), rclcpp::QoS{1}, func);
  }

  // Create tf timer
  auto cti = std::make_shared<tf2_ros::CreateTimerROS>(
    this->get_node_base_interface(), this->get_node_timers_interface());
  tf_buffer_.setCreateTimerInterface(cti);

  // Create ROS time based timer.
  // If the delay compensation is enabled, the timer is used to publish the output at the correct
  // time.
  if (enable_delay_compensation) {
    publisher_period_ = 1.0 / publish_rate;    // [s]
    constexpr double timer_multiplier = 20.0;  // 20 times frequent for publish timing check
    const auto timer_period = rclcpp::Rate(publish_rate * timer_multiplier).period();
    publish_timer_ = rclcpp::create_timer(
      this, get_clock(), timer_period, std::bind(&MultiObjectTracker::onTimer, this));
  }

  // Initialize processor
  {
    std::map<std::uint8_t, std::string> tracker_map;
    tracker_map.insert(
      std::make_pair(Label::CAR, this->declare_parameter<std::string>("car_tracker")));
    tracker_map.insert(
      std::make_pair(Label::TRUCK, this->declare_parameter<std::string>("truck_tracker")));
    tracker_map.insert(
      std::make_pair(Label::BUS, this->declare_parameter<std::string>("bus_tracker")));
    tracker_map.insert(
      std::make_pair(Label::TRAILER, this->declare_parameter<std::string>("trailer_tracker")));
    tracker_map.insert(std::make_pair(
      Label::PEDESTRIAN, this->declare_parameter<std::string>("pedestrian_tracker")));
    tracker_map.insert(
      std::make_pair(Label::BICYCLE, this->declare_parameter<std::string>("bicycle_tracker")));
    tracker_map.insert(std::make_pair(
      Label::MOTORCYCLE, this->declare_parameter<std::string>("motorcycle_tracker")));

    processor_ = std::make_unique<TrackerProcessor>(tracker_map);
  }

  // Data association initialization
  {
    const auto tmp = this->declare_parameter<std::vector<int64_t>>("can_assign_matrix");
    const std::vector<int> can_assign_matrix(tmp.begin(), tmp.end());
    const auto max_dist_matrix = this->declare_parameter<std::vector<double>>("max_dist_matrix");
    const auto max_area_matrix = this->declare_parameter<std::vector<double>>("max_area_matrix");
    const auto min_area_matrix = this->declare_parameter<std::vector<double>>("min_area_matrix");
    const auto max_rad_matrix = this->declare_parameter<std::vector<double>>("max_rad_matrix");
    const auto min_iou_matrix = this->declare_parameter<std::vector<double>>("min_iou_matrix");

    data_association_ = std::make_unique<DataAssociation>(
      can_assign_matrix, max_dist_matrix, max_area_matrix, min_area_matrix, max_rad_matrix,
      min_iou_matrix);
  }

  // Debugger
  debugger_ = std::make_unique<TrackerDebugger>(*this);
  published_time_publisher_ = std::make_unique<tier4_autoware_utils::PublishedTimePublisher>(this);
}

void MultiObjectTracker::onData(
  const DetectedObjects::ConstSharedPtr msg, const size_t array_number)
{
  std::pair<rclcpp::Time, DetectedObjects> pair = std::make_pair(msg->header.stamp, *msg);
  objects_data_.push_back(pair);

  // debug message
  RCLCPP_INFO(
    get_logger(), "Received data from topic %s", input_topic_names_.at(array_number).c_str());
  if (last_measurement_time_ > msg->header.stamp) {
    const double delta_time = (last_measurement_time_ - msg->header.stamp).seconds() * 1e3;
    RCLCPP_INFO(
      get_logger(), "Received data is older than the last measurement time by %f ms", delta_time);
    return;
  }
  if (last_measurement_time_ < msg->header.stamp) {
    last_measurement_time_ = msg->header.stamp;
  }

  // onMeasurement(msg);
}

void MultiObjectTracker::onMeasurement(const DetectedObjects::ConstSharedPtr input_objects_msg)
{
  // Get the time of the measurement
  const rclcpp::Time measurement_time =
    rclcpp::Time(input_objects_msg->header.stamp, this->now().get_clock_type());

  debugger_->startMeasurementTime(this->now(), measurement_time);
  runProcess(input_objects_msg);
  debugger_->endMeasurementTime(this->now());

  // Publish objects if the timer is not enabled
  if (publish_timer_ == nullptr) {
    // Publish if the delay compensation is disabled
    publish(measurement_time);
  } else {
    // Publish if the next publish time is close
    const double minimum_publish_interval = publisher_period_ * 0.70;  // 70% of the period
    if ((this->now() - last_published_time_).seconds() > minimum_publish_interval) {
      checkAndPublish(this->now());
    }
  }
}

void MultiObjectTracker::onTimer()
{
  const rclcpp::Time current_time = this->now();

  // Check the publish period
  const auto elapsed_time = (current_time - last_published_time_).seconds();
  // If the elapsed time is over the period, publish objects with prediction
  constexpr double maximum_latency_ratio = 1.11;  // 11% margin
  const double maximum_publish_latency = publisher_period_ * maximum_latency_ratio;

  if (elapsed_time < maximum_publish_latency) return;

  // update trackers from accumulated input objects
  if (objects_data_.empty()) return;

  // sort by time
  sort(objects_data_.begin(), objects_data_.end(), [](const auto & a, const auto & b) {
    return a.first.seconds() < b.first.seconds();
  });

  // run onMeasurement for each queue
  while (!objects_data_.empty()) {
    const auto & pair = objects_data_.front();
    runProcess(std::make_shared<DetectedObjects>(pair.second));
    objects_data_.erase(objects_data_.begin());
  }

  // Publish
  checkAndPublish(current_time);
}

void MultiObjectTracker::runProcess(const DetectedObjects::ConstSharedPtr input_objects_msg)
{
  // Get the time of the measurement
  const rclcpp::Time measurement_time =
    rclcpp::Time(input_objects_msg->header.stamp, this->now().get_clock_type());

  // Get the transform of the self frame
  const auto self_transform =
    getTransformAnonymous(tf_buffer_, "base_link", world_frame_id_, measurement_time);
  if (!self_transform) {
    return;
  }

  // Transform the objects to the world frame
  DetectedObjects transformed_objects;
  if (!object_recognition_utils::transformObjects(
        *input_objects_msg, world_frame_id_, tf_buffer_, transformed_objects)) {
    return;
  }

  /* prediction */
  processor_->predict(measurement_time);

  /* object association */
  std::unordered_map<int, int> direct_assignment, reverse_assignment;
  {
    const auto & list_tracker = processor_->getListTracker();
    const auto & detected_objects = transformed_objects;
    // global nearest neighbor
    Eigen::MatrixXd score_matrix = data_association_->calcScoreMatrix(
      detected_objects, list_tracker);  // row : tracker, col : measurement
    data_association_->assign(score_matrix, direct_assignment, reverse_assignment);
  }

  /* tracker update */
  processor_->update(transformed_objects, *self_transform, direct_assignment);

  /* tracker pruning */
  processor_->prune(measurement_time);

  /* spawn new tracker */
  processor_->spawn(transformed_objects, *self_transform, reverse_assignment);
}

void MultiObjectTracker::checkAndPublish(const rclcpp::Time & time)
{
  /* tracker pruning*/
  processor_->prune(time);

  // Publish
  publish(time);

  // Update last published time
  last_published_time_ = this->now();
}

void MultiObjectTracker::publish(const rclcpp::Time & time) const
{
  debugger_->startPublishTime(this->now());
  const auto subscriber_count = tracked_objects_pub_->get_subscription_count() +
                                tracked_objects_pub_->get_intra_process_subscription_count();
  if (subscriber_count < 1) {
    return;
  }
  // Create output msg
  TrackedObjects output_msg, tentative_objects_msg;
  output_msg.header.frame_id = world_frame_id_;
  processor_->getTrackedObjects(time, output_msg);

  // Publish
  tracked_objects_pub_->publish(output_msg);
  published_time_publisher_->publish_if_subscribed(tracked_objects_pub_, output_msg.header.stamp);

  // Publish debugger information if enabled
  debugger_->endPublishTime(this->now(), time);

  if (debugger_->shouldPublishTentativeObjects()) {
    TrackedObjects tentative_objects_msg;
    tentative_objects_msg.header.frame_id = world_frame_id_;
    processor_->getTentativeObjects(time, tentative_objects_msg);
    debugger_->publishTentativeObjects(tentative_objects_msg);
  }
}

}  // namespace multi_object_tracker

RCLCPP_COMPONENTS_REGISTER_NODE(multi_object_tracker::MultiObjectTracker)
