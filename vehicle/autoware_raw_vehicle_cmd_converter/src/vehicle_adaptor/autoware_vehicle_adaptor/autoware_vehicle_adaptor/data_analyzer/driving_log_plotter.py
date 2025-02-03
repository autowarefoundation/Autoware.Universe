import csv
import os

from autoware_vehicle_adaptor.data_analyzer import rosbag_to_csv
import matplotlib.pyplot as plt
import numpy as np
from scipy.spatial.transform import Rotation


def yaw_transform(raw_yaw):
    transformed_yaw = np.zeros(raw_yaw.shape)
    transformed_yaw[0] = raw_yaw[0]
    for i in range(raw_yaw.shape[0] - 1):
        rotate_num = (raw_yaw[i + 1] - transformed_yaw[i]) // (2 * np.pi)
        if raw_yaw[i + 1] - transformed_yaw[i] - 2 * rotate_num * np.pi < np.pi:
            transformed_yaw[i + 1] = raw_yaw[i + 1] - 2 * rotate_num * np.pi
        else:
            transformed_yaw[i + 1] = raw_yaw[i + 1] - 2 * (rotate_num + 1) * np.pi
    return transformed_yaw


class DrivingLogPlotter:
    def __init__(self):
        pass

    def convert_rosbag_to_csv(self, dir_name):
        rosbag_to_csv.rosbag_to_csv(dir_name)

    def load_csv(self, dir_name, control_cmd_mode="control_command"):
        self.localization_kinematic_state = np.loadtxt(
            dir_name + "/localization_kinematic_state.csv",
            delimiter=",",
            usecols=[0, 1, 4, 5, 7, 8, 9, 10, 47],
        )
        self.pos_x = self.localization_kinematic_state[:, 2]
        self.pos_y = self.localization_kinematic_state[:, 3]
        self.vel = self.localization_kinematic_state[:, 8]
        self.start_time = (
            self.localization_kinematic_state[0, 0] + 1e-9 * self.localization_kinematic_state[0, 1]
        )
        self.total_time = (
            self.localization_kinematic_state[-1, 0]
            + 1e-9 * self.localization_kinematic_state[-1, 1]
            - self.start_time
        )
        raw_yaw = Rotation.from_quat(self.localization_kinematic_state[:, 4:8]).as_euler("xyz")[
            :, 2
        ]
        self.yaw = yaw_transform(raw_yaw)
        self.localization_acceleration = np.loadtxt(
            dir_name + "/localization_acceleration.csv", delimiter=",", usecols=[0, 1, 3]
        )
        self.acc = self.localization_acceleration[:, 2]
        self.vehicle_status_steering_status = np.loadtxt(
            dir_name + "/vehicle_status_steering_status.csv", delimiter=",", usecols=[0, 1, 2]
        )
        self.steer = self.vehicle_status_steering_status[:, 2]
        self.control_cmd_mode = control_cmd_mode
        if control_cmd_mode == "control_command":
            self.control_cmd = np.loadtxt(
                dir_name + "/control_command_control_cmd.csv", delimiter=",", usecols=[0, 1, 8, 16]
            )
        elif control_cmd_mode == "control_trajectory_follower":
            self.control_cmd = np.loadtxt(
                dir_name + "/control_trajectory_follower_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        elif control_cmd_mode == "external_selected":
            self.control_cmd = np.loadtxt(
                dir_name + "/external_selected_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        else:
            print("control_cmd_mode is invalid")
            exit()
        self.acc_cmd = self.control_cmd[:, 3]
        self.steer_cmd = self.control_cmd[:, 2]
        if os.path.exists(
            dir_name + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv"
        ):
            self.compensated_control_cmd = np.loadtxt(
                dir_name + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
            self.compensated_acc_cmd = self.compensated_control_cmd[:, 3]
            self.compensated_steer_cmd = self.compensated_control_cmd[:, 2]
        else:
            self.compensated_control_cmd = None
        if os.path.exists(dir_name + "/control_command_actuation_cmd.csv"):
            self.control_command_actuation_cmd = np.loadtxt(
                dir_name + "/control_command_actuation_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 3, 4, 5],
            )
            self.accel_cmd = self.control_command_actuation_cmd[:, 2]
            self.brake_cmd = self.control_command_actuation_cmd[:, 3]
        else:
            self.control_command_actuation_cmd = None
        if os.path.exists(
            dir_name
            + "/control_trajectory_follower_lane_departure_checker_node_debug_deviation_lateral.csv"
        ):
            self.lateral_deviation = np.loadtxt(
                dir_name
                + "/control_trajectory_follower_lane_departure_checker_node_debug_deviation_lateral.csv",
                delimiter=",",
            )
        else:
            self.lateral_deviation = None
        if os.path.exists(dir_name + "/debug_mpc_v_des.csv"):
            self.v_des_stamped = np.loadtxt(
                dir_name + "/debug_mpc_v_des.csv", delimiter=",", usecols=[0, 1, 3]
            )
            self.v_des = self.v_des_stamped[:, 2]
        elif os.path.exists(dir_name + "/data_collecting_target_velocity.csv"):
            self.v_des_stamped = np.loadtxt(
                dir_name + "/data_collecting_target_velocity.csv", delimiter=",", usecols=[0, 1, 2]
            )
            self.v_des = self.v_des_stamped[:, 2]
        else:
            self.v_des_stamped = None

        self.system_operation_mode_state = np.loadtxt(
            dir_name + "/system_operation_mode_state.csv", delimiter=",", usecols=[0, 1, 2]
        )
        if self.system_operation_mode_state.ndim == 1:
            self.system_operation_mode_state = self.system_operation_mode_state.reshape(1, -1)
        with open(dir_name + "/system_operation_mode_state.csv") as f:
            reader = csv.reader(f, delimiter=",")
            autoware_control_enabled_str = np.array([row[3] for row in reader])

        self.control_enabled = np.zeros(self.system_operation_mode_state.shape[0])
        for i in range(self.system_operation_mode_state.shape[0]):
            if (
                self.system_operation_mode_state[i, 2] > 1.5
                and autoware_control_enabled_str[i] == "True"
            ):
                self.control_enabled[i] = 1.0

    def plot_position(self, skip_time=0.0, plot_time=None):
        if plot_time is None:
            plot_indices = np.where(
                (
                    self.localization_kinematic_state[:, 0]
                    + 1e-9 * self.localization_kinematic_state[:, 1]
                    >= self.start_time + skip_time
                )
                & (
                    self.localization_kinematic_state[:, 0]
                    + 1e-9 * self.localization_kinematic_state[:, 1]
                    <= self.start_time + self.total_time
                )
            )
        else:
            plot_indices = np.where(
                (
                    self.localization_kinematic_state[:, 0]
                    + 1e-9 * self.localization_kinematic_state[:, 1]
                    >= self.start_time + skip_time
                )
                & (
                    self.localization_kinematic_state[:, 0]
                    + 1e-9 * self.localization_kinematic_state[:, 1]
                    <= self.start_time + skip_time + plot_time
                )
            )
        plt.plot(self.pos_x[plot_indices], self.pos_y[plot_indices])
        plt.xlabel("x[m]")
        plt.ylabel("y[m]")
        plt.title("driveing position")
        plt.show()

    def plot_velocity(self, skip_time=0.0, plot_time=None, y_lim=[-0.1, 12.0]):
        plt.plot(
            self.localization_kinematic_state[:, 0]
            + 1e-9 * self.localization_kinematic_state[:, 1],
            self.vel,
            label="vel_obs",
        )
        if self.v_des_stamped is not None:
            plt.plot(
                self.v_des_stamped[:, 0] + 1e-9 * self.v_des_stamped[:, 1],
                self.v_des,
                label="v_des",
            )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])

        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("velocity[m/s]")
        plt.title("velocity")
        plt.legend()
        plt.show()

    def plot_yaw(self, skip_time=0.0, plot_time=None, y_lim=[-np.pi, np.pi]):
        plt.plot(
            self.localization_kinematic_state[:, 0]
            + 1e-9 * self.localization_kinematic_state[:, 1],
            self.yaw,
            label="yaw_obs",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("yaw[rad]")
        plt.title("yaw")
        plt.legend()
        plt.show()

    def plot_acc(self, skip_time=0.0, plot_time=None, y_lim=[-1.0, 1.0]):
        plt.plot(
            self.localization_acceleration[:, 0] + 1e-9 * self.localization_acceleration[:, 1],
            self.acc,
            label="acc_obs",
        )
        plt.plot(
            self.control_cmd[:, 0] + 1e-9 * self.control_cmd[:, 1],
            self.acc_cmd,
            label="acc_" + self.control_cmd_mode,
        )
        if self.compensated_control_cmd is not None:
            plt.plot(
                self.compensated_control_cmd[:, 0] + 1e-9 * self.compensated_control_cmd[:, 1],
                self.compensated_acc_cmd,
                label="acc_compensated",
            )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("acceleration[m/s^2]")
        plt.title("acceleration")
        plt.legend()
        plt.show()

    def plot_accel_brake(self, skip_time=0.0, plot_time=None, y_lim=[-1.0, 1.0]):
        if self.control_command_actuation_cmd is None:
            print("control_command_actuation_cmd is not available")
            return
        plt.plot(
            self.control_command_actuation_cmd[:, 0]
            + 1e-9 * self.control_command_actuation_cmd[:, 1],
            self.accel_cmd,
            label="accel_cmd",
        )
        plt.plot(
            self.control_command_actuation_cmd[:, 0]
            + 1e-9 * self.control_command_actuation_cmd[:, 1],
            -self.brake_cmd,
            label="brake_cmd",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("accel brake")
        plt.title("accel brake")
        plt.legend()
        plt.show()

    def plot_steer(self, skip_time=0.0, plot_time=None, y_lim=[-0.5, 0.5]):
        plt.plot(
            self.vehicle_status_steering_status[:, 0]
            + 1e-9 * self.vehicle_status_steering_status[:, 1],
            self.steer,
            label="steer_obs",
        )
        plt.plot(
            self.control_cmd[:, 0] + 1e-9 * self.control_cmd[:, 1],
            self.steer_cmd,
            label="steer_" + self.control_cmd_mode,
        )
        if self.compensated_control_cmd is not None:
            plt.plot(
                self.compensated_control_cmd[:, 0] + 1e-9 * self.compensated_control_cmd[:, 1],
                self.compensated_steer_cmd,
                label="steer_compensated",
            )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("steer[rad]")
        plt.title("steer")
        plt.legend()
        plt.show()

    def plot_acc_change(self, window=1, skip_time=0.0, plot_time=None, y_lim=[-1.0, 1.0]):
        acc_change = np.zeros(self.acc.shape[0] - window)
        for i in range(self.acc.shape[0] - window):
            acc_change[i] = (self.acc[i + window] - self.acc[i]) / (
                self.localization_acceleration[i + window, 0]
                + 1e-9 * self.localization_acceleration[i + window, 1]
                - self.localization_acceleration[i, 0]
                - 1e-9 * self.localization_acceleration[i, 1]
            )
        plt.plot(
            self.localization_acceleration[window:, 0]
            + 1e-9 * self.localization_acceleration[window:, 1],
            acc_change,
            label="acc_change",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("acceleration_change[m/s^3]")
        plt.title("acceleration_change")
        plt.legend()
        plt.show()

    def plot_steer_change(self, window=1, skip_time=0.0, plot_time=None, y_lim=[-0.5, 0.5]):
        steer_change = np.zeros(self.steer.shape[0] - window)
        for i in range(self.steer.shape[0] - window):
            steer_change[i] = (self.steer[i + window] - self.steer[i]) / (
                self.vehicle_status_steering_status[i + window, 0]
                + 1e-9 * self.vehicle_status_steering_status[i + window, 1]
                - self.vehicle_status_steering_status[i, 0]
                - 1e-9 * self.vehicle_status_steering_status[i, 1]
            )
        plt.plot(
            self.vehicle_status_steering_status[window:, 0]
            + 1e-9 * self.vehicle_status_steering_status[window:, 1],
            steer_change,
            label="steer_change",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("steer_change[rad/s]")
        plt.title("steer_change")
        plt.legend()
        plt.show()

    def plot_acc_cmd_change(self, window=1, skip_time=0.0, plot_time=None, y_lim=[-1.0, 1.0]):
        acc_cmd_change = np.zeros(self.acc_cmd.shape[0] - window)
        for i in range(self.acc_cmd.shape[0] - window):
            acc_cmd_change[i] = (self.acc_cmd[i + window] - self.acc_cmd[i]) / (
                self.control_cmd[i + window, 0]
                + 1e-9 * self.control_cmd[i + window, 1]
                - self.control_cmd[i, 0]
                - 1e-9 * self.control_cmd[i, 1]
            )
        plt.plot(
            self.control_cmd[window:, 0] + 1e-9 * self.control_cmd[window:, 1],
            acc_cmd_change,
            label="acc_cmd_change",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("acceleration_cmd_change[m/s^3]")
        plt.title("acceleration_cmd_change")
        plt.legend()
        plt.show()

    def plot_steer_cmd_change(self, window=1, skip_time=0.0, plot_time=None, y_lim=[-0.5, 0.5]):
        steer_cmd_change = np.zeros(self.steer_cmd.shape[0] - window)
        for i in range(self.steer_cmd.shape[0] - window):
            steer_cmd_change[i] = (self.steer_cmd[i + window] - self.steer_cmd[i]) / (
                self.control_cmd[i + window, 0]
                + 1e-9 * self.control_cmd[i + window, 1]
                - self.control_cmd[i, 0]
                - 1e-9 * self.control_cmd[i, 1]
            )
        plt.plot(
            self.control_cmd[window:, 0] + 1e-9 * self.control_cmd[window:, 1],
            steer_cmd_change,
            label="steer_cmd_change",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("steer_cmd_change[rad/s]")
        plt.title("steer_cmd_change")
        plt.legend()
        plt.show()

    def plot_lateral_deviation(self, skip_time=0.0, plot_time=None, y_lim=[-0.8, 0.8]):
        if self.lateral_deviation is None:
            print("lateral_deviation is not available")
            return
        plt.plot(
            self.lateral_deviation[:, 0] + 1e-9 * self.lateral_deviation[:, 1],
            self.lateral_deviation[:, 2],
            label="lateral_deviation",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("lateral_deviation[m]")
        plt.title("lateral_deviation")
        plt.legend()
        plt.show()

    def plot_control_enable(self, skip_time=0.0, plot_time=None, y_lim=[-0.1, 1.1]):
        plt.plot(
            self.system_operation_mode_state[:, 0] + 1e-9 * self.system_operation_mode_state[:, 1],
            self.control_enabled,
            label="control_enable",
        )
        if plot_time is None:
            plt.xlim([self.start_time + skip_time, self.start_time + self.total_time])
        else:
            plt.xlim([self.start_time + skip_time, self.start_time + skip_time + plot_time])
        plt.ylim(y_lim)
        plt.xlabel("time[s]")
        plt.ylabel("control_enable")
        plt.title("control_enable")
        plt.legend()
        plt.show()
