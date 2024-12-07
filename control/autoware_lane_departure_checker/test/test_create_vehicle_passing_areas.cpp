// Copyright 2024 TIER IV, Inc.
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

#include "autoware/lane_departure_checker/utils.hpp"

#include <Eigen/Core>
#include <autoware/universe_utils/geometry/boost_geometry.hpp>

#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/algorithms/covered_by.hpp>
#include <boost/geometry/algorithms/is_valid.hpp>
#include <boost/geometry/algorithms/within.hpp>

#include <gtest/gtest.h>

#include <vector>

using autoware::lane_departure_checker::utils::createVehiclePassingAreas;
using autoware::universe_utils::LinearRing2d;
using autoware::universe_utils::Point2d;

class CreateVehiclePassingAreasTest : public ::testing::Test
{
protected:
  LinearRing2d createSquare(double x, double y, double size)
  {
    LinearRing2d square;
    square.reserve(5);
    square.push_back(Point2d{x, y});                // bottom-left
    square.push_back(Point2d{x, y + size});         // top-left
    square.push_back(Point2d{x + size, y + size});  // top-right
    square.push_back(Point2d{x + size, y});         // bottom-right
    square.push_back(Point2d{x, y});                // close the square
    boost::geometry::correct(square);
    return square;
  }

  bool isPointInsideHull(const Point2d & point, const LinearRing2d & hull)
  {
    return boost::geometry::within(point, hull) || boost::geometry::covered_by(point, hull);
  }

  /**
   * @brief Validates that all points in the given footprints are contained within the hull
   *
   * For each footprint in the input vector, checks whether all its points (except the last one
   * which is identical to the first point in a LinearRing2d) are contained within the given hull.
   * This validation ensures the correctness of the convex hull generation for vehicle passing
   * areas.
   *
   * @param footprints Vector of LinearRing2d representing vehicle footprints to validate
   *                   Must not be empty and must not contain empty LinearRing2d
   * @param hull LinearRing2d representing the convex hull that should contain all footprints
   * @note The last point of each footprint is not checked as LinearRing2d is a closed ring
   *       where the last point is identical to the first point
   * @pre footprints must not be empty
   * @pre each LinearRing2d in footprints must not be empty, as it represents a closed ring
   */
  void validateFootprintsInHull(
    const std::vector<LinearRing2d> & footprints, const LinearRing2d & hull)
  {
    for (const auto & footprint : footprints) {
      // An empty footprint would be invalid and should fail the test
      EXPECT_FALSE(footprint.empty()) << "Footprint must not be empty";
      if (footprint.empty()) {
        continue;
      }

      // Iterate up to size()-1 since LinearRing2d is a closed ring where
      // the last point is the same as the first point, so we don't need
      // to check it again
      for (size_t i = 0; i < footprint.size() - 1; ++i) {
        const auto & point = footprint[i];
        EXPECT_TRUE(isPointInsideHull(point, hull))
          << "Point (" << point.x() << ", " << point.y() << ") is not inside the hull";
      }
    }
  }

  void SetUp() override
  {
    /*
    Square placement:
    Y-axis
    ^
    |
    1   +---+   +---+   +---+
    |   |s1 |   |s2 |   |s3 |
    0   +---+   +---+   +---+
    |
    +---+---+---+---+---+---+--> X-axis
        0   1   1   2   3   4

    s1: square1_ from (0,0) to (1,1)
    s2: square2_ from (1,0) to (2,1) - adjacent to s1
    s3: square3_ from (3,0) to (4,1) - 1 unit apart from s2
    */
    square1_ = createSquare(0.0, 0.0, 1.0);  // 1x1 square at origin
    square2_ = createSquare(1.0, 0.0, 1.0);  // square adjacent to square1_
    square3_ = createSquare(3.0, 0.0, 1.0);  // square one unit apart from square2_
  }

  LinearRing2d square1_;  // Reference square (0,0)
  LinearRing2d square2_;  // Adjacent square (1,0)
  LinearRing2d square3_;  // Distant square (3,0)
};

TEST_F(CreateVehiclePassingAreasTest, ReturnsEmptyAreaForEmptyInput)
{
  const std::vector<LinearRing2d> empty_footprints;
  const auto areas = createVehiclePassingAreas(empty_footprints);
  EXPECT_TRUE(areas.empty());
}

TEST_F(CreateVehiclePassingAreasTest, ReturnsSameAreaForSingleFootprint)
{
  const std::vector<LinearRing2d> single_footprint = {square1_};
  const auto areas = createVehiclePassingAreas(single_footprint);

  ASSERT_EQ(areas.size(), 1);

  auto result = areas.front();
  boost::geometry::correct(result);
  EXPECT_EQ(result, square1_);
}

TEST_F(CreateVehiclePassingAreasTest, CreatesValidHullForAdjacentFootprints)
{
  const std::vector<LinearRing2d> footprints = {square1_, square2_};
  auto areas = createVehiclePassingAreas(footprints);

  ASSERT_EQ(areas.size(), 1);
  auto & hull = areas.front();
  boost::geometry::correct(hull);

  // Basic validation of the convex hull
  EXPECT_GE(hull.size(), 5);  // At least a quadrilateral plus closing point

  // Verify all points from original footprints are inside the hull
  validateFootprintsInHull(footprints, hull);
}

TEST_F(CreateVehiclePassingAreasTest, HandlesNonAdjacentFootprints)
{
  const std::vector<LinearRing2d> footprints = {
    square1_, square3_};  // Using square3_ which is not adjacent to square1_
  auto areas = createVehiclePassingAreas(footprints);

  ASSERT_EQ(areas.size(), 1);
  auto & hull = areas.front();
  boost::geometry::correct(hull);

  // Verify all points are inside the hull even for non-adjacent squares
  validateFootprintsInHull(footprints, hull);
}
