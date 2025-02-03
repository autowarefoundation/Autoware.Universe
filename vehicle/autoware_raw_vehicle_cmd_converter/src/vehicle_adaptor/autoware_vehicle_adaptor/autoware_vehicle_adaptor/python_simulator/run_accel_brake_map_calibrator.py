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

SKIP_DATA_COLLECTION = False
SKIP_CALIBRATION = False

map_accel = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5]
map_brake = [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
map_vel = [0.0, 1.39, 2.78, 4.17, 5.56, 6.94, 8.33, 9.72, 11.11, 12.5, 13.89]

root_dir = "log_data/test_accel_brake_map_calibrator"
if not os.path.isdir("log_data"):
    os.mkdir("log_data")
if not os.path.isdir(root_dir):
    os.mkdir(root_dir)
train_dir = root_dir + "/test_python_pure_pursuit_train"
test_dir = root_dir + "/test_python_pure_pursuit_test"
test_nominal_dir = root_dir + "/test_nominal"
test_NN_dir = root_dir + "/test_NN"
correct_map_dir = "../actuation_cmd_maps/accel_brake_maps/default_parameter"
low_quality_map_dir = "../actuation_cmd_maps/accel_brake_maps/low_quality_map"
sim_setting_dict = {}
sim_setting_dict["accel_brake_map_control_path"] = low_quality_map_dir
simulator = python_simulator.PythonSimulator()
simulator.perturbed_sim(sim_setting_dict)

if not SKIP_DATA_COLLECTION:
    y_length = 60.0
    x_length = 120.0
    figure_eight_data = {
        "circle_x_length": x_length,
        "circle_y_length": y_length,
    }
    simulator.setattr(figure_eight_data)
    simulator.drive_sim(
        data_collection_seed=0,
        max_control_time=900.0,
        control_type=ControlType.pp_eight,
        save_dir=train_dir,
    )
    figure_eight_data = {
        "circle_x_length": 0.9 * x_length,
        "circle_y_length": 0.9 * y_length,
    }
    simulator.setattr(figure_eight_data)
    simulator.drive_sim(
        data_collection_seed=1,
        max_control_time=900.0,
        control_type=ControlType.pp_eight,
        save_dir=test_dir,
    )
if not SKIP_CALIBRATION:
    calibrator = accel_brake_map_calibrator.Calibrator()
    tester = accel_brake_map_calibrator.Calibrator()
    calibrator.add_data_from_csv(train_dir)
    tester.add_data_from_csv(test_dir)

    print("calibrate by NN")
    calibrator.calibrate_by_NN()
    calibrator.save_accel_brake_map_NN(map_vel, map_accel, map_brake, test_NN_dir)

    print("nominal map")
    print("train data error")
    calibrator.calc_calibration_error(low_quality_map_dir)
    print("test data error")
    tester.calc_calibration_error(low_quality_map_dir)
    print("NN map")
    print("train data error")
    calibrator.calc_calibration_error(test_NN_dir)
    print("test data error")
    tester.calc_calibration_error(test_NN_dir)

print("nominal test")
simulator.drive_sim(save_dir=test_nominal_dir)
print("NN test")
sim_setting_dict["accel_brake_map_control_path"] = test_NN_dir
simulator.perturbed_sim(sim_setting_dict)
simulator.drive_sim(save_dir=test_NN_dir)
print("Correct map test")
sim_setting_dict["accel_brake_map_control_path"] = correct_map_dir
simulator.perturbed_sim(sim_setting_dict)
simulator.drive_sim(save_dir=root_dir + "correct_map")
