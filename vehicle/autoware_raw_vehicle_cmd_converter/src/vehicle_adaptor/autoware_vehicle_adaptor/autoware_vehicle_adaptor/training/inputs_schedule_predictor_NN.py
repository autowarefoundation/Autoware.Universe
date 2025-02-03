import copy
import csv
import os
from typing import List
from typing import Tuple

from autoware_vehicle_adaptor.param import parameters
from autoware_vehicle_adaptor.training import convert_model_to_csv
from autoware_vehicle_adaptor.training.early_stopping import EarlyStopping
import numpy as np
import scipy.interpolate
from scipy.ndimage import gaussian_filter
import torch
import torch.nn as nn

# Number of steps in the history used as input to the neural network.
input_step = max(
    parameters.controller_acc_input_history_len, parameters.controller_steer_input_history_len
)
# Number of steps in the future to predict.
output_step = max(
    parameters.acc_input_schedule_prediction_len, parameters.steer_input_schedule_prediction_len
)
control_dt = parameters.control_dt  # Time step for the controller
acc_cmd_smoothing_sigma = 10.0  # smoothing parameter for acceleration command
steer_cmd_smoothing_sigma = 10.0  # smoothing parameter for steering command


def data_smoothing(data: np.ndarray, sigma: float) -> np.ndarray:
    """
    Apply a Gaussian filter to smooth the input data.
    - data: Input array to smooth.
    - sigma: Smoothing parameter.
    """
    data_smoothed = gaussian_filter(data, sigma)
    return data_smoothed


# Neural Network definition: Input Schedule Predictor
class InputsSchedulePredictorNN(nn.Module):
    """
    LSTM-based model for predicting input schedules (e.g., acceleration and steering).
    Output is a sequence of change rates for the input values.
    """

    def __init__(
        self,
        dataset_num: int,
        lstm_hidden_size: int = 64,
        post_decoder_hidden_size: Tuple[int, int] = (16, 16),
        control_dt: float = 0.033,
        target_size: int = 35,
        input_rate_limit: float = 1.0,
        augmented_input_size: int = 15,
        vel_scaling: float = 0.1,
        vel_bias: float = 0.0,
    ):
        """
        Constructor
        - dataset_num: Number of controller types included in the data
        - lstm_hidden_size: Number of hidden units in the LSTM.
        - post_decoder_hidden_size: Hidden layer sizes in the post-decoder fully connected network.
        - control_dt: Time step for controller.
        - target_size: Prediction horizon size.
        - input_rate_limit: Maximum limit for the output rate of change.
        - augmented_input_size: Size of the augmented input features, derived from the history of inputs for the decoder.
        - vel_scaling: Scaling factor for velocity inputs.
        - vel_bias: Bias to apply to velocity inputs.
        """
        super(InputsSchedulePredictorNN, self).__init__()
        # Initialize parameters
        self.dataset_num = dataset_num
        self.control_dt = control_dt
        self.target_size = target_size
        self.input_size = 2  # Input dimensions (velocity and input signals)
        self.output_size = 1  # Single output dimension (rate of change)
        self.augmented_input_size = augmented_input_size
        self.vel_scaling = vel_scaling
        self.vel_bias = vel_bias
        # Define the LSTM layers for encoding and decoding
        self.lstm_encoder = nn.LSTM(
            self.input_size, lstm_hidden_size, num_layers=2, batch_first=True
        )
        self.lstm_decoder = nn.LSTM(
            self.output_size + self.augmented_input_size,
            lstm_hidden_size,
            num_layers=2,
            batch_first=True,
        )
        # Fully connected layers after the LSTM decoder
        self.post_decoder = nn.Sequential(
            nn.Linear(lstm_hidden_size, post_decoder_hidden_size[0]), nn.ReLU()
        )
        # Dataset-specific adaptive scaling parameters
        self.post_decoder_adaptive_scales = nn.ParameterList(
            [nn.Parameter(torch.ones(post_decoder_hidden_size[0])) for _ in range(dataset_num)]
        )
        # Final layers to produce the output
        self.finalize = nn.Sequential(
            nn.Linear(post_decoder_hidden_size[0], post_decoder_hidden_size[1]),
            nn.ReLU(),
            nn.Linear(post_decoder_hidden_size[1], self.output_size),
            nn.Tanh(),
        )
        # Define input transformation matrices for augmented inputs, focusing on updating the history of inputs during decoding.
        input_transition_matrix = torch.zeros(self.augmented_input_size, self.augmented_input_size)
        input_transition_matrix[1:, :-1] = torch.eye(self.augmented_input_size - 1)
        input_transition_matrix[-1, -1] = 1.0
        add_new_input = torch.zeros(1, self.augmented_input_size)
        add_new_input[0, -1] = 1.0
        # Register as non-trainable buffers
        self.register_buffer("input_transition_matrix", input_transition_matrix)
        self.register_buffer("add_new_input", add_new_input)
        # Input rate limit for the model outputs
        self.input_rate_limit = input_rate_limit

    def forward(
        self,
        x: torch.Tensor,
        dataset_idx: torch.Tensor,
        true_prediction: torch.Tensor | None = None,
    ) -> torch.Tensor:
        """
        Forward pass of the model.
        - x: Input sequence (batch_size, seq_length, input_size).
        - dataset_idx: Index of the dataset for adaptive scaling.
        - true_prediction: True values for teacher forcing during training.
        """
        device = x.device
        # Scale the velocity input
        x_vel_scaled = x.clone()
        x_vel_scaled[:, :, 0] = (x_vel_scaled[:, :, 0] - self.vel_bias) * self.vel_scaling
        # Retrieve adaptive scales for the dataset
        post_decoder_scales_tensor = torch.stack(list(self.post_decoder_adaptive_scales)).to(device)
        post_decoder_scales = post_decoder_scales_tensor[dataset_idx].unsqueeze(1)
        # Encode the input sequence using the LSTM encoder
        _, (hidden_raw, cell_raw) = self.lstm_encoder(x_vel_scaled)
        hidden = hidden_raw
        cell = cell_raw
        # Initialize decoder inputs
        decoder_input_head = x[:, -self.augmented_input_size :, -1]
        decoder_input_tail = (x[:, [-1], -1] - x[:, [-2], -1]) / self.control_dt
        outputs = []
        # Perform decoding for the target size
        for i in range(self.target_size):
            decoder_output, (hidden, cell) = self.lstm_decoder(
                torch.cat([decoder_input_head, decoder_input_tail], dim=1).unsqueeze(1),
                (hidden, cell),
            )
            # Apply the post-decoder layers
            decoder_output = self.post_decoder(decoder_output)
            decoder_output = decoder_output * post_decoder_scales
            output = self.finalize(decoder_output)[:, :, 0] * self.input_rate_limit

            outputs.append(output)
            if true_prediction is not None:  # Teacher forcing case
                decoder_input_head = (
                    decoder_input_head @ self.input_transition_matrix
                    + true_prediction[:, i, :] @ self.add_new_input * self.control_dt
                )
                decoder_input_tail = true_prediction[:, i, :]
            else:
                decoder_input_head = (
                    decoder_input_head @ self.input_transition_matrix
                    + output @ self.add_new_input * self.control_dt
                )
                decoder_input_tail = output
        # Concatenate outputs
        outputs_tensor = torch.cat(outputs, dim=1).unsqueeze(2)
        return outputs_tensor


class AddDataFromCsv:
    """
    A helper class for loading and managing training and validation data from CSV files.
    """

    def __init__(self) -> None:
        # Initialize lists to store data for acceleration and steering
        self.X_acc_train_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for acceleration training input sequences
        self.Y_acc_train_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for acceleration training target sequences
        self.X_steer_train_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for steering training input sequences
        self.Y_steer_train_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for steering training target sequences
        self.indices_train_list: List[int] = (
            []
        )  # List of indices indicating boundaries between datasets in training data
        self.X_acc_val_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for acceleration validation input sequences
        self.Y_acc_val_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for acceleration validation target sequences
        self.X_steer_val_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for steering validation input sequences
        self.Y_steer_val_list: List[np.ndarray] = (
            []
        )  # List of NumPy arrays for steering validation target sequences
        self.indices_val_list: List[int] = []
        self.dataset_num: int = 0  # Number of controller types included in the data

    def clear_data(self):
        """
        Clear all stored data to reset the class instance.
        """
        self.X_acc_train_list = []
        self.Y_acc_train_list = []
        self.X_steer_train_list = []
        self.Y_steer_train_list = []
        self.indices_train_list = []
        self.X_acc_val_list = []
        self.Y_acc_val_list = []
        self.X_steer_val_list = []
        self.Y_steer_val_list = []
        self.indices_val_list = []
        self.dataset_num = 0

    def add_data_from_csv(
        self,
        dir_name: str,
        add_mode: str = "as_train",
        control_cmd_mode: str | None = None,
        dataset_idx: int = 0,
        reverse_steer: bool = False,
    ) -> None:
        """
        Load data from CSV files and prepare it for training or validation.
        - dir_name: Path to the directory containing the data files.
        - add_mode: Specify "as_train" or "as_val" to determine how to store the data.
        - control_cmd_mode: Specify which control command CSV to use (e.g., "control_command").
        - dataset_idx: Index of the dataset being added, representing the controller type.
        - reverse_steer: If True, reverse the sign of the steering commands.
        """
        # Load localization state data
        # columns: time_sec, time_nsec, x, y, quaternion_x, quaternion_y, quaternion_z, quaternion_w, linear_velocity_x
        localization_kinematic_state = np.loadtxt(
            dir_name + "/localization_kinematic_state.csv",
            delimiter=",",
            usecols=[0, 1, 4, 5, 7, 8, 9, 10, 47],
        )
        vel = localization_kinematic_state[:, 8]
        # Load control command based on the specified mode
        # columns: time_sec, time_nsec, steering input, acceleration input
        if control_cmd_mode == "compensated_control_cmd":  # compensated by the vehicle adaptor
            control_cmd = np.loadtxt(
                dir_name + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        elif control_cmd_mode == "control_command":  # controller inputs after the filter
            control_cmd = np.loadtxt(
                dir_name + "/control_command_control_cmd.csv", delimiter=",", usecols=[0, 1, 8, 16]
            )
        elif (
            control_cmd_mode == "control_trajectory_follower"
        ):  # controller raw inputs by the trajectory follower node
            control_cmd = np.loadtxt(
                dir_name + "/control_trajectory_follower_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        elif (
            control_cmd_mode == "external_selected"
        ):  # controller inputs by the external selector, e.g., data collecting tool
            control_cmd = np.loadtxt(
                dir_name + "/external_selected_control_cmd.csv",
                delimiter=",",
                usecols=[0, 1, 8, 16],
            )
        elif control_cmd_mode is None:
            # Automatically determine which file to load based on availability
            if os.path.exists(dir_name + "/control_command_control_cmd.csv"):
                control_cmd = np.loadtxt(
                    dir_name + "/control_command_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            elif os.path.exists(dir_name + "/control_trajectory_follower_control_cmd.csv"):
                control_cmd = np.loadtxt(
                    dir_name + "/control_trajectory_follower_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            elif os.path.exists(dir_name + "/external_selected_control_cmd.csv"):
                control_cmd = np.loadtxt(
                    dir_name + "/external_selected_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            elif os.path.exists(
                dir_name + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv"
            ):
                control_cmd = np.loadtxt(
                    dir_name
                    + "/vehicle_raw_vehicle_cmd_converter_debug_compensated_control_cmd.csv",
                    delimiter=",",
                    usecols=[0, 1, 8, 16],
                )
            else:
                print("control command csv is not found")
                return
        else:
            print("control_cmd_mode is invalid")
            return
        # Extract acceleration and steering commands
        acc_cmd = control_cmd[:, 3]
        steer_cmd = control_cmd[:, 2]
        # Load system operation mode data, and determine the operation start and end times
        system_operation_mode_state = np.loadtxt(
            dir_name + "/system_operation_mode_state.csv", delimiter=",", usecols=[0, 1, 2]
        )
        if system_operation_mode_state.ndim == 1:
            system_operation_mode_state = system_operation_mode_state.reshape(1, -1)
        with open(dir_name + "/system_operation_mode_state.csv") as f:
            reader = csv.reader(f, delimiter=",")
            autoware_control_enabled_str = np.array([row[3] for row in reader])

        control_enabled = np.zeros(system_operation_mode_state.shape[0])
        for i in range(system_operation_mode_state.shape[0]):
            if (
                system_operation_mode_state[i, 2] > 1.5
                and autoware_control_enabled_str[i] == "True"
            ):
                control_enabled[i] = 1.0
        for i in range(system_operation_mode_state.shape[0] - 1):
            if control_enabled[i] < 0.5 and control_enabled[i + 1] > 0.5:
                operation_start_time = (
                    system_operation_mode_state[i + 1, 0]
                    + 1e-9 * system_operation_mode_state[i + 1, 1]
                )
            elif control_enabled[i] > 0.5 and control_enabled[i + 1] < 0.5:
                operation_end_time = (
                    system_operation_mode_state[i + 1, 0]
                    + 1e-9 * system_operation_mode_state[i + 1, 1]
                )
                break
            operation_end_time = (
                localization_kinematic_state[-1, 0] + 1e-9 * localization_kinematic_state[-1, 1]
            )
        if system_operation_mode_state.shape[0] == 1:
            operation_end_time = (
                localization_kinematic_state[-1, 0] + 1e-9 * localization_kinematic_state[-1, 1]
            )
        if control_enabled[0] > 0.5:
            operation_start_time = (
                system_operation_mode_state[0, 0] + 1e-9 * system_operation_mode_state[0, 1]
            )
        print("operation_start_time", operation_start_time)
        print("operation_end_time", operation_end_time)
        # Synchronize timestamps and interpolate data
        min_time_stamp = max(
            [
                operation_start_time,
                localization_kinematic_state[0, 0] + 1e-9 * localization_kinematic_state[0, 1],
                control_cmd[0, 0] + 1e-9 * control_cmd[0, 1],
            ]
        )
        max_time_stamp = min(
            [
                operation_end_time,
                localization_kinematic_state[-1, 0] + 1e-9 * localization_kinematic_state[-1, 1],
                control_cmd[-1, 0] + 1e-9 * control_cmd[-1, 1],
            ]
        )
        data_num = int((max_time_stamp - min_time_stamp) / control_dt)
        data_time_stamps = min_time_stamp + control_dt * np.arange(data_num)
        # Interpolate velocity and control commands
        vel_interp = scipy.interpolate.interp1d(
            localization_kinematic_state[:, 0] + 1e-9 * localization_kinematic_state[:, 1], vel
        )(data_time_stamps)
        acc_cmd_interp = scipy.interpolate.interp1d(
            control_cmd[:, 0] + 1e-9 * control_cmd[:, 1], acc_cmd
        )(data_time_stamps)
        steer_cmd_interp = scipy.interpolate.interp1d(
            control_cmd[:, 0] + 1e-9 * control_cmd[:, 1], steer_cmd
        )(data_time_stamps)
        # Apply smoothing to commands
        acc_cmd_smoothed = data_smoothing(acc_cmd_interp, acc_cmd_smoothing_sigma)
        steer_cmd_smoothed = data_smoothing(steer_cmd_interp, steer_cmd_smoothing_sigma)
        # Prepare data segments for inputs and targets
        X_acc = []
        X_steer = []
        Y_acc = []
        Y_steer = []
        indices = []
        for i in range(data_num - input_step - output_step):
            if vel_interp[i + input_step] < 0.1:  # Skip low velocity data
                continue
            # Acceleration input sequence data
            X_acc.append(
                np.stack(
                    [
                        vel_interp[i : i + input_step],
                        acc_cmd_interp[i : i + input_step],
                    ]
                ).T
            )
            # Compute the rate of change for acceleration commands
            Y_acc.append(
                np.array(
                    acc_cmd_smoothed[i + input_step : i + input_step + output_step]
                    - acc_cmd_smoothed[i + input_step - 1 : i + input_step + output_step - 1]
                ).reshape(-1, 1)
                / control_dt
            )

            # Steering input with optional reversal
            if reverse_steer:
                # Reverse the sign of the steering command for compatibility
                X_steer.append(
                    np.stack(
                        [
                            vel_interp[i : i + input_step],
                            -steer_cmd_interp[i : i + input_step],
                        ]
                    ).T
                )
                # Compute the rate of change for reversed steering commands
                Y_steer.append(
                    -np.array(
                        steer_cmd_smoothed[i + input_step : i + input_step + output_step]
                        - steer_cmd_smoothed[i + input_step - 1 : i + input_step + output_step - 1]
                    ).reshape(-1, 1)
                    / control_dt
                )
            else:
                # Use the original steering commands without reversal
                X_steer.append(
                    np.stack(
                        [
                            vel_interp[i : i + input_step],
                            steer_cmd_interp[i : i + input_step],
                        ]
                    ).T
                )
                # Compute the rate of change for original steering commands
                Y_steer.append(
                    np.array(
                        steer_cmd_smoothed[i + input_step : i + input_step + output_step]
                        - steer_cmd_smoothed[i + input_step - 1 : i + input_step + output_step - 1]
                    ).reshape(-1, 1)
                    / control_dt
                )

            # Store the dataset index for each data segment, which indicating the controller type
            indices.append(dataset_idx)
        # Increment dataset count if this dataset is new
        if dataset_idx not in self.indices_train_list and dataset_idx not in self.indices_val_list:
            self.dataset_num += 1
        # Store the data into the appropriate lists based on the mode
        if add_mode == "as_train":
            # Append to training data
            self.X_acc_train_list += X_acc
            self.Y_acc_train_list += Y_acc
            self.X_steer_train_list += X_steer
            self.Y_steer_train_list += Y_steer
            self.indices_train_list += indices
        elif add_mode == "as_val":
            # Append to validation data
            self.X_acc_val_list += X_acc
            self.Y_acc_val_list += Y_acc
            self.X_steer_val_list += X_steer
            self.Y_steer_val_list += Y_steer
            self.indices_val_list += indices
        else:
            print("add_mode is invalid")
            return


def generate_random_vector(
    batch_size: int,
    seq_len: int,
    state_dim: int,
    dt: float,
    device: torch.device,
    vel_scaling: float,
) -> torch.Tensor:
    """
    Generate a random vector for perturbation purposes.
    - batch_size: Number of samples in the batch.
    - seq_len: Length of the sequence.
    - state_dim: Dimensionality of each state in the sequence.
    - dt: Time step for integration.
    - device: Torch device (e.g., "cpu" or "cuda").
    - vel_scaling: Scaling factor for velocity.
    Returns: Integrated random vector.
    """
    # Generate random vector
    random_vector = torch.randn(batch_size, seq_len, state_dim, device=device)
    # Scale and integrate over time
    random_vector[:, 1:, :] = random_vector[:, 1:, :] * dt
    random_vector[:, :, 0] = random_vector[:, :, 0] / vel_scaling  # Scale the velocity
    random_vector_integrated = torch.cumsum(random_vector, dim=1)
    return random_vector_integrated


def get_loss(
    criterion: nn.Module,
    model: InputsSchedulePredictorNN,
    X: torch.Tensor,
    Y: torch.Tensor,
    indices: torch.Tensor,
    stage_error_weight: float = 1e-4,
    prediction_error_weight: float = 100.0,
    max_small_input_weight: float = 50.0,
    small_input_threshold: float = 0.01,
    second_order_weight: float = 1e-2,
    integral_points: List[float] = [0.2, 0.5, 1.0],
    integral_weights: List[float] = [1e-3, 1e-2, 1.0],
    tanh_gain: float = 2.0,
    tanh_weight: float = 0.1,
    use_true_prediction: bool = False,
    alpha_jacobian: float | None = None,
    eps: float = 1e-5,
) -> torch.Tensor:
    """
    Compute the loss function for training the model.
    - criterion: Base loss function (e.g., L1 loss).
    - model: The prediction model.
    - X: Input tensor (batch_size, seq_len, input_dim).
    - Y: Target tensor (batch_size, seq_len, output_dim).
    - indices: Dataset indices for adaptive scaling.
    - stage_error_weight: Weight for stage-wise error in the loss.
    - prediction_error_weight: Weight for prediction errors.
    - max_small_input_weight: Maximum weight for small input penalties.
    - small_input_threshold: Threshold to define "small" inputs.
    - second_order_weight: Weight for penalties on second-order derivatives.
    - integral_points: Points within the prediction horizon for integral constraints.
    - integral_weights: Weights for integral penalties at different points.
    - tanh_gain: Gain parameter for the tanh penalty.
    - tanh_weight: Weight for the tanh penalty term.
    - use_true_prediction: Whether to use teacher-forced predictions.
    - alpha_jacobian: Weight for Jacobian penalties (optional).
    - eps: Small value for numerical stability.
    Returns: Computed loss as a Torch tensor.
    """
    device = X.device
    # Generate random perturbations for Jacobian calculation
    if alpha_jacobian is not None:
        random_vector = (
            generate_random_vector(
                X.size(0), X.size(1), X.size(2), control_dt, device, model.vel_scaling
            )
            * eps
        )
    # Forward pass through the model
    if use_true_prediction:  # Teacher forcing
        Y_pred = model(X, indices, Y)
        if alpha_jacobian is not None:
            Y_perturbed = model(X + random_vector, indices, Y)
    else:
        Y_pred = model(X, indices)
        if alpha_jacobian is not None:
            Y_perturbed = model(X + random_vector, indices)
    # Compute stage-wise loss
    loss = stage_error_weight * criterion(Y_pred, Y)
    # Add Jacobian penalty if applicable
    if alpha_jacobian is not None:
        loss = loss + alpha_jacobian * torch.mean(torch.abs((Y_perturbed - Y_pred) / eps))
    # inputs predicted by the model
    input_future = torch.mean(torch.abs(X[:, -1, 1] + torch.cumsum(Y, dim=1) * control_dt), dim=1)
    # weight for emphasizing small inputs
    small_input_weight = torch.where(
        input_future > small_input_threshold,
        torch.tensor(1.0, device=X.device),
        1.0 / (input_future + 1.0 / max_small_input_weight),
    )

    # Compute the prediction error
    for i in range(len(integral_points)):
        loss = loss + integral_weights[i] * tanh_weight * torch.mean(
            torch.abs(
                torch.tanh(
                    tanh_gain
                    * (
                        Y_pred[:, : int(model.target_size * integral_points[i]), 0].sum(dim=1)
                        - Y[:, : int(model.target_size * integral_points[i]), 0].sum(dim=1)
                    )
                )
            )
        )
        loss = loss + integral_weights[i] * prediction_error_weight * torch.mean(
            small_input_weight
            * torch.abs(
                Y_pred[:, : int(model.target_size * integral_points[i])].sum(dim=1) * control_dt
                - Y[:, : int(model.target_size * integral_points[i])].sum(dim=1) * control_dt
            )
        )
    # Add penalty for second-order derivatives
    loss = loss + second_order_weight * torch.mean(
        torch.abs((Y_pred[:, 1:] - Y_pred[:, :-1]) / control_dt)
    )
    # Add penalty for the final time step.
    # This cost assumes the rate of change for the next input is zero.
    # To match the scale with other stages, the cost is divided by model.target_size.
    loss = (
        loss
        + second_order_weight
        * torch.mean(torch.abs(Y_pred[:, -1] / control_dt))
        / model.target_size
    )
    return loss


def validate_in_batches(
    criterion: nn.Module,
    model: InputsSchedulePredictorNN,
    X_val: torch.Tensor,
    Y_val: torch.Tensor,
    indices: torch.Tensor,
    batch_size: int = 1000,
    tanh_gain: float = 2.0,
    tanh_weight: float = 0.1,
    max_small_input_weight: float = 50.0,
    small_input_threshold: float = 0.01,
    alpha_jacobian: float | None = None,
) -> float:
    """
    Validate the model in batches to avoid memory overflow.
    - criterion: Base loss function (e.g., L1 loss).
    - model: The prediction model.
    - X_val: Validation input tensor (batch_size, seq_len, input_dim).
    - Y_val: Validation target tensor (batch_size, seq_len, output_dim).
    - indices: Dataset indices for adaptive scaling.
    - batch_size: Size of each validation batch.
    - tanh_gain: Gain parameter for the tanh penalty.
    - tanh_weight: Weight for the tanh penalty term.
    - max_small_input_weight: Maximum weight for small input penalties.
    - small_input_threshold: Threshold to define "small" inputs.
    - alpha_jacobian: Weight for Jacobian penalties (optional).
    Returns: Average validation loss.
    """
    model.eval()
    val_loss = 0.0
    num_batches = (X_val.size(0) + batch_size - 1) // batch_size
    with torch.no_grad():
        for i in range(num_batches):
            start = i * batch_size
            end = start + batch_size
            X_val_batch = X_val[start:end]
            Y_val_batch = Y_val[start:end]
            indices_batch = indices[start:end]
            # Compute loss for the current batch
            loss = get_loss(
                criterion,
                model,
                X_val_batch,
                Y_val_batch,
                indices_batch,
                tanh_gain=tanh_gain,
                tanh_weight=tanh_weight,
                max_small_input_weight=max_small_input_weight,
                small_input_threshold=small_input_threshold,
                alpha_jacobian=alpha_jacobian,
            ).item()
            val_loss += loss * (end - start)  # Scale by batch size
    val_loss /= X_val.size(0)  # Normalize by total validation size
    return val_loss


class TrainInputsSchedulePredictorNN(AddDataFromCsv):
    """
    A class for training the Inputs Schedule Predictor Neural Network.
    Inherits data handling methods from AddDataFromCsv.
    """

    def __init__(
        self,
        max_iter: int = 10000,
        tol: float = 1e-5,
        alpha_1: float = 0.1**7,
        alpha_2: float = 0.1**7,
        tanh_gain_acc: float = 2.0,
        tanh_weight_acc: float = 0.1,
        tanh_gain_steer: float = 10.0,
        tanh_weight_steer: float = 0.01,
        alpha_jacobian: float | None = None,
    ) -> None:
        """
        Initialize the training class.
        - max_iter: Maximum number of iterations for training.
        - tol: Tolerance for early stopping.
        - alpha_1: L2 regularization weight for model parameters.
        - alpha_2: L1 regularization weight for model parameters.
        - tanh_gain_acc: Gain parameter for the tanh penalty on acceleration.
        - tanh_weight_acc: Weight for the tanh penalty on acceleration.
        - tanh_gain_steer: Gain parameter for the tanh penalty on steering.
        - tanh_weight_steer: Weight for the tanh penalty on steering.
        - alpha_jacobian: Weight for the Jacobian penalty (optional).
        """
        super().__init__()
        self.max_iter = max_iter
        self.tol = tol
        self.alpha_1 = alpha_1
        self.alpha_2 = alpha_2
        self.alpha_jacobian = alpha_jacobian
        self.tanh_gain_acc = tanh_gain_acc
        self.tanh_gain_steer = tanh_gain_steer
        self.tanh_weight_acc = tanh_weight_acc
        self.tanh_weight_steer = tanh_weight_steer
        self.device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
        self.model_acc: InputsSchedulePredictorNN | None = None
        self.model_steer: InputsSchedulePredictorNN | None = None

    def train_model(
        self,
        model: InputsSchedulePredictorNN,
        X_train: torch.Tensor,
        Y_train: torch.Tensor,
        indices_train: torch.Tensor,
        X_val: torch.Tensor,
        Y_val: torch.Tensor,
        indices_val: torch.Tensor,
        batch_sizes: List[int],
        learning_rates: List[float],
        patience: int,
        tanh_gain: float,
        tanh_weight: float,
        max_small_input_weight: float = 50.0,
        small_input_threshold: float = 0.01,
    ) -> None:
        """
        Train the model using the given dataset.
        - model: The Inputs Schedule Predictor Neural Network to train.
        - X_train: Training input tensor.
        - Y_train: Training target tensor.
        - indices_train: Dataset indices for training samples.
        - X_val: Validation input tensor.
        - Y_val: Validation target tensor.
        - indices_val: Dataset indices for validation samples.
        - batch_sizes: List of batch sizes to use during training.
        - learning_rates: List of learning rates to use during training.
        - patience: Number of epochs to wait for improvement before early stopping.
        - tanh_gain: Gain parameter for the tanh penalty.
        - tanh_weight: Weight for the tanh penalty term.
        - max_small_input_weight: Maximum weight for small input penalties.
        - small_input_threshold: Threshold to define "small" inputs.
        """
        print("sample_size:", X_train.shape[0] + X_val.shape[0])
        print("training_size:", X_train.shape[0])
        print("validation_size:", X_val.shape[0])
        print("patience:", patience)
        # Define the loss function.
        criterion = nn.L1Loss()
        # Define the optimizer.
        optimizer = torch.optim.Adam(model.parameters(), lr=learning_rates[0])
        # Compute the initial validation loss
        initial_loss = validate_in_batches(
            criterion,
            model,
            X_val,
            Y_val,
            indices_val,
            tanh_gain=tanh_gain,
            tanh_weight=tanh_weight,
            max_small_input_weight=max_small_input_weight,
            small_input_threshold=small_input_threshold,
        )
        print("initial_loss:", initial_loss)
        batch_size = batch_sizes[0]

        print("batch_size:", batch_size)
        # Define the early stopping object.
        early_stopping = EarlyStopping(initial_loss, tol=self.tol, patience=patience)
        # Data loader for training.
        train_dataset = torch.utils.data.TensorDataset(X_train, Y_train, indices_train)
        train_loader = torch.utils.data.DataLoader(
            train_dataset, batch_size=batch_size, shuffle=True
        )
        # learning_rate index
        learning_rate_index = 0
        print("learning rate:", learning_rates[learning_rate_index])
        # Training loop.
        for i in range(self.max_iter):
            model.train()
            for X_batch, Y_batch, indices_batch in train_loader:
                optimizer.zero_grad()
                # Use true prediction for large learning rates
                use_true_prediction = learning_rates[learning_rate_index] > 5e-3
                # Compute loss
                loss = get_loss(
                    criterion,
                    model,
                    X_batch,
                    Y_batch,
                    indices_batch,
                    use_true_prediction=use_true_prediction,
                    tanh_gain=tanh_gain,
                    tanh_weight=tanh_weight,
                    max_small_input_weight=max_small_input_weight,
                    small_input_threshold=small_input_threshold,
                    alpha_jacobian=self.alpha_jacobian,
                )
                # Add L2 and L1 regularization terms
                for w in model.named_parameters():
                    if "adaptive_scales" not in w[0]:  # Exclude adaptive scales from regularization
                        loss += self.alpha_1 * torch.sum(w[1] ** 2)  # L2 regularization
                        loss += self.alpha_2 * torch.sum(torch.abs(w[1]))  # L1 regularization
                # Add penalty for adaptive scales
                for j in range(self.dataset_num):
                    index_rates = (indices_batch == j).sum().item() / indices_batch.size(0)
                    loss += (
                        self.alpha_1
                        * index_rates
                        * torch.mean(torch.abs(model.post_decoder_adaptive_scales[j] - 1.0))
                    )
                # Backpropagation
                loss.backward()
                optimizer.step()
            # Validation step
            model.eval()
            val_loss = validate_in_batches(
                criterion,
                model,
                X_val,
                Y_val,
                indices_val,
                tanh_gain=tanh_gain,
                tanh_weight=tanh_weight,
                max_small_input_weight=max_small_input_weight,
                small_input_threshold=small_input_threshold,
            )
            if i % 10 == 0:
                print(val_loss, i)
            if early_stopping(val_loss):
                learning_rate_index += 1
                batch_size = batch_sizes[min(learning_rate_index, len(batch_sizes) - 1)]
                if learning_rate_index >= len(learning_rates):
                    break
                else:
                    print("update learning rate to ", learning_rates[learning_rate_index])
                    optimizer = torch.optim.Adam(
                        model.parameters(), lr=learning_rates[learning_rate_index]
                    )
                    train_loader = torch.utils.data.DataLoader(
                        train_dataset, batch_size=batch_size, shuffle=True
                    )
                    early_stopping.reset()

    def relearn_model(
        self,
        model: InputsSchedulePredictorNN,
        X_train: torch.Tensor,
        Y_train: torch.Tensor,
        indices_train: torch.Tensor,
        X_val: torch.Tensor,
        Y_val: torch.Tensor,
        indices_val: torch.Tensor,
        batch_sizes: List[int],
        learning_rates: List[float],
        patience: int,
        tanh_gain: float,
        tanh_weight: float,
        max_small_input_weight: float = 50.0,
        small_input_threshold: float = 0.01,
        randomize: float = 0.001,
    ) -> Tuple[InputsSchedulePredictorNN, bool]:
        """
        Fine-tune the model by adding random noise to parameters and retraining.
        - model: The initial trained model.
        - X_train: Training input tensor.
        - Y_train: Training target tensor.
        - indices_train: Dataset indices for training samples.
        - X_val: Validation input tensor.
        - Y_val: Validation target tensor.
        - indices_val: Dataset indices for validation samples.
        - batch_sizes: List of batch sizes to use during retraining.
        - learning_rates: List of learning rates to use during retraining.
        - patience: Number of epochs to wait for improvement before early stopping.
        - tanh_gain: Gain parameter for the tanh penalty.
        - tanh_weight: Weight for the tanh penalty term.
        - max_small_input_weight: Maximum weight for small input penalties.
        - small_input_threshold: Threshold to define "small" inputs.
        - randomize: Amount of random noise added to model parameters.
        Returns: The retrained model.
        """
        # Flatten LSTM parameters
        model.lstm_decoder.flatten_parameters()
        model.lstm_encoder.flatten_parameters()
        # Define the loss function.
        criterion = nn.L1Loss()
        # Compute the original validation loss
        original_val_loss = validate_in_batches(
            criterion,
            model,
            X_val,
            Y_val,
            indices_val,
            tanh_gain=tanh_gain,
            tanh_weight=tanh_weight,
            max_small_input_weight=max_small_input_weight,
            small_input_threshold=small_input_threshold,
        )
        # Copy the model and add random noise to its parameters
        relearned_model = copy.deepcopy(model)
        relearned_model.lstm_decoder.flatten_parameters()
        relearned_model.lstm_encoder.flatten_parameters()
        with torch.no_grad():
            for w in relearned_model.parameters():
                w.copy_(w + randomize * torch.randn_like(w))
        # Retrain the model
        self.train_model(
            relearned_model,
            X_train,
            Y_train,
            indices_train,
            X_val,
            Y_val,
            indices_val,
            batch_sizes,
            learning_rates,
            patience,
            tanh_gain,
            tanh_weight,
            max_small_input_weight=max_small_input_weight,
            small_input_threshold=small_input_threshold,
        )
        # Compute the validation loss after retraining
        relearned_val_loss = validate_in_batches(
            criterion,
            relearned_model,
            X_val,
            Y_val,
            indices_val,
            tanh_gain=tanh_gain,
            tanh_weight=tanh_weight,
            max_small_input_weight=max_small_input_weight,
            small_input_threshold=small_input_threshold,
        )
        # Compare the original and retrained validation losse
        print("original_val_loss:", original_val_loss)
        print("relearned_val_loss:", relearned_val_loss)
        # Return the model with the better validation loss
        if relearned_val_loss < original_val_loss:
            return relearned_model, True
        else:
            return model, False

    def get_trained_model(
        self,
        learning_rates: List[float] = [1e-3, 1e-4, 1e-5, 1e-6],
        patience: int = 10,
        batch_sizes: List[int] = [1000],
        jerk_limit: float = 1.0,
        steer_rate_limit: float = 1.0,
        augmented_input_size: int = 15,
        max_small_acc_weight: float = 10.0,
        small_acc_threshold: float = 0.01,
        max_small_steer_weight: float = 30.0,
        small_steer_threshold: float = 0.005,
        cmd_mode: str = "both",
    ) -> None:
        """
        Train the acceleration and/or steering models based on the provided mode.
        - learning_rates: List of learning rates to use during training.
        - patience: Number of epochs to wait for improvement before early stopping.
        - batch_sizes: List of batch sizes to use during training.
        - jerk_limit: Input rate limit for acceleration.
        - steer_rate_limit: Input rate limit for steering.
        - augmented_input_size: Size of augmented input features.
        - max_small_acc_weight: Maximum weight for penalties on small acceleration inputs.
        - small_acc_threshold: Threshold to define "small" acceleration inputs.
        - max_small_steer_weight: Maximum weight for penalties on small steering inputs.
        - small_steer_threshold: Threshold to define "small" steering inputs.
        - cmd_mode: Mode to specify which model(s) to train ("both", "acc", or "steer").
        """
        if cmd_mode == "both" or cmd_mode == "acc":
            self.model_acc = InputsSchedulePredictorNN(
                dataset_num=self.dataset_num,
                target_size=parameters.acc_input_schedule_prediction_len,
                input_rate_limit=jerk_limit,
                augmented_input_size=augmented_input_size,
            ).to(self.device)
            self.train_model(
                self.model_acc,
                torch.tensor(
                    np.array(self.X_acc_train_list)[
                        :, output_step - parameters.controller_acc_input_history_len :
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(
                    np.array(self.Y_acc_train_list)[
                        :, : parameters.acc_input_schedule_prediction_len
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(
                    np.array(self.indices_train_list), dtype=torch.long, device=self.device
                ),
                torch.tensor(
                    np.array(self.X_acc_val_list)[
                        :, output_step - parameters.controller_acc_input_history_len :
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(
                    np.array(self.Y_acc_val_list)[
                        :, : parameters.acc_input_schedule_prediction_len
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(np.array(self.indices_val_list), dtype=torch.long, device=self.device),
                batch_sizes,
                learning_rates,
                patience,
                self.tanh_gain_acc,
                self.tanh_weight_acc,
                max_small_input_weight=max_small_acc_weight,
                small_input_threshold=small_acc_threshold,
            )
        if cmd_mode == "both" or cmd_mode == "steer":
            self.model_steer = InputsSchedulePredictorNN(
                dataset_num=self.dataset_num,
                target_size=parameters.steer_input_schedule_prediction_len,
                input_rate_limit=steer_rate_limit,
                augmented_input_size=augmented_input_size,
            ).to(self.device)
            self.train_model(
                self.model_steer,
                torch.tensor(
                    np.array(self.X_steer_train_list)[
                        :, output_step - parameters.controller_steer_input_history_len :
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(
                    np.array(self.Y_steer_train_list)[
                        :, : parameters.steer_input_schedule_prediction_len
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(
                    np.array(self.indices_train_list), dtype=torch.long, device=self.device
                ),
                torch.tensor(
                    np.array(self.X_steer_val_list)[
                        :, output_step - parameters.controller_steer_input_history_len :
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(
                    np.array(self.Y_steer_val_list)[
                        :, : parameters.steer_input_schedule_prediction_len
                    ],
                    dtype=torch.float32,
                    device=self.device,
                ),
                torch.tensor(np.array(self.indices_val_list), dtype=torch.long, device=self.device),
                batch_sizes,
                learning_rates,
                patience,
                self.tanh_gain_steer,
                self.tanh_weight_steer,
                max_small_input_weight=max_small_steer_weight,
                small_input_threshold=small_steer_threshold,
            )

    def relearn_acc(
        self,
        learning_rates: List[float] = [1e-3, 1e-4, 1e-5, 1e-6],
        patience: int = 10,
        batch_sizes: List[int] = [1000],
        max_small_acc_weight: float = 10.0,
        small_acc_threshold: float = 0.01,
        randomize: float = 0.001,
    ) -> bool:
        """
        Retrain the acceleration model by adding random noise to parameters and retraining.
        - learning_rates: List of learning rates to use during retraining.
        - patience: Number of epochs to wait for improvement before early stopping.
        - batch_sizes: List of batch sizes to use during retraining.
        - max_small_acc_weight: Maximum weight for penalties on small acceleration inputs.
        - small_acc_threshold: Threshold to define "small" acceleration inputs.
        - randomize: Amount of random noise added to model parameters.
        """
        if self.model_acc is None:
            print("Acceleration model is not trained yet.")
            return False
        self.model_acc.to(self.device)
        self.model_acc, updated = self.relearn_model(
            self.model_acc,
            torch.tensor(np.array(self.X_acc_train_list), dtype=torch.float32, device=self.device),
            torch.tensor(np.array(self.Y_acc_train_list), dtype=torch.float32, device=self.device),
            torch.tensor(np.array(self.indices_train_list), dtype=torch.long, device=self.device),
            torch.tensor(np.array(self.X_acc_val_list), dtype=torch.float32, device=self.device),
            torch.tensor(np.array(self.Y_acc_val_list), dtype=torch.float32, device=self.device),
            torch.tensor(np.array(self.indices_val_list), dtype=torch.long, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            self.tanh_gain_acc,
            self.tanh_weight_acc,
            max_small_input_weight=max_small_acc_weight,
            small_input_threshold=small_acc_threshold,
            randomize=randomize,
        )
        return updated

    def relearn_steer(
        self,
        learning_rates: List[float] = [1e-3, 1e-4, 1e-5, 1e-6],
        patience: int = 10,
        batch_sizes: List[int] = [1000],
        max_small_steer_weight: float = 30.0,
        small_steer_threshold: float = 0.005,
        randomize: float = 0.001,
    ) -> bool:
        """
        Retrain the steering model by adding random noise to parameters and retraining.
        - learning_rates: List of learning rates to use during retraining.
        - patience: Number of epochs to wait for improvement before early stopping.
        - batch_sizes: List of batch sizes to use during retraining.
        - steer_rate_limit: Input rate limit for steering.
        - max_small_steer_weight: Maximum weight for penalties on small steering inputs.
        - small_steer_threshold: Threshold to define "small" steering inputs.
        - randomize: Amount of random noise added to model parameters.
        """
        if self.model_steer is None:
            print("Steering model is not trained yet.")
            return False
        self.model_steer.to(self.device)
        self.model_steer, updated = self.relearn_model(
            self.model_steer,
            torch.tensor(
                np.array(self.X_steer_train_list), dtype=torch.float32, device=self.device
            ),
            torch.tensor(
                np.array(self.Y_steer_train_list), dtype=torch.float32, device=self.device
            ),
            torch.tensor(np.array(self.indices_train_list), dtype=torch.long, device=self.device),
            torch.tensor(np.array(self.X_steer_val_list), dtype=torch.float32, device=self.device),
            torch.tensor(np.array(self.Y_steer_val_list), dtype=torch.float32, device=self.device),
            torch.tensor(np.array(self.indices_val_list), dtype=torch.long, device=self.device),
            batch_sizes,
            learning_rates,
            patience,
            self.tanh_gain_steer,
            self.tanh_weight_steer,
            max_small_input_weight=max_small_steer_weight,
            small_input_threshold=small_steer_threshold,
            randomize=randomize,
        )
        return updated

    def save_model(
        self, path: str = "inputs_schedule_predictor_model", cmd_mode: str = "both"
    ) -> None:
        """
        Save the trained models to files and export them as CSV for further use.
        - path: Directory path where the models will be saved.
        - cmd_mode: Mode specifying which model(s) to save ("both", "acc", or "steer").
        """
        # Ensure the directory exists
        if not os.path.exists(path):
            os.makedirs(path)
        # Save the acceleration model if specified
        if cmd_mode == "both" or cmd_mode == "acc":
            if self.model_acc is None:
                print("Acceleration model is not trained yet.")
                return
            self.model_acc.to("cpu")
            torch.save(self.model_acc, path + "/acc_schedule_predictor.pth")
            convert_model_to_csv.convert_inputs_schedule_model_to_csv(
                self.model_acc, path + "/acc_schedule_predictor"
            )
        # Save the steering model if specified
        if cmd_mode == "both" or cmd_mode == "steer":
            if self.model_steer is None:
                print("Steering model is not trained yet.")
                return
            self.model_steer.to("cpu")
            torch.save(self.model_steer, path + "/steer_schedule_predictor.pth")
            convert_model_to_csv.convert_inputs_schedule_model_to_csv(
                self.model_steer, path + "/steer_schedule_predictor"
            )
