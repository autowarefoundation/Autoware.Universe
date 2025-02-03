import argparse
import os

import python_simulator
import simplejson as json
from utils.data_collection_utils import ControlType

# python3 run_pp_eight.py --save_dir=test_dir --max_lateral_accel=0.5


if __name__ == "__main__":

    simulator = python_simulator.PythonSimulator()
    simulator.data_collection = True

    if os.path.isfile("sim_setting.json"):
        with open("sim_setting.json", "r") as file:
            sim_setting_dict = json.load(file)
            print("load sim_setting.json")
            print("sim_setting_dict", sim_setting_dict)
            # モデルをいじる場合は, dict形式でパラメータを与えれば良い
            simulator.perturbed_sim(sim_setting_dict)
    else:
        print("sim_setting.json not found")

    parser = argparse.ArgumentParser()
    parser.add_argument("--save_dir", default=None)
    parser.add_argument("--max_lateral_accel", default=None, type=float)

    args = parser.parse_args()

    save_dir = args.save_dir
    max_lateral_accel = args.max_lateral_accel
    simulator.drive_sim(
        control_type=ControlType.pp_eight,
        max_control_time=15000,
        save_dir=save_dir,
        max_lateral_accel=max_lateral_accel,
    )
