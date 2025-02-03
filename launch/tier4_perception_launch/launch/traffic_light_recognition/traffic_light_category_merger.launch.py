# Copyright 2025 TIER IV, Inc.
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


import launch
from launch.actions import DeclareLaunchArgument
from launch.actions import GroupAction
from launch.actions import IncludeLaunchDescription
from launch.actions import OpaqueFunction
from launch.substitutions import LaunchConfiguration
from launch.substitutions import PathJoinSubstitution
from launch_ros.actions import PushRosNamespace
from launch_ros.substitutions import FindPackageShare
import yaml


def create_traffic_light_category_merger(namespace):
    package = FindPackageShare("autoware_traffic_light_category_merger")
    include = PathJoinSubstitution([package, "launch/traffic_light_category_merger.launch.xml"])

    arguments = {
        "input/car_signals": "classified/car/traffic_signals",
        "input/pedestrian_signals": "classified/pedestrian/traffic_signals",
        "output/traffic_light_signals": f"/perception/traffic_light_recognition/{namespace}/classification/traffic_signals",
    }.items()

    group = GroupAction(
        [
            PushRosNamespace(namespace),
            PushRosNamespace("classification"),
            IncludeLaunchDescription(include, launch_arguments=arguments),
        ]
    )

    return group


def launch_setup(context, *args, **kwargs):
    # Load all camera namespaces
    all_camera_namespaces = LaunchConfiguration("all_camera_namespaces").perform(context)

    # Convert string to list
    all_camera_namespaces = yaml.load(all_camera_namespaces, Loader=yaml.FullLoader)
    if not isinstance(all_camera_namespaces, list):
        raise ValueError(
            "all_camera_namespaces is not a list. You should declare it like `['camera6', 'camera7']`."
        )
    if not all((isinstance(v, str) for v in all_camera_namespaces)):
        raise ValueError(
            "all_camera_namespaces is not a list of strings. You should declare it like `['camera6', 'camera7']`."
        )

    # Create containers for all cameras
    traffic_light_recognition_containers = [
        create_traffic_light_category_merger(namespace) for namespace in all_camera_namespaces
    ]
    return traffic_light_recognition_containers


def generate_launch_description():
    launch_arguments = []

    def add_launch_arg(name: str, default_value=None, description=None):
        # a default_value of None is equivalent to not passing that kwarg at all
        launch_arguments.append(
            DeclareLaunchArgument(name, default_value=default_value, description=description)
        )

    add_launch_arg("all_camera_namespaces", "[camera6, camera7]")

    return launch.LaunchDescription(
        [
            *launch_arguments,
            OpaqueFunction(function=launch_setup),
        ]
    )
