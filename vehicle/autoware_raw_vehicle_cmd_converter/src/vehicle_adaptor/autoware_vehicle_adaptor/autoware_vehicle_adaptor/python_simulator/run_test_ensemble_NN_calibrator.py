# Copyright 2024 Proxima Technology Inc, TIER IV
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

import os

from autoware_vehicle_adaptor.calibrator import accel_brake_map_calibrator
import numpy as np
import python_simulator
from utils.data_collection_utils import ControlType

given_map_dir = "../actuation_cmd_maps/accel_brake_maps/1"
sim_map_dir = "../actuation_cmd_maps/accel_brake_maps/9"
# given_map_dir = "../actuation_cmd_maps/accel_brake_maps/low_quality_map"
# sim_map_dir = "../actuation_cmd_maps/accel_brake_maps/default_parameter"

simulator = python_simulator.PythonSimulator()
simulator.data_collection = True
root_dir = "log_data/test_ensemble_NN_calibrator"
calibrator = accel_brake_map_calibrator.Calibrator()

if not os.path.isdir("log_data"):
    os.mkdir("log_data")
if not os.path.isdir(root_dir):
    os.mkdir(root_dir)
sim_setting_dict = {}
sim_setting_dict["accel_brake_map_sim_path"] = sim_map_dir
tmp_map_dir = given_map_dir
for i in range(5):
    sim_setting_dict["accel_brake_map_control_path"] = tmp_map_dir
    simulator.perturbed_sim(sim_setting_dict)
    simulator.drive_sim(save_dir=root_dir + "/test_by_mpc_" + str(i))
    simulator.log_updater.clear_list()
    tmp_train_dir = root_dir + "/test_pure_pursuit_" + str(i)
    simulator.drive_sim(
        control_type=ControlType.pp_eight,
        max_control_time=1500,
        save_dir=tmp_train_dir,
        max_lateral_accel=0.5,
    )
    calibrator.add_data_from_csv(tmp_train_dir)
    # calibrator.extract_data_for_calibration()
    calibrator.outlier_exclusion_by_linear_regression()
    calibrator.calc_data_loader_weights()
    calibrator.calc_base_map_error(given_map_dir)
    calibrator.calibrate_by_ensemble_NN()
    calibrator.save_accel_brake_map_ensemble_NN(
        save_dir=tmp_train_dir, save_heat_map=True, true_map_dir=sim_map_dir
    )
    tmp_map_dir = tmp_train_dir
    simulator.log_updater.clear_list()

sim_setting_dict["accel_brake_map_control_path"] = tmp_map_dir
simulator.perturbed_sim(sim_setting_dict)
simulator.drive_sim(save_dir=root_dir + "/test_by_mpc_final")
