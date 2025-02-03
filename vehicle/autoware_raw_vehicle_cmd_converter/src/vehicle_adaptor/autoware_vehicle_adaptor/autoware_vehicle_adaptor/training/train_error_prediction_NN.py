import copy
import json
import os
from pathlib import Path
import random
import types

from autoware_vehicle_adaptor.param import parameters
from autoware_vehicle_adaptor.training import add_data_from_csv
from autoware_vehicle_adaptor.training import convert_model_to_csv
from autoware_vehicle_adaptor.training import error_prediction_NN
from autoware_vehicle_adaptor.training.early_stopping import EarlyStopping
from autoware_vehicle_adaptor.training.training_utils import (
    plot_relearned_vs_original_prediction_error,
)
from autoware_vehicle_adaptor.training.training_utils import TrainErrorPredictionNNFunctions
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

get_loss = TrainErrorPredictionNNFunctions.get_loss
validate_in_batches = TrainErrorPredictionNNFunctions.validate_in_batches
get_each_component_loss = TrainErrorPredictionNNFunctions.get_each_component_loss
get_losses = TrainErrorPredictionNNFunctions.get_losses
get_signed_prediction_error = TrainErrorPredictionNNFunctions.get_signed_prediction_error
get_sequence_data = TrainErrorPredictionNNFunctions.get_sequence_data

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


class train_error_prediction_NN(add_data_from_csv.add_data_from_csv):
    """Class for training the error prediction NN."""

    def __init__(
        self, max_iter=10000, tol=1e-5, alpha_1=0.1**7, alpha_2=0.1**7, alpha_jacobian=0.1**4
    ):
        super().__init__()
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.max_iter = max_iter
        self.tol = tol
        self.alpha_1 = alpha_1
        self.alpha_2 = alpha_2
        self.alpha_jacobian = alpha_jacobian
        self.models = None
        self.weights_for_dataloader = None

        self.past_length = past_length
        self.prediction_length = prediction_length
        self.prediction_step = prediction_step
        self.acc_queue_size = acc_queue_size
        self.steer_queue_size = steer_queue_size
        self.adaptive_weight = torch.ones(len(state_component_predicted_index)).to(self.device)

    def train_model(
        self,
        model: error_prediction_NN.ErrorPredictionNN,
        X_train: torch.Tensor,
        Y_train: torch.Tensor,
        batch_sizes: list,
        learning_rates: list,
        patience: int,
        X_val: torch.Tensor,
        Y_val: torch.Tensor,
        fix_lstm: bool = False,
        randomize_fix_lstm: float = 0.001,
        integration_prob: float = 0.1,
        X_replay: torch.Tensor | None = None,
        Y_replay: torch.Tensor | None = None,
        replay_data_rate: float = 0.05,
    ):
        """Train the error prediction NN."""

        model = model.to(self.device)
        print("sample_size: ", X_train.shape[0] + X_val.shape[0])
        print("patience: ", patience)
        # Define the loss function.
        criterion = nn.L1Loss()
        # Fix the LSTM
        if fix_lstm:
            self.fix_lstm(model, randomize=randomize_fix_lstm)
        # save the original adaptive weight
        original_adaptive_weight = self.adaptive_weight.clone()
        print("original_adaptive_weight: ", original_adaptive_weight)
        # Define the optimizer.
        optimizer = torch.optim.Adam(model.parameters(), lr=learning_rates[0])
        # Define the initial loss.
        initial_loss = validate_in_batches(
            model, criterion, X_val, Y_val, adaptive_weight=self.adaptive_weight
        )
        print("initial_loss: ", initial_loss)
        batch_size = batch_sizes[0]
        print("batch_size: ", batch_size)
        # Define the early stopping object.
        early_stopping = EarlyStopping(initial_loss, tol=self.tol, patience=patience)
        # Data Loader
        if self.weights_for_dataloader is None:
            weighted_sampler = None
            train_dataset = DataLoader(
                TensorDataset(X_train, Y_train), batch_size=batch_size, shuffle=True
            )
        else:
            weighted_sampler = WeightedRandomSampler(
                weights=self.weights_for_dataloader,
                num_samples=len(self.weights_for_dataloader),
                replacement=True,
            )
            train_dataset = DataLoader(
                TensorDataset(X_train, Y_train), batch_size=batch_size, sampler=weighted_sampler
            )
        # learning_rate index
        learning_rate_index = 0
        # batch_size index
        # Print learning rate
        print("learning rate: ", learning_rates[learning_rate_index])
        # Train the model.
        for i in range(self.max_iter):
            model.train()

            for X_batch, Y_batch in train_dataset:
                optimizer.zero_grad()
                # outputs = model(X_batch)
                loss = get_loss(
                    criterion,
                    model,
                    X_batch,
                    Y_batch,
                    adaptive_weight=self.adaptive_weight,
                    integral_prob=integration_prob,
                    alpha_jacobian=self.alpha_jacobian,
                )
                for w in model.parameters():
                    loss += self.alpha_1 * torch.norm(w, 1) + self.alpha_2 * torch.norm(w, 2) ** 2
                if X_replay is not None:
                    replay_mask = torch.rand(X_replay.size(0)) < replay_data_rate * X_train.size(
                        0
                    ) / X_replay.size(0)
                    X_replay_batch = X_replay[replay_mask]
                    Y_replay_batch = Y_replay[replay_mask]
                    loss += get_loss(
                        criterion,
                        model,
                        X_replay_batch,
                        Y_replay_batch,
                        adaptive_weight=self.adaptive_weight,
                        integral_prob=integration_prob,
                        alpha_jacobian=self.alpha_jacobian,
                    )
                loss.backward()
                optimizer.step()
            model.eval()
            val_loss = validate_in_batches(
                model, criterion, X_val, Y_val, adaptive_weight=self.adaptive_weight
            )
            val_loss_with_original_weight = validate_in_batches(
                model, criterion, X_val, Y_val, adaptive_weight=original_adaptive_weight
            )
            if i % 10 == 1:
                print("epoch: ", i)
                print("val_loss with original weight: ", val_loss_with_original_weight)
                print("val_loss: ", val_loss)
            if early_stopping(val_loss):
                learning_rate_index += 1
                batch_size = batch_sizes[min(learning_rate_index, len(batch_sizes) - 1)]
                if learning_rate_index >= len(learning_rates):
                    break
                batch_size = batch_sizes[min(learning_rate_index, len(batch_sizes) - 1)]
                optimizer = torch.optim.Adam(
                    model.parameters(), lr=learning_rates[learning_rate_index]
                )
                print("update learning rate to ", learning_rates[learning_rate_index])
                print("batch size:", batch_size)
                if self.weights_for_dataloader is None:
                    train_dataset = DataLoader(
                        TensorDataset(X_train, Y_train), batch_size=batch_size, shuffle=True
                    )
                else:
                    train_dataset = DataLoader(
                        TensorDataset(X_train, Y_train),
                        batch_size=batch_size,
                        sampler=weighted_sampler,
                    )
                early_stopping.reset()
                if learning_rates[learning_rate_index - 1] < 3e-4:
                    self.update_adaptive_weight(model, X_train, Y_train)
                print("adaptive_weight: ", self.adaptive_weight)

    def relearn_model(
        self,
        model: error_prediction_NN.ErrorPredictionNN,
        X_train: torch.Tensor,
        Y_train: torch.Tensor,
        batch_sizes: list,
        learning_rates: list,
        patience: int,
        X_val: torch.Tensor,
        Y_val: torch.Tensor,
        fix_lstm: bool = False,
        randomize_fix_lstm: float = 0.001,
        integration_prob: float = 0.1,
        randomize: float = 0.001,
        reset_weight: bool = False,
        X_test=None,
        Y_test=None,
        X_replay=None,
        Y_replay=None,
        replay_data_rate=0.05,
        plt_save_dir=None,
        window_size=10,
        save_path=None,
        always_update_model=False,
    ):
        print("randomize: ", randomize)
        self.update_adaptive_weight(model, X_train, Y_train)
        original_adaptive_weight = self.adaptive_weight.clone()
        criterion = nn.L1Loss()
        original_train_loss, original_each_component_train_loss, Y_train_pred_origin = get_losses(
            model, criterion, X_train, Y_train, adaptive_weight=original_adaptive_weight
        )
        original_val_loss, original_each_component_val_loss, Y_val_pred_origin = get_losses(
            model, criterion, X_val, Y_val, adaptive_weight=original_adaptive_weight
        )
        if X_test is not None:
            original_test_loss, original_each_component_test_loss, Y_test_pred_origin = get_losses(
                model, criterion, X_test, Y_test, adaptive_weight=original_adaptive_weight
            )
        else:
            original_test_loss = None
            original_each_component_test_loss = None
            Y_test_pred_origin = None
        if reset_weight:
            relearned_model = error_prediction_NN.ErrorPredictionNN(
                prediction_length=prediction_length,
                state_component_predicted=state_component_predicted,
            ).to(self.device)
        else:
            relearned_model = copy.deepcopy(model)
            relearned_model.lstm_encoder.flatten_parameters()
            relearned_model.lstm.flatten_parameters()
            with torch.no_grad():
                if fix_lstm:
                    relearned_model.complimentary_layer[0].weight += randomize * torch.randn_like(
                        model.complimentary_layer[0].weight
                    )
                    relearned_model.complimentary_layer[0].bias += randomize * torch.randn_like(
                        model.complimentary_layer[0].bias
                    )
                    relearned_model.linear_relu[0].weight += randomize * torch.randn_like(
                        model.linear_relu[0].weight
                    )
                    relearned_model.linear_relu[0].bias += randomize * torch.randn_like(
                        model.linear_relu[0].bias
                    )
                    relearned_model.final_layer.weight += randomize * torch.randn_like(
                        model.final_layer.weight
                    )
                    relearned_model.final_layer.bias += randomize * torch.randn_like(
                        model.final_layer.bias
                    )
                else:
                    for w in relearned_model.parameters():
                        w += randomize * torch.randn_like(w)
        self.train_model(
            relearned_model,
            X_train,
            Y_train,
            batch_sizes,
            learning_rates,
            patience,
            X_val,
            Y_val,
            fix_lstm=fix_lstm,
            randomize_fix_lstm=randomize_fix_lstm,
            integration_prob=integration_prob,
            X_replay=X_replay,
            Y_replay=Y_replay,
            replay_data_rate=replay_data_rate,
        )
        relearned_train_loss, relearned_each_component_train_loss, Y_train_pred_relearned = (
            get_losses(
                relearned_model,
                criterion,
                X_train,
                Y_train,
                adaptive_weight=original_adaptive_weight,
            )
        )
        relearned_val_loss, relearned_each_component_val_loss, Y_val_pred_relearned = get_losses(
            relearned_model, criterion, X_val, Y_val, adaptive_weight=original_adaptive_weight
        )
        if X_test is not None:
            relearned_test_loss, relearned_each_component_test_loss, Y_test_pred_relearned = (
                get_losses(
                    relearned_model,
                    criterion,
                    X_test,
                    Y_test,
                    adaptive_weight=original_adaptive_weight,
                )
            )
        else:
            relearned_test_loss = None
            relearned_each_component_test_loss = None
            Y_test_pred_relearned = None

        (
            nominal_signed_train_prediction_error,
            original_signed_train_prediction_error,
            relearned_signed_train_prediction_error,
        ) = get_signed_prediction_error(model, relearned_model, X_train, Y_train, window_size)
        (
            nominal_signed_val_prediction_error,
            original_signed_val_prediction_error,
            relearned_signed_val_prediction_error,
        ) = get_signed_prediction_error(model, relearned_model, X_val, Y_val, window_size)

        if X_test is not None:
            (
                nominal_signed_test_prediction_error,
                original_signed_test_prediction_error,
                relearned_signed_test_prediction_error,
            ) = get_signed_prediction_error(model, relearned_model, X_test, Y_test, window_size)
        else:
            nominal_signed_test_prediction_error = None
            original_signed_test_prediction_error = None
            relearned_signed_test_prediction_error = None

        plot_relearned_vs_original_prediction_error(
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
            original_test_loss,
            relearned_test_loss,
            original_each_component_test_loss,
            relearned_each_component_test_loss,
            nominal_signed_test_prediction_error,
            original_signed_test_prediction_error,
            relearned_signed_test_prediction_error,
            Y_test,
            Y_test_pred_origin,
            Y_test_pred_relearned,
            plt_save_dir,
        )
        if save_path is not None:
            self.save_given_model(relearned_model, save_path)
        if relearned_val_loss < original_val_loss or always_update_model:
            return relearned_model, True
        else:
            return model, False

    def get_trained_model(
        self, learning_rates=[1e-3, 1e-4, 1e-5, 1e-6], patience=10, batch_sizes=[100, 10, 100]
    ):
        print("state_component_predicted: ", state_component_predicted)
        # Define Time Series Data
        X_train_np, Y_train_np = get_sequence_data(
            self.X_train_list, self.Y_train_list, self.division_indices_train
        )
        X_val_np, Y_val_np = get_sequence_data(
            self.X_val_list, self.Y_val_list, self.division_indices_val
        )
        self.model = error_prediction_NN.ErrorPredictionNN(
            prediction_length=prediction_length, state_component_predicted=state_component_predicted
        ).to(self.device)
        self.update_adaptive_weight(
            None,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
        )
        self.train_model(
            self.model,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            torch.tensor(X_val_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_val_np, dtype=torch.float32, device=self.device),
        )

    def get_relearned_model(
        self,
        learning_rates=[1e-3, 1e-4, 1e-5, 1e-6],
        patience=10,
        batch_sizes=[100],
        reset_weight=False,
        randomize=0.001,
        plt_save_dir=None,
        save_path=None,
        use_replay_data=False,
        replay_data_rate=0.05,
        always_update_model=False,
    ):
        self.model.to(self.device)
        # Define Time Series Data
        X_train_np, Y_train_np = get_sequence_data(
            self.X_train_list, self.Y_train_list, self.division_indices_train
        )
        X_val_np, Y_val_np = get_sequence_data(
            self.X_val_list, self.Y_val_list, self.division_indices_val
        )
        if len(self.X_test_list) > 0:
            X_test_np, Y_test_np = get_sequence_data(
                self.X_test_list, self.Y_test_list, self.division_indices_test
            )
            X_test = torch.tensor(X_test_np, dtype=torch.float32, device=self.device)
            Y_test = torch.tensor(Y_test_np, dtype=torch.float32, device=self.device)
        else:
            X_test = None
            Y_test = None
        if use_replay_data and len(self.X_replay_list) > 0:
            X_replay_np, Y_replay_np = get_sequence_data(
                self.X_replay_list, self.Y_replay_list, self.division_indices_replay
            )
            X_replay = torch.tensor(X_replay_np, dtype=torch.float32, device=self.device)
            Y_replay = torch.tensor(Y_replay_np, dtype=torch.float32, device=self.device)
        else:
            X_replay = None
            Y_replay = None

        self.model, updated = self.relearn_model(
            self.model,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            torch.tensor(X_val_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_val_np, dtype=torch.float32, device=self.device),
            randomize=randomize,
            X_test=X_test,
            Y_test=Y_test,
            reset_weight=reset_weight,
            X_replay=X_replay,
            Y_replay=Y_replay,
            replay_data_rate=replay_data_rate,
            plt_save_dir=plt_save_dir,
            save_path=save_path,
            always_update_model=always_update_model,
        )
        return updated

    def initialize_ensemble_models(self):
        self.models = [self.model]

    def get_updated_temp_model(
        self,
        learning_rates=[1e-3, 1e-4, 1e-5, 1e-6],
        patience=10,
        batch_sizes=[100, 10, 100],
        use_replay_data=False,
        replay_data_rate=0.05,
        randomize_fix_lstm=0.0,
    ):
        self.temp_model = copy.deepcopy(self.model)
        X_train_np, Y_train_np = get_sequence_data(
            self.X_train_list, self.Y_train_list, self.division_indices_train
        )
        X_val_np, Y_val_np = get_sequence_data(
            self.X_val_list, self.Y_val_list, self.division_indices_val
        )
        if use_replay_data and len(self.X_replay_list) > 0:
            X_replay_np, Y_replay_np = get_sequence_data(
                self.X_replay_list, self.Y_replay_list, self.division_indices_replay
            )
            X_replay = torch.tensor(X_replay_np, dtype=torch.float32, device=self.device)
            Y_replay = torch.tensor(Y_replay_np, dtype=torch.float32, device=self.device)
        else:
            X_replay = None
            Y_replay = None
        self.temp_model.to(self.device)
        self.update_adaptive_weight(
            self.temp_model,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
        )
        self.train_model(
            self.temp_model,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            torch.tensor(X_val_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_val_np, dtype=torch.float32, device=self.device),
            fix_lstm=True,
            randomize_fix_lstm=randomize_fix_lstm,
            X_replay=X_replay,
            Y_replay=Y_replay,
            replay_data_rate=replay_data_rate,
        )

    def relearn_temp_model(
        self,
        learning_rates=[1e-3, 1e-4, 1e-5, 1e-6],
        patience=10,
        batch_sizes=[100],
        randomize=0.001,
        plt_save_dir=None,
        save_path=None,
        use_replay_data=False,
        replay_data_rate=0.05,
        randomize_fix_lstm=0.0,
    ):
        self.temp_model.to(self.device)
        # Define Time Series Data
        X_train_np, Y_train_np = get_sequence_data(
            self.X_train_list, self.Y_train_list, self.division_indices_train
        )
        X_val_np, Y_val_np = get_sequence_data(
            self.X_val_list, self.Y_val_list, self.division_indices_val
        )
        if len(self.X_test_list) > 0:
            X_test_np, Y_test_np = get_sequence_data(
                self.X_test_list, self.Y_test_list, self.division_indices_test
            )
            X_test = torch.tensor(X_test_np, dtype=torch.float32, device=self.device)
            Y_test = torch.tensor(Y_test_np, dtype=torch.float32, device=self.device)
        else:
            X_test = None
            Y_test = None
        if use_replay_data and len(self.X_replay_list) > 0:
            X_replay_np, Y_replay_np = get_sequence_data(
                self.X_replay_list, self.Y_replay_list, self.division_indices_replay
            )
            X_replay = torch.tensor(X_replay_np, dtype=torch.float32, device=self.device)
            Y_replay = torch.tensor(Y_replay_np, dtype=torch.float32, device=self.device)
        else:
            X_replay = None
            Y_replay = None

        self.temp_model, updated = self.relearn_model(
            self.temp_model,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            torch.tensor(X_val_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_val_np, dtype=torch.float32, device=self.device),
            randomize=randomize,
            X_test=X_test,
            Y_test=Y_test,
            X_replay=X_replay,
            Y_replay=Y_replay,
            replay_data_rate=replay_data_rate,
            plt_save_dir=plt_save_dir,
            save_path=save_path,
            fix_lstm=True,
            randomize_fix_lstm=randomize_fix_lstm,
        )
        return updated

    def add_temp_model_to_ensemble(self):
        self.models.append(self.temp_model)

    def get_trained_ensemble_models(
        self,
        learning_rates=[1e-3, 1e-4, 1e-5, 1e-6],
        patience=10,
        batch_sizes=[100, 10, 100],
        ensemble_size=5,
    ):
        print("state_component_predicted: ", state_component_predicted)
        # Define Time Series Data
        X_train_np, Y_train_np = get_sequence_data(
            self.X_train_list, self.Y_train_list, self.division_indices_train
        )
        X_val_np, Y_val_np = get_sequence_data(
            self.X_val_list, self.Y_val_list, self.division_indices_val
        )
        self.model = error_prediction_NN.ErrorPredictionNN(
            prediction_length=prediction_length, state_component_predicted=state_component_predicted
        ).to(self.device)
        print("______________________________")
        print("ensemble number: ", 0)
        print("______________________________")
        self.train_model(
            self.model,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            torch.tensor(X_val_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_val_np, dtype=torch.float32, device=self.device),
        )
        self.models = [self.model]
        for i in range(ensemble_size - 1):
            print("______________________________")
            print("ensemble number: ", i + 1)
            print("______________________________")
            temp_model = copy.deepcopy(self.model)
            self.train_model(
                temp_model,
                torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
                torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
                batch_sizes,
                learning_rates,
                patience,
                torch.tensor(X_val_np, dtype=torch.float32, device=self.device),
                torch.tensor(Y_val_np, dtype=torch.float32, device=self.device),
                fix_lstm=True,
            )
            self.models.append(temp_model)

    def update_saved_model(
        self, path, learning_rates=[1e-4, 1e-5, 1e-6], patience=10, batch_sizes=[100, 10, 100]
    ):
        X_train_np, Y_train_np = get_sequence_data(
            self.X_train_list, self.Y_train_list, self.division_indices_train
        )
        X_val_np, Y_val_np = get_sequence_data(
            self.X_val_list, self.Y_val_list, self.division_indices_val
        )
        self.model = torch.load(path)
        self.model.to(self.device)
        self.train_model(
            self.model,
            torch.tensor(X_train_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_train_np, dtype=torch.float32, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            torch.tensor(X_val_np, dtype=torch.float32, device=self.device),
            torch.tensor(Y_val_np, dtype=torch.float32, device=self.device),
        )

    def save_model(self, path="vehicle_model.pth"):
        self.model.to("cpu")
        torch.save(self.model, path)
        save_dir = path.replace(".pth", "")
        convert_model_to_csv.convert_model_to_csv(self.model, save_dir)

    def save_given_model(self, model, path="vehicle_model.pth"):
        model.to("cpu")
        torch.save(model, path)
        save_dir = path.replace(".pth", "")
        convert_model_to_csv.convert_model_to_csv(model, save_dir)

    def save_ensemble_models(self, paths):
        for i in range(len(paths)):
            temp_model = self.models[i]
            temp_model.to("cpu")
            torch.save(temp_model, paths[i])
            save_dir = paths[i].replace(".pth", "")
            convert_model_to_csv.convert_model_to_csv(temp_model, save_dir)

    def fix_lstm(
        self,
        model,
        randomize=0.001,
    ):
        # freeze the encoder layers
        for param in model.acc_encoder_layer_1.parameters():
            param.requires_grad = False
        for param in model.acc_encoder_layer_2.parameters():
            param.requires_grad = False
        for param in model.steer_encoder_layer_1.parameters():
            param.requires_grad = False
        for param in model.steer_encoder_layer_2.parameters():
            param.requires_grad = False
        for param in model.lstm_encoder.parameters():
            param.requires_grad = False
        # freeze shallow layers of the decoder
        for param in model.acc_layer_1.parameters():
            param.requires_grad = False
        for param in model.steer_layer_1.parameters():
            param.requires_grad = False
        for param in model.acc_layer_2.parameters():
            param.requires_grad = False
        for param in model.steer_layer_2.parameters():
            param.requires_grad = False
        for param in model.lstm.parameters():
            param.requires_grad = False

        # lb = -randomize
        # ub = randomize
        # nn.init.uniform_(model.complimentary_layer[0].weight, a=lb, b=ub)
        # nn.init.uniform_(model.complimentary_layer[0].bias, a=lb, b=ub)
        # nn.init.uniform_(model.linear_relu[0].weight, a=lb, b=ub)
        # nn.init.uniform_(model.linear_relu[0].bias, a=lb, b=ub)
        # nn.init.uniform_(model.final_layer.weight, a=lb, b=ub)
        # nn.init.uniform_(model.final_layer.bias, a=lb, b=ub)
        with torch.no_grad():
            model.complimentary_layer[0].weight += randomize * torch.randn_like(
                model.complimentary_layer[0].weight
            )
            model.complimentary_layer[0].bias += randomize * torch.randn_like(
                model.complimentary_layer[0].bias
            )
            model.linear_relu[0].weight += randomize * torch.randn_like(model.linear_relu[0].weight)
            model.linear_relu[0].bias += randomize * torch.randn_like(model.linear_relu[0].bias)
            model.final_layer.weight += randomize * torch.randn_like(model.final_layer.weight)
            model.final_layer.bias += randomize * torch.randn_like(model.final_layer.bias)

    def update_adaptive_weight(self, model, X, Y, batch_size=3000):
        if model is not None:
            model.to(self.device)
            model.eval()
        num_batches = (X.size(0) + batch_size - 1) // batch_size
        prediction_error = torch.zeros(self.adaptive_weight.shape[0], device=self.device)
        with torch.no_grad():
            for i in range(num_batches):
                start_idx = i * batch_size
                end_idx = min((i + 1) * batch_size, X.size(0))

                X_batch = X[start_idx:end_idx]
                Y_batch = Y[start_idx:end_idx]
                if model is not None:
                    Y_pred, _ = model(
                        X_batch,
                        previous_error=Y_batch[:, :past_length, -2:],
                        mode="get_lstm_states",
                    )
                    prediction_error += torch.mean(
                        torch.abs(Y_pred - Y_batch[:, past_length:]), dim=(0, 1)
                    ) * (end_idx - start_idx)
                else:
                    prediction_error += torch.mean(
                        torch.abs(Y_batch[:, past_length:]), dim=(0, 1)
                    ) * (end_idx - start_idx)
            prediction_error /= X.size(0)
        print("prediction_error:", prediction_error)
        self.adaptive_weight = 1.0 / (prediction_error + 1e-4)
        for i in range(len(self.adaptive_weight)):
            if self.adaptive_weight[i] > torch.max(self.adaptive_weight[-2:]):
                self.adaptive_weight[i] = torch.max(
                    self.adaptive_weight[-2:]
                )  # acc and steer are respected
        self.adaptive_weight = self.adaptive_weight / torch.mean(self.adaptive_weight)
