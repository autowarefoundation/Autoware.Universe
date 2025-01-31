import os

import launch
import launch_ros.actions
from launch.actions import OpaqueFunction
import launch.substitutions


def launch_data_monitor(context):
    # Open a new terminal and run data_monitor.py
    os.system(
        "gnome-terminal -- /bin/bash -c 'ros2 run learning_based_vehicle_calibration data_monitor.py; exec bash'"
    )


def generate_launch_description():
    return launch.LaunchDescription(
        [
            launch.actions.DeclareLaunchArgument(
                name="max_data",
                default_value="1500",
                description="Max number of data to collect for each scenario",
            ),
            launch.actions.DeclareLaunchArgument(
                name="num_of_queue",
                default_value="20",
                description="Window size of mean filter used to smooth data",
            ),
            launch.actions.DeclareLaunchArgument(
                name="speed_threshold",
                default_value="2.8",
                description="Threshold between low and high speeds in m/s",
            ),
            launch.actions.DeclareLaunchArgument(
                name="steering_threshold",
                default_value="0.03490658503988659",
                description="Steering radians value which corresponds to 2 degrees, over which we do not collect data",
            ),
            launch.actions.DeclareLaunchArgument(
                name="throttle_deadzone",
                default_value="5",
                description="Percentage of throttle deadzone",
            ),
            launch.actions.DeclareLaunchArgument(
                name="brake_deadzone",
                default_value="5",
                description="Percentage of break deadzone",
            ),
            launch.actions.DeclareLaunchArgument(
                name="max_velocity",
                default_value="11.1",
                description="Max speed in m/s over which we do not collect data",
            ),
            launch.actions.DeclareLaunchArgument(
                name="throttle_threshold1",
                default_value="30",
                description="Threshold throttle percentage 1",
            ),
            launch.actions.DeclareLaunchArgument(
                name="throttle_threshold2",
                default_value="55",
                description="Threshold throttle percentage 2",
            ),
            launch.actions.DeclareLaunchArgument(
                name="brake_threshold1",
                default_value="15",
                description="Threshold brake percentage 1",
            ),
            launch.actions.DeclareLaunchArgument(
                name="brake_threshold2",
                default_value="25",
                description="Threshold brake percentage 2",
            ),
            launch.actions.DeclareLaunchArgument(
                name="consistency_threshold",
                default_value="20",
                description="If 2 consecutive throttle or brake commands differ for more than 20, they are not consistent so we do not collect them",
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
                name="Recovery_Mode",
                default_value="False",
                description="If False, the node will create new csv tables from scratch. If True, it will recover the previous csv tables and will start to collect data from the previous indexes",
            ),
            OpaqueFunction(
                function=launch_data_monitor,
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="data_collection.py",
                name="data_collection",
                output="screen",
                parameters=[
                    {"max_data": launch.substitutions.LaunchConfiguration("max_data")},
                    {
                        "num_of_queue": launch.substitutions.LaunchConfiguration(
                            "num_of_queue"
                        )
                    },
                    {
                        "speed_threshold": launch.substitutions.LaunchConfiguration(
                            "speed_threshold"
                        )
                    },
                    {
                        "steering_threshold": launch.substitutions.LaunchConfiguration(
                            "steering_threshold"
                        )
                    },
                    {
                        "throttle_deadzone": launch.substitutions.LaunchConfiguration(
                            "throttle_deadzone"
                        )
                    },
                    {
                        "brake_deadzone": launch.substitutions.LaunchConfiguration(
                            "brake_deadzone"
                        )
                    },
                    {
                        "max_velocity": launch.substitutions.LaunchConfiguration(
                            "max_velocity"
                        )
                    },
                    {
                        "throttle_threshold1": launch.substitutions.LaunchConfiguration(
                            "throttle_threshold1"
                        )
                    },
                    {
                        "throttle_threshold2": launch.substitutions.LaunchConfiguration(
                            "throttle_threshold2"
                        )
                    },
                    {
                        "brake_threshold1": launch.substitutions.LaunchConfiguration(
                            "brake_threshold1"
                        )
                    },
                    {
                        "brake_threshold2": launch.substitutions.LaunchConfiguration(
                            "brake_threshold2"
                        )
                    },
                    {
                        "consistency_threshold": launch.substitutions.LaunchConfiguration(
                            "consistency_threshold"
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
