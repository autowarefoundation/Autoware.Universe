// Copyright 2025 TIER IV, Inc.
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

#pragma once

#include "autoware/image_projection_based_fusion/fusion_collector.hpp"
#include "autoware/image_projection_based_fusion/fusion_matching_strategy.hpp"
// TODO(vivid): maybe we should move this to different node.hpp
#include "autoware/image_projection_based_fusion/fusion_types.hpp"

#include <autoware/image_projection_based_fusion/camera_projection.hpp>
#include <autoware/image_projection_based_fusion/debugger.hpp>
#include <autoware/universe_utils/ros/debug_publisher.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>
#include <diagnostic_updater/diagnostic_updater.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_perception_msgs/msg/detected_objects.hpp>
#include <diagnostic_msgs/msg/diagnostic_array.hpp>
#include <diagnostic_msgs/msg/diagnostic_status.hpp>
#include <sensor_msgs/msg/camera_info.hpp>
#include <sensor_msgs/msg/image.hpp>
#include <sensor_msgs/point_cloud2_iterator.hpp>
#include <tier4_perception_msgs/msg/detected_objects_with_feature.hpp>

#include <message_filters/pass_through.h>
#include <message_filters/subscriber.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <message_filters/synchronizer.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl_conversions/pcl_conversions.h>
#include <tf2_ros/buffer.h>
#include <tf2_ros/transform_listener.h>

#include <cstddef>
#include <list>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace autoware::image_projection_based_fusion
{

template <class Msg3D, class Msg2D, class ExportObj>
class FusionNode : public rclcpp::Node
{
public:
  /** \brief constructor. */
  explicit FusionNode(const std::string & node_name, const rclcpp::NodeOptions & options);
  /** \brief constructor.
   * \param queue_size the maximum queue size
   */
  explicit FusionNode(
    const std::string & node_name, const rclcpp::NodeOptions & options, int queue_size);

  virtual void preprocess(Msg3D & output_msg);
  virtual void fuse_on_single_image(
    const Msg3D & input_msg, const Det2dStatus<Msg2D> & det2d, const Msg2D & input_roi_msg,
    Msg3D & output_msg) = 0;
  void export_process(
    typename Msg3D::SharedPtr & output_det3d_msg,
    std::unordered_map<std::size_t, double> id_to_stamp_map,
    std::shared_ptr<FusionCollectorInfoBase> collector_info);
  std::optional<std::unordered_map<std::string, std::string>> find_concatenation_status(
    double timestamp);
  void show_diagnostic_message(
    std::unordered_map<std::size_t, double> id_to_stamp_map,
    std::shared_ptr<FusionCollectorInfoBase> collector_info);

private:
  // Common process methods
  void camera_info_callback(
    const sensor_msgs::msg::CameraInfo::ConstSharedPtr input_camera_info_msg,
    const std::size_t camera_id);

  void init_strategy();
  void manage_collector_list();
  void manage_concatenated_status_map(const double & current_timestam);

  static std::string format_timestamp(double timestamp);
  void check_fusion_status(diagnostic_updater::DiagnosticStatusWrapper & stat);

  // camera projection
  float approx_grid_cell_w_size_;
  float approx_grid_cell_h_size_;

  bool debug_mode_{false};

  std::size_t rois_number_;
  // timer
  double timeout_sec_{};

  std::vector<typename rclcpp::Subscription<Msg2D>::SharedPtr> rois_subs_;
  std::vector<rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr> camera_info_subs_;

  std::unique_ptr<FusionMatchingStrategy<Msg3D, Msg2D, ExportObj>> fusion_matching_strategy_;
  std::mutex fusion_collectors_mutex_;
  std::list<std::shared_ptr<FusionCollector<Msg3D, Msg2D, ExportObj>>> fusion_collectors_;

  std::unordered_map<std::size_t, double> id_to_noise_window_map_;
  std::unordered_map<std::size_t, double> id_to_offset_map_;
  double cloud_noise_window_;

  // timestamp: (key, value)
  std::unordered_map<double, std::unordered_map<std::string, std::string>> concatenated_status_map_;

  diagnostic_updater::Updater diagnostic_updater_{this};
  std::shared_ptr<FusionCollectorInfoBase> diagnostic_collector_info_;
  std::unordered_map<std::size_t, double> diagnostic_id_to_stamp_map_;

  double current_output_msg_timestamp_{0.0};
  double latest_output_msg_timestamp_{0.0};
  double rosbag_length_{10.0};
  bool publish_previous_but_late_output_msg_{false};
  bool drop_previous_but_late_output_msg_{false};
  bool publish_output_msg_{false};
  bool det3d_fused_{true};

protected:
  void set_det2d_status(std::size_t rois_number);

  // callback for main subscription
  void sub_callback(const typename Msg3D::ConstSharedPtr det3d_msg);
  // callback for roi subscription
  void roi_callback(const typename Msg2D::ConstSharedPtr det2d_msg, const std::size_t roi_i);

  void diagnostic_callback(const diagnostic_msgs::msg::DiagnosticArray::SharedPtr diagnostic_msg);

  // Custom process methods
  virtual void postprocess(const Msg3D & processing_msg, ExportObj & output_msg);
  virtual void publish(const ExportObj & output_msg);

  // Members
  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  // 2d detection management
  std::vector<Det2dStatus<Msg2D>> det2d_list_;

  // 3d detection subscription
  typename rclcpp::Subscription<Msg3D>::SharedPtr det3d_sub_;
  rclcpp::Subscription<diagnostic_msgs::msg::DiagnosticArray>::SharedPtr sub_diag_;

  // parameters for out_of_scope filter
  float filter_scope_min_x_;
  float filter_scope_max_x_;
  float filter_scope_min_y_;
  float filter_scope_max_y_;
  float filter_scope_min_z_;
  float filter_scope_max_z_;

  std::string matching_strategy_;

  // output publisher
  typename rclcpp::Publisher<ExportObj>::SharedPtr pub_ptr_;

  // debugger
  std::shared_ptr<Debugger> debugger_;
  std::unique_ptr<autoware::universe_utils::DebugPublisher> debug_internal_pub_;

  /** \brief processing time publisher. **/
  std::unique_ptr<autoware::universe_utils::StopWatch<std::chrono::milliseconds>> stop_watch_ptr_;
  std::unique_ptr<autoware::universe_utils::DebugPublisher> debug_publisher_;

  // timekeeper
  rclcpp::Publisher<autoware::universe_utils::ProcessingTimeDetail>::SharedPtr
    detailed_processing_time_publisher_;
  std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_;
};

}  // namespace autoware::image_projection_based_fusion
