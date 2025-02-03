import json
from pathlib import Path

import numpy as np
import yaml

package_path_json = str(Path(__file__).parent.parent) + "/package_path.json"
with open(package_path_json, "r") as file:
    package_path = json.load(file)
pure_pursuit_param_path = (
    package_path["path"] + "/autoware_vehicle_adaptor/param/pure_pursuit/pure_pursuit_param.yaml"
)
with open(pure_pursuit_param_path, "r") as yml:
    pure_pursuit_param = yaml.safe_load(yml)


class PurePursuitController:
    def __init__(self, wheel_base):
        self.wheel_base = wheel_base
        self.acc_kp = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"]["acc_kp"]
        self.acc_ki = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"]["acc_ki"]
        self.lookahead_time = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "lookahead_time"
        ]
        self.min_lookahead = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "min_lookahead"
        ]
        self.steer_kp_param = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "steer_kp_param"
        ]
        self.steer_kd_param = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "steer_kd_param"
        ]
        self.integration_decay = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "integration_decay"
        ]
        self.integration_limit = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "integration_limit"
        ] / (self.acc_ki + 1e-5)
        self.lon_acc_lim = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "lon_acc_lim"
        ]
        self.lon_jerk_lim = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "lon_jerk_lim"
        ]
        self.steer_lim = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"]["steer_lim"]
        self.steer_rate_lim = pure_pursuit_param["pure_pursuit_parameter"]["pure_pursuit"][
            "steer_rate_lim"
        ]

        self.integrated_vel_error = 0.0
        self.previous_acc_cmd = 0.0
        self.previous_steer_cmd = 0.0

    def initialize(self, control_dt):
        self.integrated_vel_error = 0.0
        self.previous_acc_cmd = 0.0
        self.previous_steer_cmd = 0.0
        self.control_dt = control_dt

    def pure_pursuit_control(
        self,
        pos_xy_obs,
        pos_yaw_obs,
        longitudinal_vel_obs,
        pos_xy_ref,
        pos_yaw_ref,
        longitudinal_vel_ref,
        acc_gain_scaling=1.0,
        steer_gain_scaling=1.0,
    ):
        pure_pursuit_acc_kp = acc_gain_scaling * self.acc_kp
        pure_pursuit_acc_ki = acc_gain_scaling * self.acc_ki
        pure_pursuit_lookahead_time = self.lookahead_time
        pure_pursuit_min_lookahead = self.min_lookahead
        pure_pursuit_steer_kp_param = steer_gain_scaling * self.steer_kp_param
        pure_pursuit_steer_kd_param = steer_gain_scaling * self.steer_kd_param

        longitudinal_vel_err = longitudinal_vel_obs - longitudinal_vel_ref
        self.integrated_vel_error = (
            self.integration_decay * self.integrated_vel_error + longitudinal_vel_err
        )
        self.integrated_vel_error = np.clip(
            self.integrated_vel_error, -self.integration_limit, self.integration_limit
        )
        # print("integrated_vel_error", self.integrated_vel_error)
        pure_pursuit_acc_cmd = (
            -pure_pursuit_acc_kp * longitudinal_vel_err
            - pure_pursuit_acc_ki * self.integrated_vel_error
        )

        cos_yaw = np.cos(pos_yaw_ref)
        sin_yaw = np.sin(pos_yaw_ref)
        diff_position = pos_xy_obs - pos_xy_ref
        lat_err = -sin_yaw * diff_position[0] + cos_yaw * diff_position[1]
        yaw_err = pos_yaw_obs - pos_yaw_ref
        while True:
            if yaw_err > np.pi:
                yaw_err -= 2.0 * np.pi
            if yaw_err < (-np.pi):
                yaw_err += 2.0 * np.pi
            if np.abs(yaw_err) < np.pi:
                break

        lookahead = pure_pursuit_min_lookahead + pure_pursuit_lookahead_time * np.abs(
            longitudinal_vel_obs
        )
        pure_pursuit_steer_kp = (
            pure_pursuit_steer_kp_param * self.wheel_base / (lookahead * lookahead)
        )
        pure_pursuit_steer_kd = pure_pursuit_steer_kd_param * self.wheel_base / lookahead
        pure_pursuit_steer_cmd = -pure_pursuit_steer_kp * lat_err - pure_pursuit_steer_kd * yaw_err

        pure_pursuit_acc_cmd = np.clip(pure_pursuit_acc_cmd, -self.lon_acc_lim, self.lon_acc_lim)
        pure_pursuit_acc_cmd = np.clip(
            pure_pursuit_acc_cmd,
            self.previous_acc_cmd - self.lon_jerk_lim * self.control_dt,
            self.previous_acc_cmd + self.lon_jerk_lim * self.control_dt,
        )
        self.previous_acc_cmd = pure_pursuit_acc_cmd
        pure_pursuit_steer_cmd = np.clip(pure_pursuit_steer_cmd, -self.steer_lim, self.steer_lim)
        pure_pursuit_steer_cmd = np.clip(
            pure_pursuit_steer_cmd,
            self.previous_steer_cmd - self.steer_rate_lim * self.control_dt,
            self.previous_steer_cmd + self.steer_rate_lim * self.control_dt,
        )
        self.previous_steer_cmd = pure_pursuit_steer_cmd
        return np.array([pure_pursuit_acc_cmd, pure_pursuit_steer_cmd])
