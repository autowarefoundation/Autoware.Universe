import os
import sys

from autoware_vehicle_adaptor.training import train_error_prediction_NN
import numpy as np
import python_simulator
import simplejson as json
from utils.data_collection_utils import ControlType

SKIP_DATA_COLLECTION = False
# states_ref_mode = "predict_by_polynomial_regression"
# states_ref_mode = "controller_steer_prediction"
# states_ref_mode = "controller_d_steer_schedule"
# states_ref_mode = "controller_prediction"
states_ref_mode = "controller_d_inputs_schedule"
simulator = python_simulator.PythonSimulator()
dir_additional_name = ""
if not os.path.isdir("log_data"):
    os.mkdir("log_data")
if os.path.isfile("supporting_data/sim_setting.json"):
    with open("supporting_data/sim_setting.json", "r") as file:
        sim_setting_dict = json.load(file)
        print("load sim_setting.json")
        print("sim_setting_dict", sim_setting_dict)
        simulator.perturbed_sim(sim_setting_dict)
else:
    print("sim_setting.json not found")
if len(sys.argv) > 1:
    if sys.argv[1] == "nominal_test":
        if len(sys.argv) > 2:
            save_dir = "log_data/" + sys.argv[2]
            if not os.path.isdir(save_dir):
                os.mkdir(save_dir)
        save_dir += "/test_python_nominal_sim"

        simulator.drive_sim(
            max_control_time=300.0, save_dir=save_dir
        )  # ,course_csv_data="mpc_figure_eight_course_data.csv")
    else:
        save_dir = "log_data/" + sys.argv[1]
        if not os.path.isdir(save_dir):
            os.mkdir(save_dir)
if len(sys.argv) == 1 or (len(sys.argv) > 1 and sys.argv[1] != "nominal_test"):
    train_dir = save_dir + "/test_python_pure_pursuit_train"
    val_dir = save_dir + "/test_python_pure_pursuit_val"
    test_dir = save_dir + "/test_python_pure_pursuit_test"
    if not SKIP_DATA_COLLECTION:
        simulator.drive_sim(
            data_collection_seed=0,
            max_control_time=900.0,
            control_type=ControlType.pp_eight,
            save_dir=train_dir,
        )
        simulator.drive_sim(
            data_collection_seed=1,
            max_control_time=900.0,
            control_type=ControlType.pp_eight,
            save_dir=val_dir,
        )
        simulator.drive_sim(
            data_collection_seed=2,
            max_control_time=900.0,
            control_type=ControlType.pp_eight,
            save_dir=test_dir,
        )
    model_idx = 0
    model_trainer = train_error_prediction_NN.train_error_prediction_NN()
    model_trainer.add_data_from_csv(train_dir, add_mode="as_train")
    model_trainer.add_data_from_csv(val_dir, add_mode="as_val")
    model_trainer.add_data_from_csv(test_dir, add_mode="as_test")
    model_trainer.calc_dataloader_weights()
    model_trainer.get_trained_model(learning_rates=[1e-3, 1e-4, 1e-5, 1e-6], batch_sizes=[100])
    model_path = train_dir + "/vehicle_model_" + str(model_idx) + ".pth"
    model_trainer.save_model(path=model_path)
    simulator.drive_sim(
        save_dir=save_dir + "/test_python_vehicle_adaptor_sim_" + str(model_idx),
        max_control_time=100.0,
        use_vehicle_adaptor=True,
        vehicle_adaptor_model_path=model_path,
        states_ref_mode=states_ref_mode,
    )
    for i in range(10):
        model_idx += 1
        model_path = train_dir + "/vehicle_model_" + str(model_idx) + ".pth"
        drive_data_dir = save_dir + "/test_python_vehicle_adaptor_sim_" + str(model_idx)
        if model_trainer.get_relearned_model(
            learning_rates=[1e-3, 1e-4, 1e-5, 1e-6],
            batch_sizes=[100],
            randomize=0.01,
            plt_save_dir=drive_data_dir,
            save_path=model_path,
        ):
            simulator.drive_sim(
                save_dir=drive_data_dir,
                max_control_time=100.0,
                use_vehicle_adaptor=True,
                vehicle_adaptor_model_path=model_path,
                states_ref_mode=states_ref_mode,
            )
    for i in range(10):
        model_idx += 1
        model_path = train_dir + "/vehicle_model_" + str(model_idx) + ".pth"
        drive_data_dir = save_dir + "/test_python_vehicle_adaptor_sim_" + str(model_idx)
        if model_trainer.get_relearned_model(
            learning_rates=[1e-3, 1e-4, 1e-5, 1e-6, 1e-7],
            batch_sizes=[100],
            randomize=0.001,
            plt_save_dir=drive_data_dir,
            save_path=model_path,
        ):
            simulator.drive_sim(
                save_dir=drive_data_dir,
                max_control_time=100.0,
                use_vehicle_adaptor=True,
                vehicle_adaptor_model_path=model_path,
                states_ref_mode=states_ref_mode,
            )
        if model_trainer.get_relearned_model(
            learning_rates=[1e-4, 1e-5, 1e-6, 1e-7],
            batch_sizes=[100],
            randomize=0.001,
            plt_save_dir=drive_data_dir,
            save_path=model_path,
        ):
            simulator.drive_sim(
                save_dir=drive_data_dir,
                max_control_time=100.0,
                use_vehicle_adaptor=True,
                vehicle_adaptor_model_path=model_path,
                states_ref_mode=states_ref_mode,
            )
        if model_trainer.get_relearned_model(
            learning_rates=[1e-7],
            batch_sizes=[30, 1000],
            randomize=0.0,
            plt_save_dir=drive_data_dir,
            save_path=model_path,
        ):
            simulator.drive_sim(
                save_dir=drive_data_dir,
                max_control_time=100.0,
                use_vehicle_adaptor=True,
                vehicle_adaptor_model_path=model_path,
                states_ref_mode=states_ref_mode,
            )
        if model_trainer.get_relearned_model(
            learning_rates=[1e-4, 1e-5, 1e-6, 1e-7],
            batch_sizes=[1000],
            randomize=0.0,
            plt_save_dir=drive_data_dir,
            save_path=model_path,
        ):
            simulator.drive_sim(
                save_dir=drive_data_dir,
                max_control_time=100.0,
                use_vehicle_adaptor=True,
                vehicle_adaptor_model_path=model_path,
                states_ref_mode=states_ref_mode,
            )
