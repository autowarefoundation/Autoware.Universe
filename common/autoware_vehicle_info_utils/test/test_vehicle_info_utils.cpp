// Copyright 2020-2024 Tier IV, Inc.
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

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <autoware_vehicle_info_utils/vehicle_info_utils.hpp>

#include <gtest/gtest.h>

class VehicleInfoUtilTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    rclcpp::init(0, nullptr);
    rclcpp::NodeOptions options;
    options.arguments(
      {"--ros-args", "--params-file",
       ament_index_cpp::get_package_share_directory("autoware_vehicle_info_utils") +
         "/config/vehicle_info.param.yaml"});
    node_ = std::make_shared<rclcpp::Node>("test_vehicle_info_utils", options);
  }

  void TearDown() override { rclcpp::shutdown(); }

  std::shared_ptr<rclcpp::Node> node_;
};

TEST_F(VehicleInfoUtilTest, check_vehicle_info_value)
{
  std::optional<autoware::vehicle_info_utils::VehicleInfoUtils> vehicle_info_util_opt{std::nullopt};
  ASSERT_NO_THROW(
    { vehicle_info_util_opt.emplace(autoware::vehicle_info_utils::VehicleInfoUtils(*node_)); });
  const auto & vehicle_info_util = vehicle_info_util_opt.value();
  const auto vehicle_info = vehicle_info_util.getVehicleInfo();

  EXPECT_FLOAT_EQ(0.39, vehicle_info.wheel_radius_m);
  EXPECT_FLOAT_EQ(0.42, vehicle_info.wheel_width_m);
  EXPECT_FLOAT_EQ(2.74, vehicle_info.wheel_base_m);
  EXPECT_FLOAT_EQ(1.63, vehicle_info.wheel_tread_m);
  EXPECT_FLOAT_EQ(1.0, vehicle_info.front_overhang_m);
}
