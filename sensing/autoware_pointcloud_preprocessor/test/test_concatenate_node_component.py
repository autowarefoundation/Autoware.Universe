#!/usr/bin/env python3

# Copyright 2024 TIER IV, Inc.
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

import random
import struct
import time
from typing import List
from typing import Tuple
import unittest

from geometry_msgs.msg import TransformStamped
from geometry_msgs.msg import TwistWithCovarianceStamped
import launch
import launch.actions
from launch_ros.actions import ComposableNodeContainer
from launch_ros.descriptions import ComposableNode
import launch_testing
import numpy as np
import pytest
import rclpy
from rclpy.qos import QoSDurabilityPolicy
from rclpy.qos import QoSHistoryPolicy
from rclpy.qos import QoSProfile
from rclpy.qos import QoSReliabilityPolicy
from rclpy.time import Time
from sensor_msgs.msg import PointCloud2
from sensor_msgs.msg import PointField
from sensor_msgs_py import point_cloud2
from std_msgs.msg import Header
from tf2_ros.static_transform_broadcaster import StaticTransformBroadcaster

INPUT_LIDAR_TOPICS = [
    "/test/sensing/lidar/left/pointcloud",
    "/test/sensing/lidar/right/pointcloud",
    "/test/sensing/lidar/top/pointcloud",
]
FRAME_ID_LISTS = [
    "left_lidar",
    "right_lidar",
    "top_lidar",
]

TIMEOUT_SEC = 0.2
TIMESTAMP_OFFSET = [0.0, 0.04, 0.08]
TIMESTAMP_NOISE = 0.01  # 10 ms

NUM_OF_POINTS = 3
MILLISECONDS = 1000000


STANDARD_TOLERANCE = 1e-4
COARSE_TOLERANCE = TIMESTAMP_NOISE * 2

GLOBAL_SECONDS = 10
GLOBAL_NANOSECONDS = 100000000

# Set to True if you want to check the output of the component tests.
DEBUG = False


@pytest.mark.launch_test
def generate_test_description():
    nodes = []

    nodes.append(
        ComposableNode(
            package="autoware_pointcloud_preprocessor",
            plugin="autoware::pointcloud_preprocessor::PointCloudConcatenateDataSynchronizerComponent",
            name="test_concatenate_node",
            remappings=[
                ("~/input/twist", "/test/sensing/vehicle_velocity_converter/twist_with_covariance"),
                ("output", "/test/sensing/lidar/concatenated/pointcloud"),
            ],
            parameters=[
                {
                    "use_cuda": False,
                    "debug_mode": False,
                    "has_static_tf_only": False,
                    "rosbag_length": 0.0,
                    "maximum_queue_size": 5,
                    "timeout_sec": TIMEOUT_SEC,
                    "is_motion_compensated": True,
                    "publish_synchronized_pointcloud": True,
                    "keep_input_frame_in_synchronized_pointcloud": True,
                    "publish_previous_but_late_pointcloud": True,
                    "synchronized_pointcloud_postfix": "pointcloud",
                    "input_twist_topic_type": "twist",
                    "input_topics": INPUT_LIDAR_TOPICS,
                    "output_frame": "base_link",
                    "matching_strategy.type": "advanced",
                    "matching_strategy.lidar_timestamp_offsets": TIMESTAMP_OFFSET,
                    "matching_strategy.lidar_timestamp_noise_window": [
                        TIMESTAMP_NOISE,
                        TIMESTAMP_NOISE,
                        TIMESTAMP_NOISE,
                    ],
                }
            ],
            extra_arguments=[{"use_intra_process_comms": True}],
        )
    )

    container = ComposableNodeContainer(
        name="test_concatenate_data_container",
        namespace="pointcloud_preprocessor",
        package="rclcpp_components",
        executable="component_container",
        composable_node_descriptions=nodes,
        output="screen",
    )

    return launch.LaunchDescription(
        [
            container,
            # Start tests right away - no need to wait for anything
            launch_testing.actions.ReadyToTest(),
        ]
    )


def create_header(timestamp: Time, frame_id_index: int, is_base_link: bool) -> Header:
    header = Header()
    header.stamp = timestamp

    if is_base_link:
        header.frame_id = "base_link"
    else:
        header.frame_id = FRAME_ID_LISTS[frame_id_index]
    return header


def create_points() -> List[Tuple[float, float, float]]:
    return [(1.0, 0.0, 0.0), (0.0, 1.0, 0.0), (0.0, 0.0, 1.0)]


def create_fields() -> (
    Tuple[List[int], List[int], List[int], List[float], List[float], List[float], List[int]]
):
    # The values of the fields do not influence the results.
    intensities = [255] * NUM_OF_POINTS
    return_types = [1] * NUM_OF_POINTS
    channels = [1] * NUM_OF_POINTS
    azimuths = [0.0] * NUM_OF_POINTS
    elevations = [0.0] * NUM_OF_POINTS
    distances = [1.0] * NUM_OF_POINTS
    timestamps = [0] * NUM_OF_POINTS
    return intensities, return_types, channels, azimuths, elevations, distances, timestamps


def get_pointcloud_msg(
    timestamp: Time, is_generate_points: bool, frame_id_index: int, is_base_link: bool
) -> PointCloud2:
    header = create_header(timestamp, frame_id_index, is_base_link)
    points = create_points()
    intensities, return_types, channels, azimuths, elevations, distances, timestamps = (
        create_fields()
    )

    pointcloud_data = bytearray()

    if is_generate_points:
        for i in range(NUM_OF_POINTS):
            pointcloud_data += struct.pack("fff", points[i][0], points[i][1], points[i][2])
            pointcloud_data += struct.pack("B", intensities[i])
            pointcloud_data += struct.pack("B", return_types[i])
            pointcloud_data += struct.pack("H", channels[i])
            pointcloud_data += struct.pack("f", azimuths[i])
            pointcloud_data += struct.pack("f", elevations[i])
            pointcloud_data += struct.pack("f", distances[i])
            pointcloud_data += struct.pack("I", timestamps[i])

    fields = [
        PointField(name="x", offset=0, datatype=PointField.FLOAT32, count=1),
        PointField(name="y", offset=4, datatype=PointField.FLOAT32, count=1),
        PointField(name="z", offset=8, datatype=PointField.FLOAT32, count=1),
        PointField(name="intensity", offset=12, datatype=PointField.UINT8, count=1),
        PointField(name="return_type", offset=13, datatype=PointField.UINT8, count=1),
        PointField(name="channel", offset=14, datatype=PointField.UINT16, count=1),
        PointField(name="azimuth", offset=16, datatype=PointField.FLOAT32, count=1),
        PointField(name="elevation", offset=20, datatype=PointField.FLOAT32, count=1),
        PointField(name="distance", offset=24, datatype=PointField.FLOAT32, count=1),
        PointField(name="time_stamp", offset=28, datatype=PointField.UINT32, count=1),
    ]

    pointcloud_msg = PointCloud2(
        header=header,
        height=1,
        width=NUM_OF_POINTS,
        is_dense=True,
        is_bigendian=False,
        point_step=32,  # 3*4 + 1 + 1 + 2 + 4 + 4 + 4 + 4 = 32 bytes per point
        row_step=32 * NUM_OF_POINTS,
        fields=fields,
        data=pointcloud_data,
    )

    return pointcloud_msg


def generate_transform_msg(
    parent_frame: str,
    child_frame: str,
    x: float,
    y: float,
    z: float,
    qx: float,
    qy: float,
    qz: float,
    qw: float,
) -> TransformStamped:
    tf_msg = TransformStamped()
    tf_msg.header.stamp = Time(seconds=GLOBAL_SECONDS, nanoseconds=GLOBAL_NANOSECONDS).to_msg()
    tf_msg.header.frame_id = parent_frame
    tf_msg.child_frame_id = child_frame
    tf_msg.transform.translation.x = x
    tf_msg.transform.translation.y = y
    tf_msg.transform.translation.z = z
    tf_msg.transform.rotation.x = qx
    tf_msg.transform.rotation.y = qy
    tf_msg.transform.rotation.z = qz
    tf_msg.transform.rotation.w = qw
    return tf_msg


def generate_static_transform_msgs() -> List[TransformStamped]:
    tf_top_lidar_msg = generate_transform_msg(
        parent_frame="base_link",
        child_frame=FRAME_ID_LISTS[0],
        x=0.0,
        y=0.0,
        z=5.0,
        qx=0.0,
        qy=0.0,
        qz=0.0,
        qw=1.0,
    )

    tf_right_lidar_msg = generate_transform_msg(
        parent_frame="base_link",
        child_frame=FRAME_ID_LISTS[1],
        x=0.0,
        y=5.0,
        z=5.0,
        qx=0.0,
        qy=0.0,
        qz=0.0,
        qw=1.0,
    )

    tf_left_lidar_msg = generate_transform_msg(
        parent_frame="base_link",
        child_frame=FRAME_ID_LISTS[2],
        x=0.0,
        y=-5.0,
        z=5.0,
        qx=0.0,
        qy=0.0,
        qz=0.0,
        qw=1.0,
    )

    return [tf_top_lidar_msg, tf_right_lidar_msg, tf_left_lidar_msg]


def generate_twist_msg() -> TwistWithCovarianceStamped:
    twist_header = Header()
    twist_header.stamp = Time(seconds=GLOBAL_SECONDS, nanoseconds=GLOBAL_NANOSECONDS).to_msg()
    twist_header.frame_id = "base_link"
    twist_msg = TwistWithCovarianceStamped()
    twist_msg.header = twist_header
    twist_msg.twist.twist.linear.x = 1.0

    return twist_msg


def get_output_points(cloud_msg) -> np.ndarray:
    points_list = []
    for point in point_cloud2.read_points(cloud_msg, field_names=("x", "y", "z")):
        points_list.append([point[0], point[1], point[2]])
    points = np.array(points_list, dtype=np.float32)
    return points


class TestConcatenateNode(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        # Init ROS at once
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        # Shutdown ROS at once
        rclpy.shutdown()

    def setUp(self):
        self.node = rclpy.create_node("test_concat_node")
        tf_msg = generate_static_transform_msgs()
        self.tf_broadcaster = StaticTransformBroadcaster(self.node)
        self.tf_broadcaster.sendTransform(tf_msg)
        self.msg_buffer = []
        self.twist_publisher, self.pointcloud_publishers = self.create_pub_sub()

    def tearDown(self):
        self.node.destroy_node()

    def callback(self, msg: PointCloud2):
        self.msg_buffer.append(msg)

    def create_pub_sub(self):
        # QoS profile for sensor data
        sensor_qos = QoSProfile(
            history=QoSHistoryPolicy.KEEP_LAST,
            depth=10,
            reliability=QoSReliabilityPolicy.BEST_EFFORT,
            durability=QoSDurabilityPolicy.VOLATILE,
        )
        # Publishers
        twist_publisher = self.node.create_publisher(
            TwistWithCovarianceStamped,
            "/test/sensing/vehicle_velocity_converter/twist_with_covariance",
            10,
        )

        pointcloud_publishers = {}
        for idx, input_lidar_topic in enumerate(INPUT_LIDAR_TOPICS):
            pointcloud_publishers[idx] = self.node.create_publisher(
                PointCloud2,
                input_lidar_topic,
                qos_profile=sensor_qos,
            )

        # create subscriber
        self.msg_buffer = []
        self.node.create_subscription(
            PointCloud2,
            "/test/sensing/lidar/concatenated/pointcloud",
            self.callback,
            qos_profile=sensor_qos,
        )

        return twist_publisher, pointcloud_publishers

    def test_1_normal_inputs(self):
        """Test the normal situation when no pointcloud is delayed or dropped.

        This can test that
        1. Concatenate works fine when all pointclouds are arrived in time.
        2. The motion compensation of concatenation works well.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        for frame_idx, _ in enumerate(INPUT_LIDAR_TOPICS):
            pointcloud_seconds = GLOBAL_SECONDS
            pointcloud_nanoseconds = GLOBAL_NANOSECONDS + frame_idx * MILLISECONDS * 40  # add 40 ms
            pointcloud_timestamp = Time(
                seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
            ).to_msg()
            pointcloud_msg = get_pointcloud_msg(
                timestamp=pointcloud_timestamp,
                is_generate_points=True,
                frame_id_index=frame_idx,
                is_base_link=False,
            )
            self.pointcloud_publishers[frame_idx].publish(pointcloud_msg)
            time.sleep(0.01)

        rclpy.spin_once(self.node, timeout_sec=0.1)

        self.assertEqual(
            len(self.msg_buffer),
            1,
            "The number of concatenate pointcloud has different number as expected.",
        )

        expected_pointcloud = np.array(
            [
                [1.08, -5, 5],
                [0.08, -4, 5],
                [0.08, -5, 6],
                [1.04, 5, 5],
                [0.04, 6, 5],
                [0.04, 5, 6],
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud = get_output_points(self.msg_buffer[0])

        if DEBUG:
            print("concatenate_cloud: ", concatenate_cloud)

        self.assertTrue(
            np.allclose(concatenate_cloud, expected_pointcloud, atol=1e-3),
            "The concatenation node have weird output",
        )

        self.assertEqual(
            self.msg_buffer[0].header.frame_id,
            "base_link",
            "The concatenate pointcloud frame id is not base_link",
        )

        GLOBAL_SECONDS += 1

    def test_2_normal_inputs_with_noise(self):
        """Test the normal situation when no pointcloud is delayed or dropped. Additionally, the pointcloud's timestamp is not ideal which has some noise.

        This can test that
        1. Concatenate works fine when pointclouds' timestamp has noise.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        for frame_idx, _ in enumerate(INPUT_LIDAR_TOPICS):
            noise = random.uniform(-10, 10) * MILLISECONDS
            pointcloud_seconds = GLOBAL_SECONDS
            pointcloud_nanoseconds = (
                GLOBAL_NANOSECONDS + frame_idx * MILLISECONDS * 40 + noise
            )  # add 40 ms and noise (-10 to 10 ms)
            pointcloud_timestamp = Time(
                seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
            ).to_msg()
            pointcloud_msg = get_pointcloud_msg(
                timestamp=pointcloud_timestamp,
                is_generate_points=True,
                frame_id_index=frame_idx,
                is_base_link=False,
            )
            self.pointcloud_publishers[frame_idx].publish(pointcloud_msg)
            time.sleep(0.01)

        rclpy.spin_once(self.node, timeout_sec=0.1)

        self.assertEqual(
            len(self.msg_buffer),
            1,
            "The number of concatenate pointcloud has different number as expected.",
        )

        expected_pointcloud = np.array(
            [
                [1.08, -5, 5],
                [0.08, -4, 5],
                [0.08, -5, 6],
                [1.04, 5, 5],
                [0.04, 6, 5],
                [0.04, 5, 6],
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud = get_output_points(self.msg_buffer[0])
        if DEBUG:
            print("concatenate_cloud: ", concatenate_cloud)

        self.assertTrue(
            np.allclose(concatenate_cloud, expected_pointcloud, atol=2e-2),
            "The concatenation node have weird output",
        )

    def test_3_abnormal_null_pointcloud(self):
        """Test the abnormal situation when a pointcloud is empty.

        This can test that
        1. The concatenate node ignore empty pointcloud and concatenate the remain pointcloud.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        for frame_idx, _ in enumerate(INPUT_LIDAR_TOPICS):
            pointcloud_seconds = GLOBAL_SECONDS
            pointcloud_nanoseconds = GLOBAL_NANOSECONDS + frame_idx * MILLISECONDS * 40  # add 40 ms
            pointcloud_timestamp = Time(
                seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
            ).to_msg()

            if frame_idx == len(INPUT_LIDAR_TOPICS) - 1:
                pointcloud_msg = get_pointcloud_msg(
                    timestamp=pointcloud_timestamp,
                    is_generate_points=False,
                    frame_id_index=len(INPUT_LIDAR_TOPICS) - 1,
                    is_base_link=False,
                )
            else:
                pointcloud_msg = get_pointcloud_msg(
                    timestamp=pointcloud_timestamp,
                    is_generate_points=True,
                    frame_id_index=frame_idx,
                    is_base_link=False,
                )

            self.pointcloud_publishers[frame_idx].publish(pointcloud_msg)
            time.sleep(0.01)

        time.sleep(TIMEOUT_SEC)  # timeout threshold
        rclpy.spin_once(self.node, timeout_sec=0.1)

        self.assertEqual(
            len(self.msg_buffer),
            1,
            "The number of concatenate pointcloud has different number as expected.",
        )

        expected_pointcloud = np.array(
            [
                [1.04, 5, 5],
                [0.04, 6, 5],
                [0.04, 5, 6],
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud = get_output_points(self.msg_buffer[0])
        if DEBUG:
            print("concatenate_cloud: ", concatenate_cloud)

        self.assertTrue(
            np.allclose(concatenate_cloud, expected_pointcloud, atol=1e-3),
            "The concatenation node have weird output",
        )

        GLOBAL_SECONDS += 1

    def test_4_abnormal_null_pointcloud_and_drop(self):
        """Test the abnormal situation when a pointcloud is empty and other pointclouds are dropped.

        This can test that
        1. The concatenate node publish an empty pointcloud.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        pointcloud_seconds = GLOBAL_SECONDS
        pointcloud_nanoseconds = GLOBAL_NANOSECONDS
        pointcloud_timestamp = Time(
            seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
        ).to_msg()

        pointcloud_msg = get_pointcloud_msg(
            timestamp=pointcloud_timestamp,
            is_generate_points=False,
            frame_id_index=0,
            is_base_link=False,
        )

        self.pointcloud_publishers[0].publish(pointcloud_msg)
        time.sleep(0.01)

        time.sleep(TIMEOUT_SEC)  # timeout threshold
        rclpy.spin_once(self.node, timeout_sec=0.1)

        self.assertEqual(
            len(self.msg_buffer),
            1,
            "The number of concatenate pointcloud has different number as expected.",
        )

        GLOBAL_SECONDS += 1

    def test_5_abnormal_multiple_pointcloud_drop(self):
        """Test the abnormal situation when multiple pointclouds were dropped (only one pointcloud arrive).

        This can test that
        1. The concatenate node concatenates the single pointcloud after the timeout.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        pointcloud_seconds = GLOBAL_SECONDS
        pointcloud_nanoseconds = GLOBAL_NANOSECONDS
        pointcloud_timestamp = Time(
            seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
        ).to_msg()

        pointcloud_msg = get_pointcloud_msg(
            timestamp=pointcloud_timestamp,
            is_generate_points=True,
            frame_id_index=0,
            is_base_link=False,
        )

        self.pointcloud_publishers[0].publish(pointcloud_msg)
        time.sleep(0.01)

        time.sleep(TIMEOUT_SEC)  # timeout threshold
        rclpy.spin_once(self.node, timeout_sec=0.1)

        self.assertEqual(
            len(self.msg_buffer),
            1,
            "The number of concatenate pointcloud has different number as expected.",
        )

        expected_pointcloud = np.array(
            [
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud = get_output_points(self.msg_buffer[0])
        if DEBUG:
            print("concatenate_cloud: ", concatenate_cloud)

        self.assertTrue(
            np.allclose(concatenate_cloud, expected_pointcloud, atol=1e-3),
            "The concatenation node have weird output",
        )

    def test_6_abnormal_single_pointcloud_drop(self):
        """Test the abnormal situation when a pointcloud was dropped.

        This can test that
        1. The concatenate node concatenate the remain pointcloud after the timeout.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        for frame_idx, _ in enumerate(INPUT_LIDAR_TOPICS[:-1]):
            pointcloud_seconds = GLOBAL_SECONDS
            pointcloud_nanoseconds = GLOBAL_NANOSECONDS + frame_idx * MILLISECONDS * 40  # add 40 ms
            pointcloud_timestamp = Time(
                seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
            ).to_msg()
            pointcloud_msg = get_pointcloud_msg(
                timestamp=pointcloud_timestamp,
                is_generate_points=True,
                frame_id_index=frame_idx,
                is_base_link=False,
            )
            self.pointcloud_publishers[frame_idx].publish(pointcloud_msg)
            time.sleep(0.02)

        time.sleep(TIMEOUT_SEC)  # timeout threshold
        rclpy.spin_once(self.node, timeout_sec=0.1)

        # Should receive only one concatenate pointcloud
        self.assertEqual(
            len(self.msg_buffer),
            1,
            "The number of concatenate pointcloud has different number as expected.",
        )

        expected_pointcloud = np.array(
            [
                [1.04, 5, 5],
                [0.04, 6, 5],
                [0.04, 5, 6],
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud = get_output_points(self.msg_buffer[0])
        if DEBUG:
            print("concatenate_cloud: ", concatenate_cloud)

        self.assertTrue(
            np.allclose(concatenate_cloud, expected_pointcloud, atol=1e-3),
            "The concatenation node have weird output",
        )

        GLOBAL_SECONDS += 1

    def test_7_abnormal_pointcloud_delay(self):
        """Test the abnormal situation when a pointcloud was delayed after the timeout.

        This can test that
        1. The concatenate node concatenate the remain pointcloud after the timeout.
        2. The concatenate node will publish the delayed pointcloud after the timeout.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        for frame_idx, _ in enumerate(INPUT_LIDAR_TOPICS[:-1]):
            pointcloud_seconds = GLOBAL_SECONDS
            pointcloud_nanoseconds = GLOBAL_NANOSECONDS + frame_idx * MILLISECONDS * 40  # add 40 ms
            pointcloud_timestamp = Time(
                seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
            ).to_msg()
            pointcloud_msg = get_pointcloud_msg(
                timestamp=pointcloud_timestamp,
                is_generate_points=True,
                frame_id_index=frame_idx,
                is_base_link=False,
            )
            self.pointcloud_publishers[frame_idx].publish(pointcloud_msg)
            time.sleep(0.02)

        time.sleep(TIMEOUT_SEC)  # timeout threshold
        rclpy.spin_once(self.node, timeout_sec=0.1)

        pointcloud_seconds = GLOBAL_SECONDS
        pointcloud_nanoseconds = (
            GLOBAL_NANOSECONDS + (len(INPUT_LIDAR_TOPICS) - 1) * MILLISECONDS * 40
        )  # add 40 ms
        pointcloud_timestamp = Time(
            seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
        ).to_msg()
        pointcloud_msg = get_pointcloud_msg(
            timestamp=pointcloud_timestamp,
            is_generate_points=True,
            frame_id_index=len(INPUT_LIDAR_TOPICS) - 1,
            is_base_link=False,
        )

        self.pointcloud_publishers[len(INPUT_LIDAR_TOPICS) - 1].publish(pointcloud_msg)

        time.sleep(TIMEOUT_SEC)  # timeout threshold
        rclpy.spin_once(self.node, timeout_sec=0.1)

        # Should receive only one concatenate pointcloud
        self.assertEqual(
            len(self.msg_buffer),
            2,
            "The number of concatenate pointcloud has different number as expected.",
        )

        expected_pointcloud1 = np.array(
            [
                [1.04, 5, 5],
                [0.04, 6, 5],
                [0.04, 5, 6],
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud1 = get_output_points(self.msg_buffer[0])
        if DEBUG:
            print("concatenate_cloud 1: ", concatenate_cloud1)

        self.assertTrue(
            np.allclose(concatenate_cloud1, expected_pointcloud1, atol=1e-3),
            "The concatenation node have weird output",
        )

        expected_pointcloud2 = np.array(
            [
                [1, -5, 5],
                [0, -4, 5],
                [0, -5, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud2 = get_output_points(self.msg_buffer[1])
        if DEBUG:
            print("concatenate_cloud 2: ", concatenate_cloud2)

        self.assertTrue(
            np.allclose(concatenate_cloud2, expected_pointcloud2, atol=1e-3),
            "The concatenation node have weird output",
        )

        GLOBAL_SECONDS += 1

    def test_8_abnormal_pointcloud_drop_continue_normal(self):
        """Test the abnormal situation when a pointcloud was dropped. Afterward, next iteration of pointclouds comes normally.

        This can test that
        1. The concatenate node concatenate the remain pointcloud after the timeout.
        2. The concatenate node concatenate next iteration pointclouds when all of the pointcloud arrived.
        """
        time.sleep(1)
        global GLOBAL_SECONDS

        twist_msg = generate_twist_msg()
        self.twist_publisher.publish(twist_msg)

        for frame_idx, _ in enumerate(INPUT_LIDAR_TOPICS[:-1]):
            pointcloud_seconds = GLOBAL_SECONDS
            pointcloud_nanoseconds = GLOBAL_NANOSECONDS + frame_idx * MILLISECONDS * 40  # add 40 ms
            pointcloud_timestamp = Time(
                seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
            ).to_msg()
            pointcloud_msg = get_pointcloud_msg(
                timestamp=pointcloud_timestamp,
                is_generate_points=True,
                frame_id_index=frame_idx,
                is_base_link=False,
            )
            self.pointcloud_publishers[frame_idx].publish(pointcloud_msg)
            time.sleep(0.01)

        time.sleep(TIMEOUT_SEC)
        rclpy.spin_once(self.node)

        next_global_nanosecond = GLOBAL_NANOSECONDS + 100 * MILLISECONDS
        for frame_idx, _ in enumerate(INPUT_LIDAR_TOPICS):
            pointcloud_seconds = GLOBAL_SECONDS
            pointcloud_nanoseconds = (
                next_global_nanosecond + frame_idx * MILLISECONDS * 40
            )  # add 40 ms
            pointcloud_timestamp = Time(
                seconds=pointcloud_seconds, nanoseconds=pointcloud_nanoseconds
            ).to_msg()
            pointcloud_msg = get_pointcloud_msg(
                timestamp=pointcloud_timestamp,
                is_generate_points=True,
                frame_id_index=frame_idx,
                is_base_link=False,
            )
            self.pointcloud_publishers[frame_idx].publish(pointcloud_msg)
            time.sleep(0.01)

        rclpy.spin_once(self.node)

        self.assertEqual(
            len(self.msg_buffer),
            2,
            "The number of concatenate pointcloud has different number as expected.",
        )

        expected_pointcloud1 = np.array(
            [
                [1.04, 5, 5],
                [0.04, 6, 5],
                [0.04, 5, 6],
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud1 = get_output_points(self.msg_buffer[0])
        if DEBUG:
            print("concatenate_cloud 1: ", concatenate_cloud1)

        self.assertTrue(
            np.allclose(concatenate_cloud1, expected_pointcloud1, atol=1e-3),
            "The concatenation node have weird output",
        )

        expected_pointcloud2 = np.array(
            [
                [1.08, -5, 5],
                [0.08, -4, 5],
                [0.08, -5, 6],
                [1.04, 5, 5],
                [0.04, 6, 5],
                [0.04, 5, 6],
                [1, 0, 5],
                [0, 1, 5],
                [0, 0, 6],
            ],
            dtype=np.float32,
        )

        concatenate_cloud2 = get_output_points(self.msg_buffer[1])
        if DEBUG:
            print("concatenate_cloud 2: ", concatenate_cloud2)

        self.assertTrue(
            np.allclose(concatenate_cloud2, expected_pointcloud2, atol=1e-3),
            "The concatenation node have weird output",
        )

        GLOBAL_SECONDS += 1
