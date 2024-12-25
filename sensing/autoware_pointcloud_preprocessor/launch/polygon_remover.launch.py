# Copyright 2022 The Autoware Contributors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os

from ament_index_python.packages import get_package_share_directory
import launch
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
from launch_ros.parameter_descriptions import ParameterFile


def generate_launch_description():
    ns = "pointcloud_preprocessor"
    pkg = "autoware_pointcloud_preprocessor"

    param_file = os.path.join(
        get_package_share_directory("autoware_vehicle_info_utils"), "config/polygon_remover.yaml"
    )

    my_component = ComposableNode(
        package=pkg,
        plugin="autoware::pointcloud_preprocessor::PolygonRemoverComponent",
        name="polygon_remover",
        parameters=[ParameterFile(param_file, allow_substs=True)],
    )

    # set container to run all required components in the same process
    container = ComposableNodeContainer(
        name="pointcloud_preprocessor_container",
        namespace=ns,
        package="rclcpp_components",
        executable="component_container",
        composable_node_descriptions=[my_component],
        output="screen",
    )

    return launch.LaunchDescription(
        [
            container,
        ]
    )
