#! /usr/bin/env python3
import math

from collections import deque
from statistics import mean

import rclpy
import rclpy.node
import pandas as pd

from tier4_vehicle_msgs.msg import ActuationStatusStamped
from autoware_auto_vehicle_msgs.msg import VelocityReport
from autoware_auto_vehicle_msgs.msg import SteeringReport
from sensor_msgs.msg import Imu
from std_msgs.msg import Float32


from tqdm import tqdm

from learning_based_accel_brake_map_calibrator.msg import LongitudinalProgress
from learning_based_accel_brake_map_calibrator.msg import LongitudinalProcesses


class primotest(rclpy.node.Node):

    def __init__(self):

        super().__init__("primo_test")

        # data sharing member variables
        self.braking = 0.0
        self.braking_prec = 0.0
        self.throttling = 0.0
        self.throttling_prec = 0.0
        self.steering = 0.0
        self.acceleration = 0.0
        self.velocity = 0.0
        self.pitch_angle = 0.0
        self.flag = 0
        self.g = 9.80665

        # launch params initialization to default_values
        self.declare_parameter("max_data", 1500)
        self.declare_parameter("num_of_queue", 20)
        self.declare_parameter("speed_threshold", 2.8)
        self.declare_parameter("steering_threshold", 0.03490658503988659)
        self.declare_parameter("throttle_deadzone", 5)
        self.declare_parameter("brake_deadzone", 5)
        self.declare_parameter("max_velocity", 11.1)
        self.declare_parameter("throttle_threshold1", 30)
        self.declare_parameter("throttle_threshold2", 55)
        self.declare_parameter("brake_threshold1", 15)
        self.declare_parameter("brake_threshold2", 25)
        self.declare_parameter("consistency_threshold", 20)
        self.declare_parameter("pitch_topic", "/sensing/gnss/chc/pitch")
        self.declare_parameter(
            "actuation_status_topic", "/vehicle/status/actuation_status"
        )
        self.declare_parameter(
            "steering_status_topic", "/vehicle/status/steering_status"
        )
        self.declare_parameter(
            "velocity_status_topic", "/vehicle/status/velocity_status"
        )
        self.declare_parameter("imu_topic", "/sensing/gnss/chc/imu")

        self.declare_parameter("Recovery_Mode", False)

        # Load params from launch file

        self.MAX_DATA = (
            self.get_parameter("max_data").get_parameter_value().integer_value
        )
        self.NUM_OF_QUEUE = (
            self.get_parameter("num_of_queue").get_parameter_value().integer_value
        )
        self.SPEED_THRESHOLD = (
            self.get_parameter("speed_threshold").get_parameter_value().double_value
        )
        self.STEERING_THRESHOLD = (
            self.get_parameter("steering_threshold").get_parameter_value().double_value
        )
        self.THROTTLE_DEADZONE = (
            self.get_parameter("throttle_deadzone").get_parameter_value().integer_value
        )
        self.BRAKE_DEADZONE = (
            self.get_parameter("brake_deadzone").get_parameter_value().integer_value
        )
        self.MAX_VELOCITY = (
            self.get_parameter("max_velocity").get_parameter_value().double_value
        )
        self.THROTTLE_THRESHOLD1 = (
            self.get_parameter("throttle_threshold1")
            .get_parameter_value()
            .integer_value
        )
        self.THROTTLE_THRESHOLD2 = (
            self.get_parameter("throttle_threshold2")
            .get_parameter_value()
            .integer_value
        )
        self.BRAKE_THRESHOLD1 = (
            self.get_parameter("brake_threshold1").get_parameter_value().integer_value
        )
        self.BRAKE_THRESHOLD2 = (
            self.get_parameter("brake_threshold2").get_parameter_value().integer_value
        )
        self.CONSISTENCY_THRESHOLD = (
            self.get_parameter("consistency_threshold")
            .get_parameter_value()
            .integer_value
        )
        # Get topic names from parameters
        self.pitch_topic = (
            self.get_parameter("pitch_topic").get_parameter_value().string_value
        )
        self.actuation_status_topic = (
            self.get_parameter("actuation_status_topic")
            .get_parameter_value()
            .string_value
        )
        self.steering_status_topic = (
            self.get_parameter("steering_status_topic")
            .get_parameter_value()
            .string_value
        )
        self.velocity_status_topic = (
            self.get_parameter("velocity_status_topic")
            .get_parameter_value()
            .string_value
        )
        self.imu_topic = (
            self.get_parameter("imu_topic").get_parameter_value().string_value
        )

        self.RECOVERY_MODE = (
            self.get_parameter("Recovery_Mode").get_parameter_value().bool_value
        )

        if self.RECOVERY_MODE:
            df_existing1 = pd.read_csv("throttling.csv")
            df_existing2 = pd.read_csv("braking.csv")

            self.k = int(df_existing1["Low_V_0_deadzone"].iloc[0])
            self.i = int(df_existing1["Low_V_deadzone_thr1"].iloc[0])
            self.j = int(df_existing1["Low_V_thr1_thr2"].iloc[0])
            self.h = int(df_existing1["Low_V_thr2_max"].iloc[0])
            self.d = int(df_existing1["High_V_0_deadzone"].iloc[0])
            self.a = int(df_existing1["High_V_deadzone_thr1"].iloc[0])
            self.b = int(df_existing1["High_V_thr1_thr2"].iloc[0])
            self.c = int(df_existing1["High_V_thr2_max"].iloc[0])
            self.vel = df_existing1["Velocity"].tolist()
            self.cmd = df_existing1["Throttling"].tolist()
            self.acc = df_existing1["Acceleration_with_pitch_comp"].tolist()
            self.acc2 = df_existing1["Acceleration_measured"].tolist()
            self.pitch = df_existing1["Pitch_angle"].tolist()

            self.kk = int(df_existing2["Low_V_0_deadzone"].iloc[0])
            self.ii = int(df_existing2["Low_V_deadzone_thr1"].iloc[0])
            self.jj = int(df_existing2["Low_V_thr1_thr2"].iloc[0])
            self.hh = int(df_existing2["Low_V_thr2_max"].iloc[0])
            self.dd = int(df_existing2["High_V_0_deadzone"].iloc[0])
            self.aa = int(df_existing2["High_V_deadzone_thr1"].iloc[0])
            self.bb = int(df_existing2["High_V_thr1_thr2"].iloc[0])
            self.cc = int(df_existing2["High_V_thr2_max"].iloc[0])
            self.velb = df_existing2["Velocity"].tolist()
            self.cmdb = df_existing2["Braking"].tolist()
            self.accb = df_existing2["Acceleration_with_pitch_comp"].tolist()
            self.accb2 = df_existing2["Acceleration_measured"].tolist()
            self.pitch2 = df_existing2["Pitch_angle"].tolist()

        else:
            self.i = self.j = self.h = self.k = self.a = self.b = self.c = self.d = (
                self.d
            ) = self.ii = self.jj = self.hh = self.kk = self.aa = self.bb = self.cc = (
                self.dd
            ) = 0
            self.vel = []
            self.cmd = []
            self.acc = []
            self.acc2 = []
            self.velb = []
            self.cmdb = []
            self.accb = []
            self.accb2 = []
            self.pitch = []
            self.pitch2 = []

        # custom messages definitions and initialization
        self.long_progress_throttle_msg = [LongitudinalProgress() for _ in range(8)]
        self.long_progress_brake_msg = [LongitudinalProgress() for _ in range(8)]
        self.long_processes_throttle_msg = LongitudinalProcesses()
        self.long_processes_brake_msg = LongitudinalProcesses()
        self.long_processes_throttle_msg.processes = [
            LongitudinalProgress() for _ in range(8)
        ]
        self.long_processes_brake_msg.processes = [
            LongitudinalProgress() for _ in range(8)
        ]

        self.long_processes_throttle_msg.header.frame_id = "Throttle scenario"
        self.long_processes_brake_msg.header.frame_id = "Brake scenario"

        self.long_progress_throttle_msg[0].pedal_value_start = 0
        self.long_progress_throttle_msg[0].pedal_value_end = self.THROTTLE_DEADZONE
        self.long_progress_throttle_msg[0].velocity_start = 0.0
        self.long_progress_throttle_msg[0].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_throttle_msg.processes[0] = self.long_progress_throttle_msg[
            0
        ]
        self.long_progress_brake_msg[0].pedal_value_start = 0
        self.long_progress_brake_msg[0].pedal_value_end = self.BRAKE_DEADZONE
        self.long_progress_brake_msg[0].velocity_start = 0.0
        self.long_progress_brake_msg[0].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_brake_msg.processes[0] = self.long_progress_brake_msg[0]
        self.long_progress_throttle_msg[0].data_count = self.k
        self.long_progress_throttle_msg[0].progress = int(self.k * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[0].data_count = self.kk
        self.long_progress_brake_msg[0].progress = int(self.kk * 100 / self.MAX_DATA)

        self.long_progress_throttle_msg[1].pedal_value_start = self.THROTTLE_DEADZONE
        self.long_progress_throttle_msg[1].pedal_value_end = self.THROTTLE_THRESHOLD1
        self.long_progress_throttle_msg[1].velocity_start = 0.0
        self.long_progress_throttle_msg[1].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_throttle_msg.processes[1] = self.long_progress_throttle_msg[
            1
        ]
        self.long_progress_brake_msg[1].pedal_value_start = self.BRAKE_DEADZONE
        self.long_progress_brake_msg[1].pedal_value_end = self.BRAKE_THRESHOLD1
        self.long_progress_brake_msg[1].velocity_start = 0.0
        self.long_progress_brake_msg[1].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_brake_msg.processes[1] = self.long_progress_brake_msg[1]
        self.long_progress_throttle_msg[1].data_count = self.i
        self.long_progress_throttle_msg[1].progress = int(self.i * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[1].data_count = self.ii
        self.long_progress_brake_msg[1].progress = int(self.ii * 100 / self.MAX_DATA)

        self.long_progress_throttle_msg[2].pedal_value_start = self.THROTTLE_THRESHOLD1
        self.long_progress_throttle_msg[2].pedal_value_end = self.THROTTLE_THRESHOLD2
        self.long_progress_throttle_msg[2].velocity_start = 0.0
        self.long_progress_throttle_msg[2].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_throttle_msg.processes[2] = self.long_progress_throttle_msg[
            2
        ]
        self.long_progress_brake_msg[2].pedal_value_start = self.BRAKE_THRESHOLD1
        self.long_progress_brake_msg[2].pedal_value_end = self.BRAKE_THRESHOLD2
        self.long_progress_brake_msg[2].velocity_start = 0.0
        self.long_progress_brake_msg[2].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_brake_msg.processes[2] = self.long_progress_brake_msg[2]
        self.long_progress_throttle_msg[2].data_count = self.j
        self.long_progress_throttle_msg[2].progress = int(self.j * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[2].data_count = self.jj
        self.long_progress_brake_msg[2].progress = int(self.jj * 100 / self.MAX_DATA)

        self.long_progress_throttle_msg[3].pedal_value_start = self.THROTTLE_THRESHOLD2
        self.long_progress_throttle_msg[3].pedal_value_end = 100
        self.long_progress_throttle_msg[3].velocity_start = 0.0
        self.long_progress_throttle_msg[3].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_throttle_msg.processes[3] = self.long_progress_throttle_msg[
            3
        ]
        self.long_progress_brake_msg[3].pedal_value_start = self.BRAKE_THRESHOLD2
        self.long_progress_brake_msg[3].pedal_value_end = 100
        self.long_progress_brake_msg[3].velocity_start = 0.0
        self.long_progress_brake_msg[3].velocity_end = self.SPEED_THRESHOLD
        self.long_processes_brake_msg.processes[3] = self.long_progress_brake_msg[3]
        self.long_progress_throttle_msg[3].data_count = self.h
        self.long_progress_throttle_msg[3].progress = int(self.h * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[3].data_count = self.hh
        self.long_progress_brake_msg[3].progress = int(self.hh * 100 / self.MAX_DATA)

        self.long_progress_throttle_msg[4].pedal_value_start = 0
        self.long_progress_throttle_msg[4].pedal_value_end = self.THROTTLE_DEADZONE
        self.long_progress_throttle_msg[4].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_throttle_msg[4].velocity_end = self.MAX_VELOCITY
        self.long_processes_throttle_msg.processes[4] = self.long_progress_throttle_msg[
            4
        ]
        self.long_progress_brake_msg[4].pedal_value_start = 0
        self.long_progress_brake_msg[4].pedal_value_end = self.BRAKE_DEADZONE
        self.long_progress_brake_msg[4].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_brake_msg[4].velocity_end = self.MAX_VELOCITY
        self.long_processes_brake_msg.processes[4] = self.long_progress_brake_msg[4]
        self.long_progress_throttle_msg[4].data_count = self.d
        self.long_progress_throttle_msg[4].progress = int(self.d * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[4].data_count = self.dd
        self.long_progress_brake_msg[4].progress = int(self.dd * 100 / self.MAX_DATA)

        self.long_progress_throttle_msg[5].pedal_value_start = self.THROTTLE_DEADZONE
        self.long_progress_throttle_msg[5].pedal_value_end = self.THROTTLE_THRESHOLD1
        self.long_progress_throttle_msg[5].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_throttle_msg[5].velocity_end = self.MAX_VELOCITY
        self.long_processes_throttle_msg.processes[5] = self.long_progress_throttle_msg[
            5
        ]
        self.long_progress_brake_msg[5].pedal_value_start = self.BRAKE_DEADZONE
        self.long_progress_brake_msg[5].pedal_value_end = self.BRAKE_THRESHOLD1
        self.long_progress_brake_msg[5].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_brake_msg[5].velocity_end = self.MAX_VELOCITY
        self.long_processes_brake_msg.processes[5] = self.long_progress_brake_msg[5]
        self.long_progress_throttle_msg[5].data_count = self.a
        self.long_progress_throttle_msg[5].progress = int(self.a * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[5].data_count = self.aa
        self.long_progress_brake_msg[5].progress = int(self.aa * 100 / self.MAX_DATA)

        self.long_progress_throttle_msg[6].pedal_value_start = self.THROTTLE_THRESHOLD1
        self.long_progress_throttle_msg[6].pedal_value_end = self.THROTTLE_THRESHOLD2
        self.long_progress_throttle_msg[6].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_throttle_msg[6].velocity_end = self.MAX_VELOCITY
        self.long_processes_throttle_msg.processes[6] = self.long_progress_throttle_msg[
            6
        ]
        self.long_progress_brake_msg[6].pedal_value_start = self.BRAKE_THRESHOLD1
        self.long_progress_brake_msg[6].pedal_value_end = self.BRAKE_THRESHOLD2
        self.long_progress_brake_msg[6].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_brake_msg[6].velocity_end = self.MAX_VELOCITY
        self.long_processes_brake_msg.processes[6] = self.long_progress_brake_msg[6]
        self.long_progress_throttle_msg[6].data_count = self.b
        self.long_progress_throttle_msg[6].progress = int(self.b * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[6].data_count = self.bb
        self.long_progress_brake_msg[6].progress = int(self.bb * 100 / self.MAX_DATA)

        self.long_progress_throttle_msg[7].pedal_value_start = self.THROTTLE_THRESHOLD2
        self.long_progress_throttle_msg[7].pedal_value_end = 100
        self.long_progress_throttle_msg[7].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_throttle_msg[7].velocity_end = self.MAX_VELOCITY
        self.long_processes_throttle_msg.processes[7] = self.long_progress_throttle_msg[
            7
        ]
        self.long_progress_brake_msg[7].pedal_value_start = self.BRAKE_THRESHOLD2
        self.long_progress_brake_msg[7].pedal_value_end = 100
        self.long_progress_brake_msg[7].velocity_start = self.SPEED_THRESHOLD
        self.long_progress_brake_msg[7].velocity_end = self.MAX_VELOCITY
        self.long_processes_brake_msg.processes[7] = self.long_progress_brake_msg[7]
        self.long_progress_throttle_msg[7].data_count = self.c
        self.long_progress_throttle_msg[7].progress = int(self.c * 100 / self.MAX_DATA)
        self.long_progress_brake_msg[7].data_count = self.cc
        self.long_progress_brake_msg[7].progress = int(self.cc * 100 / self.MAX_DATA)

        self.progress_bar0 = tqdm(
            initial=self.k,
            total=self.MAX_DATA,
            desc="                                        Low speed: 0 - Throttle deadzone  ",
            dynamic_ncols=True,
        )
        self.progress_bar1 = tqdm(
            initial=self.i,
            total=self.MAX_DATA,
            desc="                                        Low speed: Throttle deadzone - "
            + str(self.THROTTLE_THRESHOLD1)
            + " ",
            dynamic_ncols=True,
        )
        self.progress_bar2 = tqdm(
            initial=self.j,
            total=self.MAX_DATA,
            desc="                                        Low speed: Throttle "
            + str(self.THROTTLE_THRESHOLD1)
            + " - "
            + str(self.THROTTLE_THRESHOLD2)
            + "       ",
            dynamic_ncols=True,
        )
        self.progress_bar3 = tqdm(
            initial=self.h,
            total=self.MAX_DATA,
            desc="                                        Low speed: Throttle > "
            + str(self.THROTTLE_THRESHOLD2)
            + "          ",
            dynamic_ncols=True,
        )
        self.progress_bar4 = tqdm(
            initial=self.d,
            total=self.MAX_DATA,
            desc="                                        High speed: 0 - Throttle deadzone ",
            dynamic_ncols=True,
        )
        self.progress_bar5 = tqdm(
            initial=self.a,
            total=self.MAX_DATA,
            desc="                                        High speed: Throttle deadzone - "
            + str(self.THROTTLE_THRESHOLD2),
            dynamic_ncols=True,
        )
        self.progress_bar6 = tqdm(
            initial=self.b,
            total=self.MAX_DATA,
            desc="                                        High speed: Throttle "
            + str(self.THROTTLE_THRESHOLD1)
            + " - "
            + str(self.THROTTLE_THRESHOLD2)
            + "      ",
            dynamic_ncols=True,
        )
        self.progress_bar7 = tqdm(
            initial=self.c,
            total=self.MAX_DATA,
            desc="                                        High speed: Throttle > "
            + str(self.THROTTLE_THRESHOLD2)
            + "         ",
            dynamic_ncols=True,
        )
        self.progress_bar8 = tqdm(
            initial=self.kk,
            total=self.MAX_DATA,
            desc="                                        Low speed: 0 - Brake deadzone     ",
            dynamic_ncols=True,
        )
        self.progress_bar9 = tqdm(
            initial=self.ii,
            total=self.MAX_DATA,
            desc="                                        Low speed: Brake deadzone - "
            + str(self.BRAKE_THRESHOLD1)
            + "    ",
            dynamic_ncols=True,
        )
        self.progress_bar10 = tqdm(
            initial=self.jj,
            total=self.MAX_DATA,
            desc="                                        Low speed: Brake "
            + str(self.BRAKE_THRESHOLD1)
            + " - "
            + str(self.BRAKE_THRESHOLD2)
            + "          ",
            dynamic_ncols=True,
        )
        self.progress_bar11 = tqdm(
            initial=self.hh,
            total=self.MAX_DATA,
            desc="                                        Low speed: Brake > "
            + str(self.BRAKE_THRESHOLD2)
            + "             ",
            dynamic_ncols=True,
        )
        self.progress_bar12 = tqdm(
            initial=self.dd,
            total=self.MAX_DATA,
            desc="                                        High speed: 0 - Brake deadzone    ",
            dynamic_ncols=True,
        )
        self.progress_bar13 = tqdm(
            initial=self.aa,
            total=self.MAX_DATA,
            desc="                                        High speed: Brake deadzone - "
            + str(self.BRAKE_THRESHOLD1)
            + "   ",
            dynamic_ncols=True,
        )
        self.progress_bar14 = tqdm(
            initial=self.bb,
            total=self.MAX_DATA,
            desc="                                        High speed: Brake "
            + str(self.BRAKE_THRESHOLD1)
            + " - "
            + str(self.BRAKE_THRESHOLD2)
            + "         ",
            dynamic_ncols=True,
        )
        self.progress_bar15 = tqdm(
            initial=self.cc,
            total=self.MAX_DATA,
            desc="                                        High speed: Brake > "
            + str(self.BRAKE_THRESHOLD2)
            + "            ",
            dynamic_ncols=True,
        )

        self.create_subscription(
            Float32, self.pitch_topic, self.pitch_topic_callback, 1
        )
        self.create_subscription(
            ActuationStatusStamped,
            self.actuation_status_topic,
            self.actuation_topic_callback,
            1,
        )
        self.create_subscription(
            SteeringReport, self.steering_status_topic, self.steer_topic_callback, 1
        )
        self.create_subscription(
            VelocityReport, self.velocity_status_topic, self.velocity_topic_callback, 1
        )
        self.create_subscription(Imu, self.imu_topic, self.imu_topic_callback, 1)
        self.progress_throttle = self.create_publisher(
            LongitudinalProcesses, "/scenarios_collection_longitudinal_throttling", 10
        )
        self.progress_brake = self.create_publisher(
            LongitudinalProcesses, "/scenarios_collection_longitudinal_braking", 10
        )

        self.timer = self.create_timer(0.02, self.test_callback)
        self.timer1 = self.create_timer(0.5, self.throttle_message_callback)
        self.timer2 = self.create_timer(0.5, self.brake_message_callback)

        self.queue_velocity = deque()
        self.queue_acceleration = deque()
        self.queue_acceleration_mov_avg = deque()
        self.queue_pitch_angle = deque()
        self.queue_pitch_angle_mov_avg = deque()
        self.queue_throttle = deque()
        self.queue_braking = deque()

    def pitch_topic_callback(self, msg):

        self.pitch_angle = float(msg.data)
        # apply a mean filter
        if len(self.queue_pitch_angle) < self.NUM_OF_QUEUE:
            self.queue_pitch_angle.append(self.pitch_angle)
        else:
            self.queue_pitch_angle.popleft()

    def velocity_topic_callback(self, msg):
        self.velocity = float(msg.longitudinal_velocity)
        if len(self.queue_velocity) < self.NUM_OF_QUEUE:
            self.queue_velocity.append(self.velocity)
        else:
            self.queue_velocity.popleft()

    def actuation_topic_callback(self, msg):

        self.braking = float(msg.status.brake_status) * 100.0
        if len(self.queue_braking) < self.NUM_OF_QUEUE:
            self.queue_braking.append(self.braking)
        else:
            self.queue_braking.popleft()
        self.throttling = float(msg.status.accel_status) * 100.0
        if len(self.queue_throttle) < self.NUM_OF_QUEUE:
            self.queue_throttle.append(self.throttling)
        else:
            self.queue_throttle.popleft()

    def steer_topic_callback(self, msg):

        self.steering = float(msg.steering_tire_angle)

    def imu_topic_callback(self, msg):

        self.acceleration = float(msg.linear_acceleration.x)
        if len(self.queue_acceleration) < self.NUM_OF_QUEUE:
            self.queue_acceleration.append(self.acceleration)
        else:
            self.queue_acceleration.popleft()

    def collection_throttling(self):

        self.vel.append(abs(mean(self.queue_velocity)))
        self.cmd.append(mean(self.queue_throttle))
        if mean(self.queue_velocity) < 0:
            self.acc.append(
                -1 * mean(self.queue_acceleration)
                - self.g * math.sin(math.radians(-1 * mean(self.queue_pitch_angle)))
            )
            self.acc2.append(-1 * mean(self.queue_acceleration))
            self.pitch.append(-1 * mean(self.queue_pitch_angle))
        else:
            self.acc.append(
                mean(self.queue_acceleration)
                - self.g * math.sin(math.radians(mean(self.queue_pitch_angle)))
            )
            self.acc2.append(mean(self.queue_acceleration))
            self.pitch.append(mean(self.queue_pitch_angle))

        # save data in csv file
        dict1 = {
            "Velocity": self.vel,
            "Throttling": self.cmd,
            "Acceleration_with_pitch_comp": self.acc,
            "Acceleration_measured": self.acc2,
            "Pitch_angle": self.pitch,
            "Low_V_0_deadzone": self.k,
            "Low_V_deadzone_thr1": self.i,
            "Low_V_thr1_thr2": self.j,
            "Low_V_thr2_max": self.h,
            "High_V_0_deadzone": self.d,
            "High_V_deadzone_thr1": self.a,
            "High_V_thr1_thr2": self.b,
            "High_V_thr2_max": self.c,
        }
        df1 = pd.DataFrame(dict1)
        df1.to_csv("throttling.csv")

        self.throttling_prec = mean(self.queue_throttle)

    def collection_braking(self):

        self.velb.append(abs(mean(self.queue_velocity)))
        self.cmdb.append(mean(self.queue_braking))
        if mean(self.queue_velocity) < 0:
            self.accb.append(
                -1 * mean(self.queue_acceleration)
                - self.g * math.sin(math.radians(mean(self.queue_pitch_angle)))
            )
            self.accb2.append(-1 * mean(self.queue_acceleration))
            self.pitch2.append(-1 * mean(self.queue_pitch_angle))
        else:
            self.accb.append(
                mean(self.queue_acceleration)
                - self.g * math.sin(math.radians(mean(self.queue_pitch_angle)))
            )
            self.accb2.append(mean(self.queue_acceleration))
            self.pitch2.append(mean(self.queue_pitch_angle))

        dict2 = {
            "Velocity": self.velb,
            "Braking": self.cmdb,
            "Acceleration_with_pitch_comp": self.accb,
            "Acceleration_measured": self.accb2,
            "Pitch_angle": self.pitch2,
            "Low_V_0_deadzone": self.kk,
            "Low_V_deadzone_thr1": self.ii,
            "Low_V_thr1_thr2": self.jj,
            "Low_V_thr2_max": self.hh,
            "High_V_0_deadzone": self.dd,
            "High_V_deadzone_thr1": self.aa,
            "High_V_thr1_thr2": self.bb,
            "High_V_thr2_max": self.cc,
        }
        df2 = pd.DataFrame(dict2)
        df2.to_csv("braking.csv")

        self.braking_prec = mean(self.queue_braking)

    def throttle_message_publish(self, count: int, index: int):

        self.long_progress_throttle_msg[index].data_count = count
        self.long_progress_throttle_msg[index].progress = int(
            count * 100 / self.MAX_DATA
        )
        self.long_processes_throttle_msg.header.stamp = self.get_clock().now().to_msg()
        self.long_processes_throttle_msg.processes[index] = (
            self.long_progress_throttle_msg[index]
        )

    def brake_message_publish(self, count: int, index: int):

        self.long_progress_brake_msg[index].data_count = count
        self.long_progress_brake_msg[index].progress = int(count * 100 / self.MAX_DATA)
        self.long_processes_brake_msg.header.stamp = self.get_clock().now().to_msg()
        self.long_processes_brake_msg.processes[index] = self.long_progress_brake_msg[
            index
        ]

    def test_callback(self):

        if len(self.queue_throttle) >= self.NUM_OF_QUEUE and (
            len(self.queue_braking) >= self.NUM_OF_QUEUE
        ):

            # THROTTLING SCENARIO to train throttling model
            if (
                self.braking == 0
                and abs(self.steering) < self.STEERING_THRESHOLD
                and abs(self.throttling_prec - mean(self.queue_throttle))
                <= self.CONSISTENCY_THRESHOLD
            ):

                # low velocity scenario

                if 0 < abs(self.velocity) <= self.SPEED_THRESHOLD:

                    if (
                        0 <= self.throttling <= self.THROTTLE_DEADZONE
                        and self.k < self.MAX_DATA
                        and self.flag == 0
                    ):

                        self.collection_throttling()
                        self.progress_bar0.update(1)
                        self.k += 1

                        self.throttle_message_publish(self.k, 0)

                    elif (
                        self.THROTTLE_DEADZONE
                        < self.throttling
                        <= self.THROTTLE_THRESHOLD1
                        and self.i < self.MAX_DATA
                    ):

                        self.collection_throttling()
                        self.progress_bar1.update(1)
                        self.flag = 0
                        self.i += 1

                        self.throttle_message_publish(self.i, 1)

                    elif (
                        self.THROTTLE_THRESHOLD1
                        < self.throttling
                        <= self.THROTTLE_THRESHOLD2
                        and self.j < self.MAX_DATA
                    ):

                        self.collection_throttling()
                        self.progress_bar2.update(1)
                        self.flag = 0
                        self.j += 1

                        self.throttle_message_publish(self.j, 2)

                    elif (
                        self.throttling > self.THROTTLE_THRESHOLD2
                        and self.h < self.MAX_DATA
                    ):

                        self.collection_throttling()
                        self.progress_bar3.update(1)
                        self.flag = 0
                        self.h += 1

                        self.throttle_message_publish(self.h, 3)

                # high velocity scenario

                elif self.SPEED_THRESHOLD < abs(self.velocity) <= self.MAX_VELOCITY:

                    if (
                        0 <= self.throttling <= self.THROTTLE_DEADZONE
                        and self.d < self.MAX_DATA
                        and self.flag == 0
                    ):

                        self.collection_throttling()
                        self.progress_bar4.update(1)
                        self.d += 1

                        self.throttle_message_publish(self.d, 4)

                    elif (
                        self.THROTTLE_DEADZONE
                        < self.throttling
                        <= self.THROTTLE_THRESHOLD1
                        and self.a < self.MAX_DATA
                    ):

                        self.collection_throttling()
                        self.progress_bar5.update(1)
                        self.flag = 0
                        self.a += 1

                        self.throttle_message_publish(self.a, 5)

                    elif (
                        self.THROTTLE_THRESHOLD1
                        < self.throttling
                        <= self.THROTTLE_THRESHOLD2
                        and self.b < self.MAX_DATA
                    ):

                        self.collection_throttling()
                        self.progress_bar6.update(1)
                        self.flag = 0
                        self.b += 1

                        self.throttle_message_publish(self.b, 6)

                    elif (
                        self.throttling > self.THROTTLE_THRESHOLD2
                        and self.c < self.MAX_DATA
                    ):

                        self.collection_throttling()
                        self.progress_bar7.update(1)
                        self.flag = 0
                        self.c += 1

                        self.throttle_message_publish(self.c, 7)

            # BRAKING SCENARIO to train braking model

            if (
                self.throttling == 0
                and abs(self.steering) < self.STEERING_THRESHOLD
                and abs(self.braking_prec - mean(self.queue_braking))
                <= self.CONSISTENCY_THRESHOLD
            ):

                # low velocity scenario

                if 0 < abs(self.velocity) <= self.SPEED_THRESHOLD:

                    if (
                        0 <= self.braking <= self.BRAKE_DEADZONE
                        and self.kk < self.MAX_DATA
                        and self.flag == 1
                    ):

                        self.collection_braking()
                        self.progress_bar8.update(1)
                        self.kk += 1

                        self.brake_message_publish(self.kk, 0)

                    elif (
                        self.BRAKE_DEADZONE < self.braking <= self.BRAKE_THRESHOLD1
                        and self.ii < self.MAX_DATA
                    ):

                        self.collection_braking()
                        self.progress_bar9.update(1)
                        self.flag = 1
                        self.ii += 1

                        self.brake_message_publish(self.ii, 1)

                    elif (
                        self.BRAKE_THRESHOLD1 < self.braking <= self.BRAKE_THRESHOLD2
                        and self.jj < self.MAX_DATA
                    ):

                        self.collection_braking()
                        self.progress_bar10.update(1)
                        self.flag = 1
                        self.jj += 1

                        self.brake_message_publish(self.jj, 2)

                    elif (
                        self.braking > self.BRAKE_THRESHOLD2 and self.hh < self.MAX_DATA
                    ):

                        self.collection_braking()
                        self.progress_bar11.update(1)
                        self.flag = 1
                        self.hh += 1

                        self.brake_message_publish(self.hh, 3)

                # high velocity scenario

                elif self.SPEED_THRESHOLD < abs(self.velocity) <= self.MAX_VELOCITY:

                    if (
                        0 <= self.braking <= self.BRAKE_DEADZONE
                        and self.dd < self.MAX_DATA
                        and self.flag == 1
                    ):

                        self.collection_braking()
                        self.progress_bar12.update(1)
                        self.dd += 1

                        self.brake_message_publish(self.dd, 4)

                    elif (
                        self.BRAKE_DEADZONE < self.braking <= self.BRAKE_THRESHOLD1
                        and self.aa < self.MAX_DATA
                    ):

                        self.collection_braking()
                        self.progress_bar13.update(1)
                        self.flag = 1
                        self.aa += 1

                        self.brake_message_publish(self.aa, 5)

                    elif (
                        self.BRAKE_THRESHOLD1 < self.braking <= self.BRAKE_THRESHOLD2
                        and self.bb < self.MAX_DATA
                    ):

                        self.collection_braking()
                        self.progress_bar14.update(1)
                        self.flag = 1
                        self.bb += 1

                        self.brake_message_publish(self.bb, 6)

                    elif (
                        self.braking > self.BRAKE_THRESHOLD2 and self.cc < self.MAX_DATA
                    ):

                        self.collection_braking()
                        self.progress_bar15.update(1)
                        self.flag = 1
                        self.cc += 1

                        self.brake_message_publish(self.cc, 7)

    def throttle_message_callback(self):
        self.progress_throttle.publish(self.long_processes_throttle_msg)

    def brake_message_callback(self):
        self.progress_brake.publish(self.long_processes_brake_msg)


def main():
    rclpy.init()
    node = primotest()
    rclpy.spin(node)


if __name__ == "__main__":
    main()
