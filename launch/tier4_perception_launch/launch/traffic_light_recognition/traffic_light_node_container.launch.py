# Copyright 2020 Tier IV, Inc. All rights reserved.
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

from itertools import chain
import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.actions import GroupAction
from launch.actions import OpaqueFunction
from launch.actions import SetLaunchConfiguration
from launch.conditions import IfCondition
from launch.conditions import UnlessCondition
from launch.substitutions import LaunchConfiguration
from launch.substitutions import PathJoinSubstitution
from launch.substitutions import PythonExpression
from launch_ros.actions import ComposableNodeContainer
from launch_ros.actions import LoadComposableNodes
from launch_ros.actions import PushRosNamespace
from launch_ros.descriptions import ComposableNode
from launch_ros.parameter_descriptions import ParameterFile
import yaml


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
        create_traffic_light_node_container(namespace, context, *args, **kwargs)
        for namespace in all_camera_namespaces
    ]
    traffic_light_recognition_containers = list(chain(*traffic_light_recognition_containers))

    return traffic_light_recognition_containers


def create_traffic_light_node_container(namespace, context, *args, **kwargs):
    camera_arguments = {
        "input/camera_info": f"/sensing/camera/{namespace}/camera_info",
        "input/image": f"/sensing/camera/{namespace}/image_raw",
        "output/rois": f"/perception/traffic_light_recognition/{namespace}/detection/rois",
        "output/traffic_signals": f"/perception/traffic_light_recognition/{namespace}/classification/traffic_signals",
        "output/car/traffic_signals": f"/perception/traffic_light_recognition/{namespace}/classification/car/traffic_signals",
        "output/pedestrian/traffic_signals": f"/perception/traffic_light_recognition/{namespace}/classification/pedestrian/traffic_signals",
    }

    def create_parameter_dict(*args):
        result = {}
        for x in args:
            result[x] = LaunchConfiguration(x)
        return result

    fine_detector_model_param = ParameterFile(
        param_file=LaunchConfiguration("fine_detector_param_path").perform(context),
        allow_substs=True,
    )
    car_traffic_light_classifier_model_param = ParameterFile(
        param_file=LaunchConfiguration("car_classifier_param_path").perform(context),
        allow_substs=True,
    )
    pedestrian_traffic_light_classifier_model_param = ParameterFile(
        param_file=LaunchConfiguration("pedestrian_classifier_param_path").perform(context),
        allow_substs=True,
    )

    container = ComposableNodeContainer(
        name="traffic_light_node_container",
        namespace="",
        package="rclcpp_components",
        executable=LaunchConfiguration("container_executable"),
        composable_node_descriptions=[
            ComposableNode(
                package="autoware_traffic_light_classifier",
                plugin="autoware::traffic_light::TrafficLightClassifierNodelet",
                name="car_traffic_light_classifier",
                namespace="classification",
                parameters=[car_traffic_light_classifier_model_param],
                remappings=[
                    ("~/input/image", camera_arguments["input/image"]),
                    ("~/input/rois", camera_arguments["output/rois"]),
                    ("~/output/traffic_signals", "classified/car/traffic_signals"),
                ],
                extra_arguments=[
                    {"use_intra_process_comms": LaunchConfiguration("use_intra_process")}
                ],
            ),
            ComposableNode(
                package="autoware_traffic_light_classifier",
                plugin="autoware::traffic_light::TrafficLightClassifierNodelet",
                name="pedestrian_traffic_light_classifier",
                namespace="classification",
                parameters=[pedestrian_traffic_light_classifier_model_param],
                remappings=[
                    ("~/input/image", camera_arguments["input/image"]),
                    ("~/input/rois", camera_arguments["output/rois"]),
                    ("~/output/traffic_signals", "classified/pedestrian/traffic_signals"),
                ],
                extra_arguments=[
                    {"use_intra_process_comms": LaunchConfiguration("use_intra_process")}
                ],
            ),
            ComposableNode(
                package="autoware_traffic_light_visualization",
                plugin="autoware::traffic_light::TrafficLightRoiVisualizerNode",
                name="traffic_light_roi_visualizer",
                parameters=[create_parameter_dict("use_ml_detector", "use_image_transport")],
                remappings=[
                    ("~/input/image", camera_arguments["input/image"]),
                    ("~/input/rois", camera_arguments["output/rois"]),
                    ("~/input/rough/rois", "detection/rough/rois"),
                    (
                        "~/input/traffic_signals",
                        camera_arguments["output/traffic_signals"],
                    ),
                    ("~/output/image", "debug/rois"),
                    ("~/output/image/compressed", "debug/rois/compressed"),
                    ("~/output/image/compressedDepth", "debug/rois/compressedDepth"),
                    ("~/output/image/theora", "debug/rois/theora"),
                ],
                extra_arguments=[
                    {"use_intra_process_comms": LaunchConfiguration("use_intra_process")}
                ],
            ),
        ],
        output="both",
    )

    decompressor_loader = LoadComposableNodes(
        composable_node_descriptions=[
            ComposableNode(
                package="autoware_image_transport_decompressor",
                plugin="autoware::image_preprocessor::ImageTransportDecompressor",
                name="traffic_light_image_decompressor",
                namespace=namespace,
                parameters=[{"encoding": "rgb8"}],
                remappings=[
                    (
                        "~/input/compressed_image",
                        [camera_arguments["input/image"], "/compressed"],
                    ),
                    ("~/output/raw_image", camera_arguments["input/image"]),
                ],
                extra_arguments=[
                    {"use_intra_process_comms": LaunchConfiguration("use_intra_process")}
                ],
            ),
        ],
        target_container=container,
        condition=IfCondition(LaunchConfiguration("enable_image_decompressor")),
    )

    fine_detector_loader = LoadComposableNodes(
        composable_node_descriptions=[
            ComposableNode(
                package="autoware_traffic_light_fine_detector",
                plugin="autoware::traffic_light::TrafficLightFineDetectorNode",
                name="traffic_light_fine_detector",
                namespace=f"{namespace}/detection",
                parameters=[fine_detector_model_param],
                remappings=[
                    ("~/input/image", camera_arguments["input/image"]),
                    ("~/input/rois", "rough/rois"),
                    ("~/expect/rois", "expect/rois"),
                    ("~/output/rois", camera_arguments["output/rois"]),
                ],
                extra_arguments=[
                    {"use_intra_process_comms": LaunchConfiguration("use_intra_process")}
                ],
            ),
        ],
        target_container=container,
        condition=IfCondition(
            PythonExpression(
                [
                    "'",
                    LaunchConfiguration("ml_detection_model_type"),
                    "' == 'fine_detection_model' ",
                ]
            )
        ),
    )
    # cspell: ignore semseg
    whole_img_detector_loader = LoadComposableNodes(
        composable_node_descriptions=[
            ComposableNode(
                package="autoware_tensorrt_yolox",
                plugin="autoware::tensorrt_yolox::TrtYoloXNode",
                name="traffic_light_detector",
                namespace=f"{namespace}/detection",
                parameters=[
                    LaunchConfiguration("whole_image_detector_param_path"),
                    {
                        "model_path": PathJoinSubstitution(
                            [
                                LaunchConfiguration("whole_image_detector_model_path"),
                                "tensorrt_yolox",
                                LaunchConfiguration("whole_image_detector_model_name"),
                            ]
                        ),
                        "label_path": PathJoinSubstitution(
                            [
                                LaunchConfiguration("whole_image_detector_model_path"),
                                "tensorrt_yolox/car_ped_tl_detector_labels.txt",
                            ]
                        ),
                        "color_map_path": PathJoinSubstitution(
                            [
                                LaunchConfiguration("whole_image_detector_model_path"),
                                "tensorrt_yolox/semseg_color_map.csv",
                            ]
                        ),
                        "build_only": False,
                        "clip_value": 0.0,
                    },
                ],
                remappings=[
                    ("~/in/image", camera_arguments["input/image"]),
                    ("~/out/objects", "ml_detected/rois"),
                ],
                extra_arguments=[
                    {"use_intra_process_comms": LaunchConfiguration("use_intra_process")}
                ],
            ),
            ComposableNode(
                package="autoware_traffic_light_selector",
                plugin="autoware::traffic_light::TrafficLightSelectorNode",
                name="traffic_light_selector",
                namespace=f"{namespace}/detection",
                parameters=[
                    {
                        "max_iou_threshold": -0.5,
                    }
                ],
                remappings=[
                    ("input/detected_rois", "ml_detected/rois"),
                    ("input/rough_rois", "rough/rois"),
                    ("input/expect_rois", "expect/rois"),
                    ("input/camera_info", camera_arguments["input/camera_info"]),
                    ("output/traffic_light_rois", camera_arguments["output/rois"]),
                ],
            ),
        ],
        target_container=container,
        condition=IfCondition(
            PythonExpression(
                [
                    "'",
                    LaunchConfiguration("ml_detection_model_type"),
                    "' == 'whole_image_detection_model' ",
                ]
            )
        ),
    )

    return [
        GroupAction([PushRosNamespace(namespace), container]),
        decompressor_loader,
        fine_detector_loader,
        whole_img_detector_loader,
    ]


def generate_launch_description():
    launch_arguments = []

    def add_launch_arg(name: str, default_value=None, description=None):
        # a default_value of None is equivalent to not passing that kwarg at all
        launch_arguments.append(
            DeclareLaunchArgument(name, default_value=default_value, description=description)
        )

    tensorrt_yolox_share_dir = get_package_share_directory("autoware_tensorrt_yolox")
    fine_detector_share_dir = get_package_share_directory("autoware_traffic_light_fine_detector")
    classifier_share_dir = get_package_share_directory("autoware_traffic_light_classifier")
    add_launch_arg("all_camera_namespaces", "[camera6, camera7]")
    add_launch_arg("enable_image_decompressor", "True")
    add_launch_arg("use_ml_detector", "True")
    add_launch_arg("ml_detection_model_type", "fine_detection_model")
    add_launch_arg("use_image_transport", "True")

    # traffic_light_fine_detector
    add_launch_arg(
        "fine_detector_param_path",
        os.path.join(fine_detector_share_dir, "config", "traffic_light_fine_detector.param.yaml"),
    )
    # traffic_light_detector
    add_launch_arg("whole_image_detector_model_path", "$HOME/autoware_data")
    add_launch_arg("whole_image_detector_model_name", "tlr_car_ped_yolox_s_960_960_batch_1")
    add_launch_arg(
        "whole_image_detector_param_path",
        os.path.join(tensorrt_yolox_share_dir, "config", "yolox_traffic_light_detector.param.yaml"),
    )

    # traffic_light_classifier
    add_launch_arg(
        "car_classifier_param_path",
        os.path.join(
            classifier_share_dir, "config", "car_traffic_light_classifier_efficientNet.param.yaml"
        ),
    )
    add_launch_arg(
        "pedestrian_classifier_param_path",
        os.path.join(
            classifier_share_dir,
            "config",
            "pedestrian_traffic_light_classifier_efficientNet.param.yaml",
        ),
    )

    add_launch_arg("use_intra_process", "False")
    add_launch_arg("use_multithread", "False")

    set_container_executable = SetLaunchConfiguration(
        "container_executable",
        "component_container",
        condition=UnlessCondition(LaunchConfiguration("use_multithread")),
    )

    set_container_mt_executable = SetLaunchConfiguration(
        "container_executable",
        "component_container_mt",
        condition=IfCondition(LaunchConfiguration("use_multithread")),
    )

    return LaunchDescription(
        [
            *launch_arguments,
            set_container_executable,
            set_container_mt_executable,
            OpaqueFunction(function=launch_setup),
        ]
    )
