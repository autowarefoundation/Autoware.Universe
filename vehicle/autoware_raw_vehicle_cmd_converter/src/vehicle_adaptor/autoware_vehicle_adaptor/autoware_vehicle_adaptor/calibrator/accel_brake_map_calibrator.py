import csv
from itertools import islice
import json
import os
from pathlib import Path

from autoware_vehicle_adaptor.calibrator import actuation_map_csv_writer
from autoware_vehicle_adaptor.calibrator import collected_data_counter
from autoware_vehicle_adaptor.src import actuation_map_2d
from autoware_vehicle_adaptor.training.early_stopping import EarlyStopping
from matplotlib.colors import TwoSlopeNorm
import matplotlib.pyplot as plt
import numpy as np
import scipy.interpolate
from scipy.spatial import ConvexHull
import seaborn as sns
import torch
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import TensorDataset
from torch.utils.data import WeightedRandomSampler

control_dt = 0.033
default_map_accel = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5]
default_map_brake = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
default_map_vel = [0.0, 1.39, 2.78, 4.17, 5.56, 6.94, 8.33, 9.72, 11.11, 12.5, 13.89]


class AddDataFromCSV:
    def __init__(self):
        self.accel_data_input = []
        self.accel_data_output = []
        self.brake_data_input = []
        self.brake_data_output = []
        self.accel_data_input_for_NN = []
        self.accel_data_output_for_NN = []
        self.brake_data_input_for_NN = []
        self.brake_data_output_for_NN = []
        self.dataloader_weights_accel = None
        self.dataloader_weights_brake = None
        self.extracted_indices_accel = None
        self.extracted_indices_brake = None
        self.calc_base_map_error_performed = False
        self.collected_data_counter = collected_data_counter.CollectedDataCounter()

    def clear_data(self):
        self.accel_data_input = []
        self.accel_data_output = []
        self.brake_data_input = []
        self.brake_data_output = []
        self.accel_data_output_residual = []
        self.brake_data_output_residual = []

    def initialize_for_calibration(self):
        self.accel_data_input_for_NN = self.accel_data_input.copy()
        self.accel_data_output_for_NN = self.accel_data_output.copy()
        self.brake_data_input_for_NN = self.brake_data_input.copy()
        self.brake_data_output_for_NN = self.brake_data_output.copy()
        self.dataloader_weights_accel = None
        self.dataloader_weights_brake = None
        self.extracted_indices_accel = None
        self.extracted_indices_brake = None
        if self.calc_base_map_error_performed:
            self.calc_base_map_error_performed = False
            print("calc_base_map_error is reset")

    def add_data_from_csv(
        self,
        dir_name,
        smoothing_window=10,
        acc_change_threshold=0.2,
        base_map_dir=None,
        control_cmd_mode=None,
    ):
        localization_kinematic_state = np.loadtxt(
            dir_name + "/localization_kinematic_state.csv",
            delimiter=",",
            usecols=[0, 1, 4, 5, 7, 8, 9, 10, 47],
        )
        vel = localization_kinematic_state[:, 8]

        localization_acceleration = np.loadtxt(
            dir_name + "/localization_acceleration.csv", delimiter=",", usecols=[0, 1, 3]
        )
        acc = localization_acceleration[:, 2]
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

        if os.path.exists(dir_name + "/control_command_actuation_cmd.csv"):
            control_command_actuation_cmd = np.loadtxt(
                dir_name + "/control_command_actuation_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 3, 4, 5],
            )
            accel = control_command_actuation_cmd[:, 2]
            brake = control_command_actuation_cmd[:, 3]
            min_time_stamp = max(
                [
                    operation_start_time,
                    localization_kinematic_state[0, 0] + 1e-9 * localization_kinematic_state[0, 1],
                    control_command_actuation_cmd[0, 0]
                    + 1e-9 * control_command_actuation_cmd[0, 1],
                    localization_acceleration[0, 0] + 1e-9 * localization_acceleration[0, 1],
                ]
            )
            max_time_stamp = min(
                [
                    operation_end_time,
                    localization_kinematic_state[-1, 0]
                    + 1e-9 * localization_kinematic_state[-1, 1],
                    control_command_actuation_cmd[-1, 0]
                    + 1e-9 * control_command_actuation_cmd[-1, 1],
                    localization_acceleration[-1, 0] + 1e-9 * localization_acceleration[-1, 1],
                ]
            )
        else:
            if control_cmd_mode == "compensated_control_cmd":
                control_cmd = np.loadtxt(
                    dir_name
                    + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            elif control_cmd_mode == "control_command":
                control_cmd = np.loadtxt(
                    dir_name + "/control_command_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
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
                    dir_name
                    + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv"
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
            min_time_stamp = max(
                [
                    operation_start_time,
                    localization_kinematic_state[0, 0] + 1e-9 * localization_kinematic_state[0, 1],
                    acc_cmd[0, 0] + 1e-9 * acc_cmd[0, 0],
                    localization_acceleration[0, 0] + 1e-9 * localization_acceleration[0, 1],
                ]
            )
            max_time_stamp = min(
                [
                    operation_end_time,
                    localization_kinematic_state[-1, 0]
                    + 1e-9 * localization_kinematic_state[-1, 1],
                    acc_cmd[-1, 0] + 1e-9 * acc_cmd[-1, 0],
                    localization_acceleration[-1, 0] + 1e-9 * localization_acceleration[-1, 1],
                ]
            )
        data_num = int((max_time_stamp - min_time_stamp) / control_dt)
        data_time_stamps = min_time_stamp + control_dt * np.arange(data_num)
        vel_interp = scipy.interpolate.interp1d(
            localization_kinematic_state[:, 0] + 1e-9 * localization_kinematic_state[:, 1], vel
        )(data_time_stamps)
        acc_interp = scipy.interpolate.interp1d(
            localization_acceleration[:, 0] + 1e-9 * localization_acceleration[:, 1], acc
        )(data_time_stamps)
        if os.path.exists(dir_name + "/control_command_actuation_cmd.csv"):
            accel_interp = scipy.interpolate.interp1d(
                control_command_actuation_cmd[:, 0] + 1e-9 * control_command_actuation_cmd[:, 1],
                accel,
            )(data_time_stamps)
            brake_interp = scipy.interpolate.interp1d(
                control_command_actuation_cmd[:, 0] + 1e-9 * control_command_actuation_cmd[:, 1],
                brake,
            )(data_time_stamps)
        elif base_map_dir is None:
            print("control_command_actuation_cmd.csv is not found")
            return
        else:
            acc_cmd_interp = scipy.interpolate.interp1d(
                acc_cmd[:, 0] + 1e-9 * acc_cmd[:, 1], acc_cmd[:, 2]
            )(data_time_stamps)
            accel_map = actuation_map_2d.ActuationMap2D(base_map_dir + "/accel_map.csv")
            brake_map = actuation_map_2d.ActuationMap2D(base_map_dir + "/brake_map.csv")
            accel_interp = np.zeros(data_num)
            brake_interp = np.zeros(data_num)
            for i in range(data_num):
                if accel_map.is_map_used(vel_interp[i], acc_cmd_interp[i]):
                    accel_interp[i] = accel_map.get_actuation_cmd(vel_interp[i], acc_cmd_interp[i])
                    brake_interp[i] = -1e-10
                else:
                    accel_interp[i] = -1e-10
                    brake_interp[i] = brake_map.get_actuation_cmd(vel_interp[i], acc_cmd_interp[i])
        for i in range(smoothing_window, data_num - smoothing_window):
            vel_window = vel_interp[i - smoothing_window : i + smoothing_window + 1]
            accel_window = accel_interp[i - smoothing_window : i + smoothing_window + 1]
            brake_window = brake_interp[i - smoothing_window : i + smoothing_window + 1]
            acc_window = acc_interp[i - smoothing_window : i + smoothing_window + 1]
            acc_change = acc_window.max() - acc_window.min()
            if acc_change >= acc_change_threshold or vel_interp[i] < 0.1:
                continue
            if np.all(brake_window <= 0):
                self.accel_data_input.append([vel_window.mean(), accel_window.mean()])
                self.accel_data_output.append(acc_window.mean())
            elif np.all(accel_window <= 0):
                self.brake_data_input.append([vel_window.mean(), brake_window.mean()])
                self.brake_data_output.append(acc_window.mean())
        self.initialize_for_calibration()

    def outlier_exclusion_by_linear_regression(self):
        self.collected_data_counter.clear()
        for i in range(len(self.accel_data_input)):
            self.collected_data_counter.add_data_point(
                self.accel_data_input[i][0], self.accel_data_output[i], i
            )
        inlier_accel = self.collected_data_counter.outlier_exclusion_by_linear_regression(
            self.accel_data_input, self.accel_data_output
        )
        self.collected_data_counter.clear()
        for i in range(len(self.brake_data_input)):
            self.collected_data_counter.add_data_point(
                self.brake_data_input[i][0], self.brake_data_output[i], i
            )
        inlier_brake = self.collected_data_counter.outlier_exclusion_by_linear_regression(
            self.brake_data_input, self.brake_data_output
        )
        self.accel_data_input = np.array(self.accel_data_input)[inlier_accel].tolist()
        self.accel_data_output = np.array(self.accel_data_output)[inlier_accel].tolist()
        self.brake_data_input = np.array(self.brake_data_input)[inlier_brake].tolist()
        self.brake_data_output = np.array(self.brake_data_output)[inlier_brake].tolist()
        self.initialize_for_calibration()

    def calc_data_loader_weights(self, maximum_weight=0.02):
        self.collected_data_counter.clear()
        for i in range(len(self.accel_data_input)):
            self.collected_data_counter.add_data_point(
                self.accel_data_input[i][0], self.accel_data_output[i], i
            )
        self.dataloader_weights_accel = self.collected_data_counter.calc_weights(maximum_weight)
        self.collected_data_counter.clear()
        for i in range(len(self.brake_data_input)):
            self.collected_data_counter.add_data_point(
                self.brake_data_input[i][0],
                self.brake_data_output[i],
                i + len(self.accel_data_input),
            )
        self.dataloader_weights_brake = self.collected_data_counter.calc_weights(maximum_weight)
        if self.extracted_indices_accel is not None:
            self.dataloader_weights_accel = np.array(self.dataloader_weights_accel)[
                self.extracted_indices_accel
            ].tolist()
        if self.extracted_indices_brake is not None:
            self.dataloader_weights_brake = np.array(self.dataloader_weights_brake)[
                self.extracted_indices_brake
            ].tolist()

    def extract_data_for_calibration(self, max_data_num=50):
        self.collected_data_counter.clear()
        for i in range(len(self.accel_data_input)):
            self.collected_data_counter.add_data_point(
                self.accel_data_input[i][0], self.accel_data_output[i], i
            )
        self.extracted_indices_accel = self.collected_data_counter.get_extracted_indices(
            max_data_num
        )
        self.collected_data_counter.clear()
        for i in range(len(self.brake_data_input)):
            self.collected_data_counter.add_data_point(
                self.brake_data_input[i][0], self.brake_data_output[i], i
            )
        self.extracted_indices_brake = self.collected_data_counter.get_extracted_indices(
            max_data_num
        )
        self.accel_data_input_for_NN = []
        self.accel_data_output_for_NN = []
        self.brake_data_input_for_NN = []
        self.brake_data_output_for_NN = []
        for i in self.extracted_indices_accel:
            self.accel_data_input_for_NN.append(self.accel_data_input[i])
            self.accel_data_output_for_NN.append(self.accel_data_output[i])
        for i in self.extracted_indices_brake:
            self.brake_data_input_for_NN.append(self.brake_data_input[i])
            self.brake_data_output_for_NN.append(self.brake_data_output[i])
        if self.dataloader_weights_accel is not None:
            self.dataloader_weights_accel = np.array(self.dataloader_weights_accel)[
                self.extracted_indices_accel
            ].tolist()
        if self.dataloader_weights_brake is not None:
            self.dataloader_weights_brake = np.array(self.dataloader_weights_brake)[
                self.extracted_indices_brake
            ].tolist()

    def calc_base_map_error(self, base_map_dir):
        accel_map_path = base_map_dir + "/accel_map.csv"
        brake_map_path = base_map_dir + "/brake_map.csv"
        self.base_accel_map = actuation_map_2d.ActuationMap2D(accel_map_path)
        self.base_brake_map = actuation_map_2d.ActuationMap2D(brake_map_path)
        for i in range(len(self.accel_data_input)):
            accel_data = self.accel_data_input_for_NN[i]
            self.accel_data_output_for_NN[i] = self.accel_data_output_for_NN[
                i
            ] - self.base_accel_map.get_sim_actuation(accel_data[0], accel_data[1])
        for i in range(len(self.brake_data_input)):
            brake_data = self.brake_data_input_for_NN[i]
            self.brake_data_output_for_NN[i] = self.brake_data_output_for_NN[
                i
            ] - self.base_brake_map.get_sim_actuation(brake_data[0], brake_data[1])
        self.calc_base_map_error_performed = True

    def calc_calibration_error(self, map_dir):
        accel_map_path = map_dir + "/accel_map.csv"
        brake_map_path = map_dir + "/brake_map.csv"
        accel_map = actuation_map_2d.ActuationMap2D(accel_map_path)
        brake_map = actuation_map_2d.ActuationMap2D(brake_map_path)
        accel_output_predicted = []
        brake_output_predicted = []
        for i in range(len(self.accel_data_input)):
            accel_data = self.accel_data_input[i]
            accel_output_predicted.append(accel_map.get_sim_actuation(accel_data[0], accel_data[1]))
        for i in range(len(self.brake_data_input)):
            brake_data = self.brake_data_input[i]
            brake_output_predicted.append(brake_map.get_sim_actuation(brake_data[0], brake_data[1]))
        accel_error = np.mean(
            np.abs(np.array(accel_output_predicted) - np.array(self.accel_data_output))
        )
        brake_error = np.mean(
            np.abs(np.array(brake_output_predicted) - np.array(self.brake_data_output))
        )
        print("accel_error: ", accel_error)
        print("brake_error: ", brake_error)

    def plot_training_data(self, save_dir=None):
        plt.scatter(
            np.array(self.accel_data_input_for_NN)[:, 0],
            np.array(self.accel_data_output_for_NN),
            label="accel_data",
        )
        plt.scatter(
            np.array(self.brake_data_input_for_NN)[:, 0],
            np.array(self.brake_data_output_for_NN),
            label="brake_data",
        )
        plt.title("vel vs acc plots")
        plt.xlabel("vel_obs [m/s]")
        plt.ylabel("acc_obs [m/s^2]")
        plt.legend()
        if save_dir is not None:
            plt.savefig(save_dir + "/training_data.png")
            plt.close()
        else:
            plt.show()

        fig = plt.figure()
        ax = fig.add_subplot(111, projection="3d")
        ax.scatter(
            np.array(self.accel_data_input_for_NN)[:, 0],
            np.array(self.accel_data_input_for_NN)[:, 1],
            np.array(self.accel_data_output_for_NN),
        )
        ax.set_xlabel("vel_obs [m/s]")
        ax.set_ylabel("accel_input [m/s^2]")
        ax.set_zlabel("acc_obs [m/s^2]")
        if save_dir is not None:
            plt.savefig(save_dir + "/accel_3d.png")
            plt.close()
        else:
            plt.show()

        fig = plt.figure()
        ax = fig.add_subplot(111, projection="3d")
        ax.scatter(
            np.array(self.brake_data_input_for_NN)[:, 0],
            np.array(self.brake_data_input_for_NN)[:, 1],
            np.array(self.brake_data_output_for_NN),
        )
        ax.set_xlabel("vel_obs [m/s]")
        ax.set_ylabel("brake_input [m/s^2]")
        ax.set_zlabel("acc_obs [m/s^2]")
        if save_dir is not None:
            plt.savefig(save_dir + "/brake_3d.png")
            plt.close()
        else:
            plt.show()

    def map_validation(
        self,
        accel_map_matrix,
        brake_map_matrix,
        map_vel=default_map_vel,
        map_accel=default_map_accel,
        map_brake=default_map_brake,
    ):
        accel_map_matrix_valid = accel_map_matrix.copy()
        brake_map_matrix_valid = brake_map_matrix.copy()
        for i in range(len(map_vel)):
            for j in range(1, len(map_accel)):
                if accel_map_matrix_valid[j, i] < accel_map_matrix_valid[j - 1, i]:
                    accel_map_matrix_valid[j, i] = accel_map_matrix_valid[j - 1, i] + 1e-6
                    print("map validator is applied at vel: ", map_vel[i], "accel: ", map_accel[j])
        for i in range(len(map_vel)):
            for j in range(1, len(map_brake)):
                if brake_map_matrix_valid[j, i] > brake_map_matrix_valid[j - 1, i]:
                    brake_map_matrix_valid[j, i] = brake_map_matrix_valid[j - 1, i] - 1e-6
                    print("map validator is applied at vel: ", map_vel[i], "brake: ", map_brake[j])
        return accel_map_matrix_valid, brake_map_matrix_valid


def calc_monotone_constraint_cost(model, test_points, map_matrix, mode, monotone_margin=1e-3):
    if mode == "accel":
        cost = torch.relu(
            model(test_points[:, :-1])
            - model(test_points[:, 1:])
            + map_matrix[:-1]
            - map_matrix[1:]
            + monotone_margin
        ).sum()
    elif mode == "brake":
        cost = torch.relu(
            model(test_points[:, 1:])
            - model(test_points[:, :-1])
            + map_matrix[1:]
            - map_matrix[:-1]
            + monotone_margin
        ).sum()
    return cost


class CalibrationNN(nn.Module):
    def __init__(
        self,
        max_map_error=None,
    ):
        super(CalibrationNN, self).__init__()
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(2, 64),
            nn.ReLU(),
            nn.Linear(64, 32),
            nn.ReLU(),
            nn.Linear(32, 1),
        )
        self.max_map_error = max_map_error

    def forward(self, x):
        x = self.linear_relu_stack(x)
        if self.max_map_error is not None:
            x = self.max_map_error * torch.tanh(x)
        return x


def validate_in_batches(model, criterion, X_val, Y_val, batch_size=10000):
    model.eval()
    val_loss = 0.0
    num_batches = (X_val.size(0) + batch_size - 1) // batch_size

    with torch.no_grad():
        for i in range(num_batches):
            start_idx = i * batch_size
            end_idx = min((i + 1) * batch_size, X_val.size(0))

            X_batch = X_val[start_idx:end_idx]
            Y_batch = Y_val[start_idx:end_idx]

            outputs = model(X_batch)
            loss = criterion(outputs, Y_batch)
            val_loss += loss.item() * (end_idx - start_idx)

    val_loss /= X_val.size(0)
    return val_loss


class CalibratorByNeuralNetwork(AddDataFromCSV):
    def __init__(self):
        super().__init__()
        self.accel_NN_model = None
        self.brake_NN_model = None
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.max_iter = 10000
        self.tol = 1e-5
        self.alpha_1 = 1e-7
        self.alpha_2 = 1e-7
        self.max_sample_per_epoch = 2500
        self.monotone_constraint_weight = 1.0
        self.accel_brake_zero_weight = 1.0

    def train_calibrator_NN(
        self,
        model_accel,
        X_accel,
        y_accel,
        model_brake,
        X_brake,
        y_brake,
        batch_size,
        learning_rates,
        patience,
        weights_accel=None,
        weights_brake=None,
        test_points_accel=None,
        test_points_brake=None,
        map_matrix_accel=None,
        map_matrix_brake=None,
    ):
        accel_sample_size = X_accel.shape[0]
        brake_sample_size = X_brake.shape[0]
        print("accel sample size: ", accel_sample_size)
        print("brake sample size: ", brake_sample_size)
        print("patience: ", patience)
        num_train_accel = int(3 * accel_sample_size / 4)
        num_train_brake = int(3 * brake_sample_size / 4)
        id_all_accel = np.random.choice(accel_sample_size, accel_sample_size, replace=False)
        id_train_accel = id_all_accel[:num_train_accel]
        id_val_accel = id_all_accel[num_train_accel:]
        X_train_accel = X_accel[id_train_accel]
        y_train_accel = y_accel[id_train_accel]
        X_val_accel = X_accel[id_val_accel]
        y_val_accel = y_accel[id_val_accel]
        id_all_brake = np.random.choice(brake_sample_size, brake_sample_size, replace=False)
        id_train_brake = id_all_brake[:num_train_brake]
        id_val_brake = id_all_brake[num_train_brake:]
        X_train_brake = X_brake[id_train_brake]
        y_train_brake = y_brake[id_train_brake]
        X_val_brake = X_brake[id_val_brake]
        y_val_brake = y_brake[id_val_brake]
        # Define the loss function.
        criterion = nn.L1Loss()
        # Define the optimizer.
        optimizer_accel = torch.optim.Adam(model_accel.parameters(), lr=learning_rates[0])
        optimizer_brake = torch.optim.Adam(model_brake.parameters(), lr=learning_rates[0])
        # Define the initial loss.
        initial_loss_accel = validate_in_batches(
            model_accel, criterion, X_train_accel, y_train_accel.view(-1, 1)
        )
        initial_loss_brake = validate_in_batches(
            model_brake, criterion, X_train_brake, y_train_brake.view(-1, 1)
        )
        initial_loss = initial_loss_accel + initial_loss_brake
        print("initial_loss: ", initial_loss)
        print("initial_loss_accel: ", initial_loss_accel)
        print("initial_loss_brake: ", initial_loss_brake)
        # Define the early stopping.
        early_stopping = EarlyStopping(initial_loss, tol=self.tol, patience=patience)
        # Data Loader
        if weights_accel is None:
            train_dataset_accel = DataLoader(
                TensorDataset(X_train_accel, y_train_accel), batch_size=batch_size, shuffle=True
            )
        else:
            weights_train_accel = np.array(weights_accel)[id_train_accel]
            sampler_accel = WeightedRandomSampler(
                weights=weights_train_accel, num_samples=len(weights_train_accel), replacement=True
            )
            train_dataset_accel = DataLoader(
                TensorDataset(X_train_accel, y_train_accel),
                batch_size=batch_size,
                sampler=sampler_accel,
            )
        if weights_brake is None:
            train_dataset_brake = DataLoader(
                TensorDataset(X_train_brake, y_train_brake), batch_size=batch_size, shuffle=True
            )
        else:
            weights_train_brake = np.array(weights_brake)[id_train_brake]
            sampler_brake = WeightedRandomSampler(
                weights=weights_train_brake, num_samples=len(weights_train_brake), replacement=True
            )
            train_dataset_brake = DataLoader(
                TensorDataset(X_train_brake, y_train_brake),
                batch_size=batch_size,
                sampler=sampler_brake,
            )

        # learning_rate index
        learning_rate_index = 0
        # Print learning rate
        print("learning rate: ", learning_rates[learning_rate_index])
        for i in range(self.max_iter):
            model_accel.train()
            model_brake.train()
            for X_accel_batch, y_accel_batch in islice(
                train_dataset_accel, round(self.max_sample_per_epoch / batch_size) + 1
            ):
                for X_brake_batch, y_brake_batch in islice(train_dataset_brake, 1):
                    optimizer_accel.zero_grad()
                    optimizer_brake.zero_grad()
                    outputs_accel = model_accel(X_accel_batch)
                    outputs_brake = model_brake(X_brake_batch)
                    loss = criterion(outputs_accel, y_accel_batch.view(-1, 1))
                    loss += criterion(outputs_brake, y_brake_batch.view(-1, 1))
                    for w in model_accel.parameters():
                        loss += (
                            self.alpha_1 * torch.norm(w, 1) + self.alpha_2 * torch.norm(w, 2) ** 2
                        )
                    for w in model_brake.parameters():
                        loss += (
                            self.alpha_1 * torch.norm(w, 1) + self.alpha_2 * torch.norm(w, 2) ** 2
                        )
                    if (
                        test_points_accel is not None
                        and test_points_brake is not None
                        and map_matrix_accel is not None
                        and map_matrix_brake is not None
                    ):
                        loss += self.monotone_constraint_weight * calc_monotone_constraint_cost(
                            model_accel, test_points_accel, map_matrix_accel, mode="accel"
                        )
                        loss += self.monotone_constraint_weight * calc_monotone_constraint_cost(
                            model_brake, test_points_brake, map_matrix_brake, mode="brake"
                        )
                        loss += self.accel_brake_zero_weight * criterion(
                            model_accel(test_points_accel[:, 0]) + map_matrix_accel[0],
                            model_brake(test_points_brake[:, 0]) + map_matrix_brake[0],
                        )
                    loss.backward()
                    optimizer_accel.step()
                    optimizer_brake.step()
            model_accel.eval()
            model_brake.eval()
            val_loss_accel = validate_in_batches(
                model_accel, criterion, X_val_accel, y_val_accel.view(-1, 1), batch_size
            )
            val_loss_brake = validate_in_batches(
                model_brake, criterion, X_val_brake, y_val_brake.view(-1, 1), batch_size
            )
            val_loss = val_loss_accel + val_loss_brake
            if i % 10 == 1:
                print("epoch: ", i)
                print("val_loss: ", val_loss)
                print("val_loss_accel: ", val_loss_accel)
                print("val_loss_brake: ", val_loss_brake)
            if early_stopping(val_loss):
                learning_rate_index += 1
                if learning_rate_index >= len(learning_rates):
                    break
                else:
                    print(
                        "------update learning rate to ",
                        learning_rates[learning_rate_index],
                        "------",
                    )
                    optimizer_accel = torch.optim.Adam(
                        model_accel.parameters(), lr=learning_rates[learning_rate_index]
                    )
                    optimizer_brake = torch.optim.Adam(
                        model_brake.parameters(), lr=learning_rates[learning_rate_index]
                    )
                    early_stopping.reset()

    def calibrate_by_NN(
        self,
        learning_rates=[1e-2, 1e-3, 1e-4, 1e-5, 1e-6],
        patience=50,
        batch_size=100,
        max_iter=None,
        tol=None,
        alpha_1=None,
        alpha_2=None,
    ):
        if max_iter is not None:
            self.max_iter = max_iter
        if tol is not None:
            self.tol = tol
        if alpha_1 is not None:
            self.alpha_1 = alpha_1
        if alpha_2 is not None:
            self.alpha_2 = alpha_2
        self.accel_NN_model = CalibrationNN().to(self.device)
        self.brake_NN_model = CalibrationNN().to(self.device)

        X_accel = torch.tensor(
            np.array(self.accel_data_input_for_NN), dtype=torch.float32, device=self.device
        )
        y_accel = torch.tensor(
            np.array(self.accel_data_output_for_NN), dtype=torch.float32, device=self.device
        )
        X_brake = torch.tensor(
            np.array(self.brake_data_input_for_NN), dtype=torch.float32, device=self.device
        )
        y_brake = torch.tensor(
            np.array(self.brake_data_output_for_NN), dtype=torch.float32, device=self.device
        )

        self.train_calibrator_NN(
            self.accel_NN_model,
            X_accel,
            y_accel,
            self.brake_NN_model,
            X_brake,
            y_brake,
            batch_size,
            learning_rates,
            patience,
        )

    def calc_accel_brake_map_NN(
        self, map_vel=default_map_vel, map_accel=default_map_accel, map_brake=default_map_brake
    ):
        self.accel_NN_model.eval()
        self.brake_NN_model.eval()
        self.accel_map_matrix_NN = np.zeros((len(map_accel), len(map_vel)))
        self.brake_map_matrix_NN = np.zeros((len(map_brake), len(map_vel)))
        for i in range(len(map_vel)):
            for j in range(len(map_accel)):
                self.accel_map_matrix_NN[j, i] = self.accel_NN_model(
                    torch.tensor(
                        [map_vel[i], map_accel[j]], dtype=torch.float32, device=self.device
                    )
                ).item()
                if self.calc_base_map_error_performed:
                    self.accel_map_matrix_NN[j, i] += self.base_accel_map.get_sim_actuation(
                        map_vel[i], map_accel[j]
                    )
            for j in range(len(map_brake)):
                self.brake_map_matrix_NN[j, i] = self.brake_NN_model(
                    torch.tensor(
                        [map_vel[i], map_brake[j]], dtype=torch.float32, device=self.device
                    )
                ).item()
                if self.calc_base_map_error_performed:
                    self.brake_map_matrix_NN[j, i] += self.base_brake_map.get_sim_actuation(
                        map_vel[i], map_brake[j]
                    )
            self.accel_map_matrix_NN[0] = 0.5 * (
                self.accel_map_matrix_NN[0] + self.brake_map_matrix_NN[0]
            )
            self.brake_map_matrix_NN[0] = self.accel_map_matrix_NN[0]

    def save_accel_brake_map_NN(
        self,
        map_vel=default_map_vel,
        map_accel=default_map_accel,
        map_brake=default_map_brake,
        save_dir=".",
    ):
        if not os.path.isdir(save_dir):
            os.mkdir(save_dir)
        self.calc_accel_brake_map_NN(map_vel, map_accel, map_brake)
        actuation_map_csv_writer.map_csv_writer(
            map_vel, map_accel, self.accel_map_matrix_NN, save_dir + "/accel_map.csv"
        )
        actuation_map_csv_writer.map_csv_writer(
            map_vel, map_brake, self.brake_map_matrix_NN, save_dir + "/brake_map.csv"
        )


class CalibratorByEnsembleNN(CalibratorByNeuralNetwork):
    def __init__(self):
        super().__init__()
        self.accel_NN_models = []
        self.brake_NN_models = []
        self.ensemble_num = 5
        self.max_map_error = 1.0

    def calibrate_by_ensemble_NN(
        self,
        learning_rates=[1e-2, 1e-3, 1e-4, 1e-5, 1e-6],
        patience=50,
        batch_size=100,
        max_iter=None,
        tol=None,
        alpha_1=None,
        alpha_2=None,
        ensemble_num=None,
        clear_model=False,
        map_vel=default_map_vel,
        map_accel=default_map_accel,
        map_brake=default_map_brake,
    ):
        test_points_accel_np = np.array([[[v, a] for a in map_accel] for v in map_vel])
        test_points_brake_np = np.array([[[v, b] for b in map_brake] for v in map_vel])
        test_points_accel = torch.tensor(
            test_points_accel_np, dtype=torch.float32, device=self.device
        )
        test_points_brake = torch.tensor(
            test_points_brake_np, dtype=torch.float32, device=self.device
        )
        if self.calc_base_map_error_performed:
            base_map_matrix_accel_np = np.zeros((len(map_accel), len(map_vel)))
            base_map_matrix_brake_np = np.zeros((len(map_brake), len(map_vel)))
            for i in range(len(map_vel)):
                for j in range(len(map_accel)):
                    base_map_matrix_accel_np[j, i] = self.base_accel_map.get_sim_actuation(
                        map_vel[i], map_accel[j]
                    )
                for j in range(len(map_brake)):
                    base_map_matrix_brake_np[j, i] = self.base_brake_map.get_sim_actuation(
                        map_vel[i], map_brake[j]
                    )
            base_map_matrix_accel = torch.tensor(
                base_map_matrix_accel_np, dtype=torch.float32, device=self.device
            )
            base_map_matrix_brake = torch.tensor(
                base_map_matrix_brake_np, dtype=torch.float32, device=self.device
            )
        else:
            base_map_matrix_accel = None
            base_map_matrix_brake = None
        if max_iter is not None:
            self.max_iter = max_iter
        if tol is not None:
            self.tol = tol
        if alpha_1 is not None:
            self.alpha_1 = alpha_1
        if alpha_2 is not None:
            self.alpha_2 = alpha_2
        if ensemble_num is not None:
            if clear_model:
                self.ensemble_num = ensemble_num
            elif ensemble_num != self.ensemble_num:
                print("ensemble_num is not updated because clear_model is False")
        if clear_model:
            self.accel_NN_models.clear()
            self.brake_NN_models.clear()
        for i in range(self.ensemble_num):
            print("______________________________")
            print("ensemble number: ", i)
            print("______________________________")
            if self.calc_base_map_error_performed:
                max_map_error = self.max_map_error
            else:
                max_map_error = None
            self.accel_NN_models.append(CalibrationNN(max_map_error).to(self.device))
            self.brake_NN_models.append(CalibrationNN(max_map_error).to(self.device))
            X_accel = torch.tensor(
                np.array(self.accel_data_input_for_NN), dtype=torch.float32, device=self.device
            )
            y_accel = torch.tensor(
                np.array(self.accel_data_output_for_NN), dtype=torch.float32, device=self.device
            )
            X_brake = torch.tensor(
                np.array(self.brake_data_input_for_NN), dtype=torch.float32, device=self.device
            )
            y_brake = torch.tensor(
                np.array(self.brake_data_output_for_NN), dtype=torch.float32, device=self.device
            )
            print("calibrate accel and brake")
            self.train_calibrator_NN(
                self.accel_NN_models[i],
                X_accel,
                y_accel,
                self.brake_NN_models[i],
                X_brake,
                y_brake,
                batch_size,
                learning_rates,
                patience,
                self.dataloader_weights_accel,
                self.dataloader_weights_brake,
                test_points_accel,
                test_points_brake,
                base_map_matrix_accel,
                base_map_matrix_brake,
            )

    def calc_accel_brake_map_ensemble_NN(
        self, map_vel=default_map_vel, map_accel=default_map_accel, map_brake=default_map_brake
    ):
        self.accel_map_matrix_ensemble_NN = np.zeros(
            (self.ensemble_num, len(map_accel), len(map_vel))
        )
        self.brake_map_matrix_ensemble_NN = np.zeros(
            (self.ensemble_num, len(map_brake), len(map_vel))
        )
        for k in range(self.ensemble_num):
            for i in range(len(map_vel)):
                for j in range(len(map_accel)):
                    self.accel_map_matrix_ensemble_NN[k, j, i] = self.accel_NN_models[k](
                        torch.tensor(
                            [map_vel[i], map_accel[j]], dtype=torch.float32, device=self.device
                        )
                    ).item()
                    if self.calc_base_map_error_performed:
                        self.accel_map_matrix_ensemble_NN[
                            k, j, i
                        ] += self.base_accel_map.get_sim_actuation(map_vel[i], map_accel[j])
                for j in range(len(map_brake)):
                    self.brake_map_matrix_ensemble_NN[k, j, i] = self.brake_NN_models[k](
                        torch.tensor(
                            [map_vel[i], map_brake[j]], dtype=torch.float32, device=self.device
                        )
                    ).item()
                    if self.calc_base_map_error_performed:
                        self.brake_map_matrix_ensemble_NN[
                            k, j, i
                        ] += self.base_brake_map.get_sim_actuation(map_vel[i], map_brake[j])
                self.accel_map_matrix_ensemble_NN[k, 0] = 0.5 * (
                    self.accel_map_matrix_ensemble_NN[k, 0]
                    + self.brake_map_matrix_ensemble_NN[k, 0]
                )
                self.brake_map_matrix_ensemble_NN[k, 0] = self.accel_map_matrix_ensemble_NN[k, 0]
        self.accel_map_matrix_NN, self.brake_map_matrix_NN = self.map_validation(
            np.mean(self.accel_map_matrix_ensemble_NN, axis=0),
            np.mean(self.brake_map_matrix_ensemble_NN, axis=0),
            map_vel,
            map_accel,
            map_brake,
        )
        self.accel_map_matrix_std_NN = np.std(self.accel_map_matrix_ensemble_NN, axis=0)
        self.brake_map_matrix_std_NN = np.std(self.brake_map_matrix_ensemble_NN, axis=0)

    def save_accel_brake_map_ensemble_NN(
        self,
        map_vel=default_map_vel,
        map_accel=default_map_accel,
        map_brake=default_map_brake,
        save_dir=".",
        save_heat_map=False,
        true_map_dir=None,
        compare_with_base_map=False,
    ):
        if not os.path.isdir(save_dir):
            os.mkdir(save_dir)
        self.calc_accel_brake_map_ensemble_NN(map_vel, map_accel, map_brake)
        actuation_map_csv_writer.map_csv_writer(
            map_vel, map_accel, self.accel_map_matrix_NN, save_dir + "/accel_map.csv"
        )
        actuation_map_csv_writer.map_csv_writer(
            map_vel, map_brake, self.brake_map_matrix_NN, save_dir + "/brake_map.csv"
        )
        actuation_map_csv_writer.map_csv_writer(
            map_vel, map_accel, self.accel_map_matrix_std_NN, save_dir + "/accel_map_std.csv"
        )
        actuation_map_csv_writer.map_csv_writer(
            map_vel, map_brake, self.brake_map_matrix_std_NN, save_dir + "/brake_map_std.csv"
        )
        if save_heat_map:
            if true_map_dir is None:
                package_path_json = str(Path(__file__).parent.parent) + "/package_path.json"
                with open(package_path_json, "r") as file:
                    package_path = json.load(file)

                true_map_dir = (
                    package_path["path"]
                    + "/autoware_vehicle_adaptor/actuation_cmd_maps/accel_brake_maps/default_parameter"
                )
            true_accel_map_path = true_map_dir + "/accel_map.csv"
            true_brake_map_path = true_map_dir + "/brake_map.csv"
            true_accel_map = actuation_map_2d.ActuationMap2D(true_accel_map_path)
            true_brake_map = actuation_map_2d.ActuationMap2D(true_brake_map_path)
            prediction_signed_error_accel = -self.accel_map_matrix_NN
            prediction_signed_error_brake = -self.brake_map_matrix_NN
            for i in range(len(map_vel)):
                for j in range(len(map_accel)):
                    prediction_signed_error_accel[j, i] += true_accel_map.get_sim_actuation(
                        map_vel[i], map_accel[j]
                    )
                for j in range(len(map_brake)):
                    prediction_signed_error_brake[j, i] += true_brake_map.get_sim_actuation(
                        map_vel[i], map_brake[j]
                    )

            fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(24, 15), tight_layout=True)
            fig.suptitle("calibration result")
            error_norm = TwoSlopeNorm(vcenter=0, vmin=-0.5, vmax=0.5)
            std_norm = TwoSlopeNorm(vcenter=0, vmin=-0.2, vmax=0.2)
            sns.heatmap(
                prediction_signed_error_accel,
                annot=True,
                cmap="coolwarm",
                xticklabels=map_vel,
                yticklabels=map_accel,
                ax=axes[0, 0],
                linewidths=0.1,
                linecolor="gray",
                norm=error_norm,
            )
            axes[0, 0].set_xlabel("velocity [m/s]")
            axes[0, 0].set_ylabel("accel")
            if compare_with_base_map:
                axes[0, 0].set_title("Prediction error accel (base - prediction)")
            else:
                axes[0, 0].set_title("Prediction error accel (true - prediction)")
            sns.heatmap(
                self.accel_map_matrix_std_NN,
                annot=True,
                cmap="coolwarm",
                xticklabels=map_vel,
                yticklabels=map_accel,
                ax=axes[0, 1],
                linewidths=0.1,
                linecolor="gray",
                norm=std_norm,
            )
            axes[0, 1].set_xlabel("velocity [m/s]")
            axes[0, 1].set_ylabel("accel")
            axes[0, 1].set_title("Prediction std accel")
            sns.heatmap(
                prediction_signed_error_brake,
                annot=True,
                cmap="coolwarm",
                xticklabels=map_vel,
                yticklabels=map_accel,
                ax=axes[1, 0],
                linewidths=0.1,
                linecolor="gray",
                norm=error_norm,
            )
            axes[1, 0].set_xlabel("velocity [m/s]")
            axes[1, 0].set_ylabel("brake")
            if compare_with_base_map:
                axes[1, 0].set_title("Prediction error brake (base - prediction)")
            else:
                axes[1, 0].set_title("Prediction error brake (true - prediction)")
            sns.heatmap(
                self.brake_map_matrix_std_NN,
                annot=True,
                cmap="coolwarm",
                xticklabels=map_vel,
                yticklabels=map_brake,
                ax=axes[1, 1],
                linewidths=0.1,
                linecolor="gray",
                norm=std_norm,
            )
            axes[1, 1].set_xlabel("velocity [m/s]")
            axes[1, 1].set_ylabel("brake")
            axes[1, 1].set_title("Prediction std brake")
            plt.savefig(save_dir + "/calibration_result.png")
            plt.close()


def calc_distance(x, start_point, end_point):
    x_bar = x - start_point
    y_bar = end_point - start_point
    if x_bar[0] * y_bar[1] - x_bar[1] * y_bar[0] >= 0:
        r = np.dot(x_bar, y_bar) / np.dot(y_bar, y_bar)
        r = min(max(r, 0.0), 1.0)
        return np.linalg.norm(x_bar - r * y_bar)
    else:
        return -1.0


class DummyDataGenerator(AddDataFromCSV):
    def __init__(self):
        super().__init__()
        self.vel_scale = 1.0
        self.accel_brake_scale = 1.0
        self.distance_threshold = 0.01

    def calc_scale(
        self,
        map_vel=default_map_vel,
        map_accel=default_map_accel,
        map_brake=default_map_brake,
        distance_threshold=None,
    ):
        self.vel_scale = (len(map_vel) - 1) / np.abs(map_vel[-1] - map_vel[0])
        self.accel_brake_scale = (len(map_accel) + len(map_brake) - 2) / (
            np.abs(map_accel[-1]) + np.abs(map_brake[-1])
        )
        if distance_threshold is None:
            self.distance_threshold = 0.5
        else:
            self.distance_threshold = distance_threshold

    def calc_convex_hull(self):
        # this process does not depend on the scale
        self.accel_brake_data_input = self.accel_data_input_for_NN.copy()
        for i in range(len(self.brake_data_input_for_NN)):
            brake_data_input = self.brake_data_input_for_NN[i]
            self.accel_brake_data_input.append([brake_data_input[0], -brake_data_input[1]])
        self.convex_hull_accel_brake = ConvexHull(self.accel_brake_data_input)

    def calc_convex_hull_distance(self, x, mode):
        vertices_appended = np.append(
            self.convex_hull_accel_brake.vertices, self.convex_hull_accel_brake.vertices[0]
        )
        if mode == "accel":
            x_scaled = np.array([self.vel_scale, self.accel_brake_scale]) * np.array(x)
        elif mode == "brake":
            x_scaled = np.array([self.vel_scale, -self.accel_brake_scale]) * np.array(x)
        distance_list = []
        for i in range(len(vertices_appended) - 1):
            start_point = np.array([self.vel_scale, self.accel_brake_scale]) * np.array(
                self.accel_brake_data_input[vertices_appended[i]]
            )
            end_point = np.array([self.vel_scale, self.accel_brake_scale]) * np.array(
                self.accel_brake_data_input[vertices_appended[i + 1]]
            )
            distance = calc_distance(x_scaled, start_point, end_point)
            if distance >= 0.0:
                distance_list.append(distance)
        if len(distance_list) == 0:
            return 0.0
        else:
            return min(distance_list)

    def add_dummy_data(
        self,
        map_vel=default_map_vel,
        map_accel=default_map_accel,
        map_brake=default_map_brake,
        weight_coef=0.5,
        distance_threshold=None,
        save_dir=".",
        save_convex_hull_plot=False,
        show_flag=False,
    ):
        self.calc_scale(map_vel, map_accel, map_brake, distance_threshold)
        self.calc_convex_hull()
        accel_addition_points = []
        brake_addition_points = []
        max_accel_weight = np.max(np.array(self.dataloader_weights_accel))
        max_brake_weight = np.max(np.array(self.dataloader_weights_brake))
        for i in range(len(map_vel)):
            for j in range(len(map_accel)):
                x = np.array([map_vel[i], map_accel[j]])
                dist = self.calc_convex_hull_distance(x, "accel")
                if dist > self.distance_threshold:
                    self.accel_data_input_for_NN.append(x)
                    self.accel_data_output_for_NN.append(0.0)
                    accel_addition_points.append(x)
                    print(
                        "add dummy data at vel: ",
                        map_vel[i],
                        " accel: ",
                        map_accel[j],
                        " dist: ",
                        dist,
                    )
                    if self.dataloader_weights_accel is not None:
                        self.dataloader_weights_accel.append(weight_coef * dist * max_accel_weight)
            for j in range(len(map_brake)):
                x = np.array([map_vel[i], map_brake[j]])
                dist = self.calc_convex_hull_distance(x, "brake")
                if dist > self.distance_threshold:
                    self.brake_data_input_for_NN.append(x)
                    self.brake_data_output_for_NN.append(0.0)
                    brake_addition_points.append(x)
                    print(
                        "add dummy data at vel: ",
                        map_vel[i],
                        " brake: ",
                        map_brake[j],
                        " dist: ",
                        dist,
                    )
                    if self.dataloader_weights_brake is not None:
                        self.dataloader_weights_brake.append(weight_coef * dist * max_brake_weight)
        if save_convex_hull_plot or show_flag:
            plt.title("Convex Hull")
            plt.scatter(
                np.array(self.accel_brake_data_input)[:, 0],
                np.array(self.accel_brake_data_input)[:, 1],
                label="data points",
            )
            for simplex in self.convex_hull_accel_brake.simplices:
                plt.plot(
                    np.array(self.accel_brake_data_input)[simplex, 0],
                    np.array(self.accel_brake_data_input)[simplex, 1],
                    "k-",
                )
            plt.scatter(
                np.array(self.accel_brake_data_input)[self.convex_hull_accel_brake.vertices, 0],
                np.array(self.accel_brake_data_input)[self.convex_hull_accel_brake.vertices, 1],
                color="r",
                label="convex hull vertices",
            )
            plt.scatter(
                np.array(accel_addition_points)[:, 0],
                np.array(accel_addition_points)[:, 1],
                label="accel addition points",
            )
            plt.scatter(
                np.array(brake_addition_points)[:, 0],
                -np.array(brake_addition_points)[:, 1],
                label="brake addition points",
            )
            plt.xlabel("velocity [m/s]")
            plt.ylabel("accel brake")
            plt.legend()
            plt.title("Convex Hull")

            if show_flag:
                plt.show()
            if save_convex_hull_plot:
                plt.savefig(save_dir + "/convex_hull.png")
                plt.close()


class Calibrator(CalibratorByEnsembleNN, DummyDataGenerator):
    def __init__(self):
        super().__init__()
