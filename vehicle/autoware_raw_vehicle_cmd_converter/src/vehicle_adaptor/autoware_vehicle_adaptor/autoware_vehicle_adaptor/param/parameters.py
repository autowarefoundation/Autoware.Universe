import json
from pathlib import Path

import yaml

package_path_json = str(Path(__file__).parent.parent) + "/package_path.json"
with open(package_path_json, "r") as file:
    package_path = json.load(file)
nominal_param_path = package_path["path"] + "/autoware_vehicle_adaptor/param/nominal_param.yaml"
with open(nominal_param_path, "r") as yml:
    nominal_param = yaml.safe_load(yml)
wheel_base = float(nominal_param["nominal_parameter"]["vehicle_info"]["wheel_base"])
acc_time_delay = float(nominal_param["nominal_parameter"]["acceleration"]["acc_time_delay"])
acc_time_constant = float(nominal_param["nominal_parameter"]["acceleration"]["acc_time_constant"])
steer_time_delay = float(nominal_param["nominal_parameter"]["steering"]["steer_time_delay"])
steer_time_constant = float(nominal_param["nominal_parameter"]["steering"]["steer_time_constant"])


mpc_param_path = (
    package_path["path"] + "/autoware_vehicle_adaptor/param/nominal_ilqr/mpc_param.yaml"
)
with open(mpc_param_path, "r") as yml:
    mpc_param = yaml.safe_load(yml)
mpc_predict_step = int(mpc_param["mpc_parameter"]["setting"]["predict_step"])
mpc_horizon_len = int(mpc_param["mpc_parameter"]["setting"]["horizon_len"])
mpc_control_dt = float(mpc_param["mpc_parameter"]["setting"]["control_dt"])


trained_model_param_path = (
    package_path["path"] + "/autoware_vehicle_adaptor/param/trained_model_param.yaml"
)
with open(trained_model_param_path, "r") as yml:
    trained_model_param = yaml.safe_load(yml)
prediction_length = int(trained_model_param["trained_model_parameter"]["lstm"]["predict_lstm_len"])
past_length = int(trained_model_param["trained_model_parameter"]["lstm"]["update_lstm_len"])
integration_length = int(trained_model_param["trained_model_parameter"]["loss"]["integration_len"])
integration_weight = float(
    trained_model_param["trained_model_parameter"]["loss"]["integration_weight"]
)
add_position_to_prediction = bool(
    trained_model_param["trained_model_parameter"]["setting"]["add_position_to_prediction"]
)
add_vel_to_prediction = bool(
    trained_model_param["trained_model_parameter"]["setting"]["add_vel_to_prediction"]
)
add_yaw_to_prediction = bool(
    trained_model_param["trained_model_parameter"]["setting"]["add_yaw_to_prediction"]
)
use_position_observation = bool(
    trained_model_param["trained_model_parameter"]["setting"]["use_position_observation"]
)
use_vel_observation = bool(
    trained_model_param["trained_model_parameter"]["setting"]["use_vel_observation"]
)
use_yaw_observation = bool(
    trained_model_param["trained_model_parameter"]["setting"]["use_yaw_observation"]
)

state_component_predicted = []
state_component_predicted_index = []
if add_position_to_prediction:
    state_component_predicted.append("x")
    state_component_predicted_index.append(0)
    state_component_predicted.append("y")
    state_component_predicted_index.append(1)
if add_vel_to_prediction:
    state_component_predicted.append("vel")
    state_component_predicted_index.append(2)
if add_yaw_to_prediction:
    state_component_predicted.append("yaw")
    state_component_predicted_index.append(3)
state_component_predicted.append("acc")
state_component_predicted_index.append(4)
state_component_predicted.append("steer")
state_component_predicted_index.append(5)
state_name_to_predicted_index = {}
for i in range(len(state_component_predicted)):
    state_name_to_predicted_index[state_component_predicted[i]] = i

optimization_param_path = (
    package_path["path"] + "/autoware_vehicle_adaptor/param/optimization_param.yaml"
)


with open(optimization_param_path, "r") as yml:
    optimization_param = yaml.safe_load(yml)
acc_queue_size = int(trained_model_param["trained_model_parameter"]["queue_size"]["acc_queue_size"])
steer_queue_size = int(
    trained_model_param["trained_model_parameter"]["queue_size"]["steer_queue_size"]
)
control_dt = 0.033
acc_delay_step = round(acc_time_delay / control_dt)
steer_delay_step = round(steer_time_delay / control_dt)

acc_input_schedule_prediction_len = int(
    optimization_param["optimization_parameter"]["inputs_schedule_prediction_NN"][
        "acc_input_schedule_prediction_len"
    ]
)
steer_input_schedule_prediction_len = int(
    optimization_param["optimization_parameter"]["inputs_schedule_prediction_NN"][
        "steer_input_schedule_prediction_len"
    ]
)
controller_acc_input_history_len = int(
    optimization_param["optimization_parameter"]["inputs_schedule_prediction_NN"][
        "controller_acc_input_history_len"
    ]
)
controller_steer_input_history_len = int(
    optimization_param["optimization_parameter"]["inputs_schedule_prediction_NN"][
        "controller_steer_input_history_len"
    ]
)
