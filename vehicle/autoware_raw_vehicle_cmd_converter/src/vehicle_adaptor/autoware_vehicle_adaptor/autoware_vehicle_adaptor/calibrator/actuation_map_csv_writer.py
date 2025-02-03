import csv

import numpy as np


def map_csv_writer(actuation_map_state, actuation_map_cmd, actuation_map_matrix, save_path):
    with open(save_path, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["default"] + actuation_map_state)
        for i in range(actuation_map_matrix.shape[0]):
            writer.writerow([actuation_map_cmd[i]] + actuation_map_matrix[i].tolist())
