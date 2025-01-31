import os

import launch
import launch_ros.actions
from launch.actions import OpaqueFunction


def launch_data_monitor_steer(context):
    # Open a new terminal and run data_monitor.py
    os.system(
        "gnome-terminal -- /bin/bash -c 'ros2 run learning_based_vehicle_calibration data_monitor_steer.py; exec bash'"
    )


def generate_launch_description():
    return launch.LaunchDescription(
        [
            launch.actions.DeclareLaunchArgument(
                "max_data",
                default_value="10000",
                description="Max number of data to collect for each scenario",
            ),
            launch.actions.DeclareLaunchArgument(
                "max_velocity",
                default_value="1.95",
                description="Max speed in m/s over which we do not collect data",
            ),
            launch.actions.DeclareLaunchArgument(
                "throttle_threshold",
                default_value="12",
                description="Threshold between low and high throttle command",
            ),
            launch.actions.DeclareLaunchArgument(
                "steering_threshold_1",
                default_value="0.04",
                description="Radians value",
            ),
            launch.actions.DeclareLaunchArgument(
                "steering_threshold_2",
                default_value="0.10",
                description="Radians value",
            ),
            launch.actions.DeclareLaunchArgument(
                "steering_threshold_3",
                default_value="0.20",
                description="Radians value",
            ),
            launch.actions.DeclareLaunchArgument(
                "steering_threshold_4",
                default_value="0.30",
                description="Radians value",
            ),
            launch.actions.DeclareLaunchArgument(
                "steering_threshold_5",
                default_value="0.40",
                description="Radians value",
            ),
            # Add launch arguments for topic names
            launch.actions.DeclareLaunchArgument(
                name="pitch_topic",
                default_value="/sensing/gnss/chc/pitch",
                description="Topic for pitch data",
            ),
            launch.actions.DeclareLaunchArgument(
                name="actuation_status_topic",
                default_value="/vehicle/status/actuation_status",
                description="Topic for actuation status data (brake and acceleration)",
            ),
            launch.actions.DeclareLaunchArgument(
                name="steering_status_topic",
                default_value="/vehicle/status/steering_status",
                description="Topic for steering status data",
            ),
            launch.actions.DeclareLaunchArgument(
                name="velocity_status_topic",
                default_value="/vehicle/status/velocity_status",
                description="Topic for velocity status data",
            ),
            launch.actions.DeclareLaunchArgument(
                name="imu_topic",
                default_value="/sensing/gnss/chc/imu",
                description="Topic for IMU data",
            ),
            launch.actions.DeclareLaunchArgument(
                "Recovery_Mode",
                default_value="False",
                description="If False, the node will create new csv tables from scratch. If True, it will recover the previous csv tables and will start to collect data from the previous indexes",
            ),
            OpaqueFunction(
                function=launch_data_monitor_steer,
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="data_collection_steer.py",
                name="data_collection_steer",
                output="screen",
                parameters=[
                    {"max_data": launch.substitutions.LaunchConfiguration("max_data")},
                    {
                        "max_velocity": launch.substitutions.LaunchConfiguration(
                            "max_velocity"
                        )
                    },
                    {
                        "throttle_threshold": launch.substitutions.LaunchConfiguration(
                            "throttle_threshold"
                        )
                    },
                    {
                        "steering_threshold_1": launch.substitutions.LaunchConfiguration(
                            "steering_threshold_1"
                        )
                    },
                    {
                        "steering_threshold_2": launch.substitutions.LaunchConfiguration(
                            "steering_threshold_2"
                        )
                    },
                    {
                        "steering_threshold_3": launch.substitutions.LaunchConfiguration(
                            "steering_threshold_3"
                        )
                    },
                    {
                        "steering_threshold_4": launch.substitutions.LaunchConfiguration(
                            "steering_threshold_4"
                        )
                    },
                    {
                        "steering_threshold_5": launch.substitutions.LaunchConfiguration(
                            "steering_threshold_5"
                        )
                    },
                    {
                        "pitch_topic": launch.substitutions.LaunchConfiguration(
                            "pitch_topic"
                        )
                    },
                    {
                        "actuation_status_topic": launch.substitutions.LaunchConfiguration(
                            "actuation_status_topic"
                        )
                    },
                    {
                        "steering_status_topic": launch.substitutions.LaunchConfiguration(
                            "steering_status_topic"
                        )
                    },
                    {
                        "velocity_status_topic": launch.substitutions.LaunchConfiguration(
                            "velocity_status_topic"
                        )
                    },
                    {
                        "imu_topic": launch.substitutions.LaunchConfiguration(
                            "imu_topic"
                        )
                    },
                    {
                        "Recovery_Mode": launch.substitutions.LaunchConfiguration(
                            "Recovery_Mode"
                        )
                    },
                ],
            ),
        ]
    )
