import csv
import datetime
import math
import os
import time

from autoware_vehicle_adaptor.controller import pure_pursuit_controller
from autoware_vehicle_adaptor.controller import pure_pursuit_gain_updater
from autoware_vehicle_adaptor.param import parameters
from autoware_vehicle_adaptor.src import actuation_map_2d
from autoware_vehicle_adaptor.src import vehicle_adaptor_compensator
from matplotlib.colors import LogNorm

# from utils.density_estimation import KinematicStates
# from utils.density_estimation import visualize_speed_acc
# from utils.density_estimation import visualize_speed_steer
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import scipy.interpolate
import seaborn as sns
import torch
from utils import data_collection_utils
from utils import delay_compensator
from utils import parameter_change_utils
from utils.data_collection_utils import ControlType
from utils.parameter_change_utils import ChangeParam

x_index = 0
y_index = 1
vel_index = 2
yaw_index = 3
acc_index = 4
steer_index = 5

USE_SMART_MPC_PACKAGE = False
UPDATE_PP_GAIN = False
if USE_SMART_MPC_PACKAGE:
    from autoware_smart_mpc_trajectory_follower.scripts import drive_controller
else:
    from autoware_vehicle_adaptor.controller import nominal_ilqr


class PythonSimulator:
    def __init__(self):
        self.vehicle_type = None
        self.acc_scaling = 1.0
        self.steer_scaling = 1.0
        self.steer_dead_band = 0.0012
        self.acc_dead_band = 0.0
        self.accel_dead_band = 0.0
        self.brake_dead_band = 0.0
        self.measurement_steer_bias = 0.0
        self.sim_dt = 0.0033
        self.control_step = 10
        self.max_control_time = 100.0
        self.acc_noise = 0.08
        self.acc_smoothing_constant = 0.9

        self.control_dt = parameters.mpc_control_dt
        self.wheel_base = parameters.wheel_base
        self.horizon_len = parameters.mpc_horizon_len
        self.predict_step = parameters.mpc_predict_step
        self.acc_time_delay = parameters.acc_time_delay
        self.steer_time_delay = parameters.steer_time_delay
        self.acc_time_constant = parameters.acc_time_constant
        self.steer_time_constant = parameters.steer_time_constant

        self.acc_delay_step_sim = round(self.acc_time_delay / self.sim_dt)
        self.steer_delay_step_sim = round(self.steer_time_delay / self.sim_dt)
        self.initial_error = np.array([0.001, 0.03, 0.01, 0.0005, 0.0, 0.0])

        self.X_des_history = []
        self.tracking_error_list = []

        self.data_collection_seed = 1
        self.acc_amp_range = 0.05
        self.acc_period_range = [5.0, 20.0]
        self.steer_amp_range = 0.005
        self.steer_period_range = [5.0, 30.0]
        self.target_vel_acc_max = 1.2
        self.constant_vel_time = 5.0
        self.target_vel_split_size = 5

        self.circle_y_length = 40.0
        self.circle_x_length = 80.0
        self.step_response_max_input = 0.01
        self.step_response_max_length = 1.5
        self.step_response_interval = 5.0
        self.step_response_min_length = 0.5
        self.smoothing_trajectory_data_flag = True

        self.data_collection = False
        self.target_vel_on_line = 6.0
        self.target_acc_on_line = 0.0

        # self.steer_rate_lim = 0.35
        # self.vel_rate_lim = 7.0

        self.steer_rate_lim = 3.0
        self.vel_rate_lim = 2.0
        self.adaptive_gear_ratio_coef_control_input = [15.713, 0.053, 0.042]
        self.adaptive_gear_ratio_coef_sim_input = [15.713, 0.053, 0.042]
        self.adaptive_gear_ratio_coef_control_obs = [15.713, 0.053, 0.042]
        self.adaptive_gear_ratio_coef_sim_obs = [15.713, 0.053, 0.042]
        self.accel_brake_map_control_path = None
        self.accel_brake_map_sim_path = None
        self.accel_time_delay = self.acc_time_delay
        self.brake_time_delay = self.acc_time_delay

        if USE_SMART_MPC_PACKAGE:
            self.controller = drive_controller.drive_controller(use_trained_model=False)
        else:
            self.controller = nominal_ilqr.NominalILQRController()
        self.pure_pursuit_controller = pure_pursuit_controller.PurePursuitController(
            self.wheel_base
        )
        self.log_updater = data_collection_utils.driving_log_updater()
        self.vehicle_adaptor = vehicle_adaptor_compensator.VehicleAdaptor()
        self.perturbed_sim_flag = False
        self.nominal_setting_dict = {}
        self.sim_setting_dict = {}
        self.nominal_params = {}
        for param_name in ChangeParam.keys():
            self.nominal_params[param_name] = getattr(self, param_name)
        self.use_accel_brake_map = False

        default_accel_map_path = (
            "../actuation_cmd_maps/accel_brake_maps/default_parameter/accel_map.csv"
        )
        default_brake_map_path = (
            "../actuation_cmd_maps/accel_brake_maps/default_parameter/brake_map.csv"
        )
        # self.control_accel_map_path = "../actuation_cmd_maps/accel_brake_maps/low_quality_map/accel_map.csv"
        # self.control_brake_map_path = "../actuation_cmd_maps/accel_brake_maps/low_quality_map/brake_map.csv"
        self.accel_map_sim = actuation_map_2d.ActuationMap2D(default_accel_map_path)
        self.brake_map_sim = actuation_map_2d.ActuationMap2D(default_brake_map_path)
        self.accel_map_control = actuation_map_2d.ActuationMap2D(default_accel_map_path)
        self.brake_map_control = actuation_map_2d.ActuationMap2D(default_brake_map_path)
        self.accel_delay_step_sim = self.acc_delay_step_sim
        self.brake_delay_step_sim = self.acc_delay_step_sim
        self.use_adaptive_gear_ratio_input = False
        self.use_adaptive_gear_ratio_obs = False

        # self.acc_delay_extrapolator = delay_compensator.LinearExtrapolator()
        # self.steer_delay_extrapolator = delay_compensator.LinearExtrapolator()
        # self.acc_delay_extrapolator = delay_compensator.PolynomialExtrapolator()
        # self.steer_delay_extrapolator = delay_compensator.PolynomialExtrapolator()
        # self.acc_delay_extrapolator = delay_compensator.NNExtrapolator(cmd_mode="acc")
        # self.steer_delay_extrapolator = delay_compensator.NNExtrapolator(cmd_mode="steer")

    def perturbed_sim(self, sim_setting_dict):
        self.sim_setting_dict.clear()
        self.perturbed_sim_flag = False
        if "vehicle_type" in sim_setting_dict.keys():
            if sim_setting_dict["vehicle_type"] not in range(
                len(parameter_change_utils.vehicle_type_params)
            ):
                print(f"invalid vehicle_type: {sim_setting_dict['vehicle_type']}")
                return
            for key_vehicle, value in parameter_change_utils.vehicle_type_params[
                sim_setting_dict["vehicle_type"]
            ].items():
                if key_vehicle not in sim_setting_dict:
                    sim_setting_dict[key_vehicle] = value

        for key in sim_setting_dict.keys():
            if key not in self.nominal_params:
                print(f"invalid key: {key}")
                return

        for key, value in self.nominal_params.items():
            if key not in sim_setting_dict:
                setattr(self, key, value)
            else:
                setattr(self, key, sim_setting_dict[key])
                self.nominal_setting_dict[key] = value
                self.sim_setting_dict[key] = sim_setting_dict[key]
                self.perturbed_sim_flag = True
        if any(
            atr in sim_setting_dict
            for atr in [
                "accel_brake_map_control_path",
                "accel_brake_map_sim_path",
                "accel_time_delay",
                "brake_time_delay",
                "accel_dead_band",
                "brake_dead_band",
            ]
        ):
            self.use_accel_brake_map = True
        else:
            self.use_accel_brake_map = False
        self.acc_delay_step_sim = round(self.acc_time_delay / self.sim_dt)
        self.steer_delay_step_sim = round(self.steer_time_delay / self.sim_dt)
        self.accel_delay_step_sim = round(self.acc_time_delay / self.sim_dt)
        self.brake_delay_step_sim = round(self.brake_time_delay / self.sim_dt)
        # self.acc_delay_extrapolator.set_params(max(self.acc_time_delay - self.nominal_params["acc_time_delay"], 0.0))
        # self.steer_delay_extrapolator.set_params(max(self.steer_time_delay - self.nominal_params["steer_time_delay"], 0.0))

    def setattr(self, data):
        for key, value in data.items():
            setattr(self, key, value)

    def common_initialization(self):
        if self.accel_brake_map_control_path is not None:
            self.accel_map_control = actuation_map_2d.ActuationMap2D(
                self.accel_brake_map_control_path + "/accel_map.csv"
            )
            self.brake_map_control = actuation_map_2d.ActuationMap2D(
                self.accel_brake_map_control_path + "/brake_map.csv"
            )
        if self.accel_brake_map_sim_path is not None:
            self.accel_map_sim = actuation_map_2d.ActuationMap2D(
                self.accel_brake_map_sim_path + "/accel_map.csv"
            )
            self.brake_map_sim = actuation_map_2d.ActuationMap2D(
                self.accel_brake_map_sim_path + "/brake_map.csv"
            )
        self.use_adaptive_gear_ratio_input = False
        self.use_adaptive_gear_ratio_obs = False
        if "adaptive_gear_ratio_coef_control_input" in self.sim_setting_dict:
            self.adaptive_gear_ratio_coef_control_input = self.sim_setting_dict[
                "adaptive_gear_ratio_coef_control_input"
            ]
            self.use_adaptive_gear_ratio_input = True
        if "adaptive_gear_ratio_coef_sim_input" in self.sim_setting_dict:
            self.adaptive_gear_ratio_coef_sim_input = self.sim_setting_dict[
                "adaptive_gear_ratio_coef_sim_input"
            ]
            self.use_adaptive_gear_ratio_input = True
        if "adaptive_gear_ratio_coef_control_obs" in self.sim_setting_dict:
            self.adaptive_gear_ratio_coef_control_obs = self.sim_setting_dict[
                "adaptive_gear_ratio_coef_control_obs"
            ]
            self.use_adaptive_gear_ratio_obs = True
        if "adaptive_gear_ratio_coef_sim_obs" in self.sim_setting_dict:
            self.adaptive_gear_ratio_coef_sim_obs = self.sim_setting_dict[
                "adaptive_gear_ratio_coef_sim_obs"
            ]
            self.use_adaptive_gear_ratio_obs = True
        self.pure_pursuit_controller.initialize(self.sim_dt * self.control_step)

    def initialize_mpc_drive(self, initial_error, course_csv_data, use_vehicle_adaptor):
        self.common_initialization()
        # self.trajectory_data = np.loadtxt("supporting_data/slalom_course_data.csv", delimiter=",")
        # self.trajectory_data = np.loadtxt("supporting_data/mpc_figure_eight_course_data.csv", delimiter=",")
        self.trajectory_data = np.loadtxt("supporting_data/" + course_csv_data, delimiter=",")
        self.trajectory_interpolator_list = [
            scipy.interpolate.interp1d(self.trajectory_data[:, 0], self.trajectory_data[:, 1 + i])
            for i in range(self.trajectory_data.shape[1] - 1)
        ]
        self.states_current = self.trajectory_data[0, 1:7].copy()
        initial_steer = self.measurement_steer_bias
        self.states_current[steer_index] += initial_steer
        self.initial_steer_input = self.states_current[steer_index] / self.steer_scaling
        self.states_current += initial_error
        self.X_des_history.clear()
        self.tracking_error_list.clear()
        self.log_updater.clear_list()
        self.acc_input_queue = [0.0] * self.acc_delay_step_sim
        self.accel_input_queue = [0.0] * self.accel_delay_step_sim
        self.brake_input_queue = [0.0] * self.brake_delay_step_sim
        self.steer_input_queue = [self.initial_steer_input] * self.steer_delay_step_sim
        initial_steer_wheel_input = steer_to_steer_wheel(
            self.states_current[vel_index],
            self.initial_steer_input,
            self.adaptive_gear_ratio_coef_control_input,
        )
        self.steer_wheel_input_queue = [initial_steer_wheel_input] * self.steer_delay_step_sim
        if USE_SMART_MPC_PACKAGE:
            self.controller.send_initialize_input_queue()
        else:
            self.controller.send_initialization_signal()
        if use_vehicle_adaptor:
            self.log_updater.set_data_collection_mode("autonomous_driving_with_vehicle_adaptor")
        else:
            self.log_updater.set_data_collection_mode("autonomous_driving")

    def initialize_pp_eight_drive(self, seed, initial_error):
        self.common_initialization()
        self.figure_eight = data_collection_utils.FigureEight(
            self.circle_y_length,
            self.circle_x_length,
            acc_max=self.target_vel_acc_max,
            constant_vel_time=self.constant_vel_time,
            split_size=self.target_vel_split_size,
            smoothing_trajectory_data_flag=True,  # smoothing_trajectory_data_flag,
        )
        (
            self.trajectory_position_data,
            self.trajectory_yaw_data,
            self.curvature,
            self.parts,
            self.achievement_rates,
        ) = self.figure_eight.get_trajectory_points(0.01)
        self.states_current = np.zeros(6)
        self.states_current[x_index] = self.trajectory_position_data[0][0]
        self.states_current[y_index] = self.trajectory_position_data[0][1]
        self.states_current[vel_index] = self.figure_eight.v_start
        self.states_current[yaw_index] = self.trajectory_yaw_data[0]
        self.states_current += initial_error
        initial_steer = self.measurement_steer_bias
        self.states_current[steer_index] += initial_steer
        self.initial_steer_input = self.states_current[steer_index] / self.steer_scaling
        self.states_current += initial_error
        self.tracking_error_list.clear()
        self.log_updater.clear_list()
        self.acc_input_queue = [0.0] * self.acc_delay_step_sim
        self.accel_input_queue = [0.0] * self.accel_delay_step_sim
        self.brake_input_queue = [0.0] * self.brake_delay_step_sim
        self.steer_input_queue = [self.initial_steer_input] * self.steer_delay_step_sim
        initial_steer_wheel_input = steer_to_steer_wheel(
            self.states_current[vel_index],
            self.initial_steer_input,
            self.adaptive_gear_ratio_coef_control_input,
        )
        self.steer_wheel_input_queue = [initial_steer_wheel_input] * self.steer_delay_step_sim

        self.get_input_noise = data_collection_utils.GetInputNoise()
        self.get_input_noise.set_step_response(
            self.step_response_interval,
            self.step_response_max_input,
            self.step_response_max_length,
            self.step_response_min_length,
        )
        self.get_input_noise.create_additional_sine_data(
            seed,
            [0, self.max_control_time],
            self.acc_amp_range,
            self.acc_period_range,
            self.steer_amp_range,
            self.steer_period_range,
        )
        self.pp_gain_updater = pure_pursuit_gain_updater.pure_pursuit_gain_updater(
            control_dt=self.control_dt
        )
        self.acc_gain_scaling = 1.0
        self.steer_gain_scaling = 1.0  # L/2.79

        self.acc_gain_scaling_decay = 0.9
        self.steer_gain_scaling_decay = 0.9
        self.previous_pp_index = 0

        self.target_vel_list = []
        self.log_updater.set_data_collection_mode("data_collection")

    def F_sim(self, states, inputs, mode="raw_acc"):
        v = states[vel_index]
        yaw = states[yaw_index]
        acc = states[acc_index]
        steer = states[steer_index]
        acc_input = inputs[0]
        steer_input = inputs[1]
        steer_diff = self.steer_scaling * steer_input - steer
        if steer_diff >= self.steer_dead_band:
            steer_diff = steer_diff - self.steer_dead_band
        elif steer_diff <= -self.steer_dead_band:
            steer_diff = steer_diff + self.steer_dead_band
        else:
            steer_diff = 0.0

        acc_diff = self.acc_scaling * acc_input - acc
        if mode == "raw_acc":
            acc_dead_band = self.acc_dead_band
        elif mode == "accel":
            acc_dead_band = self.accel_dead_band
        elif mode == "brake":
            acc_dead_band = self.brake_dead_band
        else:
            print("invalid mode")
            return
        if acc_diff >= acc_dead_band:
            acc_diff = acc_diff - acc_dead_band
        elif acc_diff <= -acc_dead_band:
            acc_diff = acc_diff + acc_dead_band
        else:
            acc_diff = 0.0
        states_dot = np.zeros(6)
        states_dot[x_index] = v * np.cos(yaw)
        states_dot[y_index] = v * np.sin(yaw)
        states_dot[vel_index] = acc
        states_dot[yaw_index] = v / self.wheel_base * np.tan(steer - self.measurement_steer_bias)
        states_dot[acc_index] = acc_diff / self.acc_time_constant
        states_dot[steer_index] = steer_diff / self.steer_time_constant
        states_new = states + states_dot * self.sim_dt
        if states_new[vel_index] < 0.0:
            states_new[vel_index] = 0.0
            states_new[acc_index] = max(0.0, states_new[acc_index])
        return states_new

    def drive_sim(
        self,
        data_collection_seed=None,
        max_control_time=None,
        save_dir=None,
        initial_error=None,
        control_type: ControlType = ControlType.mpc,
        use_vehicle_adaptor=False,
        vehicle_adaptor_model_path="vehicle_model.pth",
        use_offline_features=False,
        offline_feature_dir="vehicle_model",
        states_ref_mode="predict_by_polynomial_regression",
        course_csv_data="slalom_course_data.csv",  # "slalom_course_data.csv" or "mpc_figure_eight_course_data.csv"
        max_lateral_accel=None,
    ):
        # if control_type != ControlType.mpc:
        #    print(f"\n[run {control_type.value}]\n")
        # else:
        #    print("\n[run slalom_drive]\n")
        #    print("mode:", self.controller.mode)
        print(f"\n[run {control_type.value}]\n")
        if initial_error is None:
            initial_error = self.initial_error
            print("initial_error: ", initial_error)
        if self.perturbed_sim_flag:
            print("perturbed_sim: ", self.sim_setting_dict)
            print("nominal_model: ", self.nominal_setting_dict)
        if save_dir is None:
            save_dir = "log_data/python_sim_log_" + str(datetime.datetime.now())
        if data_collection_seed is None:
            data_collection_seed = self.data_collection_seed
        if not os.path.isdir("log_data"):
            os.mkdir("log_data")
        if not os.path.isdir(save_dir):
            os.mkdir(save_dir)
            if self.data_collection:
                os.mkdir(save_dir + "/data_counts_whole_course")
                os.mkdir(save_dir + "/data_counts_on_line")

        if max_control_time is not None:
            self.max_control_time = max_control_time
        if control_type == ControlType.mpc:
            self.initialize_mpc_drive(initial_error, course_csv_data, use_vehicle_adaptor)
        elif control_type == ControlType.pp_eight:
            self.initialize_pp_eight_drive(data_collection_seed, initial_error)
        if use_vehicle_adaptor:
            self.vehicle_adaptor.clear_NN_params()
            self.set_NN_params(vehicle_adaptor_model_path)
            if use_offline_features:
                self.set_offline_features(offline_feature_dir)
            self.vehicle_adaptor.send_initialized_flag()
        prev_u_actual_input = np.zeros(2)
        prev_u_actual_input[1] = self.initial_steer_input
        break_flag = False
        t_current = 0.0
        simulation_num = 0
        prev_acc_obs = None
        prev_vel_obs = None
        while True:
            # steer_wheel_obs = steer_to_steer_wheel(states_for_controller[vel_index], states_for_controller[steer_index], self.adaptive_gear_ratio_coef_sim_obs)
            agr_obs_sim = get_adaptive_gear_ratio_by_steer(
                self.states_current[vel_index],
                self.states_current[steer_index],
                self.adaptive_gear_ratio_coef_sim_obs,
            )
            steer_wheel_obs = agr_obs_sim * self.states_current[steer_index]
            if simulation_num % self.control_step == 0:
                states_for_controller = self.states_current.copy()
                if self.use_adaptive_gear_ratio_obs:
                    agr_obs_control = get_adaptive_gear_ratio_by_steer_wheel(
                        states_for_controller[vel_index],
                        steer_wheel_obs,
                        self.adaptive_gear_ratio_coef_control_obs,
                    )
                    states_for_controller[steer_index] = steer_wheel_obs / agr_obs_control
                    # states_for_controller[steer_index] = steer_wheel_to_steer(states_for_controller[vel_index], steer_wheel_obs, self.adaptive_gear_ratio_coef_control_obs)
                np.random.seed()
                states_for_controller[vel_index] += (
                    np.sqrt(0.5)
                    * self.acc_noise
                    * np.random.randn()
                    * self.sim_dt
                    * self.control_step
                )
                if prev_acc_obs is not None:
                    raw_acc = (states_for_controller[vel_index] - prev_vel_obs) / (
                        self.sim_dt * self.control_step
                    )
                    states_for_controller[acc_index] = (
                        self.acc_smoothing_constant * prev_acc_obs
                        + (1.0 - self.acc_smoothing_constant) * raw_acc
                    )
                prev_acc_obs = states_for_controller[acc_index]
                prev_vel_obs = states_for_controller[vel_index]
                if control_type == ControlType.mpc:
                    X_des, U_des, tracking_error, break_flag = self.get_mpc_trajectory(
                        t_current, states_for_controller
                    )
                    self.X_des_history.append(X_des[0])
                    self.tracking_error_list.append(tracking_error)
                    # print(self.states_current)
                    # print(X_des[:, :6])
                    start_ilqr_time = time.time()
                    if USE_SMART_MPC_PACKAGE:
                        u_opt = self.controller.update_input_queue_and_get_optimal_control(
                            self.log_updater.control_cmd_time_stamp_list,
                            self.log_updater.control_cmd_acc_list,
                            self.log_updater.control_cmd_steer_list,
                            states_for_controller,
                            X_des,
                            U_des,
                            t_current,
                            t_current,
                        )
                    else:
                        u_opt = self.controller.compute_optimal_control(
                            t_current, states_for_controller, X_des[:, :6].T
                        )

                    # print("ilqr time: ", time.time() - start_ilqr_time)

                elif control_type == ControlType.pp_eight:
                    self.pp_gain_updater.state_queue_updater(
                        states_for_controller[vel_index],
                        states_for_controller[yaw_index],
                        states_for_controller[acc_index],
                        states_for_controller[steer_index],
                    )
                    (
                        target_position,
                        target_yaw,
                        nearest_index,  # self.previous_pp_index,
                    ) = data_collection_utils.get_pure_pursuit_info(
                        states_for_controller,
                        self.trajectory_position_data,
                        self.trajectory_yaw_data,
                        self.previous_pp_index,
                    )

                    part = self.parts[nearest_index]  # self.previous_pp_index]
                    achievement_rate = self.achievement_rates[nearest_index]

                    if not self.data_collection:
                        target_vel = self.figure_eight.get_target_velocity(t_current)
                    else:
                        if max_lateral_accel is None:
                            max_lateral_accel = 0.5
                        max_vel_from_lateral_acc = np.sqrt(
                            max_lateral_accel / self.curvature[nearest_index]
                        )
                        target_vel = min(20.0, max_vel_from_lateral_acc)

                        min_index_list = []
                        min_data_num_margin = 10
                        if 0.0 < achievement_rate and achievement_rate < 0.075:

                            N = self.log_updater.num_bins
                            vel_idx = 0
                            acc_idx = 0
                            min_num_data = self.log_updater.collected_data_counts.min()
                            exclude_idx_list = [
                                (0, 0),
                                (1, 0),
                                (2, 0),
                                (0, 1),
                                (1, 1),
                                (2, 1),
                                (0, 2),
                                (1, 2),
                            ]
                            # specified_idx_list = [(6+i,6+j) for i in range(0,3) for j in range(0,3)]
                            for i in range(0, N):
                                for j in range(0, N):
                                    if (i, j) not in exclude_idx_list:
                                        # if (i,j) in specified_idx_list:
                                        if (
                                            min_num_data + min_data_num_margin
                                            > self.log_updater.collected_data_counts[i, j]
                                        ):
                                            min_index_list.append((j, i))

                            acc_idx, vel_idx = min_index_list[
                                np.random.randint(0, len(min_index_list))
                            ]
                            self.target_acc_on_line = self.log_updater.a_bin_centers[acc_idx]
                            self.target_vel_on_line = self.log_updater.v_bin_centers[vel_idx]

                        if part == "linear_positive" or part == "linear_negative":
                            current_vel = states_for_controller[2]

                            if 0.0 <= achievement_rate and achievement_rate < 0.7:
                                delta = 2.0
                                target_vel = self.target_vel_on_line
                                target_vel = np.clip(
                                    target_vel, current_vel - delta, current_vel + delta
                                )

                            elif 0.7 <= achievement_rate and achievement_rate < 1.0:
                                # gain of pure pursuit
                                acc_kp_of_pure_pursuit = self.pure_pursuit_controller.acc_kp
                                target_vel = (
                                    current_vel + self.target_acc_on_line / acc_kp_of_pure_pursuit
                                )  # * np.cos( ( achievement_rate - 0.7 ) / 0.3 * 4.0 * np.pi)
                                target_vel = max(target_vel, 0.5)

                    if max_lateral_accel is not None:
                        max_vel_from_lateral_acc = np.sqrt(
                            max_lateral_accel / self.curvature[nearest_index]
                        )
                        target_vel = min(target_vel, max_vel_from_lateral_acc)

                    self.target_vel_list.append(np.array([t_current, target_vel]))
                    u_opt = self.pure_pursuit_controller.pure_pursuit_control(
                        pos_xy_obs=states_for_controller[:2],
                        pos_yaw_obs=states_for_controller[yaw_index],
                        longitudinal_vel_obs=states_for_controller[vel_index],
                        pos_xy_ref=target_position[:2],
                        pos_yaw_ref=target_yaw,
                        longitudinal_vel_ref=target_vel,
                        acc_gain_scaling=self.acc_gain_scaling,
                        steer_gain_scaling=self.steer_gain_scaling,
                    )
                    u_opt += self.get_input_noise.get_input_noise(t_current)
                    break_flag = self.figure_eight.break_flag

                    self.previous_pp_index = nearest_index

                if use_vehicle_adaptor:
                    start_time = time.time()
                    if (
                        states_ref_mode == "controller_d_inputs_schedule"
                        and control_type == ControlType.mpc
                    ):
                        if USE_SMART_MPC_PACKAGE:
                            nominal_inputs = self.controller.nominal_inputs
                        else:
                            nominal_inputs = self.controller.get_d_inputs_schedule().T
                        self.vehicle_adaptor.set_controller_d_inputs_schedule(
                            nominal_inputs[:, 0], nominal_inputs[:, 1]
                        )
                    if (
                        states_ref_mode == "controller_prediction"
                        and control_type == ControlType.mpc
                    ):
                        if USE_SMART_MPC_PACKAGE:
                            nominal_traj = self.controller.nominal_traj
                        else:
                            # self.vehicle_adaptor.set_controller_prediction(self.controller.nominal_traj[:,2],self.controller.nominal_traj[:,3],self.controller.nominal_traj[:,4], self.controller.nominal_traj[:,5])
                            nominal_traj = self.controller.get_states_prediction().T

                        self.vehicle_adaptor.set_controller_prediction(
                            nominal_traj[:, 0],
                            nominal_traj[:, 1],
                            nominal_traj[:, 2],
                            nominal_traj[:, 3],
                            nominal_traj[:, 4],
                            nominal_traj[:, 5],
                        )  # (self.controller.nominal_traj[:,0],self.controller.nominal_traj[:,1],self.controller.nominal_traj[:,2],self.controller.nominal_traj[:,3],self.controller.nominal_traj[:,4], self.controller.nominal_traj[:,5])
                    if (
                        states_ref_mode == "controller_d_steer_schedule"
                        and control_type == ControlType.mpc
                    ):
                        if USE_SMART_MPC_PACKAGE:
                            nominal_inputs = self.controller.nominal_inputs
                        else:
                            nominal_inputs = self.controller.get_d_inputs_schedule().T
                        self.vehicle_adaptor.set_controller_d_steer_schedule(nominal_inputs[:, 1])
                    if (
                        states_ref_mode == "controller_steer_prediction"
                        and control_type == ControlType.mpc
                    ):
                        if USE_SMART_MPC_PACKAGE:
                            nominal_traj = self.controller.nominal_traj
                        else:
                            nominal_traj = self.controller.get_states_prediction().T
                        self.vehicle_adaptor.set_controller_steer_prediction(nominal_traj[:, 5])

                    u_opt_adjusted = self.vehicle_adaptor.get_adjusted_inputs(
                        t_current, states_for_controller, u_opt[0], u_opt[1]
                    )
                    # print("vehicle_adaptor time: ", time.time() - start_time)
                else:
                    u_opt_adjusted = u_opt.copy()

                    # use controller schedule
                    # if USE_SMART_MPC_PACKAGE:
                    #    nominal_inputs = self.controller.nominal_inputs
                    # else:
                    #    nominal_inputs = self.controller.get_d_inputs_schedule().T
                    # u_opt_adjusted[1] += 2 * nominal_inputs[0,1] * self.control_dt
                    # skip_time = 1 # 0.5sec
                    # skip_time = 2 # 0.6sec
                    # skip_time = 3 # 0.7sec
                    # skip_time = 4 # 0.8sec
                    # skip_time = 5 # 0.9sec
                    # skip_time = 6 # 1.0sec

                    # use linear extrapolator or polynomial extrapolator
                    # for i in range(skip_time):
                    #    u_opt_adjusted[1] += 3 * nominal_inputs[i+1,1] * self.control_dt
                    # u_opt_adjusted[1] += 2 * nominal_inputs[skip_time+1,1] * self.control_dt

                    # acc_adjusted = self.acc_delay_extrapolator.compensate(t_current, u_opt[0])
                    # steer_adjusted = self.steer_delay_extrapolator.compensate(t_current, u_opt[1])
                    # u_opt_adjusted = np.array([acc_adjusted, steer_adjusted])

                    # use NN extrapolator
                    # acc_adjusted = self.acc_delay_extrapolator.compensate(t_current, u_opt[0], states_for_controller[vel_index])
                    # steer_adjusted = self.steer_delay_extrapolator.compensate(t_current, u_opt[1], states_for_controller[vel_index])
                    # u_opt_adjusted = np.array([acc_adjusted, steer_adjusted])
                if control_type == ControlType.mpc:
                    if t_current < 5.0:
                        u_opt_adjusted[1] = self.initial_steer_input
                if self.accel_map_control.is_map_used(
                    states_for_controller[vel_index], u_opt_adjusted[0]
                ):
                    accel_input = self.accel_map_control.get_actuation_cmd(
                        states_for_controller[vel_index], u_opt_adjusted[0]
                    )
                    brake_input = -1e-10
                else:
                    accel_input = -1e-10
                    brake_input = self.brake_map_control.get_actuation_cmd(
                        states_for_controller[vel_index], u_opt_adjusted[0]
                    )
                # steer_wheel_input = steer_to_steer_wheel(states_for_controller[vel_index], u_opt_adjusted[1], self.adaptive_gear_ratio_coef_control_input)
                agr_control_input = get_adaptive_gear_ratio_by_steer_wheel(
                    states_for_controller[vel_index],
                    steer_wheel_obs,
                    self.adaptive_gear_ratio_coef_control_input,
                )
                steer_wheel_input = agr_control_input * u_opt_adjusted[1]
                if control_type == ControlType.mpc:
                    self.log_updater.update(
                        t_current,
                        states_for_controller,
                        u_opt,
                        u_opt_adjusted,
                        accel_input,
                        brake_input,
                    )
                else:
                    self.log_updater.update(
                        t_current,
                        states_for_controller,
                        u_opt,
                        u_opt_adjusted,
                        accel_input,
                        brake_input,
                        part,
                    )
                if control_type == ControlType.pp_eight:
                    self.pp_gain_updater.input_queue_updater(u_opt[0], u_opt[1])
                    if (
                        simulation_num % (100 * self.control_step) == 0 and UPDATE_PP_GAIN
                    ):  # update u_opt
                        self.acc_gain_scaling = (
                            self.acc_gain_scaling_decay * self.acc_gain_scaling
                            + (1 - self.acc_gain_scaling_decay)
                            * self.pp_gain_updater.get_acc_gain_scaling()
                        )

                        self.steer_gain_scaling = (
                            self.steer_gain_scaling_decay * self.steer_gain_scaling
                            + (1 - self.steer_gain_scaling_decay)
                            * self.pp_gain_updater.get_steer_gain_scaling()
                        )
            self.acc_input_queue.append(u_opt_adjusted[0])
            self.accel_input_queue.append(accel_input)
            self.brake_input_queue.append(brake_input)
            self.steer_input_queue.append(u_opt_adjusted[1])
            self.steer_wheel_input_queue.append(steer_wheel_input)
            accel_input_with_delay = self.accel_input_queue.pop(0)
            brake_input_with_delay = self.brake_input_queue.pop(0)
            steer_wheel_input_with_delay = self.steer_wheel_input_queue.pop(0)
            raw_acc_input = self.acc_input_queue.pop(0)
            raw_steer_input = self.steer_input_queue.pop(0)
            if accel_input_with_delay > 0:
                actual_acc_input = self.accel_map_sim.get_sim_actuation(
                    self.states_current[vel_index], accel_input_with_delay
                )
                mode = "accel"
            else:
                actual_acc_input = self.brake_map_sim.get_sim_actuation(
                    self.states_current[vel_index], brake_input_with_delay
                )
                mode = "brake"
            # actual_steer_input = steer_wheel_to_steer(self.states_current[vel_index], steer_wheel_input_with_delay, self.adaptive_gear_ratio_coef_sim_input)
            agr_sim_input = get_adaptive_gear_ratio_by_steer_wheel(
                self.states_current[vel_index],
                steer_wheel_input_with_delay,
                self.adaptive_gear_ratio_coef_sim_input,
            )
            actual_steer_input = steer_wheel_input_with_delay / agr_sim_input
            u_actual_input = np.zeros(2)
            if self.use_accel_brake_map:
                u_actual_input[0] = actual_acc_input
            else:
                u_actual_input[0] = raw_acc_input
                mode = "raw_acc"
            if self.use_adaptive_gear_ratio_input:
                u_actual_input[1] = actual_steer_input
            else:
                u_actual_input[1] = raw_steer_input
            # Logic-level restrictions on control inputs
            delta_steer_max = self.steer_rate_lim * self.sim_dt
            u_actual_input[0] = np.clip(u_actual_input[0], -self.vel_rate_lim, self.vel_rate_lim)
            u_actual_input[1] = np.clip(
                u_actual_input[1],
                prev_u_actual_input[1] - delta_steer_max,
                prev_u_actual_input[1] + delta_steer_max,
            )
            self.states_current = self.F_sim(self.states_current, u_actual_input, mode)
            prev_u_actual_input = u_actual_input.copy()
            if control_type == ControlType.mpc and (simulation_num % 1000 == 999 or break_flag):
                self.save_mpc_drive_record(t_current, save_dir, use_vehicle_adaptor)

            # visualization of pp eight run data via heat map
            if (
                control_type == ControlType.pp_eight
                and (simulation_num % 10000 == 0 or break_flag)
                and self.data_collection
            ):
                self.save_vel_acc_heat(t_current, save_dir)

            simulation_num += 1
            t_current += self.sim_dt
            if t_current >= self.max_control_time:
                break_flag = True

            if break_flag:
                break

        if control_type == ControlType.pp_eight:
            self.save_pp_eight_record(t_current, save_dir)
        self.log_updater.save(save_dir)

    def set_NN_params(self, vehicle_adaptor_model_path):
        if isinstance(vehicle_adaptor_model_path, str):
            vehicle_model = torch.load(vehicle_adaptor_model_path)
            load_dir = vehicle_adaptor_model_path.replace(".pth", "")
            self.set_NN_params_from_model(vehicle_model, load_dir)
        else:
            for path in vehicle_adaptor_model_path:
                vehicle_model = torch.load(path)
                load_dir = path.replace(".pth", "")
                self.set_NN_params_from_model(vehicle_model, load_dir)

    def set_NN_params_from_model(self, vehicle_model, load_dir):

        state_component_predicted_dir = load_dir + "/state_component_predicted.csv"
        state_component_predicted = []
        with open(state_component_predicted_dir, "r", newline="") as f:
            reader = csv.reader(f)
            for row in reader:
                state_component_predicted.extend(row)
        weight_encoder_ih = []
        weight_encoder_hh = []
        bias_encoder_ih = []
        bias_encoder_hh = []
        for i in range(vehicle_model.num_layers_encoder):
            weight_encoder_ih.append(
                vehicle_model.lstm_encoder.__getattr__("weight_ih_l" + str(i))
                .detach()
                .numpy()
                .astype(np.float64)
            )
            weight_encoder_hh.append(
                vehicle_model.lstm_encoder.__getattr__("weight_hh_l" + str(i))
                .detach()
                .numpy()
                .astype(np.float64)
            )
            bias_encoder_ih.append(
                vehicle_model.lstm_encoder.__getattr__("bias_ih_l" + str(i))
                .detach()
                .numpy()
                .astype(np.float64)
            )
            bias_encoder_hh.append(
                vehicle_model.lstm_encoder.__getattr__("bias_hh_l" + str(i))
                .detach()
                .numpy()
                .astype(np.float64)
            )
        self.vehicle_adaptor.set_NN_params(
            vehicle_model.acc_encoder_layer_1[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.steer_encoder_layer_1[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.acc_encoder_layer_2[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.steer_encoder_layer_2[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.acc_layer_1[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.steer_layer_1[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.acc_layer_2[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.steer_layer_2[0].weight.detach().numpy().astype(np.float64),
            weight_encoder_ih,
            weight_encoder_hh,
            vehicle_model.lstm.weight_ih_l0.detach().numpy().astype(np.float64),
            vehicle_model.lstm.weight_hh_l0.detach().numpy().astype(np.float64),
            vehicle_model.complimentary_layer[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.linear_relu[0].weight.detach().numpy().astype(np.float64),
            vehicle_model.final_layer.weight.detach().numpy().astype(np.float64),
            vehicle_model.acc_encoder_layer_1[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.steer_encoder_layer_1[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.acc_encoder_layer_2[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.steer_encoder_layer_2[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.acc_layer_1[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.steer_layer_1[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.acc_layer_2[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.steer_layer_2[0].bias.detach().numpy().astype(np.float64),
            bias_encoder_ih,
            bias_encoder_hh,
            vehicle_model.lstm.bias_ih_l0.detach().numpy().astype(np.float64),
            vehicle_model.lstm.bias_hh_l0.detach().numpy().astype(np.float64),
            vehicle_model.complimentary_layer[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.linear_relu[0].bias.detach().numpy().astype(np.float64),
            vehicle_model.final_layer.bias.detach().numpy().astype(np.float64),
            vehicle_model.vel_scaling,
            vehicle_model.vel_bias,
            state_component_predicted,
        )

    def set_offline_features(self, csv_dir):
        self.vehicle_adaptor.set_offline_features_from_csv(csv_dir)

    def save_pp_eight_record(self, t_current, save_dir):
        t_current = math.ceil(t_current * 100 / 60) / 100
        X = np.array(self.log_updater.X_history)
        U = np.array(self.log_updater.U_history)

        title_str = "control_using_pure_pursuit: "
        if self.perturbed_sim_flag:
            title_str += (
                "perturbed_sim: "
                + str(self.sim_setting_dict)
                + ", nominal_model: "
                + str(self.nominal_setting_dict)
            )
        title_str += " (t=" + str(t_current) + " min )"
        fig, axes = plt.subplots(nrows=3, ncols=3, figsize=(18, 12), tight_layout=True)
        fig.suptitle(title_str)

        ax1: plt.Axes = axes[0, 0]
        ax1.plot(X[:, 0], X[:, 1], label="trajectory")
        ax1.scatter(
            self.trajectory_position_data[:, 0],
            self.trajectory_position_data[:, 1],
            s=4,
            c="orange",
            label="target trajectory",
        )

        ax1.legend()
        ax1.set_xlabel("x_position [m]")
        ax1.set_ylabel("y_position [m]")

        time_normalize_1 = self.control_step * self.sim_dt
        f_s = int(1.0 / time_normalize_1)

        # cSpell:ignore numba simplejson fftfreq basefmt markerfmt
        ax2: plt.Axes = axes[0, 1]
        X_acc = np.fft.fft(U[:, 0]) / len(U[:, 0])  # Fourier transform of waveforms
        freqs = np.fft.fftfreq(len(U[:, 0])) * f_s
        ax2.stem(
            freqs,
            np.abs(X_acc),
            basefmt="k-",
            markerfmt="cx",
            label="acc input",
        )
        ax2.legend()
        ax2.set_xlim([-1.0, 1.0])
        ax2.set_xlabel("freq")
        ax2.set_ylabel("amplitude")

        ax3: plt.Axes = axes[0, 2]
        X_steer = np.fft.fft(U[:, 1]) / len(U[:, 1])  # Fourier transform of waveforms
        freqs = np.fft.fftfreq(len(U[:, 1])) * f_s
        ax3.stem(
            freqs,
            np.abs(X_steer),
            basefmt="k-",
            markerfmt="cx",
            label="steer input",
        )
        ax3.legend()
        ax3.set_xlim([-1.0, 1.0])
        ax3.set_xlabel("freq")
        ax3.set_ylabel("amplitude")

        ax4: plt.Axes = axes[1, 0]
        ax4.plot(
            np.array(self.target_vel_list)[:, 0],
            np.array(self.target_vel_list)[:, 1],
            label="vel target",
        )
        ax4.plot(time_normalize_1 * np.arange(X.shape[0]), X[:, 2], label="vel")
        ax4.legend()

        ax5: plt.Axes = axes[1, 1]
        ax5.plot(time_normalize_1 * np.arange(U.shape[0]), U[:, 0], label="acc input")
        ax5.plot(time_normalize_1 * np.arange(U.shape[0]), X[:, 4], label="acc observed")
        ax5.legend()

        ax6: plt.Axes = axes[1, 2]
        ax6.plot(time_normalize_1 * np.arange(U.shape[0]), U[:, 1], label="steer input")
        ax6.legend()

        # kinematic_states = KinematicStates(
        #    speed=X[:, 2],
        #    acc=X[:, 4],
        #    steer=X[:, 5],
        # )

        # ax7: plt.Axes = axes[2, 0]
        # fig, ax7 = visualize_speed_acc(fig=fig, ax=ax7, kinematic_states=kinematic_states)
        # ax7.plot()

        # ax8: plt.Axes = axes[2, 1]
        # fig, ax8 = visualize_speed_steer(fig=fig, ax=ax8, kinematic_states=kinematic_states)
        # ax8.plot()

        ax9: plt.Axes = axes[2, 2]
        ax9.axis("off")

        plt.savefig(
            save_dir
            + "/python_simulator_pure_pursuit_drive"
            + " (t="
            + str(t_current)
            + " min)"
            + ".png"
        )
        plt.close()

    def save_mpc_drive_record(self, t_current, save_dir, use_vehicle_adaptor):
        fig = plt.figure(figsize=(24, 15), tight_layout=True)

        if not use_vehicle_adaptor:
            title_str = "control_using_nominal_model: "
        else:
            title_str = "control_with_vehicle_adaptor: "
        if self.perturbed_sim_flag:
            title_str += (
                "perturbed_sim: "
                + str(self.sim_setting_dict)
                + ", nominal_model: "
                + str(self.nominal_setting_dict)
            )
        fig.suptitle(title_str)

        plt.subplot(3, 3, 1)

        ax1 = plt.subplot(3, 3, 1)
        X = np.array(self.log_updater.X_history)
        U = np.array(self.log_updater.U_history)
        X_des_hist = np.array(self.X_des_history)
        time_normalize_1 = self.control_step * self.sim_dt
        time_normalize_2 = self.control_step * self.predict_step * self.sim_dt
        if USE_SMART_MPC_PACKAGE:
            nominal_traj = self.controller.nominal_traj
        else:
            nominal_traj = self.controller.get_states_prediction().T

        if use_vehicle_adaptor:
            vehicle_adaptor_inputs_hist = np.array(
                self.log_updater.vehicle_command_control_cmd_list
            )[:, [16, 8]]
            true_states_prediction = self.F_true_prediction(
                X[-1],
                vehicle_adaptor_inputs_hist[-1],
                self.vehicle_adaptor.get_d_inputs_schedule().T,
            )
            vehicle_adaptor_states_prediction = self.vehicle_adaptor.get_states_prediction().T
        ax1.scatter(
            self.trajectory_data[:, 1],
            self.trajectory_data[:, 2],
            s=4,
            c="orange",
            label="reference_trajectory",
        )
        ax1.plot(X[:, 0], X[:, 1], label="trajectory", color="tab:blue")
        ax1.scatter(
            nominal_traj[:, 0],
            nominal_traj[:, 1],
            s=4,
            c="red",
            label="pred_trajectory_ilqr",
        )
        if use_vehicle_adaptor:
            ax1.scatter(
                true_states_prediction[:, 0],
                true_states_prediction[:, 1],
                s=4,
                c="green",
                label="true_prediction",
            )
            ax1.scatter(
                vehicle_adaptor_states_prediction[:, 0],
                vehicle_adaptor_states_prediction[:, 1],
                s=4,
                c="purple",
                label="vehicle_adaptor_prediction",
            )

        ax1.legend()
        ax1.set_xlabel("x_position [m]")
        ax1.set_ylabel("y_position [m]")

        ax2 = plt.subplot(3, 3, 2)
        tracking_error_array = np.array(self.tracking_error_list)
        lateral_deviation = (
            -np.sin(X_des_hist[:, 3]) * tracking_error_array[:, 0]
            + np.cos(X_des_hist[:, 3]) * tracking_error_array[:, 1]
        )

        ax2.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            lateral_deviation,
            label="lateral_deviation",
        )

        ax2_coef = [0.00, 0.05, -0.05, 0.10, -0.10, 0.15, -0.15, 0.20, -0.20]
        for coe in ax2_coef:
            ax2.plot(
                time_normalize_1 * np.arange(tracking_error_array.shape[0]),
                coe * np.ones(tracking_error_array.shape[0]),
                linestyle="dashed",
            )

        ax2.set_xlabel("Time [s]")
        ax2.set_ylabel("Lateral deviation [m]")
        ax2.legend()

        straight_line_index = np.where(X_des_hist[:, 0] < 250.0)[
            0
        ].max()  # Before 250 [m] is considered to be a straight run.
        total_abs_max_lateral_deviation = np.abs(lateral_deviation).max()
        total_abs_mean_lateral_deviation = np.abs(lateral_deviation).mean()
        straight_line_abs_max_lateral_deviation = np.abs(
            lateral_deviation[: straight_line_index + 1]
        ).max()
        ax2.set_title(
            "abs_max(lateral_dev) = "
            + str(total_abs_max_lateral_deviation)
            + "\nabs_max(lateral_dev[straight_line]) = "
            + str(straight_line_abs_max_lateral_deviation)
        )

        ax3 = plt.subplot(3, 3, 4)
        ax3.plot(
            time_normalize_1 * np.arange(X.shape[0]),
            X[:, 2],
            label="velocity",
            color="tab:blue",
        )
        ax3.plot(
            time_normalize_1 * np.arange(X.shape[0]),
            X_des_hist[:, 2],
            label="velocity_target",
            color="orange",
        )
        ax3.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            -tracking_error_array[:, 2],
            label="velocity_error",
            color="lightgrey",
        )
        if use_vehicle_adaptor:
            ax3.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(true_states_prediction.shape[0]),
                true_states_prediction[:, 2],
                s=4,
                label="velocity_true_prediction",
                c="green",
            )
            ax3.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(vehicle_adaptor_states_prediction.shape[0]),
                vehicle_adaptor_states_prediction[:, 2],
                s=4,
                label="velocity_vehicle_adaptor_prediction",
                c="purple",
            )
        ax3.scatter(
            time_normalize_1 * (X.shape[0] - 1)
            + time_normalize_2 * np.arange(nominal_traj.shape[0]),
            nominal_traj[:, 2],
            s=4,
            label="velocity_prediction_ilqr",
            c="red",
        )
        ax3.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            np.zeros(tracking_error_array.shape[0]),
            linestyle="dashed",
            color="darkred",
        )
        ax3.set_xlabel("Time [s]")
        ax3.set_ylabel("velocity [m/s]")
        ax3.legend()
        total_abs_max_velocity_error = np.abs(tracking_error_array[:, 2]).max()
        total_abs_mean_velocity_error = np.abs(tracking_error_array[:, 2]).mean()
        straight_line_abs_max_velocity_error = np.abs(
            tracking_error_array[: straight_line_index + 1, 2]
        ).max()
        ax3.set_title(
            "abs_max(velocity_error) = "
            + str(total_abs_max_velocity_error)
            + "\nabs_max(velocity_error[straight_line]) = "
            + str(straight_line_abs_max_velocity_error)
        )

        ax4 = plt.subplot(3, 3, 5)
        ax4.plot(time_normalize_1 * np.arange(X.shape[0]), X[:, 3], label="yaw", color="tab:blue")
        ax4.plot(
            time_normalize_1 * np.arange(X.shape[0]),
            X_des_hist[:, 3],
            label="yaw_target",
            color="orange",
        )
        ax4.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            -tracking_error_array[:, 3],
            label="yaw_error",
            color="lightgrey",
        )
        if use_vehicle_adaptor:
            ax4.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(true_states_prediction.shape[0]),
                true_states_prediction[:, 3],
                s=4,
                label="yaw_true_prediction",
                c="green",
            )
            ax4.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(vehicle_adaptor_states_prediction.shape[0]),
                vehicle_adaptor_states_prediction[:, 3],
                s=4,
                label="yaw_vehicle_adaptor_prediction",
                c="purple",
            )
        ax4.scatter(
            time_normalize_1 * (X.shape[0] - 1)
            + time_normalize_2 * np.arange(nominal_traj.shape[0]),
            nominal_traj[:, 3],
            s=4,
            label="yaw_prediction_ilqr",
            c="red",
        )
        ax4.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            np.zeros(tracking_error_array.shape[0]),
            linestyle="dashed",
            c="darkred",
        )
        ax4.set_xlabel("Time [s]")
        ax4.set_ylabel("yaw [rad]")
        ax4.legend()
        total_abs_max_yaw_error = np.abs(tracking_error_array[:, 3]).max()
        total_abs_mean_yaw_error = np.abs(tracking_error_array[:, 3]).mean()
        straight_line_abs_max_yaw_error = np.abs(
            tracking_error_array[: straight_line_index + 1, 3]
        ).max()
        ax4.set_title(
            "abs_max(yaw_error) = "
            + str(total_abs_max_yaw_error)
            + "\nabs_max(yaw_error[straight_line]) = "
            + str(straight_line_abs_max_yaw_error)
        )

        ax5 = plt.subplot(3, 3, 7)
        ax5.plot(time_normalize_1 * np.arange(X.shape[0]), X[:, 4], label="acc", color="tab:blue")
        ax5.plot(
            time_normalize_1 * np.arange(X.shape[0]), U[:, 0], label="acc_input", color="violet"
        )
        ax5.plot(
            time_normalize_1 * np.arange(X.shape[0]),
            X_des_hist[:, 4],
            label="acc_target",
            color="orange",
        )
        ax5.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            -tracking_error_array[:, 4],
            label="acc_error",
            color="lightgrey",
        )
        if use_vehicle_adaptor:
            ax5.plot(
                time_normalize_1 * np.arange(X.shape[0]),
                vehicle_adaptor_inputs_hist[:, 0],
                label="acc_vehicle_adaptor_input",
                color="purple",
            )
            ax5.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(true_states_prediction.shape[0]),
                true_states_prediction[:, 4],
                s=4,
                label="acc_true_prediction",
                c="green",
            )
            ax5.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(vehicle_adaptor_states_prediction.shape[0]),
                vehicle_adaptor_states_prediction[:, 4],
                s=4,
                label="acc_vehicle_adaptor_prediction",
                c="purple",
            )
        ax5.scatter(
            time_normalize_1 * (X.shape[0] - 1)
            + time_normalize_2 * np.arange(nominal_traj.shape[0]),
            nominal_traj[:, 4],
            s=4,
            label="acc_prediction_ilqr",
            c="red",
        )
        ax5.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            np.zeros(tracking_error_array.shape[0]),
            linestyle="dashed",
            color="darkred",
        )
        ax5.set_xlabel("Time [s]")
        ax5.set_ylabel("acc [m/s^2]")
        ax5.legend()
        total_abs_max_acc_error = np.abs(tracking_error_array[:, 4]).max()
        total_abs_mean_acc_error = np.abs(tracking_error_array[:, 4]).mean()
        straight_line_abs_max_acc_error = np.abs(
            tracking_error_array[: straight_line_index + 1, 4]
        ).max()
        ax5.set_title(
            "abs_max(acc_error) = "
            + str(total_abs_max_acc_error)
            + "\nabs_max(acc_error[straight_line]) = "
            + str(straight_line_abs_max_acc_error)
        )

        ax6 = plt.subplot(3, 3, 8)
        ax6.plot(time_normalize_1 * np.arange(X.shape[0]), X[:, 5], label="steer", color="tab:blue")
        ax6.plot(
            time_normalize_1 * np.arange(X.shape[0]),
            U[:, 1],
            label="steer_input",
            color="violet",
        )
        ax6.plot(
            time_normalize_1 * np.arange(X.shape[0]),
            X_des_hist[:, 7],
            label="steer_mpc_target",
            color="orange",
        )
        ax6.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            -tracking_error_array[:, 5],
            label="steer_error",
            color="lightgrey",
        )
        if use_vehicle_adaptor:
            ax6.plot(
                time_normalize_1 * np.arange(X.shape[0]),
                vehicle_adaptor_inputs_hist[:, 1],
                label="steer_vehicle_adaptor_input",
                color="purple",
            )
            ax6.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(true_states_prediction.shape[0]),
                true_states_prediction[:, 5],
                s=4,
                label="steer_true_prediction",
                c="green",
            )
            ax6.scatter(
                time_normalize_1 * (X.shape[0] - 1)
                + time_normalize_2 * np.arange(vehicle_adaptor_states_prediction.shape[0]),
                vehicle_adaptor_states_prediction[:, 5],
                s=4,
                label="steer_vehicle_adaptor_prediction",
                c="purple",
            )
        ax6.scatter(
            time_normalize_1 * (X.shape[0] - 1)
            + time_normalize_2 * np.arange(nominal_traj.shape[0]),
            nominal_traj[:, 5],
            s=4,
            label="steer_prediction_ilqr",
            c="red",
        )
        ax6.plot(
            time_normalize_1 * np.arange(tracking_error_array.shape[0]),
            np.zeros(tracking_error_array.shape[0]),
            linestyle="dashed",
            color="darkred",
        )
        ax6.set_xlabel("Time [s]")
        ax6.set_ylabel("steer [rad]")
        ax6.legend()
        total_abs_max_steer_error = np.abs(tracking_error_array[:, 5]).max()
        total_abs_mean_steer_error = np.abs(tracking_error_array[:, 5]).mean()
        straight_line_abs_max_steer_error = np.abs(
            tracking_error_array[: straight_line_index + 1, 5]
        ).max()
        ax6.set_title(
            "abs_max(steer_error) = "
            + str(total_abs_max_steer_error)
            + "\nabs_max(steer_error[straight_line]) = "
            + str(straight_line_abs_max_steer_error)
        )

        ax7 = plt.subplot(3, 3, 3)
        f_s = int(1.0 / time_normalize_1)

        steer_state = X[:, 5]
        X_steer = np.fft.fft(steer_state)  # Fourier transform of waveforms
        freqs = np.fft.fftfreq(len(steer_state)) * f_s
        ax7.stem(
            freqs,
            np.abs(X_steer) / len(steer_state),
            basefmt="k-",
            markerfmt="rx",
            label="steer_state",
        )

        lateral_acc = X[:, 2] * X[:, 2] * np.tan(X[:, 5]) / self.wheel_base
        steer_dot = np.zeros(X.shape[0])
        steer_dot[0] = (X[1, 5] - X[0, 5]) / time_normalize_1
        steer_dot[-1] = (X[-1, 5] - X[-2, 5]) / time_normalize_1
        steer_dot[1:-1] = 0.5 * (X[2:, 5] - X[:-2, 5]) / time_normalize_1
        lateral_jerk = 2 * X[:, 2] * np.tan(X[:, 5]) * X[:, 4] / self.wheel_base + X[:, 2] * X[
            :, 2
        ] * steer_dot / (self.wheel_base * np.cos(X[:, 5]) * np.cos(X[:, 5]))

        X_lateral_acc = np.fft.fft(lateral_acc)  # Fourier transform of waveforms
        freqs = np.fft.fftfreq(len(lateral_acc)) * f_s
        ax7.stem(
            freqs,
            np.abs(X_lateral_acc) / len(lateral_acc),
            basefmt="k-",
            markerfmt="gx",
            label="lateral_acc",
        )

        X_lateral_jerk = np.fft.fft(lateral_jerk)  # Fourier transform of waveforms
        freqs = np.fft.fftfreq(len(lateral_jerk)) * f_s
        ax7.stem(
            freqs,
            np.abs(X_lateral_jerk) / len(lateral_jerk),
            basefmt="k-",
            markerfmt="bx",
            label="lateral_jerk",
        )

        ax7.set_xlabel("Frequency in Hertz[Hz]")
        ax7.set_ylabel("Amplitude (normalized dividing by data length)")
        ax7.set_xlim(-1, 1)
        ax7.legend()

        # if use_accel_map:
        #    ax9 = plt.subplot(3, 3, 9, projection="3d")
        #    X1, X2 = np.meshgrid(current_vel_axis, accel_cmd_axis)
        #    ax9.plot_surface(X1, X2, accel_map_data)

        if not use_vehicle_adaptor:
            save_path = (
                save_dir
                + "/python_simulator_nominal_model_fig_"
                + str(round(t_current, 3))
                + ".png"
            )
        else:
            save_path = (
                save_dir
                + "/python_simulator_vehicle_adaptor_fig_"
                + str(round(t_current, 3))
                + ".png"
            )
        plt.savefig(save_path)
        plt.close()
        print(
            "save:",
            save_path + ", total_abs_max_lateral_deviation:",
            total_abs_max_lateral_deviation,
        )

    def save_vel_acc_heat(self, t_current, save_dir):
        sec_t = t_current
        t_current = math.ceil(t_current * 100 / 60) / 100
        fig, ax = plt.subplots()

        sns.set(rc={"figure.figsize": (15, 15)})
        sns.heatmap(
            np.array(self.log_updater.collected_data_counts),
            annot=True,
            cmap="coolwarm",
            xticklabels=np.round(self.log_updater.a_bin_centers, 2),
            yticklabels=np.round(self.log_updater.v_bin_centers, 2),
            ax=ax,
            linewidths=0.1,
            linecolor="gray",
            norm=LogNorm(),
        )

        ax.set_xlabel("Acceleration bins")
        ax.set_ylabel("Velocity bins")
        ax.set_title(
            "Counts of Observations in Each Grid Cell on Whole Course(" + str(t_current) + " min)"
        )

        plt.savefig(
            save_dir
            + "/data_counts_whole_course/vel_acc_counts_on_course_("
            + str(t_current)
            + " min).png"
        )
        plt.close(fig)

        fig, ax = plt.subplots()

        sns.set(rc={"figure.figsize": (15, 15)})
        sns.heatmap(
            np.array(self.log_updater.collected_data_counts_on_line),
            annot=True,
            cmap="coolwarm",
            xticklabels=np.round(self.log_updater.a_bin_centers, 2),
            yticklabels=np.round(self.log_updater.v_bin_centers, 2),
            ax=ax,
            linewidths=0.1,
            linecolor="gray",
            norm=LogNorm(),
        )

        ax.set_xlabel("Acceleration bins")
        ax.set_ylabel("Velocity bins")
        ax.set_title(
            "Counts of Observations in Each Grid Cell on Line (" + str(t_current) + " min)"
        )

        plt.savefig(
            save_dir + "/data_counts_on_line/vel_acc_counts_on_line(" + str(t_current) + " min).png"
        )
        plt.close(fig)

    def get_mpc_trajectory(self, t_current, states_for_controller):
        """Calculate the target trajectory to be used in MPC from the given data."""
        nearest_index = np.argmin(
            ((self.trajectory_data[:, 1:3] - states_for_controller[:2].reshape(1, 2)) ** 2).sum(
                axis=1
            )
            + (self.trajectory_data[:, 0] - t_current) ** 2
        )
        timestamp_mpc_trajectory = (
            np.arange(self.horizon_len + 1) * self.predict_step * self.control_step * self.sim_dt
            + self.trajectory_data[nearest_index, 0]
        )
        break_flag = False
        if timestamp_mpc_trajectory[-1] >= self.trajectory_data[-1, 0]:
            timestamp_mpc_trajectory -= (
                timestamp_mpc_trajectory[-1] - self.trajectory_data[-1, 0] - 1e-16
            )
            break_flag = True

        mpc_trajectory = np.array(
            [
                self.trajectory_interpolator_list[i](timestamp_mpc_trajectory)
                for i in range(self.trajectory_data.shape[1] - 1)
            ]
        ).T
        mpc_trajectory = np.hstack(
            [
                timestamp_mpc_trajectory.reshape(-1, 1) - self.trajectory_data[nearest_index, 0],
                mpc_trajectory,
            ]
        )

        X_des = np.zeros((self.horizon_len + 1, 8))
        X_des[:, :6] = mpc_trajectory[:, 1:7]
        X_des[:, [6, 7]] = mpc_trajectory[:, [5, 6]]
        U_des = np.zeros((self.horizon_len, 2))
        return (
            X_des,
            U_des,
            self.states_current[:6] - self.trajectory_data[nearest_index, 1:7],
            break_flag,
        )

    def F_true_prediction(
        self,
        states,
        inputs,
        d_inputs_schedule,
    ):
        true_states_prediction = np.zeros((d_inputs_schedule.shape[0] + 1, states.shape[0]))
        tmp_states = states.copy()
        tmp_inputs = inputs.copy()
        for i in range(d_inputs_schedule.shape[0]):
            for j in range(self.predict_step):
                if i != 0 or j != 0:
                    tmp_inputs += d_inputs_schedule[i] * self.sim_dt * self.control_step
                for _ in range(self.control_step):
                    if self.use_accel_brake_map:
                        if self.accel_map_control.is_map_used(tmp_states[vel_index], tmp_inputs[0]):
                            accel_input = self.accel_map_control.get_actuation_cmd(
                                tmp_states[vel_index], tmp_inputs[0]
                            )
                            brake_input = 0.0
                        else:
                            accel_input = -0.1
                            brake_input = self.brake_map_control.get_actuation_cmd(
                                tmp_states[vel_index], tmp_inputs[0]
                            )
                        if accel_input > 0:
                            actual_acc_input = self.accel_map_sim.get_sim_actuation(
                                tmp_states[vel_index], accel_input
                            )
                            mode = "accel"
                        else:
                            actual_acc_input = self.brake_map_sim.get_sim_actuation(
                                tmp_states[vel_index], brake_input
                            )
                            mode = "brake"
                        actual_tmp_inputs = tmp_inputs.copy()
                        actual_tmp_inputs[0] = actual_acc_input
                    else:
                        actual_tmp_inputs = tmp_inputs.copy()
                        mode = "raw_acc"
                    tmp_states = self.F_sim(tmp_states, actual_tmp_inputs, mode)

            true_states_prediction[i + 1] = tmp_states
        return true_states_prediction


def get_adaptive_gear_ratio_by_steer(vel, steer, adaptive_gear_ratio_coef):
    ratio = (adaptive_gear_ratio_coef[0] + adaptive_gear_ratio_coef[1] * vel * vel) / (
        1 + adaptive_gear_ratio_coef[2] * np.abs(steer)
    )
    return ratio


def get_adaptive_gear_ratio_by_steer_wheel(vel, steer_wheel, adaptive_gear_ratio_coef):
    ratio = (
        adaptive_gear_ratio_coef[0]
        + adaptive_gear_ratio_coef[1] * vel * vel
        - adaptive_gear_ratio_coef[2] * np.abs(steer_wheel)
    )
    return ratio


def steer_wheel_to_steer(vel, steer_wheel, adaptive_gear_ratio_coef):
    ratio = (
        adaptive_gear_ratio_coef[0]
        + adaptive_gear_ratio_coef[1] * vel * vel
        - adaptive_gear_ratio_coef[2] * np.abs(steer_wheel)
    )
    return steer_wheel / ratio


def steer_to_steer_wheel(vel, steer, adaptive_gear_ratio_coef):
    ratio = (adaptive_gear_ratio_coef[0] + adaptive_gear_ratio_coef[1] * vel * vel) / (
        1 + adaptive_gear_ratio_coef[2] * np.abs(steer)
    )
    return ratio * steer
