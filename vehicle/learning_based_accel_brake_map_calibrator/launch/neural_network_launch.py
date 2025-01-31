import launch
import launch_ros.actions


def generate_launch_description():
    return launch.LaunchDescription(
        [
            launch.actions.DeclareLaunchArgument(
                "filter_vel_throttle",
                default_value="10.0",
                description="Outliers threshold removal for velocity data in neural_network_throttle model",
            ),
            launch.actions.DeclareLaunchArgument(
                "filter_cmd_throttle",
                default_value="10.0",
                description="Outliers threshold removal for throttle commands data in neural_network_throttle model",
            ),
            launch.actions.DeclareLaunchArgument(
                "filter_acc_throttle",
                default_value="10.0",
                description="Outliers threshold removal for acceleration data in neural_network_throttle model",
            ),
            launch.actions.DeclareLaunchArgument(
                "filter_vel_brake",
                default_value="1.5",
                description="Outliers threshold removal for velocity data in neural_network_brake model",
            ),
            launch.actions.DeclareLaunchArgument(
                "filter_cmd_brake",
                default_value="10.0",
                description="Outliers threshold removal for brake commands data in neural_network_brake model",
            ),
            launch.actions.DeclareLaunchArgument(
                "filter_acc_brake",
                default_value="10.0",
                description="Outliers threshold removal for acceleration data in neural_network_brake model",
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="neural_network_throttle.py",
                name="neural_network_throttle",
                output="screen",
                parameters=[
                    {
                        "filter_vel_throttle": launch.substitutions.LaunchConfiguration(
                            "filter_vel_throttle"
                        ),
                        "filter_cmd_throttle": launch.substitutions.LaunchConfiguration(
                            "filter_cmd_throttle"
                        ),
                        "filter_acc_throttle": launch.substitutions.LaunchConfiguration(
                            "filter_acc_throttle"
                        ),
                    }
                ],
            ),
            launch_ros.actions.Node(
                package="learning_based_vehicle_calibration",
                executable="neural_network_brake.py",
                name="neural_network_brake",
                output="screen",
                parameters=[
                    {
                        "filter_vel_brake": launch.substitutions.LaunchConfiguration(
                            "filter_vel_brake"
                        )
                    },
                    {
                        "filter_cmd_brake": launch.substitutions.LaunchConfiguration(
                            "filter_cmd_brake"
                        )
                    },
                    {
                        "filter_acc_brake": launch.substitutions.LaunchConfiguration(
                            "filter_acc_brake"
                        )
                    },
                ],
            ),
        ]
    )
