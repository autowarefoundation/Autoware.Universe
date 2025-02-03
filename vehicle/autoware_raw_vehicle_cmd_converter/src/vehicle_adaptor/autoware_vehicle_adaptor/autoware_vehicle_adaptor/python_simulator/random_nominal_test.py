import gc
import os
import random

import python_simulator
import simplejson as json
from utils.data_collection_utils import ControlType

iter_num = 100
simulator = python_simulator.PythonSimulator()
if not os.path.isdir("log_data"):
    os.mkdir("log_data")
if not os.path.isdir("log_data/nominal_test"):
    os.mkdir("log_data/nominal_test")
base_dir = "log_data/nominal_test/"

steer_dead_band_range = [0.0008, 0.0018]
accel_dead_band_range = [0.0, 0.03]
brake_dead_band_range = [0.0, 0.03]
accel_time_delay_range = [0.1, 0.5]
brake_time_delay_range = [0.1, 0.5]
steer_time_delay_range = [0.1, 0.4]
acc_time_constant_range = [0.1, 0.5]
steer_time_constant_range = [0.1, 0.4]
acc_noise_range = [0.05, 0.12]
accel_brake_map_control_path_candidates = [
    "../actuation_cmd_maps/accel_brake_maps/" + str(i) for i in range(1, 11)
]
accel_brake_map_sim_path_candidates = [
    "../actuation_cmd_maps/accel_brake_maps/" + str(i) for i in range(1, 11)
]


sim_setting_dict = {}
for i in range(iter_num):
    sim_setting_dict["steer_dead_band"] = random.uniform(
        steer_dead_band_range[0], steer_dead_band_range[1]
    )
    sim_setting_dict["accel_dead_band"] = random.uniform(
        accel_dead_band_range[0], accel_dead_band_range[1]
    )
    sim_setting_dict["brake_dead_band"] = random.uniform(
        brake_dead_band_range[0], brake_dead_band_range[1]
    )
    sim_setting_dict["accel_time_delay"] = random.uniform(
        accel_time_delay_range[0], accel_time_delay_range[1]
    )
    sim_setting_dict["steer_time_delay"] = random.uniform(
        steer_time_delay_range[0], steer_time_delay_range[1]
    )
    sim_setting_dict["acc_time_constant"] = random.uniform(
        acc_time_constant_range[0], acc_time_constant_range[1]
    )
    sim_setting_dict["steer_time_constant"] = random.uniform(
        steer_time_constant_range[0], steer_time_constant_range[1]
    )
    sim_setting_dict["acc_noise"] = random.uniform(acc_noise_range[0], acc_noise_range[1])
    sim_setting_dict["accel_brake_map_control_path"] = random.choice(
        accel_brake_map_control_path_candidates
    )
    sim_setting_dict["accel_brake_map_sim_path"] = random.choice(
        accel_brake_map_sim_path_candidates
    )
    simulator.perturbed_sim(sim_setting_dict)

    save_dir = base_dir + "iter_" + str(i)
    if not os.path.isdir(save_dir):
        os.mkdir(save_dir)
    with open(save_dir + "/sim_setting_dict.json", "w") as f:
        json.dump(sim_setting_dict, f)
    simulator.drive_sim(
        max_control_time=900.0,
        control_type=ControlType.pp_eight,
        save_dir=save_dir + "/pp_eight",
    )
    simulator.drive_sim(
        max_control_time=120.0,
        save_dir=save_dir + "/slalom",
        course_csv_data="slalom_course_data.csv",
    )
    simulator.drive_sim(
        max_control_time=200.0,
        save_dir=save_dir + "/mpc_eight",
        course_csv_data="mpc_figure_eight_course_data.csv",
    )
    simulator.drive_sim(
        max_control_time=100.0,
        save_dir=save_dir + "/mpc_straight",
        course_csv_data="mpc_straight_line_course_data.csv",
    )
    gc.collect()
