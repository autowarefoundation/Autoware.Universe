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

#include <object_detection/predicted_objects_display.hpp>
#include <rviz_common/properties/int_property.hpp>

#include <semaphore.h>

#include <memory>
#include <set>

namespace autoware
{
namespace rviz_plugins
{
namespace object_detection
{
PredictedObjectsDisplay::PredictedObjectsDisplay() : ObjectPolygonDisplayBase("tracks")
{
  num_threads_property = new rviz_common::properties::IntProperty(
    "Num Threads", 4, "Num of Threads to use", this);  // by default, use 4 threads
  max_num_threads = 8;            // hard code the number of threads to be created
  num_threads = max_num_threads;  // this will be soon modified in the workerThread

  for (int ii = 0; ii < max_num_threads; ++ii) {
    threads.emplace_back(std::thread(&PredictedObjectsDisplay::workerThread, this));
  }

  // ParallelizedThread Initialization
  std::vector<std::thread> thread_vector;
  ending_semaphores = reinterpret_cast<sem_t *>(malloc(max_num_threads * sizeof(sem_t)));

  for (int rank = 0; rank < max_num_threads; rank++) {
    sem_init(&ending_semaphores[rank], 0, 0);
  }
}

void PredictedObjectsDisplay::workerThread()
{  // A standard working thread that waiting for jobs
  while (true) {
    std::function<void()> job;
    {
      std::unique_lock<std::mutex> lock(queue_mutex);
      mutex_condition.wait(lock, [this] { return !jobs.empty() || should_terminate; });
      if (should_terminate) {
        return;
      }
      job = jobs.front();
      jobs.pop();
    }
    job();
  }
}

void PredictedObjectsDisplay::parallelizedCreateMarkerWorkerThread(int rank)
{
  // Determine the number of objects to be processed by this thread
  int length = tmp_msg->objects.size();
  int num_object_per_thread = static_cast<int>(ceil(length / static_cast<float>(num_threads)));

  int first_object_index = rank * num_object_per_thread;
  int last_object_index = first_object_index + num_object_per_thread;
  if (last_object_index > length) {
    last_object_index = length;
  }
  std::vector<visualization_msgs::msg::Marker::SharedPtr> thread_makers;
  for (int i = first_object_index; i < last_object_index; i++) {
    thread_makers = tackle_one_object(tmp_msg, i, thread_makers);
  }
  push_tmp_markers(thread_makers);

  sem_post(&ending_semaphores[rank]);  // signal the end of the job
}

void PredictedObjectsDisplay::messageProcessorThreadJob()
{
  // Receiving
  std::unique_lock<std::mutex> lock(mutex);
  tmp_msg = this->msg;
  this->msg.reset();
  lock.unlock();

  int N = tmp_msg->objects.size();
  update_id_map(tmp_msg);

  // max_num_threads : Hard-coded for now. Define the pool of all threads.
  // num_threads: dynamically change the number of threads based on the number of objects.
  num_threads = std::min(max_num_threads, N / 2);
  num_threads = std::min(num_threads_property->getInt(), num_threads);
  num_threads = std::max(1, num_threads);

  if (num_threads > 1) {
    tmp_markers.clear();
    for (int rank = 0; rank < num_threads; rank++) {
      std::function<void()> f =
        std::bind(&PredictedObjectsDisplay::parallelizedCreateMarkerWorkerThread, this, rank);
      queueJob(f);
    }
    for (int rank = 0; rank < num_threads; rank++) {
      sem_wait(&ending_semaphores[rank]);
    }
  } else {
    tmp_markers = createMarkers(tmp_msg);
  }

  lock.lock();
  markers = tmp_markers;

  consumed = true;
}

void PredictedObjectsDisplay::push_tmp_markers(
  std::vector<visualization_msgs::msg::Marker::SharedPtr> marker_ptrs)
{
  std::lock_guard<std::mutex> _local_lock_guard(tmp_marker_mutex);
  for (auto marker_ptr : marker_ptrs) tmp_markers.push_back(marker_ptr);
}

std::vector<visualization_msgs::msg::Marker::SharedPtr> PredictedObjectsDisplay::tackle_one_object(
  PredictedObjects::ConstSharedPtr _msg, int index,
  std::vector<visualization_msgs::msg::Marker::SharedPtr> _markers)
{
  auto object = _msg->objects[index];
  // Get marker for shape
  auto shape_marker = get_shape_marker_ptr(
    object.shape, object.kinematics.initial_pose_with_covariance.pose.position,
    object.kinematics.initial_pose_with_covariance.pose.orientation, object.classification,
    get_line_width());
  if (shape_marker) {
    auto shape_marker_ptr = shape_marker.value();
    shape_marker_ptr->header = _msg->header;
    shape_marker_ptr->id = uuid_to_marker_id(object.object_id);
    _markers.push_back(shape_marker_ptr);
  }

  // Get marker for label
  auto label_marker = get_label_marker_ptr(
    object.kinematics.initial_pose_with_covariance.pose.position,
    object.kinematics.initial_pose_with_covariance.pose.orientation, object.classification);
  if (label_marker) {
    auto label_marker_ptr = label_marker.value();
    label_marker_ptr->header = _msg->header;
    label_marker_ptr->id = uuid_to_marker_id(object.object_id);
    _markers.push_back(label_marker_ptr);
  }

  // Get marker for id
  geometry_msgs::msg::Point uuid_vis_position;
  uuid_vis_position.x = object.kinematics.initial_pose_with_covariance.pose.position.x - 0.5;
  uuid_vis_position.y = object.kinematics.initial_pose_with_covariance.pose.position.y;
  uuid_vis_position.z = object.kinematics.initial_pose_with_covariance.pose.position.z - 0.5;

  auto id_marker = get_uuid_marker_ptr(object.object_id, uuid_vis_position, object.classification);
  if (id_marker) {
    auto id_marker_ptr = id_marker.value();
    id_marker_ptr->header = _msg->header;
    id_marker_ptr->id = uuid_to_marker_id(object.object_id);
    _markers.push_back(id_marker_ptr);
  }

  // Get marker for pose with covariance
  auto pose_with_covariance_marker =
    get_pose_with_covariance_marker_ptr(object.kinematics.initial_pose_with_covariance);
  if (pose_with_covariance_marker) {
    auto pose_with_covariance_marker_ptr = pose_with_covariance_marker.value();
    pose_with_covariance_marker_ptr->header = _msg->header;
    pose_with_covariance_marker_ptr->id = uuid_to_marker_id(object.object_id);
    _markers.push_back(pose_with_covariance_marker_ptr);
  }
  // Get marker for velocity text
  geometry_msgs::msg::Point vel_vis_position;
  vel_vis_position.x = uuid_vis_position.x - 0.5;
  vel_vis_position.y = uuid_vis_position.y;
  vel_vis_position.z = uuid_vis_position.z - 0.5;
  auto velocity_text_marker = get_velocity_text_marker_ptr(
    object.kinematics.initial_twist_with_covariance.twist, vel_vis_position, object.classification);
  if (velocity_text_marker) {
    auto velocity_text_marker_ptr = velocity_text_marker.value();
    velocity_text_marker_ptr->header = _msg->header;
    velocity_text_marker_ptr->id = uuid_to_marker_id(object.object_id);
    _markers.push_back(velocity_text_marker_ptr);
  }

  // Get marker for acceleration text
  geometry_msgs::msg::Point acc_vis_position;
  acc_vis_position.x = uuid_vis_position.x - 1.0;
  acc_vis_position.y = uuid_vis_position.y;
  acc_vis_position.z = uuid_vis_position.z - 1.0;
  auto acceleration_text_marker = get_acceleration_text_marker_ptr(
    object.kinematics.initial_acceleration_with_covariance.accel, acc_vis_position,
    object.classification);
  if (acceleration_text_marker) {
    auto acceleration_text_marker_ptr = acceleration_text_marker.value();
    acceleration_text_marker_ptr->header = _msg->header;
    acceleration_text_marker_ptr->id = uuid_to_marker_id(object.object_id);
    _markers.push_back(acceleration_text_marker_ptr);
  }

  // Get marker for twist
  auto twist_marker = get_twist_marker_ptr(
    object.kinematics.initial_pose_with_covariance,
    object.kinematics.initial_twist_with_covariance);
  if (twist_marker) {
    auto twist_marker_ptr = twist_marker.value();
    twist_marker_ptr->header = _msg->header;
    twist_marker_ptr->id = uuid_to_marker_id(object.object_id);
    _markers.push_back(twist_marker_ptr);
  }

  // Add marker for each candidate path
  int32_t path_count = 0;
  for (const auto & predicted_path : object.kinematics.predicted_paths) {
    // Get marker for predicted path
    auto predicted_path_marker =
      get_predicted_path_marker_ptr(object.object_id, object.shape, predicted_path);
    if (predicted_path_marker) {
      auto predicted_path_marker_ptr = predicted_path_marker.value();
      predicted_path_marker_ptr->header = _msg->header;
      predicted_path_marker_ptr->id =
        uuid_to_marker_id(object.object_id) + path_count * PATH_ID_CONSTANT;
      path_count++;
      _markers.push_back(predicted_path_marker_ptr);
    }
  }
  // Add confidence text marker for each candidate path
  path_count = 0;
  for (const auto & predicted_path : object.kinematics.predicted_paths) {
    if (predicted_path.path.empty()) {
      continue;
    }
    auto path_confidence_marker = get_path_confidence_marker_ptr(object.object_id, predicted_path);
    if (path_confidence_marker) {
      auto path_confidence_marker_ptr = path_confidence_marker.value();
      path_confidence_marker_ptr->header = _msg->header;
      path_confidence_marker_ptr->id =
        uuid_to_marker_id(object.object_id) + path_count * PATH_ID_CONSTANT;
      path_count++;
      _markers.push_back(path_confidence_marker_ptr);
    }
  }
  return _markers;
}

std::vector<visualization_msgs::msg::Marker::SharedPtr> PredictedObjectsDisplay::createMarkers(
  PredictedObjects::ConstSharedPtr msg)
{
  std::vector<visualization_msgs::msg::Marker::SharedPtr> markers;

  int length = msg->objects.size();
  for (int i = 0; i < length; i++) {
    markers = tackle_one_object(msg, i, markers);
  }
  return markers;
}

void PredictedObjectsDisplay::processMessage(PredictedObjects::ConstSharedPtr msg)
{
  std::unique_lock<std::mutex> lock(mutex);

  this->msg = msg;
  std::function<void()> f = std::bind(&PredictedObjectsDisplay::messageProcessorThreadJob, this);
  queueJob(f);
}

void PredictedObjectsDisplay::update(float wall_dt, float ros_dt)
{
  std::unique_lock<std::mutex> lock(mutex);

  if (!markers.empty()) {
    std::set new_marker_ids = std::set<rviz_default_plugins::displays::MarkerID>();
    for (const auto & marker : markers) {
      rviz_default_plugins::displays::MarkerID marker_id =
        rviz_default_plugins::displays::MarkerID(marker->ns, marker->id);
      add_marker(marker);
      new_marker_ids.insert(marker_id);
    }
    for (auto itr = existing_marker_ids.begin(); itr != existing_marker_ids.end(); itr++) {
      if (new_marker_ids.find(*itr) == new_marker_ids.end()) {
        deleteMarker(*itr);
      }
    }
    existing_marker_ids = new_marker_ids;

    markers.clear();
  }

  lock.unlock();

  ObjectPolygonDisplayBase<autoware_auto_perception_msgs::msg::PredictedObjects>::update(
    wall_dt, ros_dt);
}

}  // namespace object_detection
}  // namespace rviz_plugins
}  // namespace autoware

// Export the plugin
#include <pluginlib/class_list_macros.hpp>  // NOLINT
PLUGINLIB_EXPORT_CLASS(
  autoware::rviz_plugins::object_detection::PredictedObjectsDisplay, rviz_common::Display)
