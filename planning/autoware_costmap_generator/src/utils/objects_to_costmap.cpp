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

/*
 *  Copyright (c) 2018, Nagoya University
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither the name of Autoware nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ********************/

#include "autoware/costmap_generator/utils/objects_to_costmap.hpp"

#include <autoware/grid_map_utils/polygon_iterator.hpp>
#include <grid_map_core/TypeDefs.hpp>

#include <Eigen/src/Core/util/Constants.h>
#include <tf2/utils.h>

#include <cmath>
#include <string>

namespace autoware::costmap_generator
{
// Constructor
ObjectsToCostmap::ObjectsToCostmap()
: NUMBER_OF_POINTS(4),
  NUMBER_OF_DIMENSIONS(2),
  OBJECTS_COSTMAP_LAYER_("objects_costmap"),
  BLURRED_OBJECTS_COSTMAP_LAYER_("blurred_objects_costmap")
{
}

Eigen::MatrixXd ObjectsToCostmap::makeRectanglePoints(
  const autoware_perception_msgs::msg::PredictedObject & in_object,
  const double expand_rectangle_size)
{
  double length = in_object.shape.dimensions.x + expand_rectangle_size;
  double width = in_object.shape.dimensions.y + expand_rectangle_size;
  Eigen::MatrixXd origin_points(NUMBER_OF_DIMENSIONS, NUMBER_OF_POINTS);
  origin_points << length / 2, length / 2, -length / 2, -length / 2, width / 2, -width / 2,
    -width / 2, width / 2;

  double yaw = tf2::getYaw(in_object.kinematics.initial_pose_with_covariance.pose.orientation);
  Eigen::MatrixXd rotation_matrix(NUMBER_OF_DIMENSIONS, NUMBER_OF_DIMENSIONS);
  rotation_matrix << std::cos(yaw), -std::sin(yaw), std::sin(yaw), std::cos(yaw);
  Eigen::MatrixXd rotated_points = rotation_matrix * origin_points;

  double dx = in_object.kinematics.initial_pose_with_covariance.pose.position.x;
  double dy = in_object.kinematics.initial_pose_with_covariance.pose.position.y;
  Eigen::MatrixXd transformed_points(NUMBER_OF_DIMENSIONS, NUMBER_OF_POINTS);
  Eigen::MatrixXd ones = Eigen::MatrixXd::Ones(1, NUMBER_OF_POINTS);
  transformed_points.row(0) = rotated_points.row(0) + dx * ones;
  transformed_points.row(1) = rotated_points.row(1) + dy * ones;

  return transformed_points;
}

grid_map::Polygon ObjectsToCostmap::makePolygonFromObjectBox(
  const std_msgs::msg::Header & header,
  const autoware_perception_msgs::msg::PredictedObject & in_object,
  const double expand_rectangle_size)
{
  grid_map::Polygon polygon;
  polygon.setFrameId(header.frame_id);

  Eigen::MatrixXd rectangle_points = makeRectanglePoints(in_object, expand_rectangle_size);
  for (int col = 0; col < rectangle_points.cols(); col++) {
    polygon.addVertex(grid_map::Position(rectangle_points(0, col), rectangle_points(1, col)));
  }

  return polygon;
}

geometry_msgs::msg::Point ObjectsToCostmap::makeExpandedPoint(
  const geometry_msgs::msg::Point & in_centroid,
  const geometry_msgs::msg::Point32 & in_corner_point, const double expand_polygon_size)
{
  geometry_msgs::msg::Point expanded_point;

  if (expand_polygon_size == 0) {
    expanded_point.x = in_corner_point.x;
    expanded_point.y = in_corner_point.y;
    return expanded_point;
  }

  double theta = std::atan2(in_corner_point.y - in_centroid.y, in_corner_point.x - in_centroid.x);
  double delta_x = expand_polygon_size * std::cos(theta);
  double delta_y = expand_polygon_size * std::sin(theta);
  expanded_point.x = in_centroid.x + in_corner_point.x + delta_x;
  expanded_point.y = in_centroid.y + in_corner_point.y + delta_y;

  return expanded_point;
}

grid_map::Polygon ObjectsToCostmap::makePolygonFromObjectConvexHull(
  const std_msgs::msg::Header & header,
  const autoware_perception_msgs::msg::PredictedObject & in_object,
  const double expand_polygon_size)
{
  grid_map::Polygon polygon;
  polygon.setFrameId(header.frame_id);

  double initial_z = in_object.shape.footprint.points[0].z;
  for (size_t index = 0; index < in_object.shape.footprint.points.size(); index++) {
    if (in_object.shape.footprint.points[index].z == initial_z) {
      geometry_msgs::msg::Point centroid =
        in_object.kinematics.initial_pose_with_covariance.pose.position;
      geometry_msgs::msg::Point expanded_point =
        makeExpandedPoint(centroid, in_object.shape.footprint.points[index], expand_polygon_size);
      polygon.addVertex(grid_map::Position(expanded_point.x, expanded_point.y));
    }
  }

  return polygon;
}

void ObjectsToCostmap::setCostInPolygon(
  const grid_map::Polygon & polygon, const std::string & gridmap_layer_name, const float score,
  grid_map::GridMap & objects_costmap)
{
  for (grid_map_utils::PolygonIterator itr(objects_costmap, polygon); !itr.isPastEnd(); ++itr) {
    const float current_score = objects_costmap.at(gridmap_layer_name, *itr);
    if (score > current_score) {
      objects_costmap.at(gridmap_layer_name, *itr) = score;
    }
  }
}

void naive_mean_filter_on_grid_edges(
  // cppcheck-suppress constParameterReference
  const grid_map::Matrix & input, const int kernel_size, grid_map::Matrix & output)
{
  for (auto i = 0; i < input.rows(); ++i) {
    for (auto j = 0; j < input.cols(); ++j) {
      const auto is_inside =
        (i > kernel_size + 1 && j > kernel_size + 1) &&
        (i + kernel_size + 1 < input.rows() && j + kernel_size + 1 < input.cols());
      if (is_inside) {
        continue;
      }
      auto size = 0.0f;
      auto sum = 0.0f;
      for (auto i_offset = std::max(0, i - kernel_size);
           i_offset < i + kernel_size && i_offset < input.rows(); ++i_offset) {
        for (auto j_offset = std::max(0, j - kernel_size);
             j_offset < j + kernel_size && j_offset < input.cols(); ++j_offset) {
          ++size;
          sum += input(i_offset, j_offset);
        }
      }
      output(i, j) = sum / size;
    }
  }
}

grid_map::Matrix ObjectsToCostmap::makeCostmapFromObjects(
  const grid_map::GridMap & costmap, const double expand_polygon_size,
  const int64_t size_of_expansion_kernel,
  const autoware_perception_msgs::msg::PredictedObjects::ConstSharedPtr in_objects)
{
  grid_map::GridMap objects_costmap = costmap;
  objects_costmap.add(OBJECTS_COSTMAP_LAYER_, 0);

  for (const auto & object : in_objects->objects) {
    grid_map::Polygon polygon;
    if (object.shape.type == autoware_perception_msgs::msg::Shape::POLYGON) {
      polygon = makePolygonFromObjectConvexHull(in_objects->header, object, expand_polygon_size);
    } else if (object.shape.type == autoware_perception_msgs::msg::Shape::BOUNDING_BOX) {
      polygon = makePolygonFromObjectBox(in_objects->header, object, expand_polygon_size);
    } else if (object.shape.type == autoware_perception_msgs::msg::Shape::CYLINDER) {
      // TODO(Kenji Miyake): Add makePolygonFromObjectCylinder
      polygon = makePolygonFromObjectBox(in_objects->header, object, expand_polygon_size);
    }
    const auto highest_probability_label = *std::max_element(
      object.classification.begin(), object.classification.end(),
      [](const auto & c1, const auto & c2) { return c1.probability < c2.probability; });
    setCostInPolygon(
      polygon, OBJECTS_COSTMAP_LAYER_, highest_probability_label.probability, objects_costmap);
  }
  objects_costmap.add(BLURRED_OBJECTS_COSTMAP_LAYER_, 0.0);

  // Applying mean filter to expanded gridmap
  const auto & original_matrix = objects_costmap[OBJECTS_COSTMAP_LAYER_];
  Eigen::MatrixXf & filtered_matrix = objects_costmap[BLURRED_OBJECTS_COSTMAP_LAYER_];
  // edge of the grid: naive filter
  const auto kernel_size = static_cast<int>(static_cast<double>(size_of_expansion_kernel) / 2.0);
  naive_mean_filter_on_grid_edges(original_matrix, kernel_size, filtered_matrix);
  // inside the grid: optimized filter using Eigen block
  for (auto i = 0; i < filtered_matrix.rows() - size_of_expansion_kernel; ++i) {
    for (auto j = 0; j < filtered_matrix.cols() - size_of_expansion_kernel; ++j) {
      const auto mean =
        original_matrix.block(i, j, size_of_expansion_kernel, size_of_expansion_kernel).mean();
      filtered_matrix(i + kernel_size, j + kernel_size) = mean;
    }
  }

  objects_costmap[OBJECTS_COSTMAP_LAYER_] =
    objects_costmap[OBJECTS_COSTMAP_LAYER_].cwiseMax(filtered_matrix);

  return objects_costmap[OBJECTS_COSTMAP_LAYER_];
}
}  // namespace autoware::costmap_generator
