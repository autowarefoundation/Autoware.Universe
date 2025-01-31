import launch
import launch_ros.actions


def generate_launch_description():
    return launch.LaunchDescription(
        [
            launch.actions.DeclareLaunchArgument(
                "filter_vel",
                default_value="10.0",
                description="Outliers threshold removal for velocity data in neural_network_throttle model",
            ),
            launch.actions.DeclareLaunchArgument(
                "filter_cmd",
                default_value="10.0",
                description="Outliers threshold removal for throttle commands data in neural_network_throttle model",
            ),
            launch.actions.DeclareLaunchArgument(
                "filter_acc",
                default_value="10.0",
                description="Outliers threshold removal for acceleration data in neural_network_throttle model",
            ),
            launch.actions.DeclareLaunchArgument(
                "mean_filter_size",
                default_value="21",
                description="Window size of mean filter applied to data",
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="neural_network_steer1.py",
                name="neural_network_steer1",
                output="screen",
                parameters=[
                    {
                        "filter_vel": launch.substitutions.LaunchConfiguration(
                            "filter_vel"
                        ),
                        "filter_cmd": launch.substitutions.LaunchConfiguration(
                            "filter_cmd"
                        ),
                        "filter_acc": launch.substitutions.LaunchConfiguration(
                            "filter_acc"
                        ),
                        "mean_filter_size": launch.substitutions.LaunchConfiguration(
                            "mean_filter_size"
                        ),
                    }
                ],
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="neural_network_steer2.py",
                name="neural_network_steer2",
                output="screen",
                parameters=[
                    {
                        "filter_vel": launch.substitutions.LaunchConfiguration(
                            "filter_vel"
                        ),
                        "filter_cmd": launch.substitutions.LaunchConfiguration(
                            "filter_cmd"
                        ),
                        "filter_acc": launch.substitutions.LaunchConfiguration(
                            "filter_acc"
                        ),
                        "mean_filter_size": launch.substitutions.LaunchConfiguration(
                            "mean_filter_size"
                        ),
                    }
                ],
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="neural_network_steer3.py",
                name="neural_network_steer3",
                output="screen",
                parameters=[
                    {
                        "filter_vel": launch.substitutions.LaunchConfiguration(
                            "filter_vel"
                        ),
                        "filter_cmd": launch.substitutions.LaunchConfiguration(
                            "filter_cmd"
                        ),
                        "filter_acc": launch.substitutions.LaunchConfiguration(
                            "filter_acc"
                        ),
                        "mean_filter_size": launch.substitutions.LaunchConfiguration(
                            "mean_filter_size"
                        ),
                    }
                ],
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="neural_network_steer4.py",
                name="neural_network_steer4",
                output="screen",
                parameters=[
                    {
                        "filter_vel": launch.substitutions.LaunchConfiguration(
                            "filter_vel"
                        ),
                        "filter_cmd": launch.substitutions.LaunchConfiguration(
                            "filter_cmd"
                        ),
                        "filter_acc": launch.substitutions.LaunchConfiguration(
                            "filter_acc"
                        ),
                        "mean_filter_size": launch.substitutions.LaunchConfiguration(
                            "mean_filter_size"
                        ),
                    }
                ],
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="neural_network_steer5.py",
                name="neural_network_steer5",
                output="screen",
                parameters=[
                    {
                        "filter_vel": launch.substitutions.LaunchConfiguration(
                            "filter_vel"
                        ),
                        "filter_cmd": launch.substitutions.LaunchConfiguration(
                            "filter_cmd"
                        ),
                        "filter_acc": launch.substitutions.LaunchConfiguration(
                            "filter_acc"
                        ),
                        "mean_filter_size": launch.substitutions.LaunchConfiguration(
                            "mean_filter_size"
                        ),
                    }
                ],
            ),
        ]
    )
