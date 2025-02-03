from autoware_vehicle_adaptor.param import parameters
from autoware_vehicle_adaptor.src import inputs_prediction
import numpy as np


class LinearExtrapolator:
    def __init__(self, delay_time=0.0, past_step=3):
        self.delay_time = delay_time
        self.past_step = past_step
        self.control_buffer = []
        self.timestamp_buffer = []

    def set_params(self, delay_time):
        self.delay_time = delay_time

    def compensate(self, timestamp, x):
        self.control_buffer.append(x)
        self.timestamp_buffer.append(timestamp)
        if len(self.control_buffer) < self.past_step + 1:
            return x
        else:
            control_prev = self.control_buffer.pop(0)
            timestamp_prev = self.timestamp_buffer.pop(0)
            control_next = x + self.delay_time * (x - control_prev) / (timestamp - timestamp_prev)
            return control_next


class PolynomialExtrapolator:
    def __init__(self, delay_time=0.0, past_step=7, degree=2, minimum_decay=0.1, lam=1e-4):
        self.delay_time = delay_time
        self.past_step = past_step
        self.control_buffer = []
        self.timestamp_buffer = []
        self.degree = degree
        self.minimum_decay = minimum_decay
        self.lam = lam

    def set_params(self, delay_time):
        self.delay_time = delay_time

    def compensate(self, timestamp, x):
        self.control_buffer.append(x)
        self.timestamp_buffer.append(timestamp)
        if len(self.control_buffer) < self.past_step + 1:
            return x
        else:
            time_vector = np.array(self.timestamp_buffer)[:-1] - timestamp
            time_matrix = np.zeros((self.past_step, self.degree))
            time_matrix[:, 0] = time_vector
            for i in range(1, self.degree):
                time_matrix[:, i] = time_matrix[:, i - 1] * time_vector
            decay_vector = (1.0 - self.minimum_decay) * np.arange(self.past_step) / (
                self.past_step - 1
            ) + self.minimum_decay
            regularized_matrix = self.lam * np.eye(self.degree)
            coef_matrix = (
                np.linalg.inv(
                    time_matrix.T @ np.diag(decay_vector) @ time_matrix + regularized_matrix
                )
                @ time_matrix.T
                @ np.diag(decay_vector)
            )
            coef = coef_matrix @ (np.array(self.control_buffer)[:-1] - x)
            poly = 1.0
            predicted = x
            for i in range(self.degree):
                poly = poly * self.delay_time
                predicted += coef[i] * poly
            self.control_buffer.pop(0)
            self.timestamp_buffer.pop(0)
        return predicted


class NNExtrapolator:
    def __init__(self, delay_time=0.0, control_dt=0.033, cmd_mode="acc"):
        self.delay_time = delay_time
        self.control_dt = control_dt
        self.delay_step = int(self.delay_time / self.control_dt)
        self.predictor = inputs_prediction.InputsSchedulePrediction()
        if cmd_mode == "acc":
            self.history_len = parameters.controller_acc_input_history_len
            self.predict_steps = parameters.acc_input_schedule_prediction_len
        if cmd_mode == "steer":
            self.history_len = parameters.controller_steer_input_history_len
            self.predict_steps = parameters.steer_input_schedule_prediction_len
        else:
            print("cmd_mode should be acc or steer")
        self.predictor.set_params(
            self.history_len,
            self.predict_steps,
            control_dt,
            "inputs_schedule_prediction_model/" + cmd_mode + "_schedule_predictor",
            1,
        )

    def set_params(self, delay_time):
        self.delay_time = delay_time
        self.delay_step = int(self.delay_time / self.control_dt)

    def compensate(self, timestamp, x, vel):
        if self.delay_step == 0:
            return x
        else:
            predicted_inputs = self.predictor.get_inputs_schedule_predicted(
                np.array([vel, x]), timestamp
            )
            if self.predict_steps >= self.delay_step:
                return predicted_inputs[self.delay_step - 1]
            else:
                return predicted_inputs[-1]
