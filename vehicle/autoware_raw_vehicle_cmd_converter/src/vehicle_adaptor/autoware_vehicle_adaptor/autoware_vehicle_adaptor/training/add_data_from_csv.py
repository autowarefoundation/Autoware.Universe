import csv
import glob
import json
import os
from pathlib import Path

from autoware_vehicle_adaptor.calibrator import get_acc_input_from_csv_via_map
from autoware_vehicle_adaptor.param import parameters
from autoware_vehicle_adaptor.src import vehicle_adaptor_compensator
import numpy as np
import scipy.interpolate
from scipy.ndimage import gaussian_filter
from scipy.spatial.transform import Rotation

wheel_base = parameters.wheel_base
acc_queue_size = parameters.acc_queue_size
steer_queue_size = parameters.steer_queue_size
acc_delay_step = parameters.acc_delay_step
acc_time_delay = parameters.acc_time_delay
steer_delay_step = parameters.steer_delay_step
steer_time_delay = parameters.steer_time_delay
x_index = 0
y_index = 1
vel_index = 2
yaw_index = 3
acc_index = 4
steer_index = 5
acc_input_indices = np.arange(6, 6 + acc_queue_size)
steer_input_indices = np.arange(6 + acc_queue_size, 6 + acc_queue_size + steer_queue_size)

control_dt = parameters.control_dt
predict_step = 3
predict_dt = predict_step * control_dt

acc_time_constant = parameters.acc_time_constant
steer_time_constant = parameters.steer_time_constant

x_error_sigma_for_training = 30.0
y_error_sigma_for_training = 30.0
v_error_sigma_for_training = 10.0
theta_error_sigma_for_training = 10.0
acc_error_sigma_for_training = 10.0
steer_error_sigma_for_training = 5.0


def data_smoothing(data: np.ndarray, sigma: float) -> np.ndarray:
    """Apply a Gaussian filter to the data."""
    data_ = gaussian_filter(data, sigma)
    return data_


def yaw_transform(raw_yaw: np.ndarray) -> np.ndarray:
    """Adjust and transform within a period of 2π so that the yaw angle is continuous."""
    transformed_yaw = np.zeros(raw_yaw.shape)
    transformed_yaw[0] = raw_yaw[0]
    for i in range(raw_yaw.shape[0] - 1):
        rotate_num = (raw_yaw[i + 1] - transformed_yaw[i]) // (2 * np.pi)
        if raw_yaw[i + 1] - transformed_yaw[i] - 2 * rotate_num * np.pi < np.pi:
            transformed_yaw[i + 1] = raw_yaw[i + 1] - 2 * rotate_num * np.pi
        else:
            transformed_yaw[i + 1] = raw_yaw[i + 1] - 2 * (rotate_num + 1) * np.pi
    return transformed_yaw


class add_data_from_csv:
    def __init__(self):
        self.X_train_list = []
        self.Y_train_list = []
        self.X_val_list = []
        self.Y_val_list = []
        self.X_test_list = []
        self.Y_test_list = []
        self.X_replay_list = []
        self.Y_replay_list = []
        self.division_indices_train = []
        self.division_indices_val = []
        self.division_indices_test = []
        self.division_indices_replay = []
        self.nominal_dynamics = vehicle_adaptor_compensator.NominalDynamics()
        self.nominal_dynamics.set_params(
            wheel_base,
            acc_time_delay,
            steer_time_delay,
            acc_time_constant,
            steer_time_constant,
            acc_queue_size,
            steer_queue_size,
            control_dt,
            predict_step,
        )

    def clear_data(self):
        self.X_train_list = []
        self.Y_train_list = []
        self.X_val_list = []
        self.Y_val_list = []
        self.X_test_list = []
        self.Y_test_list = []
        self.X_replay_list = []
        self.Y_replay_list = []
        self.division_indices_train = []
        self.division_indices_val = []
        self.division_indices_test = []
        self.division_indices_replay = []

    def add_data_from_csv(
        self,
        dir_name: str,
        add_mode="divide",
        map_dir=None,
        control_cmd_mode=None,
        reverse_steer=False,
    ) -> None:
        localization_kinematic_state = np.loadtxt(
            dir_name + "/localization_kinematic_state.csv",
            delimiter=",",
            usecols=[0, 1, 4, 5, 7, 8, 9, 10, 47],
        )
        pose_position_x = localization_kinematic_state[:, 2]
        pose_position_y = localization_kinematic_state[:, 3]
        vel = localization_kinematic_state[:, 8]
        raw_yaw = Rotation.from_quat(localization_kinematic_state[:, 4:8]).as_euler("xyz")[:, 2]
        yaw = yaw_transform(raw_yaw)

        localization_acceleration = np.loadtxt(
            dir_name + "/localization_acceleration.csv", delimiter=",", usecols=[0, 1, 3]
        )
        acc = localization_acceleration[:, 2]

        vehicle_status_steering_status = np.loadtxt(
            dir_name + "/vehicle_status_steering_status.csv", delimiter=",", usecols=[0, 1, 2]
        )
        steer = vehicle_status_steering_status[:, 2]
        if control_cmd_mode == "compensated_control_cmd":
            control_cmd = np.loadtxt(
                dir_name + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        elif control_cmd_mode == "control_command":
            control_cmd = np.loadtxt(
                dir_name + "/control_command_control_cmd.csv", delimiter=",", usecols=[0, 1, 8, 16]
            )
        elif control_cmd_mode == "control_trajectory_follower":
            control_cmd = np.loadtxt(
                dir_name + "/control_trajectory_follower_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        elif control_cmd_mode == "external_selected":
            control_cmd = np.loadtxt(
                dir_name + "/external_selected_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        elif control_cmd_mode is None:
            if os.path.exists(
                dir_name + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv"
            ):
                control_cmd = np.loadtxt(
                    dir_name
                    + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            elif os.path.exists(dir_name + "/control_command_control_cmd.csv"):
                control_cmd = np.loadtxt(
                    dir_name + "/control_command_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            elif os.path.exists(dir_name + "/control_trajectory_follower_control_cmd.csv"):
                control_cmd = np.loadtxt(
                    dir_name + "/control_trajectory_follower_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            elif os.path.exists(dir_name + "/external_selected_control_cmd.csv"):
                control_cmd = np.loadtxt(
                    dir_name + "/external_selected_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            else:
                print("control command csv is not found")
                return
        else:
            print("control_cmd_mode is invalid")
            return
        acc_cmd = control_cmd[:, [0, 1, 3]]
        steer_cmd = control_cmd[:, 2]
        if map_dir is not None:
            acc_cmd = get_acc_input_from_csv_via_map.transform_accel_and_brake_to_acc_via_map(
                csv_dir=dir_name, map_dir=map_dir
            )
        system_operation_mode_state = np.loadtxt(
            dir_name + "/system_operation_mode_state.csv", delimiter=",", usecols=[0, 1, 2]
        )
        if system_operation_mode_state.ndim == 1:
            system_operation_mode_state = system_operation_mode_state.reshape(1, -1)
        with open(dir_name + "/system_operation_mode_state.csv") as f:
            reader = csv.reader(f, delimiter=",")
            autoware_control_enabled_str = np.array([row[3] for row in reader])

        control_enabled = np.zeros(system_operation_mode_state.shape[0])
        for i in range(system_operation_mode_state.shape[0]):
            if (
                system_operation_mode_state[i, 2] > 1.5
                and autoware_control_enabled_str[i] == "True"
            ):
                control_enabled[i] = 1.0
        for i in range(system_operation_mode_state.shape[0] - 1):
            if control_enabled[i] < 0.5 and control_enabled[i + 1] > 0.5:
                operation_start_time = (
                    system_operation_mode_state[i + 1, 0]
                    + 1e-9 * system_operation_mode_state[i + 1, 1]
                )
            elif control_enabled[i] > 0.5 and control_enabled[i + 1] < 0.5:
                operation_end_time = (
                    system_operation_mode_state[i + 1, 0]
                    + 1e-9 * system_operation_mode_state[i + 1, 1]
                )
                break
            operation_end_time = (
                localization_kinematic_state[-1, 0] + 1e-9 * localization_kinematic_state[-1, 1]
            )
        if system_operation_mode_state.shape[0] == 1:
            operation_end_time = (
                localization_kinematic_state[-1, 0] + 1e-9 * localization_kinematic_state[-1, 1]
            )
        if control_enabled[0] > 0.5:
            operation_start_time = (
                system_operation_mode_state[0, 0] + 1e-9 * system_operation_mode_state[0, 1]
            )
        print("operation_start_time", operation_start_time)
        print("operation_end_time", operation_end_time)

        min_time_stamp = max(
            [
                operation_start_time,
                localization_kinematic_state[0, 0] + 1e-9 * localization_kinematic_state[0, 1],
                localization_acceleration[0, 0] + 1e-9 * localization_acceleration[0, 1],
                vehicle_status_steering_status[0, 0] + 1e-9 * vehicle_status_steering_status[0, 1],
                acc_cmd[0, 0] + 1e-9 * acc_cmd[0, 1],
                control_cmd[0, 0] + 1e-9 * control_cmd[0, 1],
            ]
        )
        max_time_stamp = min(
            [
                operation_end_time,
                localization_kinematic_state[-1, 0] + 1e-9 * localization_kinematic_state[-1, 1],
                localization_acceleration[-1, 0] + 1e-9 * localization_acceleration[-1, 1],
                vehicle_status_steering_status[-1, 0]
                + 1e-9 * vehicle_status_steering_status[-1, 1],
                acc_cmd[-1, 0] + 1e-9 * acc_cmd[-1, 1],
                control_cmd[-1, 0] + 1e-9 * control_cmd[-1, 1],
            ]
        )

        trajectory_interpolator_list = []
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(
                localization_kinematic_state[:, 0] + 1e-9 * localization_kinematic_state[:, 1],
                pose_position_x,
            )
        )
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(
                localization_kinematic_state[:, 0] + 1e-9 * localization_kinematic_state[:, 1],
                pose_position_y,
            )
        )
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(
                localization_kinematic_state[:, 0] + 1e-9 * localization_kinematic_state[:, 1], vel
            )
        )
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(
                localization_kinematic_state[:, 0] + 1e-9 * localization_kinematic_state[:, 1], yaw
            )
        )
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(
                localization_acceleration[:, 0] + 1e-9 * localization_acceleration[:, 1], acc
            )
        )
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(
                vehicle_status_steering_status[:, 0] + 1e-9 * vehicle_status_steering_status[:, 1],
                steer,
            )
        )
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(acc_cmd[:, 0] + 1e-9 * acc_cmd[:, 1], acc_cmd[:, 2])
        )
        trajectory_interpolator_list.append(
            scipy.interpolate.interp1d(control_cmd[:, 0] + 1e-9 * control_cmd[:, 1], steer_cmd)
        )

        def get_interpolated_state(s):
            state = np.zeros(6)
            for i in range(6):
                state[i] = trajectory_interpolator_list[i](s)
            return state

        def get_interpolated_input(s):
            input = np.zeros(2)
            for i in range(2):
                input[i] = trajectory_interpolator_list[6 + i](s)
            return input

        s = min_time_stamp
        States = []
        Inputs = []

        TimeStamps = []

        while True:
            if s > max_time_stamp:
                break
            States.append(get_interpolated_state(s))

            Inputs.append(get_interpolated_input(s))

            TimeStamps.append(s)
            s += control_dt
        States = np.array(States)
        Inputs = np.array(Inputs)
        X_list = []
        Y_list = []
        TimeStamp_list = []

        for i in range(max(acc_queue_size, steer_queue_size), States.shape[0] - predict_step - 1):
            acc_input_queue = Inputs[i - acc_queue_size : i + predict_step, 0]
            steer_input_queue = Inputs[i - steer_queue_size : i + predict_step, 1]
            state = States[i]
            state_obs = States[i + predict_step]
            TimeStamp_list.append(TimeStamps[i])
            reverse_state = state.copy()
            reverse_state[yaw_index] = -reverse_state[yaw_index]
            reverse_state[steer_index] = -reverse_state[steer_index]
            if not reverse_steer:
                X_list.append(
                    np.concatenate(
                        (
                            state[[vel_index, acc_index, steer_index]],
                            acc_input_queue,
                            steer_input_queue,
                        )
                    )
                )
            else:
                X_list.append(
                    np.concatenate(
                        (
                            reverse_state[[vel_index, acc_index, steer_index]],
                            acc_input_queue,
                            -steer_input_queue,
                        )
                    )
                )

            u_for_predict_nom = np.zeros((predict_step, 2))
            u_for_predict_nom[:, 0] = acc_input_queue[
                acc_input_queue.shape[0]
                - acc_delay_step
                - predict_step : acc_input_queue.shape[0]
                - acc_delay_step
            ]
            u_for_predict_nom[:, 1] = steer_input_queue[
                steer_input_queue.shape[0]
                - steer_delay_step
                - predict_step : steer_input_queue.shape[0]
                - steer_delay_step
            ]
            predicted_state = self.nominal_dynamics.F_nominal_predict(
                state, u_for_predict_nom.flatten()
            )
            pseudo_predicted_state = state.copy()
            for j in range(predict_step):
                pseudo_predicted_state = self.nominal_dynamics.F_nominal(
                    pseudo_predicted_state, u_for_predict_nom[j]
                )
                pseudo_predicted_state[acc_index] = States[i + j + 1, acc_index]
                pseudo_predicted_state[steer_index] = States[i + j + 1, steer_index]
            predict_error = state_obs - predicted_state
            pseudo_predict_error = pseudo_predicted_state - predicted_state
            if not parameters.use_position_observation:
                predict_error[x_index] = pseudo_predict_error[x_index]
                predict_error[y_index] = pseudo_predict_error[y_index]
            if not parameters.use_vel_observation:
                predict_error[vel_index] = pseudo_predict_error[vel_index]
            if not parameters.use_yaw_observation:
                predict_error[yaw_index] = pseudo_predict_error[yaw_index]
            predict_error = vehicle_adaptor_compensator.rotate_data(predict_error, state[yaw_index])
            if reverse_steer:
                predict_error[y_index] = -predict_error[y_index]
                predict_error[yaw_index] = -predict_error[yaw_index]
                predict_error[steer_index] = -predict_error[steer_index]
            Y_list.append(predict_error / predict_dt)

        Y_smooth = np.array(Y_list)
        Y_smooth[:, x_index] = data_smoothing(Y_smooth[:, x_index], x_error_sigma_for_training)
        Y_smooth[:, y_index] = data_smoothing(Y_smooth[:, y_index], y_error_sigma_for_training)
        Y_smooth[:, vel_index] = data_smoothing(Y_smooth[:, vel_index], v_error_sigma_for_training)
        Y_smooth[:, yaw_index] = data_smoothing(
            Y_smooth[:, yaw_index], theta_error_sigma_for_training
        )
        Y_smooth[:, acc_index] = data_smoothing(
            Y_smooth[:, acc_index], acc_error_sigma_for_training
        )
        Y_smooth[:, steer_index] = data_smoothing(
            Y_smooth[:, steer_index], steer_error_sigma_for_training
        )

        if add_mode == "divide":
            for i in range(len(X_list)):
                if i < 3 * len(X_list) / 4:
                    self.X_train_list.append(X_list[i])
                    self.Y_train_list.append(Y_smooth[i])
                    # self.Y_train_list.append(Y_list[i])
                else:
                    self.X_val_list.append(X_list[i])
                    self.Y_val_list.append(Y_smooth[i])
                    # self.Y_val_list.append(Y_list[i])

            self.division_indices_train.append(len(self.X_train_list))
            self.division_indices_val.append(len(self.X_val_list))

        elif add_mode == "as_train":
            for i in range(len(X_list)):
                self.X_train_list.append(X_list[i])
                self.Y_train_list.append(Y_smooth[i])
                # self.Y_train_list.append(Y_list[i])

            self.division_indices_train.append(len(self.X_train_list))
        elif add_mode == "as_val":
            for i in range(len(X_list)):
                self.X_val_list.append(X_list[i])
                self.Y_val_list.append(Y_smooth[i])
                # self.Y_val_list.append(Y_list[i])

            self.division_indices_val.append(len(self.X_val_list))
        elif add_mode == "as_test":
            for i in range(len(X_list)):
                self.X_test_list.append(X_list[i])
                self.Y_test_list.append(Y_smooth[i])
                # self.Y_test_list.append(Y_list[i])

            self.division_indices_test.append(len(self.X_test_list))
        elif add_mode == "as_replay":
            for i in range(len(X_list)):
                self.X_replay_list.append(X_list[i])
                self.Y_replay_list.append(Y_smooth[i])
                # self.Y_replay_list.append(Y_list[i])

            self.division_indices_replay.append(len(self.X_replay_list))
