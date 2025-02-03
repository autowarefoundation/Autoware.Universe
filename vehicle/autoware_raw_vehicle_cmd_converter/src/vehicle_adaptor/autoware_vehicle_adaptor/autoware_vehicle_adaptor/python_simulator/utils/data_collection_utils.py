import csv
from enum import Enum

import numpy as np

RANDOM_SEED_STEP_RESPONSE = 42


class ControlType(Enum):
    ff = "feed_forward"
    pp_eight = "pure_pursuit_figure_eight"
    pp_straight = "pure_pursuit_straight"
    mpc = "smart_mpc"

    def __str__(self) -> str:
        return self.name


class DataCollectionMode(Enum):
    ff = "feed_forward"
    pp = "pure_pursuit"
    mpc = "nominal_mpc"

    def __str__(self) -> str:
        return self.name

    def toControlTypes(self) -> list[ControlType]:
        if self.name == "pp":
            return [ControlType.pp_eight, ControlType.pp_straight]
        elif self.name == "ff":
            return [ControlType.ff]
        elif self.name == "mpc":
            return [ControlType.mpc]
        else:
            print(f"Error: unexpected DataCollectionMode: {self}")
            raise Exception


class GetInputNoise:
    def __init__(self):
        self.t_acc_list = []
        self.amp_acc_list = []
        self.t_steer_list = []
        self.amp_steer_list = []

    def clear_list(self):
        self.t_acc_list.clear()
        self.amp_acc_list.clear()
        self.t_steer_list.clear()
        self.amp_steer_list.clear()

    def set_step_response(
        self,
        step_response_interval,
        step_response_max_input,
        step_response_max_length,
        step_response_min_length,
    ):
        self.step_response_interval = step_response_interval
        self.step_response_max_input = step_response_max_input
        self.step_response_max_length = step_response_max_length
        self.step_response_min_length = step_response_min_length

    def create_additional_sine_data(
        self, seed, t_range, acc_amp_range, acc_period_range, steer_amp_range, steer_period_range
    ):
        """Create sine wave data to be added randomly to feed-forward runs."""
        self.clear_list()
        np.random.seed(seed=seed)
        t_acc = 0.0
        t_steer = 0.0
        self.t_acc_list.append(t_acc)
        self.t_steer_list.append(t_steer)
        while True:
            if t_acc >= t_steer:
                period = (
                    steer_period_range[1] - steer_period_range[0]
                ) * np.random.uniform() + steer_period_range[0]
                t_steer += period
                self.t_steer_list.append(t_steer)
                self.amp_steer_list.append(steer_amp_range * np.random.uniform())
            else:
                period = (
                    acc_period_range[1] - acc_period_range[0]
                ) * np.random.uniform() + acc_period_range[0]
                t_acc += period
                self.t_acc_list.append(t_acc)
                self.amp_acc_list.append(acc_amp_range * np.random.uniform())
            if t_acc >= t_range[1] and t_steer >= t_range[1]:
                break

    def get_additional_sine(self, t):
        """Calculate current values from already created sine wave data."""
        acc_index = 0
        steer_index = 0
        for i in range(len(self.t_acc_list) - 1):
            if t < self.t_acc_list[i + 1]:
                break
            acc_index += 1
        for i in range(len(self.t_steer_list) - 1):
            if t < self.t_steer_list[i + 1]:
                break
            steer_index += 1
        acc_noise = self.amp_acc_list[acc_index] * np.sin(
            2
            * np.pi
            * (t - self.t_acc_list[acc_index])
            / (self.t_acc_list[acc_index + 1] - self.t_acc_list[acc_index])
        )
        steer_noise = self.amp_steer_list[steer_index] * np.sin(
            2
            * np.pi
            * (t - self.t_steer_list[steer_index])
            / (self.t_steer_list[steer_index + 1] - self.t_steer_list[steer_index])
        )
        return np.array([acc_noise, steer_noise])

    def get_step_response(self, t):
        step = int(t // self.step_response_interval)
        step_start_time = step * self.step_response_interval
        np.random.seed(seed=step + RANDOM_SEED_STEP_RESPONSE)

        if self.step_response_max_length > self.step_response_interval:
            print(
                f"warning: max_length = {self.step_response_max_length} > interval = {self.step_response_interval}"
            )

        length = np.random.uniform(
            self.step_response_min_length,
            min(self.step_response_max_length, self.step_response_interval),
        )
        input_u = np.random.uniform(-self.step_response_max_input, self.step_response_max_input)

        if (t - step_start_time) >= length:
            return 0.0

        return input_u

    def get_input_noise(self, t):
        result = self.get_additional_sine(t)
        result[1] += self.get_step_response(t)
        return result


def compute_curvature_radius(trajectory_position_data, trajectory_yaw_data):
    d_step = 5
    curvature_radius = []
    for i in range(len(trajectory_position_data)):
        tmp_pos = trajectory_position_data[i]
        tmp_yaw = trajectory_yaw_data[i]
        tmp_computed_flag = False
        for j in range(i, len(trajectory_position_data)):
            distance = np.sqrt(((tmp_pos[:2] - trajectory_position_data[j, :2]) ** 2).sum())
            if distance >= d_step:
                diff_yaw = tmp_yaw - trajectory_yaw_data[j]
                if diff_yaw > np.pi:
                    diff_yaw -= 2 * np.pi
                if diff_yaw < -np.pi:
                    diff_yaw += 2 * np.pi
                curvature_radius.append(distance / (1e-12 + np.abs(diff_yaw)))
                tmp_computed_flag = True
                break
        if tmp_computed_flag is False:
            curvature_radius.append(1 * curvature_radius[-1])
    curvature_radius = np.array(curvature_radius)
    return curvature_radius


def compute_curvature_radius_loop_trajectory(trajectory_position_data, trajectory_yaw_data):
    data_length = len(trajectory_position_data)
    augmented_trajectory_position_data = np.vstack(
        [trajectory_position_data, trajectory_position_data[: data_length // 2]]
    )
    augmented_trajectory_yaw_data = np.hstack(
        [trajectory_yaw_data, trajectory_yaw_data[: data_length // 2]]
    )
    return compute_curvature_radius(
        augmented_trajectory_position_data, augmented_trajectory_yaw_data
    )[:data_length]


class FigureEight:
    """Figure eight trajectory."""

    def __init__(
        self,
        y_length: float,
        x_length: float,
        v_min=1.0,
        v_max=11.0,
        split_size=5,
        acc_max=1.2,
        constant_vel_time=5.0,
        smoothing_trajectory_data_flag=False,
    ):
        if y_length >= x_length:
            raise Exception("invalid argument: y_length must be less than x_length")
        self.y_length = y_length
        self.x_length = x_length
        self.v_min = v_min
        self.v_max = v_max
        self.split_size = split_size

        self.v_mid = 0.5 * (v_min + v_max)
        self.v_start = self.v_mid

        self.period = 2 * np.pi * (v_max - self.v_mid) / acc_max
        self.constant_vel_time = constant_vel_time

        self.break_flag = False
        self.accel_mode = 1
        self.smoothing_trajectory_data_flag = smoothing_trajectory_data_flag

    @property
    def total_distance(self) -> float:
        a = self.y_length
        b = self.x_length
        arc = a * np.pi
        diagonal = 2 * np.sqrt((b - a) ** 2 + a**2)
        return arc + diagonal

    def get_trajectory_points(self, step: float):
        """Get the position and yaw angle in world coordinates of the figure eight.

        The return value is a 2-dimensional array of positions and a 1-dimensional array of yaw angles corresponding to `t`.
        """
        a = self.y_length
        b = self.x_length

        t_array = np.arange(start=0.0, stop=self.total_distance, step=step).astype("float")
        x = t_array.copy()
        y = t_array.copy()
        yaw = t_array.copy()
        curvature = t_array.copy()
        parts = []
        achievement_rates = []

        # Boundary points between circular and linear trajectory
        C = [-(b - a) / 2, -a / 2]
        D = [(b - a) / 2, -a / 2]

        R = a / 2  # radius of the circle
        OL = [-(b - a) / 2, 0]  # center of the left circle
        OR = [(b - a) / 2, 0]  # center of the right circle
        OB = np.sqrt((b - a) ** 2 + a**2) / 2  # half length of the linear trajectory
        AD = 2 * OB
        θB = np.arctan(a / (b - a))  # Angle that OB makes with respect to x-axis
        BD = np.pi * a / 2  # the length of arc BD
        AC = BD
        CO = OB

        i_end = t_array.shape[0]
        for i, t in enumerate(t_array):
            if t > OB + BD + AD + AC + CO:
                i_end = i
                break
            if 0 <= t and t <= OB:
                x[i] = (b - a) * t / (2 * OB)
                y[i] = a * t / (2 * OB)
                yaw[i] = θB
                curvature[i] = 1e-10
                parts.append("linear_positive")
                achievement_rates.append(t / (2 * OB) + 0.5)

            if OB <= t and t <= OB + BD:
                t1 = t - OB
                t1_rad = t1 / R
                x[i] = OR[0] + R * np.cos(np.pi / 2 - t1_rad)
                y[i] = OR[1] + R * np.sin(np.pi / 2 - t1_rad)
                yaw[i] = -t1_rad
                curvature[i] = 1.0 / R

                parts.append("right_circle")
                achievement_rates.append(0.0)

            if OB + BD <= t and t <= OB + BD + AD:
                t2 = t - (OB + BD)
                x[i] = D[0] - (b - a) * t2 / (2 * OB)
                y[i] = D[1] + a * t2 / (2 * OB)
                yaw[i] = np.pi - θB
                curvature[i] = 1e-10

                parts.append("linear_negative")
                achievement_rates.append(t2 / (2 * OB))

            if OB + BD + AD <= t and t <= OB + BD + AD + AC:
                t3 = t - (OB + BD + AD)
                t3_rad = t3 / R
                x[i] = OL[0] + R * np.cos(np.pi / 2 + t3_rad)
                y[i] = OL[1] + R * np.sin(np.pi / 2 + t3_rad)
                yaw[i] = np.pi + t3_rad
                curvature[i] = 1.0 / R
                parts.append("left_circle")
                achievement_rates.append(0.0)

            if OB + BD + AD + AC <= t and t <= OB + BD + AD + AC + CO:
                t4 = t - (OB + BD + AD + AC)
                x[i] = C[0] + (b - a) * t4 / (2 * OB)
                y[i] = C[1] + a * t4 / (2 * OB)
                yaw[i] = θB
                curvature[i] = 1e-10
                parts.append("linear_positive")
                achievement_rates.append(t4 / (2 * OB))

        # drop rest
        x = x[:i_end]
        y = y[:i_end]
        trajectory_position_data = np.array([x, y]).T
        trajectory_yaw_data = yaw[:i_end]
        curvature = curvature[:i_end]

        if self.smoothing_trajectory_data_flag:
            window = 1000
            w = np.ones(window) / window
            augmented_position_data = np.vstack(
                [
                    trajectory_position_data[-window:],
                    trajectory_position_data,
                    trajectory_position_data[:window],
                ]
            )
            trajectory_position_data[:, 0] = (
                1 * np.convolve(augmented_position_data[:, 0], w, mode="same")[window:-window]
            )
            trajectory_position_data[:, 1] = (
                1 * np.convolve(augmented_position_data[:, 1], w, mode="same")[window:-window]
            )
            augmented_yaw_data = np.hstack(
                [
                    trajectory_yaw_data[-window:],
                    trajectory_yaw_data,
                    trajectory_yaw_data[:window],
                ]
            )
            smoothed_trajectory_yaw_data = trajectory_yaw_data.copy()
            for i in range(len(trajectory_yaw_data)):
                tmp_yaw = trajectory_yaw_data[i]
                tmp_data = (
                    augmented_yaw_data[window + (i - window // 2) : window + (i + window // 2)]
                    - tmp_yaw
                )
                for j in range(len(tmp_data)):
                    if tmp_data[j] > np.pi:
                        tmp_data[j] -= 2 * np.pi
                    if tmp_data[j] < -np.pi:
                        tmp_data[j] += 2 * np.pi
                tmp_data = np.convolve(tmp_data, w, mode="same")
                smoothed_trajectory_yaw_data[i] = (
                    tmp_yaw + np.convolve(tmp_data, w, mode="same")[window // 2]
                )
                if smoothed_trajectory_yaw_data[i] > np.pi:
                    smoothed_trajectory_yaw_data[i] -= 2 * np.pi
                if smoothed_trajectory_yaw_data[i] < -np.pi:
                    smoothed_trajectory_yaw_data[i] += 2 * np.pi

            trajectory_yaw_data = smoothed_trajectory_yaw_data.copy()

            augmented_curvature_data = np.hstack(
                [
                    curvature[-window:],
                    curvature,
                    curvature[:window],
                ]
            )

            curvature = 1 * np.convolve(augmented_curvature_data, w, mode="same")[window:-window]

        return (
            trajectory_position_data,
            trajectory_yaw_data,
            curvature,
            parts,
            achievement_rates,
        )

    def get_vel_sine(self, t, v_range):
        """Calculate current target velocity values already created sine wave data."""
        if t < self.period / 4:
            vel = self.v_mid + v_range * np.sin(2 * np.pi * t / self.period)
        elif t < self.period / 4 + self.constant_vel_time:
            vel = self.v_mid + v_range
        elif t < 3 * self.period / 4 + self.constant_vel_time:
            vel = self.v_mid + v_range * np.sin(
                2 * np.pi * (t - self.constant_vel_time) / self.period
            )
        elif t < 3 * self.period / 4 + 2 * self.constant_vel_time:
            vel = self.v_mid - v_range
        else:
            vel = self.v_mid + v_range * np.sin(
                2 * np.pi * (t - 2 * self.constant_vel_time) / self.period
            )
        return vel

    def get_periodic_count(self, index):
        return (
            self.split_size
            - 0.5
            - np.abs(index % (2 * self.split_size - 1) - self.split_size + 0.5)
        )

    def get_target_velocity(self, t):
        index = int(t / (self.period + 2 * self.constant_vel_time))
        t1 = t - (self.period + 2 * self.constant_vel_time) * index
        if index < 2 * self.split_size:
            adjust = 0.0
            if index >= self.split_size:
                adjust = 0.5
            v_range = (
                (self.v_max - self.v_mid)
                * (self.get_periodic_count(index) + 1 - adjust)
                / self.split_size
            )
            return self.get_vel_sine(t1, v_range)
        else:
            self.break_flag = True
            return self.v_mid

    # def get_target_velocity():


def get_pure_pursuit_info(states, trajectory_position_data, trajectory_yaw_data, previous_index):
    """Calculate the target position and yaw angle required for pure pursuit."""
    search_range = (
        np.arange(
            previous_index - trajectory_position_data.shape[0] // 4,
            previous_index + trajectory_position_data.shape[0] // 4,
        )
        % trajectory_position_data.shape[0]
    )
    nearest_index = np.argmin(
        ((trajectory_position_data[search_range] - states[:2].reshape(1, 2)) ** 2).sum(axis=1)
    )
    return (
        trajectory_position_data[search_range[nearest_index]],
        trajectory_yaw_data[search_range[nearest_index]],
        search_range[nearest_index],
    )


class driving_log_updater:
    """Class for updating logs when driving on the Python simulator."""

    def __init__(self):
        self.X_history = []
        self.U_history = []
        self.control_cmd_time_stamp_list = []
        self.control_cmd_steer_list = []
        self.control_cmd_acc_list = []
        self.localization_kinematic_state_list = []
        self.localization_acceleration_list = []
        self.vehicle_status_steering_status_list = []
        self.control_cmd_list = []
        self.system_operation_mode_state_list = []

        # data collection
        # velocity and acceleration grid
        self.num_bins = 10
        self.v_min, self.v_max = 0.0, 11.8  # 40km/h ~ 11.111m/s
        self.a_min, self.a_max = -1.0, 1.0
        self.collected_data_counts = np.zeros((self.num_bins, self.num_bins))
        self.collected_data_counts_on_line = np.zeros((self.num_bins, self.num_bins))
        self.v_bins = np.linspace(self.v_min, self.v_max, self.num_bins + 1)
        self.a_bins = np.linspace(self.a_min, self.a_max, self.num_bins + 1)
        self.v_bin_centers = (self.v_bins[:-1] + self.v_bins[1:]) / 2
        self.a_bin_centers = (self.a_bins[:-1] + self.a_bins[1:]) / 2

        self.vehicle_command_control_cmd_list = []
        self.control_command_actuation_cmd_list = []

        self.data_collection_mode = "autonomous_driving"

    def set_data_collection_mode(self, data_collection_mode):
        self.data_collection_mode = data_collection_mode

    def clear_list(self):
        self.X_history.clear()
        self.U_history.clear()
        self.control_cmd_time_stamp_list.clear()
        self.control_cmd_steer_list.clear()
        self.control_cmd_acc_list.clear()
        self.localization_kinematic_state_list.clear()
        self.localization_acceleration_list.clear()
        self.vehicle_status_steering_status_list.clear()
        self.control_cmd_list.clear()
        self.system_operation_mode_state_list.clear()
        self.vehicle_command_control_cmd_list.clear()
        self.control_command_actuation_cmd_list.clear()
        self.collected_data_counts = 0.0 * self.collected_data_counts
        self.collected_data_counts_on_line = 0.0 * self.collected_data_counts_on_line

    def update(self, t_current, states, inputs, vehicle_adaptor_inputs, accel, brake, part=None):
        """Update logs."""
        self.X_history.append(states)
        self.U_history.append(inputs)
        self.control_cmd_time_stamp_list.append(t_current)
        self.control_cmd_steer_list.append(inputs[1])
        self.control_cmd_acc_list.append(inputs[0])
        if self.control_cmd_time_stamp_list[-1] - self.control_cmd_time_stamp_list[0] > 3.0:
            self.control_cmd_time_stamp_list.pop(0)
            self.control_cmd_steer_list.pop(0)
            self.control_cmd_acc_list.pop(0)
        t_sec = int(t_current)
        t_n_sec = int(1e9 * (t_current - t_sec))
        localization_kinematic_state = np.zeros(7)
        localization_acceleration = np.zeros(4)
        vehicle_status_steering_status = np.zeros(3)
        control_cmd = np.zeros(17)
        system_operation_mode_state = np.zeros(3)
        vehicle_command_control_cmd = np.zeros(17)
        control_command_actuation_cmd = np.zeros(6)
        localization_kinematic_state[0] = t_sec
        localization_kinematic_state[1] = t_n_sec
        localization_kinematic_state[2] = states[0]
        localization_kinematic_state[3] = states[1]
        localization_kinematic_state[4] = np.sin(0.5 * states[3])
        localization_kinematic_state[5] = np.cos(0.5 * states[3])
        localization_kinematic_state[6] = states[2]
        self.localization_kinematic_state_list.append(localization_kinematic_state)
        localization_acceleration[0] = t_sec
        localization_acceleration[1] = t_n_sec
        localization_acceleration[3] = states[4]
        self.localization_acceleration_list.append(localization_acceleration)
        vehicle_status_steering_status[0] = t_sec
        vehicle_status_steering_status[1] = t_n_sec
        vehicle_status_steering_status[2] = states[5]
        self.vehicle_status_steering_status_list.append(vehicle_status_steering_status)
        control_cmd[0] = t_sec
        control_cmd[1] = t_n_sec
        control_cmd[8] = inputs[1]
        control_cmd[16] = inputs[0]
        self.control_cmd_list.append(control_cmd)
        system_operation_mode_state[0] = t_sec
        system_operation_mode_state[1] = t_n_sec
        system_operation_mode_state[2] = 2.0
        self.system_operation_mode_state_list.append(system_operation_mode_state)
        vehicle_command_control_cmd[0] = t_sec
        vehicle_command_control_cmd[1] = t_n_sec
        vehicle_command_control_cmd[8] = vehicle_adaptor_inputs[1]
        vehicle_command_control_cmd[16] = vehicle_adaptor_inputs[0]
        self.vehicle_command_control_cmd_list.append(vehicle_command_control_cmd)

        # update velocity vs acceleration array (this array is visualized via heatmap)
        v = states[2]  # vel_index]
        v_bin = np.digitize(v, self.v_bins) - 1
        a = states[4]  # acc_index]
        a_bin = np.digitize(a, self.a_bins) - 1
        if part is not None:
            if 0 <= v_bin < self.num_bins and 0 <= a_bin < self.num_bins:
                self.collected_data_counts[v_bin, a_bin] += 1
                if part == "linear_positive" or part == "linear_negative":
                    self.collected_data_counts_on_line[v_bin, a_bin] += 1

        control_command_actuation_cmd[0] = t_sec
        control_command_actuation_cmd[1] = t_n_sec
        control_command_actuation_cmd[3] = accel
        control_command_actuation_cmd[4] = brake
        self.control_command_actuation_cmd_list.append(control_command_actuation_cmd)

    def save(self, save_dir):
        """Save logs in csv format."""
        localization_kinematic_state = np.zeros((len(self.localization_kinematic_state_list), 48))
        localization_kinematic_state[:, [0, 1, 4, 5, 9, 10, 47]] = np.array(
            self.localization_kinematic_state_list
        )
        np.savetxt(
            save_dir + "/localization_kinematic_state.csv",
            localization_kinematic_state,
            delimiter=",",
        )
        np.savetxt(
            save_dir + "/localization_acceleration.csv",
            np.array(self.localization_acceleration_list),
            delimiter=",",
        )
        np.savetxt(
            save_dir + "/vehicle_status_steering_status.csv",
            np.array(self.vehicle_status_steering_status_list),
            delimiter=",",
        )
        if self.data_collection_mode == "data_collection":
            np.savetxt(
                save_dir + "/external_selected_control_cmd.csv",
                np.array(self.control_cmd_list),
                delimiter=",",
            )
        else:
            np.savetxt(
                save_dir + "/control_command_control_cmd.csv",
                np.array(self.control_cmd_list),
                delimiter=",",
            )

        with open(save_dir + "/system_operation_mode_state.csv", "w") as f:
            writer = csv.writer(f)
            for i in range(len(self.system_operation_mode_state_list)):
                system_operation_mode_state_plus_true = self.system_operation_mode_state_list[
                    i
                ].tolist()
                system_operation_mode_state_plus_true.append("True")
                writer.writerow(system_operation_mode_state_plus_true)
        if self.data_collection_mode == "autonomous_driving_with_vehicle_adaptor":
            np.savetxt(
                save_dir + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv",
                np.array(self.vehicle_command_control_cmd_list),
                delimiter=",",
            )
        np.savetxt(
            save_dir + "/control_command_actuation_cmd.csv",
            np.array(self.control_command_actuation_cmd_list),
            delimiter=",",
        )
