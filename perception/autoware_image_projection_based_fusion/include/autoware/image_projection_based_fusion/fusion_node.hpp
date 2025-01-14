// Copyright 2022 TIER IV, Inc.
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

#ifndef AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__FUSION_NODE_HPP_
#define AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__FUSION_NODE_HPP_

#include <autoware/image_projection_based_fusion/camera_projection.hpp>
#include <autoware/image_projection_based_fusion/debugger.hpp>
#include <autoware/universe_utils/ros/debug_publisher.hpp>
#include <autoware/universe_utils/system/stop_watch.hpp>
#include <autoware/universe_utils/system/time_keeper.hpp>
#include <rclcpp/rclcpp.hpp>

#include <autoware_perception_msgs/msg/detected_objects.hpp>
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

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace autoware::image_projection_based_fusion
{
using autoware_perception_msgs::msg::DetectedObject;
using autoware_perception_msgs::msg::DetectedObjects;
using sensor_msgs::msg::CameraInfo;
using sensor_msgs::msg::Image;
using PointCloudMsgType = sensor_msgs::msg::PointCloud2;
using RoiMsgType = tier4_perception_msgs::msg::DetectedObjectsWithFeature;
using ClusterMsgType = tier4_perception_msgs::msg::DetectedObjectsWithFeature;
using ClusterObjType = tier4_perception_msgs::msg::DetectedObjectWithFeature;
using tier4_perception_msgs::msg::DetectedObjectWithFeature;
using PointCloud = pcl::PointCloud<pcl::PointXYZ>;
using autoware::image_projection_based_fusion::CameraProjection;
using autoware_perception_msgs::msg::ObjectClassification;

template <class Msg2D>
struct Det2dStatus
{
  // camera index
  std::size_t id = 0;
  // camera projection
  std::unique_ptr<CameraProjection> camera_projector_ptr;
  bool project_to_unrectified_image = false;
  bool approximate_camera_projection = false;
  // process flags
  bool is_fused = false;
  // timing
  double input_offset_ms = 0.0;
  // cache
  std::map<int64_t, typename Msg2D::ConstSharedPtr> cached_det2d_msgs;
};

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

private:
  // Common process methods
  void cameraInfoCallback(
    const sensor_msgs::msg::CameraInfo::ConstSharedPtr input_camera_info_msg,
    const std::size_t camera_id);

  // callback for timer
  void timer_callback();
  void setPeriod(const int64_t new_period);
  void exportProcess();

  // 2d detection management methods
  bool checkAllDet2dFused()
  {
    for (const auto & det2d : det2d_list_) {
      if (!det2d.is_fused) {
        return false;
      }
    }
    return true;
  }

  // camera projection
  float approx_grid_cell_w_size_;
  float approx_grid_cell_h_size_;

  // timer
  rclcpp::TimerBase::SharedPtr timer_;
  double timeout_ms_{};
  double match_threshold_ms_{};

  std::vector<typename rclcpp::Subscription<Msg2D>::SharedPtr> rois_subs_;
  std::vector<rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr> camera_info_subs_;

  // cache for fusion
  int64_t cached_det3d_msg_timestamp_;
  typename Msg3D::SharedPtr cached_det3d_msg_ptr_;

protected:
  void setDet2DStatus(std::size_t rois_number);

  // callback for main subscription
  void subCallback(const typename Msg3D::ConstSharedPtr input_msg);
  // callback for roi subscription
  void roiCallback(const typename Msg2D::ConstSharedPtr input_roi_msg, const std::size_t roi_i);

  // Custom process methods
  virtual void preprocess(Msg3D & output_msg);
  virtual void fuseOnSingleImage(
    const Msg3D & input_msg, const Det2dStatus<Msg2D> & det2d, const Msg2D & input_roi_msg,
    Msg3D & output_msg) = 0;
  virtual void postprocess(const Msg3D & processing_msg, ExportObj & output_msg);
  virtual void publish(const ExportObj & output_msg);

  // Members
  tf2_ros::Buffer tf_buffer_;
  tf2_ros::TransformListener tf_listener_;

  // 2d detection management
  std::vector<Det2dStatus<Msg2D>> det2d_list_;

  /** \brief A vector of subscriber. */
  typename rclcpp::Subscription<Msg3D>::SharedPtr sub_;

  // parameters for out_of_scope filter
  float filter_scope_min_x_;
  float filter_scope_max_x_;
  float filter_scope_min_y_;
  float filter_scope_max_y_;
  float filter_scope_min_z_;
  float filter_scope_max_z_;

  // output publisher
  typename rclcpp::Publisher<ExportObj>::SharedPtr pub_ptr_;

  // debugger
  std::shared_ptr<Debugger> debugger_;

  /** \brief processing time publisher. **/
  std::unique_ptr<autoware::universe_utils::StopWatch<std::chrono::milliseconds>> stop_watch_ptr_;
  std::unique_ptr<autoware::universe_utils::DebugPublisher> debug_publisher_;

  // timekeeper
  rclcpp::Publisher<autoware::universe_utils::ProcessingTimeDetail>::SharedPtr
    detailed_processing_time_publisher_;
  std::shared_ptr<autoware::universe_utils::TimeKeeper> time_keeper_;
};

}  // namespace autoware::image_projection_based_fusion

#endif  // AUTOWARE__IMAGE_PROJECTION_BASED_FUSION__FUSION_NODE_HPP_
