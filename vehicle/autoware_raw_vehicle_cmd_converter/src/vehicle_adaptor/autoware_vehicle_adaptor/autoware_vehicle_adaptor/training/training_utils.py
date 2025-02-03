import copy
import json
from multiprocessing import Process
import os
from pathlib import Path
import random
import tempfile
import time
import types

from autoware_vehicle_adaptor.param import parameters
from autoware_vehicle_adaptor.training import add_data_from_csv
from autoware_vehicle_adaptor.training import convert_model_to_csv
from autoware_vehicle_adaptor.training import error_prediction_NN
from autoware_vehicle_adaptor.training.early_stopping import EarlyStopping
import matplotlib.pyplot as plt
import numpy as np
from sklearn.linear_model import Lasso
from sklearn.neighbors import KernelDensity
import torch
from torch import nn
from torch.utils.data import DataLoader
from torch.utils.data import TensorDataset
from torch.utils.data import WeightedRandomSampler
import yaml

# torch.autograd.set_detect_anomaly(True)
prediction_length = parameters.prediction_length
past_length = parameters.past_length
integration_length = parameters.integration_length
integration_weight = parameters.integration_weight
add_position_to_prediction = parameters.add_position_to_prediction
add_vel_to_prediction = parameters.add_vel_to_prediction
add_yaw_to_prediction = parameters.add_yaw_to_prediction

state_component_predicted = parameters.state_component_predicted
state_component_predicted_index = parameters.state_component_predicted_index
state_name_to_predicted_index = parameters.state_name_to_predicted_index

acc_queue_size = parameters.acc_queue_size
steer_queue_size = parameters.steer_queue_size
control_dt = parameters.control_dt
acc_delay_step = parameters.acc_delay_step
steer_delay_step = parameters.steer_delay_step
acc_time_constant = parameters.acc_time_constant
steer_time_constant = parameters.steer_time_constant
wheel_base = parameters.wheel_base
vel_index = 0
acc_index = 1
steer_index = 2
prediction_step = parameters.mpc_predict_step
acc_input_indices_nom = np.arange(
    3 + acc_queue_size - acc_delay_step, 3 + acc_queue_size - acc_delay_step + prediction_step
)
steer_input_indices_nom = np.arange(
    3 + acc_queue_size + prediction_step + steer_queue_size - steer_delay_step,
    3 + acc_queue_size + steer_queue_size - steer_delay_step + 2 * prediction_step,
)

offline_data_size = 300  # 100
offline_data_len = 10


def transform_to_sequence_data(X, seq_size, division_indices, prediction_step):
    X_seq = []
    start_num = 0
    for i in range(len(division_indices)):
        j = 0
        while start_num + j + prediction_step * seq_size <= division_indices[i]:
            X_seq.append(X[start_num + j + prediction_step * np.arange(seq_size)])
            j += 1
        start_num = division_indices[i]

    return np.array(X_seq)


def transform_to_sequence_data_with_index(X, seq_size, division_indices, prediction_step):
    X_seq = []
    resulted_indices = []
    start_num = 0
    for i in range(len(division_indices)):
        j = 0
        while start_num + j + prediction_step * seq_size <= division_indices[i]:
            X_seq.append(X[start_num + j + prediction_step * np.arange(seq_size)])
            j += 1
            resulted_indices.append(i)
        start_num = division_indices[i]

    return np.array(X_seq), np.array(resulted_indices)


def get_offline_data(X, division_indices, prediction_step, use_division_indices):
    start_num = 0
    use_data_index_list = []
    for i in range(len(division_indices)):
        if i in use_division_indices:
            use_data_index_list.append(
                np.arange(start_num, division_indices[i] - prediction_step * offline_data_len)
            )
        start_num = division_indices[i]
    use_data_index = np.concatenate(use_data_index_list)
    selected_index = np.random.choice(use_data_index, offline_data_size)
    offline_data = []
    for i in range(offline_data_size):
        offline_data.append(X[selected_index[i] + prediction_step * np.arange(offline_data_len)])
    return np.array(offline_data)


def get_acc_steer_nominal_prediction(X, window_size=10, past_length=past_length):
    state_tmp = X[:, past_length, [acc_index, steer_index]]
    for i in range(window_size):
        for j in range(prediction_step):
            state_tmp[:, 0] = state_tmp[:, 0] + (
                X[:, past_length + i, acc_input_indices_nom[j]] - state_tmp[:, 0]
            ) * (1 - np.exp(-control_dt / acc_time_constant))
            state_tmp[:, 1] = state_tmp[:, 1] + (
                X[:, past_length + i, steer_input_indices_nom[j]] - state_tmp[:, 1]
            ) * (1 - np.exp(-control_dt / steer_time_constant))
    return state_tmp.detach().to("cpu").numpy()


def get_nominal_signed_prediction_error(X, window_size=10, past_length=past_length):
    nominal_prediction = get_acc_steer_nominal_prediction(X, window_size, past_length)
    nominal_prediction_error = (
        X[:, past_length + window_size, [acc_index, steer_index]].detach().to("cpu").numpy()
        - nominal_prediction
    )
    return nominal_prediction_error


class TrainErrorPredictionNNFunctions:
    @staticmethod
    def get_loss(
        criterion,
        model,
        X_batch,
        Y,
        adaptive_weight,
        tanh_gain=10,
        tanh_weight=0.1,
        first_order_weight=0.01,
        second_order_weight=0.01,
        randomize_previous_error=[0.5, 0.1],
        integral_prob=0.0,
        alpha_jacobian=0.01,
        calc_jacobian_len=10,
        eps=1e-6,
    ):
        randomize_scale_tensor = torch.tensor(randomize_previous_error).to(X_batch.device)
        previous_error = Y[:, :past_length, -2:] + torch.mean(
            torch.abs(Y[:, :past_length, -2:]), dim=1
        ).unsqueeze(1) * randomize_scale_tensor * torch.randn_like(Y[:, :past_length, -2:])
        Y_pred, hc = model(X_batch, previous_error=previous_error, mode="get_lstm_states")
        # Calculate the loss
        loss = criterion(Y_pred * adaptive_weight, Y[:, past_length:] * adaptive_weight)
        if alpha_jacobian is not None:
            hc_perturbed = model(
                X_batch[:, :past_length] + eps * torch.randn_like(X_batch[:, :past_length]),
                previous_error=previous_error + eps * torch.randn_like(previous_error),
                mode="only_encoder",
            )
            Y_perturbed, _ = model(
                X_batch[:, past_length : past_length + calc_jacobian_len]
                + eps * torch.randn_like(X_batch[:, past_length : past_length + calc_jacobian_len]),
                hc=hc_perturbed,
                mode="predict_with_hc",
            )
            loss += alpha_jacobian * criterion(
                (Y_perturbed - Y_pred[:, :calc_jacobian_len]) / eps, torch.zeros_like(Y_perturbed)
            )
        # Calculate the integrated loss
        if random.random() < integral_prob:
            predicted_states = X_batch[
                :, past_length, [vel_index, acc_index, steer_index]
            ].unsqueeze(1)
            for i in range(integration_length):
                if i > 0:
                    predicted_states[:, 0, 0] = X_batch[:, past_length + i, vel_index]

                predicted_states_with_input_history = torch.cat(
                    (predicted_states, X_batch[:, [past_length + i], 3:]), dim=2
                )
                predicted_error, hc = model(
                    predicted_states_with_input_history, hc=hc, mode="predict_with_hc"
                )
                for j in range(prediction_step):
                    predicted_states[:, 0, 1] = predicted_states[:, 0, 1] + (
                        X_batch[:, past_length + i, acc_input_indices_nom[j]]
                        - predicted_states[:, 0, 1]
                    ) * (1 - np.exp(-control_dt / acc_time_constant))
                    predicted_states[:, 0, 2] = predicted_states[:, 0, 2] + (
                        X_batch[:, past_length + i, steer_input_indices_nom[j]]
                        - predicted_states[:, 0, 2]
                    ) * (1 - np.exp(-control_dt / steer_time_constant))
                predicted_states[:, 0, -2] = (
                    predicted_states[:, 0, -2]
                    + predicted_error[:, 0, -2] * control_dt * prediction_step
                )
                predicted_states[:, 0, -1] = (
                    predicted_states[:, 0, -1]
                    + predicted_error[:, 0, -1] * control_dt * prediction_step
                )
            loss += (
                adaptive_weight[-2]
                * integration_weight
                * criterion(
                    predicted_states[:, 0, -2],
                    X_batch[:, past_length + integration_length, acc_index],
                )
            )
            loss += (
                adaptive_weight[-1]
                * integration_weight
                * criterion(
                    predicted_states[:, 0, -1],
                    X_batch[:, past_length + integration_length, steer_index],
                )
            )
        # Calculate the tanh loss
        loss += tanh_weight * criterion(
            torch.tanh(tanh_gain * (Y_pred[:, :, -1] - Y[:, past_length:, -1])),
            torch.zeros_like(Y_pred[:, :, -1]),
        )
        # Calculate the first order loss
        first_order_loss = first_order_weight * criterion(
            Y_pred[:, 1:] - Y_pred[:, :-1], torch.zeros_like(Y_pred[:, 1:])
        )
        # Calculate the second order loss
        second_order_loss = second_order_weight * criterion(
            Y_pred[:, 2:] - 2 * Y_pred[:, 1:-1] + Y_pred[:, :-2], torch.zeros_like(Y_pred[:, 2:])
        )
        # Calculate the total loss
        total_loss = loss + first_order_loss + second_order_loss
        return total_loss

    @staticmethod
    def validate_in_batches(
        model,
        criterion,
        X_val,
        Y_val,
        adaptive_weight,
        randomize_previous_error=[0.03, 0.03],
        batch_size=1000,
        alpha_jacobian=None,
        calc_jacobian_len=10,
        eps=1e-5,
    ):
        model.eval()
        val_loss = 0.0
        num_batches = (X_val.size(0) + batch_size - 1) // batch_size

        with torch.no_grad():
            for i in range(num_batches):
                start_idx = i * batch_size
                end_idx = min((i + 1) * batch_size, X_val.size(0))

                X_batch = X_val[start_idx:end_idx]
                Y_batch = Y_val[start_idx:end_idx]

                loss = TrainErrorPredictionNNFunctions.get_loss(
                    criterion,
                    model,
                    X_batch,
                    Y_batch,
                    adaptive_weight=adaptive_weight,
                    randomize_previous_error=randomize_previous_error,
                    alpha_jacobian=alpha_jacobian,
                    calc_jacobian_len=calc_jacobian_len,
                    eps=eps,
                )
                val_loss += loss.item() * (end_idx - start_idx)

        val_loss /= X_val.size(0)
        return val_loss

    @staticmethod
    def get_each_component_loss(
        model,
        X_val,
        Y_val,
        tanh_gain=10,
        tanh_weight=0.1,
        first_order_weight=0.01,
        second_order_weight=0.01,
        batch_size=1000,
        window_size=10,
    ):
        model.eval()
        val_loss = np.zeros(Y_val.size(2))
        tanh_loss = 0.0
        first_order_loss = 0.0
        second_order_loss = 0.0
        num_batches = (X_val.size(0) + batch_size - 1) // batch_size
        Y_pred_list = []
        with torch.no_grad():
            for i in range(num_batches):
                start_idx = i * batch_size
                end_idx = min((i + 1) * batch_size, X_val.size(0))
                if X_val.device != next(model.parameters()).device:
                    X_batch = X_val[start_idx:end_idx].to(next(model.parameters()).device)
                    Y_batch = Y_val[start_idx:end_idx].to(next(model.parameters()).device)
                else:
                    X_batch = X_val[start_idx:end_idx]
                    Y_batch = Y_val[start_idx:end_idx]
                Y_pred, _ = model(
                    X_batch, previous_error=Y_batch[:, :past_length, -2:], mode="get_lstm_states"
                )
                Y_pred_list.append(Y_pred[:, window_size])
                # Calculate the loss
                loss = torch.mean(torch.abs(Y_pred - Y_batch[:, past_length:]), dim=(0, 1))
                val_loss += loss.cpu().numpy() * (end_idx - start_idx)
                tanh_loss += (
                    tanh_weight
                    * torch.mean(
                        torch.abs(
                            torch.tanh(
                                tanh_gain * (Y_pred[:, :, -1] - Y_batch[:, past_length:, -1])
                            )
                        )
                    ).item()
                    * (end_idx - start_idx)
                )
                first_order_loss += (
                    first_order_weight
                    * torch.mean(
                        torch.abs(
                            (Y_pred[:, 1:] - Y_pred[:, :-1])
                            - (Y_batch[:, past_length + 1 :] - Y_batch[:, past_length:-1])
                        )
                    ).item()
                    * (end_idx - start_idx)
                )
                second_order_loss += (
                    second_order_weight
                    * torch.mean(
                        torch.abs(
                            (Y_pred[:, 2:] - 2 * Y_pred[:, 1:-1] + Y_pred[:, :-2])
                            - (
                                Y_batch[:, past_length + 2 :]
                                - 2 * Y_batch[:, past_length + 1 : -1]
                                + Y_batch[:, past_length:-2]
                            )
                        )
                    ).item()
                    * (end_idx - start_idx)
                )
        val_loss /= X_val.size(0) * Y_val.size(2)
        tanh_loss /= X_val.size(0)
        first_order_loss /= X_val.size(0)
        second_order_loss /= X_val.size(0)
        Y_pred_np = torch.cat(Y_pred_list, dim=0).cpu().detach().numpy()
        return (
            np.concatenate((val_loss, [tanh_loss, first_order_loss, second_order_loss])),
            Y_pred_np,
        )

    @staticmethod
    def get_Y_pred_np(model, X_val, Y_val, batch_size=1000):
        model.eval()
        device = next(model.parameters()).device
        num_batches = (X_val.size(0) + batch_size - 1) // batch_size
        Y_pred_list = []
        with torch.no_grad():
            for i in range(num_batches):
                start_idx = i * batch_size
                end_idx = min((i + 1) * batch_size, X_val.size(0))
                if X_val.device != device:
                    X_batch = X_val[start_idx:end_idx].to(device)
                    Y_batch = Y_val[start_idx:end_idx].to(device)
                else:
                    X_batch = X_val[start_idx:end_idx]
                    Y_batch = Y_val[start_idx:end_idx]
                Y_pred, _ = model(
                    X_batch, previous_error=Y_batch[:, :past_length, -2:], mode="get_lstm_states"
                )
                Y_pred_list.append(Y_pred)
        Y_pred_np = torch.cat(Y_pred_list, dim=0).cpu().detach().numpy()
        return Y_pred_np

    @staticmethod
    def get_losses(model, criterion, X, Y, adaptive_weight):
        loss = TrainErrorPredictionNNFunctions.validate_in_batches(
            model, criterion, X, Y, adaptive_weight=adaptive_weight
        )
        each_component_loss, Y_pred = TrainErrorPredictionNNFunctions.get_each_component_loss(
            model, X, Y
        )
        return loss, each_component_loss, Y_pred

    @staticmethod
    def get_acc_steer_model_prediction(model, X, Y, window_size=10, batch_size=1000):
        model.eval()
        device = next(model.parameters()).device
        num_batches = (X.size(0) + batch_size - 1) // batch_size
        prediction = []
        for k in range(num_batches):
            if X.device != device:
                X_batch = X[k * batch_size : (k + 1) * batch_size].to(device)
                Y_batch = Y[k * batch_size : (k + 1) * batch_size].to(device)
            else:
                X_batch = X[k * batch_size : (k + 1) * batch_size]
                Y_batch = Y[k * batch_size : (k + 1) * batch_size]

            _, hc = model(
                X_batch, previous_error=Y_batch[:, :past_length, -2:], mode="get_lstm_states"
            )
            states_tmp = X_batch[:, past_length, [vel_index, acc_index, steer_index]].unsqueeze(1)
            for i in range(window_size):
                states_tmp_with_input_history = torch.cat(
                    (states_tmp, X_batch[:, [past_length + i], 3:]), dim=2
                )
                predicted_error, hc = model(
                    states_tmp_with_input_history, hc=hc, mode="predict_with_hc"
                )
                states_tmp[:, 0, 0] = X_batch[:, past_length + i + 1, 0]
                for j in range(prediction_step):
                    states_tmp[:, 0, 1] = states_tmp[:, 0, 1] + (
                        X_batch[:, past_length + i, acc_input_indices_nom[j]] - states_tmp[:, 0, 1]
                    ) * (1 - np.exp(-control_dt / acc_time_constant))
                    states_tmp[:, 0, 2] = states_tmp[:, 0, 2] + (
                        X_batch[:, past_length + i, steer_input_indices_nom[j]]
                        - states_tmp[:, 0, 2]
                    ) * (1 - np.exp(-control_dt / steer_time_constant))
                states_tmp[:, 0, 1] = (
                    states_tmp[:, 0, 1]
                    + predicted_error[:, 0, state_name_to_predicted_index["acc"]]
                    * control_dt
                    * prediction_step
                )
                states_tmp[:, 0, 2] = (
                    states_tmp[:, 0, 2]
                    + predicted_error[:, 0, state_name_to_predicted_index["steer"]]
                    * control_dt
                    * prediction_step
                )
            prediction.append(states_tmp[:, 0, [1, 2]].detach().to("cpu").numpy())
        prediction = np.concatenate(prediction, axis=0)
        return prediction

    @staticmethod
    def get_acc_steer_models_prediction(models, X, Y, window_size=10, batch_size=1000):
        for model in models:
            model.eval()
        device = next(models[0].parameters()).device
        num_batches = (X.size(0) + batch_size - 1) // batch_size
        prediction = []
        for k in range(num_batches):
            if X.device != device:
                X_batch = X[k * batch_size : (k + 1) * batch_size].to(device)
                Y_batch = Y[k * batch_size : (k + 1) * batch_size].to(device)
            else:
                X_batch = X[k * batch_size : (k + 1) * batch_size]
                Y_batch = Y[k * batch_size : (k + 1) * batch_size]

            _, hc = models[0](
                X_batch, previous_error=Y_batch[:, :past_length, -2:], mode="get_lstm_states"
            )
            states_tmp = X_batch[:, past_length, [vel_index, acc_index, steer_index]].unsqueeze(1)
            for i in range(window_size):
                states_tmp_with_input_history = torch.cat(
                    (states_tmp, X_batch[:, [past_length + i], 3:]), dim=2
                )
                predicted_error_0, hc_next = models[0](
                    states_tmp_with_input_history, hc=hc, mode="predict_with_hc"
                )
                predicted_error = predicted_error_0 / len(models)
                for j in range(1, len(models)):
                    predicted_error_j, _ = models[j](
                        states_tmp_with_input_history, hc=hc, mode="predict_with_hc"
                    )
                    predicted_error += predicted_error_j / len(models)
                hc = hc_next
                states_tmp[:, 0, 0] = X_batch[:, past_length + i + 1, 0]
                for j in range(prediction_step):
                    states_tmp[:, 0, 1] = states_tmp[:, 0, 1] + (
                        X_batch[:, past_length + i, acc_input_indices_nom[j]] - states_tmp[:, 0, 1]
                    ) * (1 - np.exp(-control_dt / acc_time_constant))
                    states_tmp[:, 0, 2] = states_tmp[:, 0, 2] + (
                        X_batch[:, past_length + i, steer_input_indices_nom[j]]
                        - states_tmp[:, 0, 2]
                    ) * (1 - np.exp(-control_dt / steer_time_constant))
                states_tmp[:, 0, 1] = (
                    states_tmp[:, 0, 1]
                    + predicted_error[:, 0, state_name_to_predicted_index["acc"]]
                    * control_dt
                    * prediction_step
                )
                states_tmp[:, 0, 2] = (
                    states_tmp[:, 0, 2]
                    + predicted_error[:, 0, state_name_to_predicted_index["steer"]]
                    * control_dt
                    * prediction_step
                )
            prediction.append(states_tmp[:, 0, [1, 2]].detach().to("cpu").numpy())
        prediction = np.concatenate(prediction, axis=0)
        return prediction

    @staticmethod
    def get_model_signed_prediction_error(model, X, Y, window_size=10, batch_size=1000):
        model_prediction = TrainErrorPredictionNNFunctions.get_acc_steer_model_prediction(
            model, X, Y, window_size, batch_size
        )
        model_prediction_error = (
            X[:, past_length + window_size, [acc_index, steer_index]].detach().to("cpu").numpy()
            - model_prediction
        )
        return model_prediction_error

    @staticmethod
    def get_models_signed_prediction_error(models, X, Y, window_size=10, batch_size=1000):
        model_prediction = TrainErrorPredictionNNFunctions.get_acc_steer_models_prediction(
            models, X, Y, window_size, batch_size
        )
        model_prediction_error = (
            X[:, past_length + window_size, [acc_index, steer_index]].detach().to("cpu").numpy()
            - model_prediction
        )
        return model_prediction_error

    @staticmethod
    def get_signed_prediction_error(model, relearned_model, X, Y, window_size=10, batch_size=1000):
        nominal_prediction_error = get_nominal_signed_prediction_error(X, window_size)
        model_prediction_error = TrainErrorPredictionNNFunctions.get_model_signed_prediction_error(
            model, X, Y, window_size, batch_size
        )
        relearned_model_prediction_error = (
            TrainErrorPredictionNNFunctions.get_model_signed_prediction_error(
                relearned_model, X, Y, window_size, batch_size
            )
        )
        return nominal_prediction_error, model_prediction_error, relearned_model_prediction_error

    @staticmethod
    def get_sequence_data(
        X,
        Y,
        division_indices,
        acc_threshold=2.0,
        steer_threshold=0.8,
        acc_change_threshold=3.5,
        steer_change_threshold=1.0,
        acc_change_window=10,
        steer_change_window=10,
    ):
        X_seq = transform_to_sequence_data(
            np.array(X),
            past_length + prediction_length,
            division_indices,
            prediction_step,
        )
        Y_seq = transform_to_sequence_data(
            np.array(Y)[:, state_component_predicted_index],
            past_length + prediction_length,
            division_indices,
            prediction_step,
        )
        X_seq_filtered = []
        Y_seq_filtered = []
        for i in range(X_seq.shape[0]):
            acc = X_seq[i, :, 1]
            steer = X_seq[i, :, 2]
            acc_input = X_seq[i, :, acc_input_indices_nom]
            steer_input = X_seq[i, :, steer_input_indices_nom]
            acc_change = (acc[acc_change_window:] - acc[:-acc_change_window]) / (
                acc_change_window * control_dt
            )
            steer_change = (steer[steer_change_window:] - steer[:-steer_change_window]) / (
                steer_change_window * control_dt
            )
            if (
                (np.abs(acc).max() < acc_threshold)
                and (np.abs(steer).max() < steer_threshold)
                and (np.abs(acc_input).max() < acc_threshold)
                and (np.abs(steer_input).max() < steer_threshold)
                and (np.abs(acc_change).max() < acc_change_threshold)
                and (np.abs(steer_change).max() < steer_change_threshold)
            ):
                X_seq_filtered.append(X_seq[i])
                Y_seq_filtered.append(Y_seq[i])
        return np.array(X_seq_filtered), np.array(Y_seq_filtered)


# with offline data


class TrainErrorPredictionNNWithOfflineData:
    @staticmethod
    def get_initial_hidden_dict(
        model_for_initial_hidden, offline_data_dict
    ):  # get initial_hidden dict ind -> offline features
        initial_hidden_dict = {}
        for key in offline_data_dict.keys():
            if next(model_for_initial_hidden.parameters()).device == "cpu":
                offline_data_tensor = torch.tensor(
                    offline_data_dict[key], dtype=torch.float32
                ).unsqueeze(0)
            else:
                offline_data_tensor = torch.tensor(
                    offline_data_dict[key],
                    dtype=torch.float32,
                    device=next(model_for_initial_hidden.parameters()).device,
                ).unsqueeze(0)

            initial_hidden_dict[key] = model_for_initial_hidden(offline_data_tensor)[0]
        return initial_hidden_dict

    @staticmethod
    def get_initial_hidden_batch(
        initial_hidden_dict,
        indices_batch,  # index as np array indicating which division index is used
        device,
        initial_hidden_dim,
    ):  # get initial_hidden as torch tensor from dict ind -> initial_hidden
        initial_hidden_batch = torch.zeros(
            (len(indices_batch), initial_hidden_dim), dtype=torch.float32, device=device
        )
        for i in range(indices_batch.shape[0]):
            if indices_batch[i] in initial_hidden_dict.keys():
                initial_hidden_batch[i] = initial_hidden_dict[indices_batch[i]]
        return initial_hidden_batch

    @staticmethod
    def get_initial_hidden(
        model_for_initial_hidden,
        offline_data_dict,
        indices_batch,  # index as np array indicating which division index is used
        initial_hidden_dim,
    ):  # get initial_hidden as torch tensor from dict ind -> initial_hidden (before calculating initial_hidden)
        initial_hidden_dict = TrainErrorPredictionNNWithOfflineData.get_initial_hidden_dict(
            model_for_initial_hidden, offline_data_dict
        )
        initial_hidden = TrainErrorPredictionNNWithOfflineData.get_initial_hidden_batch(
            initial_hidden_dict,
            indices_batch,
            next(model_for_initial_hidden.parameters()).device,
            initial_hidden_dim,
        )
        return initial_hidden

    @staticmethod
    def get_loss(
        criterion,
        model,
        model_for_initial_hidden,
        X,
        Y,
        offline_data_dict,
        indices,
        adaptive_weight,
        tanh_gain=10,
        tanh_weight=0.1,
        first_order_weight=0.01,
        second_order_weight=0.01,
        randomize_previous_error=[0.5, 0.1],
        integral_prob=0.0,
        alpha_jacobian=0.01,
        alpha_jacobian_encoder=0.1,
        alpha_jacobian_gru_attention=0.1,
        alpha_hessian=0.01,
        alpha_hessian_encoder=0.1,
        alpha_hessian_gru_attention=0.1,
        calc_jacobian_len=10,
        eps=1e-6,
    ):
        """all tensor and model should be in the same device"""  # index as np array indicating which division index is used # index as np array indicating which division index is used
        initial_hidden_dim = 2 * model.lstm_hidden_total_size
        hc_initial_concat = TrainErrorPredictionNNWithOfflineData.get_initial_hidden(
            model_for_initial_hidden, offline_data_dict, indices, initial_hidden_dim
        )
        randomize_scale_tensor = torch.tensor(randomize_previous_error).to(X.device)
        Y_for_lstm_encoder = Y[:, :past_length, -2:]
        previous_error = Y_for_lstm_encoder + torch.mean(
            torch.abs(Y_for_lstm_encoder), dim=1
        ).unsqueeze(1) * randomize_scale_tensor * torch.randn_like(Y_for_lstm_encoder)

        hc_initial = (
            hc_initial_concat[:, : model.lstm_hidden_total_size].unsqueeze(0).contiguous(),
            hc_initial_concat[:, model.lstm_hidden_total_size :].unsqueeze(0).contiguous(),
        )  # initial hidden state
        Y_pred, hc = model(X, previous_error=previous_error, hc=hc_initial, mode="get_lstm_states")

        # Calculate loss
        loss = criterion(Y_pred * adaptive_weight, Y[:, past_length:] * adaptive_weight)
        if alpha_jacobian is not None:
            random_vector_hc_initial = torch.randn_like(hc_initial_concat)
            random_vector_previous_error = torch.randn_like(previous_error)
            random_vector_X = torch.randn_like(X[:, : past_length + calc_jacobian_len])

            hc_initial_perturbed_concat = hc_initial_concat + eps * random_vector_hc_initial
            hc_initial_perturbed = (
                hc_initial_perturbed_concat[:, : model.lstm_hidden_total_size]
                .unsqueeze(0)
                .contiguous(),
                hc_initial_perturbed_concat[:, model.lstm_hidden_total_size :]
                .unsqueeze(0)
                .contiguous(),
            )
            loss += alpha_jacobian_gru_attention * criterion(
                (hc_initial_perturbed_concat - hc_initial_concat) / eps,
                torch.zeros_like(hc_initial_concat),
            )

            hc_perturbed = model(
                X[:, :past_length] + eps * random_vector_X[:, :past_length],
                previous_error=previous_error + eps * random_vector_previous_error,
                hc=hc_initial_perturbed,
                mode="only_encoder",
            )

            loss += alpha_jacobian_encoder * criterion(
                (hc_perturbed[0] - hc[0]) / eps, torch.zeros_like(hc_initial[0])
            )
            loss += alpha_jacobian_encoder * criterion(
                (hc_perturbed[1] - hc[1]) / eps, torch.zeros_like(hc_initial[1])
            )

            Y_perturbed, _ = model(
                X[:, past_length : past_length + calc_jacobian_len]
                + eps * random_vector_X[:, past_length : past_length + calc_jacobian_len],
                hc=hc_perturbed,
                mode="predict_with_hc",
            )
            loss += alpha_jacobian * criterion(
                (Y_perturbed - Y_pred[:, :calc_jacobian_len]) / eps, torch.zeros_like(Y_perturbed)
            )
            if alpha_hessian is not None:
                hc_initial_perturbed_minus_concat = (
                    hc_initial_concat - eps * random_vector_hc_initial
                )
                hc_initial_perturbed_minus = (
                    hc_initial_perturbed_minus_concat[:, : model.lstm_hidden_total_size]
                    .unsqueeze(0)
                    .contiguous(),
                    hc_initial_perturbed_minus_concat[:, model.lstm_hidden_total_size :]
                    .unsqueeze(0)
                    .contiguous(),
                )
                loss += alpha_hessian_gru_attention * criterion(
                    (
                        hc_initial_perturbed_concat
                        - 2 * hc_initial_concat
                        + hc_initial_perturbed_minus_concat
                    )
                    / eps**2,
                    torch.zeros_like(hc_initial_concat),
                )
                hc_perturbed_minus = model(
                    X[:, :past_length] - eps * random_vector_X[:, :past_length],
                    previous_error=previous_error - eps * random_vector_previous_error,
                    hc=hc_initial_perturbed_minus,
                    mode="only_encoder",
                )
                loss += alpha_hessian_encoder * criterion(
                    (hc_perturbed[0] - 2 * hc[0] + hc_perturbed_minus[0]) / eps**2,
                    torch.zeros_like(hc[0]),
                )
                loss += alpha_hessian_encoder * criterion(
                    (hc_perturbed[1] - 2 * hc[1] + hc_perturbed_minus[1]) / eps**2,
                    torch.zeros_like(hc[1]),
                )
                Y_perturbed_minus, _ = model(
                    X[:, past_length : past_length + calc_jacobian_len]
                    - eps * random_vector_X[:, past_length : past_length + calc_jacobian_len],
                    hc=hc_perturbed_minus,
                    mode="predict_with_hc",
                )
                loss += alpha_hessian * criterion(
                    (Y_perturbed - 2 * Y_pred[:, :calc_jacobian_len] + Y_perturbed_minus) / eps**2,
                    torch.zeros_like(Y_perturbed),
                )
        # Calculate the integrated loss
        if random.random() < integral_prob:
            predicted_states = X[:, past_length, [vel_index, acc_index, steer_index]].unsqueeze(1)
            for i in range(integration_length):
                if i > 0:
                    predicted_states[:, 0, 0] = X[:, past_length + i, vel_index]

                predicted_states_with_input_history = torch.cat(
                    (predicted_states, X[:, [past_length + i], 3:]), dim=2
                )
                predicted_error, hc = model(
                    predicted_states_with_input_history, hc=hc, mode="predict_with_hc"
                )
                for j in range(prediction_step):
                    predicted_states[:, 0, 1] = predicted_states[:, 0, 1] + (
                        X[:, past_length + i, acc_input_indices_nom[j]] - predicted_states[:, 0, 1]
                    ) * (1 - np.exp(-control_dt / acc_time_constant))
                    predicted_states[:, 0, 2] = predicted_states[:, 0, 2] + (
                        X[:, past_length + i, steer_input_indices_nom[j]]
                        - predicted_states[:, 0, 2]
                    ) * (1 - np.exp(-control_dt / steer_time_constant))
                predicted_states[:, 0, -2] = (
                    predicted_states[:, 0, -2]
                    + predicted_error[:, 0, -2] * control_dt * prediction_step
                )
                predicted_states[:, 0, -1] = (
                    predicted_states[:, 0, -1]
                    + predicted_error[:, 0, -1] * control_dt * prediction_step
                )

            loss += (
                adaptive_weight[-2]
                * integration_weight
                * criterion(
                    predicted_states[:, 0, -2], X[:, past_length + integration_length, acc_index]
                )
            )
            loss += (
                adaptive_weight[-1]
                * integration_weight
                * criterion(
                    predicted_states[:, 0, -1], X[:, past_length + integration_length, steer_index]
                )
            )
        # Calculate the tanh loss
        loss += tanh_weight * criterion(
            torch.tanh(tanh_gain * (Y_pred[:, :, -1] - Y[:, past_length:, -1])),
            torch.zeros_like(Y_pred[:, :, -1]),
        )
        # Calculate the first order loss
        first_order_loss = first_order_weight * criterion(
            Y_pred[:, 1:] - Y_pred[:, :-1], torch.zeros_like(Y_pred[:, 1:])
        )
        # Calculate the second order loss
        second_order_loss = second_order_weight * criterion(
            Y_pred[:, 2:] - 2 * Y_pred[:, 1:-1] + Y_pred[:, :-2], torch.zeros_like(Y_pred[:, 2:])
        )
        # Calculate the total loss
        total_loss = loss + first_order_loss + second_order_loss
        return total_loss

    @staticmethod
    def validate_in_batches(
        model,
        model_for_initial_hidden,
        criterion,
        X_val,
        Y_val,
        offline_data_dict_val,
        indices_val,
        adaptive_weight,
        randomize_previous_error=[0.03, 0.03],
        batch_size=1000,
        alpha_jacobian=None,
        alpha_jacobian_encoder=0.1,
        alpha_jacobian_gru_attention=0.1,
        alpha_hessian=None,
        alpha_hessian_encoder=0.1,
        alpha_hessian_gru_attention=0.1,
        calc_jacobian_len=10,
        eps=1e-5,
    ):
        """tensors might be in the different device"""
        device = next(model.parameters()).device
        model.eval()
        model_for_initial_hidden.eval()
        val_loss = 0.0
        num_batches = (X_val.size(0) + batch_size - 1) // batch_size

        with torch.no_grad():
            for i in range(num_batches):
                start_idx = i * batch_size
                end_idx = min((i + 1) * batch_size, X_val.size(0))
                indices_batch = indices_val[start_idx:end_idx]
                if X_val.device != device:
                    X_batch = X_val[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                    Y_batch = Y_val[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                else:
                    X_batch = X_val[start_idx:end_idx]
                    Y_batch = Y_val[start_idx:end_idx]

                loss = TrainErrorPredictionNNWithOfflineData.get_loss(
                    criterion,
                    model,
                    model_for_initial_hidden,
                    X_batch,
                    Y_batch,
                    offline_data_dict_val,
                    indices_batch,
                    adaptive_weight,
                    randomize_previous_error=randomize_previous_error,
                    alpha_jacobian=alpha_jacobian,
                    alpha_jacobian_encoder=alpha_jacobian_encoder,
                    alpha_jacobian_gru_attention=alpha_jacobian_gru_attention,
                    alpha_hessian=alpha_hessian,
                    alpha_hessian_encoder=alpha_hessian_encoder,
                    alpha_hessian_gru_attention=alpha_hessian_gru_attention,
                    calc_jacobian_len=calc_jacobian_len,
                    eps=eps,
                )
                val_loss += loss.item() * (end_idx - start_idx)
        val_loss /= X_val.size(0)
        return val_loss

    @staticmethod
    def get_each_component_loss(
        model,
        model_for_initial_hidden,
        X_val,
        Y_val,
        offline_data_val,
        indices_val,
        tanh_gain=10,
        tanh_weight=0.1,
        first_order_weight=0.01,
        second_order_weight=0.01,
        batch_size=1000,
        window_size=10,
    ):
        """tensors might be in the different device"""
        device = next(model.parameters()).device
        model.eval()
        model_for_initial_hidden.eval()
        initial_hidden_dim = 2 * model.lstm_hidden_total_size

        val_loss = np.zeros(Y_val.size(2))
        tanh_loss = 0.0
        first_order_loss = 0.0
        second_order_loss = 0.0
        num_batches = (X_val.size(0) + batch_size - 1) // batch_size
        Y_pred_list = []
        with torch.no_grad():
            for i in range(num_batches):
                start_idx = i * batch_size
                end_idx = min((i + 1) * batch_size, X_val.size(0))
                indices_batch = indices_val[start_idx:end_idx]
                if X_val.device != device:
                    X_batch = X_val[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                    Y_batch = Y_val[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                else:
                    X_batch = X_val[start_idx:end_idx]
                    Y_batch = Y_val[start_idx:end_idx]
                hc_initial_concat = TrainErrorPredictionNNWithOfflineData.get_initial_hidden(
                    model_for_initial_hidden, offline_data_val, indices_batch, initial_hidden_dim
                )
                hc_initial = (
                    hc_initial_concat[:, : model.lstm_hidden_total_size].unsqueeze(0).contiguous(),
                    hc_initial_concat[:, model.lstm_hidden_total_size :].unsqueeze(0).contiguous(),
                )
                Y_pred, _ = model(
                    X_batch,
                    previous_error=Y_batch[:, :past_length, -2:],
                    hc=hc_initial,
                    mode="get_lstm_states",
                )

                Y_pred_list.append(Y_pred[:, window_size])

                # Calculate the loss
                loss = torch.mean(torch.abs(Y_pred - Y_batch[:, past_length:]), dim=(0, 1))
                val_loss += loss.cpu().numpy() * (end_idx - start_idx)
                tanh_loss += (
                    tanh_weight
                    * torch.mean(
                        torch.abs(
                            torch.tanh(
                                tanh_gain * (Y_pred[:, :, -1] - Y_batch[:, past_length:, -1])
                            )
                        )
                    ).item()
                    * (end_idx - start_idx)
                )
                first_order_loss += (
                    first_order_weight
                    * torch.mean(torch.abs(Y_pred[:, 1:] - Y_pred[:, :-1])).item()
                    * (end_idx - start_idx)
                )
                second_order_loss += (
                    second_order_weight
                    * torch.mean(
                        torch.abs(Y_pred[:, 2:] - 2 * Y_pred[:, 1:-1] + Y_pred[:, :-2])
                    ).item()
                    * (end_idx - start_idx)
                )
        val_loss /= X_val.size(0)
        tanh_loss /= X_val.size(0)
        first_order_loss /= X_val.size(0)
        second_order_loss /= X_val.size(0)
        Y_pred_np = torch.cat(Y_pred_list, dim=0).cpu().detach().numpy()
        return (
            np.concatenate((val_loss, [tanh_loss, first_order_loss, second_order_loss])),
            Y_pred_np,
        )

    @staticmethod
    def get_Y_pred_np(
        model,
        model_for_initial_hidden,
        X_val,
        Y_val,
        offline_data_val,
        indices_val,
        batch_size=1000,
    ):
        """tensors might be in the different device"""
        device = next(model.parameters()).device
        model.eval()
        model_for_initial_hidden.eval()
        initial_hidden_dim = 2 * model.lstm_hidden_total_size
        num_batches = (X_val.size(0) + batch_size - 1) // batch_size
        Y_pred_list = []
        with torch.no_grad():
            for i in range(num_batches):
                start_idx = i * batch_size
                end_idx = min((i + 1) * batch_size, X_val.size(0))
                indices_batch = indices_val[start_idx:end_idx]
                if X_val.device != device:
                    X_batch = X_val[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                    Y_batch = Y_val[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                else:
                    X_batch = X_val[start_idx:end_idx]
                    Y_batch = Y_val[start_idx:end_idx]
                hc_initial_concat = TrainErrorPredictionNNWithOfflineData.get_offline_features(
                    model_for_initial_hidden, offline_data_val, indices_batch, initial_hidden_dim
                )

                hc_initial = (
                    hc_initial_concat[:, : model.lstm_hidden_total_size].unsqueeze(0).contiguous(),
                    hc_initial_concat[:, model.lstm_hidden_total_size :].unsqueeze(0).contiguous(),
                )
                Y_pred, _ = model(
                    X_batch,
                    previous_error=Y_batch[:, :past_length, -2:],
                    hc=hc_initial,
                    mode="get_lstm_states",
                )
                # Y_pred, _ = model(X_batch, previous_error=Y_batch[:, :past_length, -2:], mode="get_lstm_states")
                Y_pred_list.append(Y_pred)
        Y_pred_np = torch.cat(Y_pred_list, dim=0).cpu().detach().numpy()
        return Y_pred_np

    @staticmethod
    def get_losses(
        model, model_for_initial_hidden, criterion, X, Y, offline_data, indices, adaptive_weight
    ):
        loss = TrainErrorPredictionNNWithOfflineData.validate_in_batches(
            model,
            model_for_initial_hidden,
            criterion,
            X,
            Y,
            offline_data,
            indices,
            adaptive_weight=adaptive_weight,
        )
        each_component_loss, Y_pred = TrainErrorPredictionNNWithOfflineData.get_each_component_loss(
            model, model_for_initial_hidden, X, Y, offline_data, indices
        )
        return loss, each_component_loss, Y_pred

    @staticmethod
    def get_acc_steer_model_prediction(
        model,
        model_for_initial_hidden,
        X,
        Y,
        offline_data,
        indices,
        window_size=10,
        batch_size=1000,
    ):
        device = next(model.parameters()).device
        model.eval()
        model_for_initial_hidden.eval()
        initial_hidden_dim = 2 * model.lstm_hidden_total_size
        num_batches = (X.size(0) + batch_size - 1) // batch_size
        prediction = []
        for k in range(num_batches):
            start_idx = k * batch_size
            end_idx = min((k + 1) * batch_size, X.size(0))
            indices_batch = indices[start_idx:end_idx]
            if X.device != device:
                X_batch = X[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                Y_batch = Y[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
            else:
                X_batch = X[start_idx:end_idx]
                Y_batch = Y[start_idx:end_idx]
            hc_initial_concat = TrainErrorPredictionNNWithOfflineData.get_initial_hidden(
                model_for_initial_hidden, offline_data, indices_batch, initial_hidden_dim
            )
            hc_initial = (
                hc_initial_concat[:, : model.lstm_hidden_total_size].unsqueeze(0).contiguous(),
                hc_initial_concat[:, model.lstm_hidden_total_size :].unsqueeze(0).contiguous(),
            )
            _, hc = model(
                X_batch,
                previous_error=Y_batch[:, :past_length, -2:],
                hc=hc_initial,
                mode="get_lstm_states",
            )

            states_tmp = X_batch[:, past_length, [vel_index, acc_index, steer_index]].unsqueeze(1)
            for i in range(window_size):
                states_tmp_with_input_history = torch.cat(
                    (states_tmp, X_batch[:, [past_length + i], 3:]), dim=2
                )
                predicted_error, hc = model(
                    states_tmp_with_input_history, hc=hc, mode="predict_with_hc"
                )
                states_tmp[:, 0, 0] = X_batch[:, past_length + i + 1, 0]
                for j in range(prediction_step):
                    states_tmp[:, 0, 1] = states_tmp[:, 0, 1] + (
                        X_batch[:, past_length + i, acc_input_indices_nom[j]] - states_tmp[:, 0, 1]
                    ) * (1 - np.exp(-control_dt / acc_time_constant))
                    states_tmp[:, 0, 2] = states_tmp[:, 0, 2] + (
                        X_batch[:, past_length + i, steer_input_indices_nom[j]]
                        - states_tmp[:, 0, 2]
                    ) * (1 - np.exp(-control_dt / steer_time_constant))
                states_tmp[:, 0, 1] = (
                    states_tmp[:, 0, 1]
                    + predicted_error[:, 0, state_name_to_predicted_index["acc"]]
                    * control_dt
                    * prediction_step
                )
                states_tmp[:, 0, 2] = (
                    states_tmp[:, 0, 2]
                    + predicted_error[:, 0, state_name_to_predicted_index["steer"]]
                    * control_dt
                    * prediction_step
                )
            prediction.append(states_tmp[:, 0, [1, 2]].detach().to("cpu").numpy())
        prediction = np.concatenate(prediction, axis=0)
        return prediction

    @staticmethod
    def get_acc_steer_models_prediction(
        models,
        model_for_initial_hidden,
        X,
        Y,
        offline_data,
        indices,
        window_size=10,
        batch_size=1000,
    ):
        device = next(models[0].parameters()).device
        for model in models:
            model.eval()
        model_for_initial_hidden.eval()
        initial_hidden_dim = 2 * models[0].lstm_hidden_total_size
        num_batches = (X.size(0) + batch_size - 1) // batch_size
        prediction = []
        for k in range(num_batches):
            start_idx = k * batch_size
            end_idx = min((k + 1) * batch_size, X.size(0))
            indices_batch = indices[start_idx:end_idx]
            if X.device != device:
                X_batch = X[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
                Y_batch = Y[start_idx:end_idx].pin_memory().to(device, non_blocking=True)
            else:
                X_batch = X[start_idx:end_idx]
                Y_batch = Y[start_idx:end_idx]
            hc_initial_concat = TrainErrorPredictionNNWithOfflineData.get_initial_hidden(
                model_for_initial_hidden,
                offline_data,
                indices_batch,
                initial_hidden_dim,
            )

            hc_initial = (
                hc_initial_concat[:, : models[0].lstm_hidden_total_size].unsqueeze(0).contiguous(),
                hc_initial_concat[:, models[0].lstm_hidden_total_size :].unsqueeze(0).contiguous(),
            )
            _, hc = models[0](
                X_batch,
                previous_error=Y_batch[:, :past_length, -2:],
                hc=hc_initial,
                mode="get_lstm_states",
            )

            states_tmp = X_batch[:, past_length, [vel_index, acc_index, steer_index]].unsqueeze(1)
            for i in range(window_size):
                states_tmp_with_input_history = torch.cat(
                    (states_tmp, X_batch[:, [past_length + i], 3:]), dim=2
                )
                predicted_error_0, hc_next = models[0](
                    states_tmp_with_input_history, hc=hc, mode="predict_with_hc"
                )
                predicted_error = predicted_error_0 / len(models)
                for j in range(1, len(models)):
                    predicted_error_j, _ = models[j](
                        states_tmp_with_input_history, hc=hc, mode="predict_with_hc"
                    )
                    predicted_error += predicted_error_j / len(models)
                hc = hc_next
                states_tmp[:, 0, 0] = X_batch[:, past_length + i + 1, 0]
                for j in range(prediction_step):
                    states_tmp[:, 0, 1] = states_tmp[:, 0, 1] + (
                        X_batch[:, past_length + i, acc_input_indices_nom[j]] - states_tmp[:, 0, 1]
                    ) * (1 - np.exp(-control_dt / acc_time_constant))
                    states_tmp[:, 0, 2] = states_tmp[:, 0, 2] + (
                        X_batch[:, past_length + i, steer_input_indices_nom[j]]
                        - states_tmp[:, 0, 2]
                    ) * (1 - np.exp(-control_dt / steer_time_constant))
                states_tmp[:, 0, 1] = (
                    states_tmp[:, 0, 1]
                    + predicted_error[:, 0, state_name_to_predicted_index["acc"]]
                    * control_dt
                    * prediction_step
                )
                states_tmp[:, 0, 2] = (
                    states_tmp[:, 0, 2]
                    + predicted_error[:, 0, state_name_to_predicted_index["steer"]]
                    * control_dt
                    * prediction_step
                )
            prediction.append(states_tmp[:, 0, [1, 2]].detach().to("cpu").numpy())
        prediction = np.concatenate(prediction, axis=0)

    @staticmethod
    def get_model_signed_prediction_error(
        model,
        model_for_initial_hidden,
        X,
        Y,
        offline_data,
        indices,
        window_size=10,
        batch_size=1000,
    ):
        model_prediction = TrainErrorPredictionNNWithOfflineData.get_acc_steer_model_prediction(
            model,
            model_for_initial_hidden,
            X,
            Y,
            offline_data,
            indices,
            window_size=window_size,
            batch_size=batch_size,
        )
        model_prediction_error = (
            X[:, past_length + window_size, [acc_index, steer_index]].detach().to("cpu").numpy()
            - model_prediction
        )
        return model_prediction_error

    @staticmethod
    def get_models_signed_prediction_error(
        models,
        model_for_initial_hidden,
        X,
        Y,
        offline_data,
        indices,
        window_size=10,
        batch_size=1000,
    ):
        model_prediction = TrainErrorPredictionNNWithOfflineData.get_acc_steer_models_prediction(
            models, model_for_initial_hidden, X, Y, offline_data, indices, window_size, batch_size
        )
        model_prediction_error = (
            X[:, past_length + window_size, [acc_index, steer_index]].detach().to("cpu").numpy()
            - model_prediction
        )
        return model_prediction_error

    @staticmethod
    def get_signed_prediction_error(
        model,
        model_for_initial_hidden,
        relearned_model,
        relearned_model_for_initial_hidden,
        X,
        Y,
        offline_data,
        indices,
        window_size=10,
        batch_size=1000,
    ):
        nominal_prediction_error = get_nominal_signed_prediction_error(
            X, window_size, past_length=past_length
        )
        model_prediction_error = (
            TrainErrorPredictionNNWithOfflineData.get_model_signed_prediction_error(
                model,
                model_for_initial_hidden,
                X,
                Y,
                offline_data,
                indices,
                window_size,
                batch_size,
            )
        )
        relearned_model_prediction_error = (
            TrainErrorPredictionNNWithOfflineData.get_model_signed_prediction_error(
                relearned_model,
                relearned_model_for_initial_hidden,
                X,
                Y,
                offline_data,
                indices,
                window_size,
                batch_size,
            )
        )
        return nominal_prediction_error, model_prediction_error, relearned_model_prediction_error

    @staticmethod
    def get_sequence_data(
        X,
        Y,
        division_indices,
        acc_threshold=2.0,
        steer_threshold=0.7,
        acc_change_threshold=3.5,
        steer_change_threshold=0.8,
        acc_change_window=10,
        steer_change_window=10,
    ):
        X_seq, indices = transform_to_sequence_data_with_index(
            np.array(X),
            past_length + prediction_length,
            division_indices,
            prediction_step,
        )
        Y_seq = transform_to_sequence_data(
            np.array(Y)[:, state_component_predicted_index],
            past_length + prediction_length,
            division_indices,
            prediction_step,
        )
        X_seq_filtered = []
        Y_seq_filtered = []
        indices_filtered = []
        for i in range(X_seq.shape[0]):
            acc = X_seq[i, :, 1]
            steer = X_seq[i, :, 2]
            acc_input = X_seq[i, :, acc_input_indices_nom]
            steer_input = X_seq[i, :, steer_input_indices_nom]
            acc_change = (acc[acc_change_window:] - acc[:-acc_change_window]) / (
                acc_change_window * control_dt
            )
            steer_change = (steer[steer_change_window:] - steer[:-steer_change_window]) / (
                steer_change_window * control_dt
            )
            if (
                (np.abs(acc).max() < acc_threshold)
                and (np.abs(steer).max() < steer_threshold)
                and (np.abs(acc_input).max() < acc_threshold)
                and (np.abs(steer_input).max() < steer_threshold)
                and (np.abs(acc_change).max() < acc_change_threshold)
                and (np.abs(steer_change).max() < steer_change_threshold)
            ):
                X_seq_filtered.append(X_seq[i])
                Y_seq_filtered.append(Y_seq[i])
                indices_filtered.append(indices[i])
        X_seq_np = np.array(X_seq_filtered)
        Y_seq_np = np.array(Y_seq_filtered)
        return (
            torch.tensor(X_seq_np, dtype=torch.float32),
            torch.tensor(Y_seq_np, dtype=torch.float32),
            np.array(indices_filtered),
        )


def plot_relearned_vs_original_prediction_error(
    window_size,
    original_train_loss,
    relearned_train_loss,
    original_each_component_train_loss,
    relearned_each_component_train_loss,
    nominal_signed_train_prediction_error,
    original_signed_train_prediction_error,
    relearned_signed_train_prediction_error,
    Y_train,
    Y_train_pred_origin,
    Y_train_pred_relearned,
    original_val_loss,
    relearned_val_loss,
    original_each_component_val_loss,
    relearned_each_component_val_loss,
    nominal_signed_val_prediction_error,
    original_signed_val_prediction_error,
    relearned_signed_val_prediction_error,
    Y_val,
    Y_val_pred_origin,
    Y_val_pred_relearned,
    original_test_loss=None,
    relearned_test_loss=None,
    original_each_component_test_loss=None,
    relearned_each_component_test_loss=None,
    nominal_signed_test_prediction_error=None,
    original_signed_test_prediction_error=None,
    relearned_signed_test_prediction_error=None,
    Y_test=None,
    Y_test_pred_origin=None,
    Y_test_pred_relearned=None,
    plt_save_dir=None,
    past_length=past_length,
):
    print("original_train_loss:", original_train_loss)
    print("relearned_train_loss:", relearned_train_loss)
    print("original_val_loss:", original_val_loss)
    print("relearned_val_loss:", relearned_val_loss)
    if original_test_loss is not None:
        print("original_test_loss:", original_test_loss)
        print("relearned_test_loss:", relearned_test_loss)
    print("original_each_component_train_loss:", original_each_component_train_loss)
    print("relearned_each_component_train_loss:", relearned_each_component_train_loss)
    print("original_each_component_val_loss:", original_each_component_val_loss)
    print("relearned_each_component_val_loss:", relearned_each_component_val_loss)
    if original_each_component_test_loss is not None:
        print("original_each_component_test_loss:", original_each_component_test_loss)
        print("relearned_each_component_test_loss:", relearned_each_component_test_loss)

    print(
        "nominal acc train prediction loss:",
        np.mean(np.abs(nominal_signed_train_prediction_error[:, 0])),
    )
    print(
        "original acc train prediction loss:",
        np.mean(np.abs(original_signed_train_prediction_error[:, 0])),
    )
    print(
        "relearned acc train prediction loss:",
        np.mean(np.abs(relearned_signed_train_prediction_error[:, 0])),
    )
    print(
        "nominal steer train prediction loss:",
        np.mean(np.abs(nominal_signed_train_prediction_error[:, 1])),
    )
    print(
        "original steer train prediction loss:",
        np.mean(np.abs(original_signed_train_prediction_error[:, 1])),
    )
    print(
        "relearned steer train prediction loss:",
        np.mean(np.abs(relearned_signed_train_prediction_error[:, 1])),
    )
    print(
        "nominal acc val prediction loss:",
        np.mean(np.abs(nominal_signed_val_prediction_error[:, 0])),
    )
    print(
        "original acc val prediction loss:",
        np.mean(np.abs(original_signed_val_prediction_error[:, 0])),
    )
    print(
        "relearned acc val prediction loss:",
        np.mean(np.abs(relearned_signed_val_prediction_error[:, 0])),
    )
    print(
        "nominal steer val prediction loss:",
        np.mean(np.abs(nominal_signed_val_prediction_error[:, 1])),
    )
    print(
        "original steer val prediction loss:",
        np.mean(np.abs(original_signed_val_prediction_error[:, 1])),
    )
    print(
        "relearned steer val prediction loss:",
        np.mean(np.abs(relearned_signed_val_prediction_error[:, 1])),
    )
    if nominal_signed_test_prediction_error is not None:
        print(
            "nominal acc test prediction loss:",
            np.mean(np.abs(nominal_signed_test_prediction_error[:, 0])),
        )
        print(
            "original acc test prediction loss:",
            np.mean(np.abs(original_signed_test_prediction_error[:, 0])),
        )
        print(
            "relearned acc test prediction loss:",
            np.mean(np.abs(relearned_signed_test_prediction_error[:, 0])),
        )
        print(
            "nominal steer test prediction loss:",
            np.mean(np.abs(nominal_signed_test_prediction_error[:, 1])),
        )
        print(
            "original steer test prediction loss:",
            np.mean(np.abs(original_signed_test_prediction_error[:, 1])),
        )
        print(
            "relearned steer test prediction loss:",
            np.mean(np.abs(relearned_signed_test_prediction_error[:, 1])),
        )
    if plt_save_dir is not None:
        if nominal_signed_test_prediction_error is None:
            fig, axes = plt.subplots(nrows=2, ncols=2, figsize=(24, 15), tight_layout=True)
        else:
            fig, axes = plt.subplots(nrows=2, ncols=3, figsize=(24, 15), tight_layout=True)
        fig.suptitle("acc steer prediction error")
        axes[0, 0].plot(nominal_signed_train_prediction_error[:, 0], label="nominal")
        axes[0, 0].plot(original_signed_train_prediction_error[:, 0], label="original")
        axes[0, 0].plot(relearned_signed_train_prediction_error[:, 0], label="relearned")
        axes[0, 0].scatter(
            np.arange(len(nominal_signed_train_prediction_error[:, 0])),
            np.zeros(len(nominal_signed_train_prediction_error[:, 0])),
            s=1,
        )
        axes[0, 0].set_title("acc prediction error for training data")
        axes[0, 0].legend()
        axes[0, 1].plot(nominal_signed_val_prediction_error[:, 0], label="nominal")
        axes[0, 1].plot(original_signed_val_prediction_error[:, 0], label="original")
        axes[0, 1].plot(relearned_signed_val_prediction_error[:, 0], label="relearned")
        axes[0, 1].scatter(
            np.arange(len(nominal_signed_val_prediction_error[:, 0])),
            np.zeros(len(nominal_signed_val_prediction_error[:, 0])),
            s=1,
        )
        axes[0, 1].set_title("acc prediction error for validation data")
        axes[0, 1].legend()
        axes[1, 0].plot(nominal_signed_train_prediction_error[:, 1], label="nominal")
        axes[1, 0].plot(original_signed_train_prediction_error[:, 1], label="original")
        axes[1, 0].plot(relearned_signed_train_prediction_error[:, 1], label="relearned")
        axes[1, 0].scatter(
            np.arange(len(nominal_signed_train_prediction_error[:, 1])),
            np.zeros(len(nominal_signed_train_prediction_error[:, 1])),
            s=1,
        )
        axes[1, 0].set_title("steer prediction error for training data")
        axes[1, 0].legend()
        axes[1, 1].plot(nominal_signed_val_prediction_error[:, 1], label="nominal")
        axes[1, 1].plot(original_signed_val_prediction_error[:, 1], label="original")
        axes[1, 1].plot(relearned_signed_val_prediction_error[:, 1], label="relearned")
        axes[1, 1].scatter(
            np.arange(len(nominal_signed_val_prediction_error[:, 1])),
            np.zeros(len(nominal_signed_val_prediction_error[:, 1])),
            s=1,
        )
        axes[1, 1].set_title("steer prediction error for validation data")
        axes[1, 1].legend()
        if nominal_signed_test_prediction_error is not None:
            axes[0, 2].plot(nominal_signed_test_prediction_error[:, 0], label="nominal")
            axes[0, 2].plot(original_signed_test_prediction_error[:, 0], label="original")
            axes[0, 2].plot(relearned_signed_test_prediction_error[:, 0], label="relearned")
            axes[0, 2].scatter(
                np.arange(len(nominal_signed_test_prediction_error[:, 0])),
                np.zeros(len(nominal_signed_test_prediction_error[:, 0])),
                s=1,
            )
            axes[0, 2].set_title("acc prediction error for test data")
            axes[0, 2].legend()
            axes[1, 2].plot(nominal_signed_test_prediction_error[:, 1], label="nominal")
            axes[1, 2].plot(original_signed_test_prediction_error[:, 1], label="original")
            axes[1, 2].plot(relearned_signed_test_prediction_error[:, 1], label="relearned")
            axes[1, 2].scatter(
                np.arange(len(nominal_signed_test_prediction_error[:, 1])),
                np.zeros(len(nominal_signed_test_prediction_error[:, 1])),
                s=1,
            )
            axes[1, 2].set_title("steer prediction error for test data")
            axes[1, 2].legend()
        if not os.path.isdir(plt_save_dir):
            os.mkdir(plt_save_dir)
        plt.savefig(plt_save_dir + "/acc_steer_prediction_error.png")
        plt.close()
        Y_nominal_train_pred = Y_train[:, past_length + window_size, :].detach().to("cpu").numpy()
        Y_nominal_val_pred = Y_val[:, past_length + window_size, :].detach().to("cpu").numpy()
        if Y_test is None:
            fig, axes = plt.subplots(
                nrows=len(state_component_predicted), ncols=2, figsize=(24, 15), tight_layout=True
            )
        else:
            Y_nominal_test_pred = Y_test[:, past_length + window_size, :].detach().to("cpu").numpy()
            fig, axes = plt.subplots(
                nrows=len(state_component_predicted), ncols=3, figsize=(24, 15), tight_layout=True
            )
        fig.suptitle("each component error")
        for i in range(len(state_component_predicted)):
            axes[i, 0].plot(Y_nominal_train_pred[:, i], label="nominal")
            axes[i, 0].plot(
                Y_nominal_train_pred[:, i] - Y_train_pred_origin[:, i], label="original"
            )
            axes[i, 0].plot(
                Y_nominal_train_pred[:, i] - Y_train_pred_relearned[:, i], label="relearned"
            )
            axes[i, 0].scatter(
                np.arange(len(Y_nominal_train_pred[:, i])),
                np.zeros(len(Y_nominal_train_pred[:, i])),
                s=1,
            )
            axes[i, 0].set_title(state_component_predicted[i] + " error for training data")
            axes[i, 0].legend()
            axes[i, 1].plot(Y_nominal_val_pred[:, i], label="nominal")
            axes[i, 1].plot(Y_nominal_val_pred[:, i] - Y_val_pred_origin[:, i], label="original")
            axes[i, 1].plot(
                Y_nominal_val_pred[:, i] - Y_val_pred_relearned[:, i], label="relearned"
            )
            axes[i, 1].scatter(
                np.arange(len(Y_nominal_val_pred[:, i])),
                np.zeros(len(Y_nominal_val_pred[:, i])),
                s=1,
            )
            axes[i, 1].set_title(state_component_predicted[i] + " error for validation data")
            axes[i, 1].legend()
            if Y_test is not None:
                axes[i, 2].plot(Y_nominal_test_pred[:, i], label="nominal")
                axes[i, 2].plot(
                    Y_nominal_test_pred[:, i] - Y_test_pred_origin[:, i], label="original"
                )
                axes[i, 2].plot(
                    Y_nominal_test_pred[:, i] - Y_test_pred_relearned[:, i], label="relearned"
                )
                axes[i, 2].scatter(
                    np.arange(len(Y_nominal_test_pred[:, i])),
                    np.zeros(len(Y_nominal_test_pred[:, i])),
                    s=4,
                )
                axes[i, 2].set_title(state_component_predicted[i] + " error for test data")
                axes[i, 2].legend()
        plt.savefig(plt_save_dir + "/each_component_error.png")
        plt.close()
