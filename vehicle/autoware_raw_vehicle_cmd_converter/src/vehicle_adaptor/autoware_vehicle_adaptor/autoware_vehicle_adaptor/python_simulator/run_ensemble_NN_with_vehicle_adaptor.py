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
from autoware_vehicle_adaptor.training import train_error_prediction_NN
import numpy as np
import python_simulator
from utils.data_collection_utils import ControlType

SKIP_CALIBRATION = False
SKIP_VEHICLE_ADAPTOR_TRAINING = False

low_quality_map_dir = "../actuation_cmd_maps/accel_brake_maps/low_quality_map"
simulator = python_simulator.PythonSimulator()
simulator.data_collection = True
root_dir = "log_data/test_ensemble_NN_calibrator_with_vehicle_adaptor"
calibrator = accel_brake_map_calibrator.Calibrator()

if not os.path.isdir("log_data"):
    os.mkdir("log_data")
if not os.path.isdir(root_dir):
    os.mkdir(root_dir)


sim_setting_dict = {}
sim_setting_dict["accel_brake_map_control_path"] = low_quality_map_dir
simulator.perturbed_sim(sim_setting_dict)
train_dir = root_dir + "/test_pure_pursuit_train"
val_dir = root_dir + "/test_pure_pursuit_val"
if not SKIP_CALIBRATION:
    simulator.drive_sim(
        control_type=ControlType.pp_eight,
        max_control_time=1500,
        save_dir=train_dir,
        max_lateral_accel=0.5,
    )
    calibrator.add_data_from_csv(train_dir)
    calibrator.extract_data_for_calibration()
    calibrator.calibrate_by_ensemble_NN(ensemble_num=10)
    calibrator.save_accel_brake_map_ensemble_NN(save_dir=train_dir, save_heat_map=True)
sim_setting_dict = {}
sim_setting_dict["accel_brake_map_control_path"] = train_dir
simulator.perturbed_sim(sim_setting_dict)
if not SKIP_VEHICLE_ADAPTOR_TRAINING:
    simulator.drive_sim(
        control_type=ControlType.pp_eight,
        max_control_time=1500,
        save_dir=val_dir,
        max_lateral_accel=0.5,
    )

    model_trainer = train_error_prediction_NN.train_error_prediction_NN()
    model_trainer.add_data_from_csv(train_dir, add_mode="as_train", map_dir=train_dir)
    model_trainer.add_data_from_csv(val_dir, add_mode="as_val", map_dir=train_dir)

    model_trainer.get_trained_model(batch_sizes=[100])
    model_trainer.save_model(path=train_dir + "/vehicle_model.pth")

simulator.drive_sim(
    save_dir=root_dir + "/test_mpc_with_calibrated_vehicle_adaptor",
    use_vehicle_adaptor=True,
    vehicle_adaptor_model_path=train_dir + "/vehicle_model.pth",
    states_ref_mode="controller_d_inputs_schedule",
)
