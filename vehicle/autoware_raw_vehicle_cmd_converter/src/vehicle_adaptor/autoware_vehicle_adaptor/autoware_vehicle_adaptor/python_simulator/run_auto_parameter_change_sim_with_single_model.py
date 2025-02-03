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

import argparse
import datetime
import os
import time
import traceback
from typing import Any
from typing import Dict

from autoware_vehicle_adaptor.training import train_error_prediction_NN
import numpy as np
import python_simulator
from utils.data_collection_utils import ControlType
from utils.data_collection_utils import DataCollectionMode
from utils.parameter_change_utils import ChangeParam
from utils.parameter_change_utils import DirGenerator

SKIP_DATA_COLLECTION = False
SKIP_TRAINING = False
# states_ref_mode = "predict_by_polynomial_regression"
# STATES_REF_MODE = "controller_prediction"
STATES_REF_MODE = "controller_d_inputs_schedule"


def run_parameter_change_sim(
    change_param: ChangeParam,
    root: str = ".",
    acc_amp_range=0.05,
    acc_period_range=[5.0, 20.0],
    steer_amp_range=0.05,
    steer_period_range=[5.0, 30.0],
    target_vel_acc_max=1.2,
    constant_vel_time=5.0,
    target_vel_split_size=5,
    step_response_max_input=0.01,
    step_response_max_length=1.5,
    step_response_interval=5.0,
    step_response_min_length=0.5,
    batch_sizes=[100],
):
    param_val_list = change_param.value()
    dir_generator = DirGenerator(root=root)
    parameter_data: Dict[str, list[Any]] = {}
    simulator = python_simulator.PythonSimulator()
    add_mode = ["as_val", "as_train"]
    validation_flags = [True, False]
    y_length = 60.0
    x_length = 120.0
    model_trainer = train_error_prediction_NN.train_error_prediction_NN()
    for i in range(len(param_val_list)):
        parameter_data[change_param.name] = param_val_list[i]
        simulator.perturbed_sim(parameter_data)
        training_data_dirs = []
        val_data_dirs = []
        for j in range(2):

            save_dir = dir_generator.test_dir_name(
                control_type=ControlType.pp_eight,
                change_param=change_param,
                index=i,
                validation_flag=validation_flags[j],
            )
            if not SKIP_DATA_COLLECTION:
                figure_eight_data = {
                    "circle_x_length": (0.9 ** (1 - j)) * x_length,
                    "circle_y_length": (0.9 ** (1 - j)) * y_length,
                    "data_collection_seed": j + 1,
                    "acc_amp_range": acc_amp_range,
                    "acc_period_range": acc_period_range,
                    "steer_amp_range": steer_amp_range,
                    "steer_period_range": steer_period_range,
                    "target_vel_acc_max": target_vel_acc_max,
                    "constant_vel_time": constant_vel_time,
                    "target_vel_split_size": target_vel_split_size,
                    "step_response_max_input": step_response_max_input,
                    "step_response_max_length": step_response_max_length,
                    "step_response_interval": step_response_interval,
                    "step_response_min_length": step_response_min_length,
                }
                simulator.setattr(figure_eight_data)
                simulator.drive_sim(
                    save_dir=save_dir, max_control_time=900.0, control_type=ControlType.pp_eight
                )
            if not SKIP_TRAINING:
                model_trainer.add_data_from_csv(save_dir, add_mode=add_mode[j])
                if j == 0:
                    val_data_dirs.append(save_dir)
                else:
                    training_data_dirs.append(save_dir)
    if not SKIP_TRAINING:
        model_trainer.get_trained_model(batch_sizes=batch_sizes)
        model_trainer.save_model(path=root + "/vehicle_model.pth")

    for i in range(len(param_val_list)):
        parameter_data[change_param.name] = param_val_list[i]
        simulator.perturbed_sim(parameter_data)
        load_dir = root
        save_dir = dir_generator.test_dir_name(
            control_type=ControlType.mpc,
            change_param=change_param,
            index=i,
            validation_flag=False,
            with_adaptor=True,
        )
        simulator.drive_sim(
            save_dir=save_dir,
            use_vehicle_adaptor=True,
            vehicle_adaptor_model_path=load_dir + "/vehicle_model.pth",
            states_ref_mode=STATES_REF_MODE,
        )


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--param_name", default=None)
    parser.add_argument("--root", default=".")
    args = parser.parse_args()

    if args.root == "time":
        args.root = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        args.root = "test_run_sim_" + args.root
    for change_param in ChangeParam.keys():
        if change_param == args.param_name:
            run_parameter_change_sim(change_param=ChangeParam.get(change_param), root=args.root)
            break
