// Copyright 2023 TIER IV, Inc.
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

#ifndef BEHAVIOR_PATH_PLANNER__SCENE_MODULE__DYNAMIC_AVOIDANCE__DYNAMIC_AVOIDANCE_MODULE_HPP_
#define BEHAVIOR_PATH_PLANNER__SCENE_MODULE__DYNAMIC_AVOIDANCE__DYNAMIC_AVOIDANCE_MODULE_HPP_

#include "behavior_path_planner/scene_module/scene_module_interface.hpp"

#include <rclcpp/rclcpp.hpp>

#include <autoware_auto_perception_msgs/msg/predicted_object.hpp>
#include <autoware_auto_planning_msgs/msg/path_with_lane_id.hpp>
#include <autoware_auto_vehicle_msgs/msg/turn_indicators_command.hpp>
#include <tier4_planning_msgs/msg/avoidance_debug_msg.hpp>
#include <tier4_planning_msgs/msg/avoidance_debug_msg_array.hpp>

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace behavior_path_planner
{
struct MinMaxValue
{
  double min_value;
  double max_value;
};

struct DynamicAvoidanceParameters
{
  // common
  bool enable_debug_info{true};

  // obstacle types to avoid
  bool avoid_car{true};
  bool avoid_truck{true};
  bool avoid_bus{true};
  bool avoid_trailer{true};
  bool avoid_unknown{false};
  bool avoid_bicycle{false};
  bool avoid_motorcycle{false};
  bool avoid_pedestrian{false};
  double min_obstacle_vel{0.0};
  int successive_num_to_entry_dynamic_avoidance_condition{0};
  int successive_num_to_exit_dynamic_avoidance_condition{0};

  double min_obj_lat_offset_to_ego_path{0.0};
  double max_obj_lat_offset_to_ego_path{0.0};

  double min_time_to_start_cut_in{0.0};
  double min_lon_offset_ego_to_cut_in_object{0.0};
  double max_time_from_outside_ego_path_for_cut_out{0.0};
  double min_cut_out_object_lat_vel{0.0};
  double max_front_object_angle{0.0};
  double min_crossing_object_vel{0.0};
  double max_crossing_object_angle{0.0};

  // drivable area generation
  double lat_offset_from_obstacle{0.0};
  double max_lat_offset_to_avoid{0.0};

  double max_time_to_collision_overtaking_object{0.0};
  double start_duration_to_avoid_overtaking_object{0.0};
  double end_duration_to_avoid_overtaking_object{0.0};
  double duration_to_hold_avoidance_overtaking_object{0.0};

  double max_time_to_collision_oncoming_object{0.0};
  double start_duration_to_avoid_oncoming_object{0.0};
  double end_duration_to_avoid_oncoming_object{0.0};
};

class DynamicAvoidanceModule : public SceneModuleInterface
{
public:
  struct DynamicAvoidanceObject
  {
    DynamicAvoidanceObject(
      const PredictedObject & predicted_object, const double arg_vel, const double arg_lat_vel,
      const std::optional<rclcpp::Time> & arg_latest_time_inside_ego_path)
    : uuid(tier4_autoware_utils::toHexString(predicted_object.object_id)),
      pose(predicted_object.kinematics.initial_pose_with_covariance.pose),
      shape(predicted_object.shape),
      vel(arg_vel),
      lat_vel(arg_lat_vel),
      latest_time_inside_ego_path(arg_latest_time_inside_ego_path)
    {
      for (const auto & path : predicted_object.kinematics.predicted_paths) {
        predicted_paths.push_back(path);
      }
    }

    std::string uuid;
    geometry_msgs::msg::Pose pose;
    autoware_auto_perception_msgs::msg::Shape shape;
    double vel;
    double lat_vel;
    std::optional<rclcpp::Time> latest_time_inside_ego_path{std::nullopt};
    std::vector<autoware_auto_perception_msgs::msg::PredictedPath> predicted_paths{};

    MinMaxValue lon_offset_to_avoid;
    MinMaxValue lat_offset_to_avoid;
    bool is_collision_left;

    void update(
      const MinMaxValue & arg_lon_offset_to_avoid, const MinMaxValue & arg_lat_offset_to_avoid,
      const bool arg_is_collision_left)
    {
      lon_offset_to_avoid = arg_lon_offset_to_avoid;
      lat_offset_to_avoid = arg_lat_offset_to_avoid;
      is_collision_left = arg_is_collision_left;
    }
  };

  struct TargetObjectsManager
  {
    TargetObjectsManager(const int arg_max_count, const int arg_min_count)
    : max_count_(arg_max_count), min_count_(arg_min_count)
    {
    }
    int max_count_;
    int min_count_;

    void initialize() { current_uuids_.clear(); }
    void updateObject(const std::string & uuid, const DynamicAvoidanceObject & object)
    {
      // add/update object
      if (object_map_.count(uuid) != 0) {
        object_map_.at(uuid) = object;
      } else {
        object_map_.emplace(uuid, object);
      }

      // increase counter
      if (counter_map_.count(uuid) != 0) {
        counter_map_.at(uuid) = std::min(max_count_ + 1, std::max(1, counter_map_.at(uuid) + 1));
      } else {
        counter_map_.emplace(uuid, 1);
      }

      // memorize uuid
      current_uuids_.push_back(uuid);
    }

    void finalize()
    {
      // decrease counter for not updated uuids
      std::vector<std::string> not_updated_uuids;
      for (const auto & object : object_map_) {
        if (
          std::find(current_uuids_.begin(), current_uuids_.end(), object.first) ==
          current_uuids_.end()) {
          not_updated_uuids.push_back(object.first);
        }
      }
      for (const auto & uuid : not_updated_uuids) {
        if (counter_map_.count(uuid) != 0) {
          counter_map_.at(uuid) = std::max(min_count_ - 1, std::min(-1, counter_map_.at(uuid) - 1));
        } else {
          counter_map_.emplace(uuid, -1);
        }
      }

      // remove objects whose counter is lower than threshold
      std::vector<std::string> obsolete_uuids;
      for (const auto & counter : counter_map_) {
        if (counter.second < min_count_) {
          obsolete_uuids.push_back(counter.first);
        }
      }
      for (const auto & obsolete_uuid : obsolete_uuids) {
        counter_map_.erase(obsolete_uuid);
        object_map_.erase(obsolete_uuid);
      }
    }
    std::vector<DynamicAvoidanceObject> getValidObjects() const
    {
      std::vector<DynamicAvoidanceObject> objects;
      for (const auto & object : object_map_) {
        if (counter_map_.count(object.first) != 0) {
          if (max_count_ <= counter_map_.at(object.first)) {
            objects.push_back(object.second);
          }
        }
      }
      return objects;
    }
    std::optional<DynamicAvoidanceObject> getValidObject(const std::string & uuid) const
    {
      // add/update object
      if (counter_map_.count(uuid) == 0) {
        return std::nullopt;
      }
      if (counter_map_.at(uuid) < max_count_) {
        return std::nullopt;
      }
      if (object_map_.count(uuid) == 0) {
        return std::nullopt;
      }
      return object_map_.at(uuid);
    }

    std::vector<std::string> current_uuids_;
    // NOTE: positive is for meeting entrying condition, and negative is for exiting.
    std::unordered_map<std::string, int> counter_map_;
    std::unordered_map<std::string, DynamicAvoidanceObject> object_map_;
  };

  DynamicAvoidanceModule(
    const std::string & name, rclcpp::Node & node,
    std::shared_ptr<DynamicAvoidanceParameters> parameters,
    const std::unordered_map<std::string, std::shared_ptr<RTCInterface>> & rtc_interface_ptr_map);

  void updateModuleParams(const std::any & parameters) override
  {
    parameters_ = std::any_cast<std::shared_ptr<DynamicAvoidanceParameters>>(parameters);
  }

  bool isExecutionRequested() const override;
  bool isExecutionReady() const override;
  ModuleStatus updateState() override;
  BehaviorModuleOutput plan() override;
  CandidateOutput planCandidate() const override;
  BehaviorModuleOutput planWaitingApproval() override;
  void updateData() override;
  void acceptVisitor(
    [[maybe_unused]] const std::shared_ptr<SceneModuleVisitor> & visitor) const override
  {
  }

private:
  struct LatLonOffset
  {
    const size_t nearest_idx;
    const double max_lat_offset;
    const double min_lat_offset;
    const double max_lon_offset;
    const double min_lon_offset;
  };

  bool isLabelTargetObstacle(const uint8_t label) const;
  void updateTargetObjects();
  std::optional<std::vector<PathPointWithLaneId>> calcPathForObjectPolygon() const;
  bool willObjectCutIn(
    const std::vector<PathPointWithLaneId> & ego_path, const PredictedPath & predicted_path,
    const double obj_tangent_vel, const LatLonOffset & lat_lon_offset) const;
  bool willObjectCutOut(
    const double obj_tangent_vel, const double obj_normal_vel, const bool is_object_left,
    const std::optional<DynamicAvoidanceObject> & prev_object) const;
  bool isObjectFarFromPath(
    const PredictedObject & predicted_object, const double obj_dist_to_path) const;
  double calcTimeToCollision(
    const std::vector<PathPointWithLaneId> & ego_path, const geometry_msgs::msg::Pose & obj_pose,
    const double obj_tangent_vel) const;
  std::optional<std::pair<size_t, size_t>> calcCollisionSection(
    const std::vector<PathPointWithLaneId> & ego_path, const PredictedPath & obj_path) const;
  LatLonOffset getLateralLongitudinalOffset(
    const std::vector<PathPointWithLaneId> & ego_path, const geometry_msgs::msg::Pose & obj_pose,
    const autoware_auto_perception_msgs::msg::Shape & obj_shape) const;
  MinMaxValue calcMinMaxLongitudinalOffsetToAvoid(
    const std::vector<PathPointWithLaneId> & path_points_for_object_polygon,
    const geometry_msgs::msg::Pose & obj_pose, const Polygon2d & obj_points, const double obj_vel,
    const double time_to_collision) const;
  MinMaxValue calcMinMaxLateralOffsetToAvoid(
    const std::vector<PathPointWithLaneId> & path_points_for_object_polygon,
    const Polygon2d & obj_points, const bool is_collision_left,
    const std::optional<DynamicAvoidanceObject> & prev_object) const;

  std::pair<lanelet::ConstLanelets, lanelet::ConstLanelets> getAdjacentLanes(
    const double forward_distance, const double backward_distance) const;
  std::optional<tier4_autoware_utils::Polygon2d> calcDynamicObstaclePolygon(
    const DynamicAvoidanceObject & object) const;

  std::vector<DynamicAvoidanceModule::DynamicAvoidanceObject> target_objects_;
  // std::vector<DynamicAvoidanceModule::DynamicAvoidanceObject> prev_target_objects_;
  std::shared_ptr<DynamicAvoidanceParameters> parameters_;

  TargetObjectsManager target_objects_manager_;
};
}  // namespace behavior_path_planner

#endif  // BEHAVIOR_PATH_PLANNER__SCENE_MODULE__DYNAMIC_AVOIDANCE__DYNAMIC_AVOIDANCE_MODULE_HPP_
