import numpy as np
import torch
from torch import nn

acc_queue_size = 15
steer_queue_size = 15
prediction_step = 3


class ErrorPredictionNN(nn.Module):
    def __init__(
        self,
        prediction_length,
        num_layers_encoder=1,
        acc_hidden_sizes=(16, 8),
        steer_hidden_sizes=(16, 8),
        lstm_hidden_size=16,
        lstm_acc_hidden_size=1,
        lstm_steer_hidden_size=1,
        complimentary_size=64,
        hidden_size=16,
        randomize=0.01,
        vel_scaling=1.0,
        vel_bias=0.0,
        state_component_predicted=["vel", "yaw", "acc", "steer"],
    ):
        super(ErrorPredictionNN, self).__init__()
        self.states_size = 3  # vel, acc, steer
        self.vel_index = 0
        self.acc_index = 1
        self.steer_index = 2
        self.vel_scaling = vel_scaling
        self.vel_bias = vel_bias
        self.lstm_hidden_size = lstm_hidden_size
        self.lstm_acc_hidden_size = lstm_acc_hidden_size
        self.lstm_steer_hidden_size = lstm_steer_hidden_size
        self.lstm_hidden_total_size = (
            lstm_hidden_size + lstm_acc_hidden_size + lstm_steer_hidden_size
        )
        self.num_layers_encoder = num_layers_encoder
        self.state_component_predicted = state_component_predicted
        self.output_size = len(state_component_predicted)
        acc_input_indices = np.arange(
            self.states_size, self.states_size + acc_queue_size + prediction_step
        )
        steer_input_indices = np.arange(
            self.states_size + acc_queue_size + prediction_step,
            self.states_size + acc_queue_size + steer_queue_size + 2 * prediction_step,
        )
        self.acc_layer_input_indices = np.concatenate(
            ([self.vel_index, self.acc_index], acc_input_indices)
        )
        self.steer_layer_input_indices = np.concatenate(
            ([self.vel_index, self.steer_index], steer_input_indices)
        )
        self.prediction_length = prediction_length
        lb = -randomize
        ub = randomize

        # Encoder
        self.acc_encoder_layer_1 = nn.Sequential(
            nn.Linear(len(self.acc_layer_input_indices), acc_hidden_sizes[0]), nn.ReLU()
        )
        nn.init.uniform_(self.acc_encoder_layer_1[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.acc_encoder_layer_1[0].bias, a=lb, b=ub)
        self.steer_encoder_layer_1 = nn.Sequential(
            nn.Linear(len(self.steer_layer_input_indices), steer_hidden_sizes[0]), nn.ReLU()
        )
        nn.init.uniform_(self.steer_encoder_layer_1[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.steer_encoder_layer_1[0].bias, a=lb, b=ub)
        self.acc_encoder_layer_2 = nn.Sequential(
            nn.Linear(acc_hidden_sizes[0], acc_hidden_sizes[1]), nn.ReLU()
        )
        nn.init.uniform_(self.acc_encoder_layer_2[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.acc_encoder_layer_2[0].bias, a=lb, b=ub)
        self.steer_encoder_layer_2 = nn.Sequential(
            nn.Linear(steer_hidden_sizes[0], steer_hidden_sizes[1]), nn.ReLU()
        )
        nn.init.uniform_(self.steer_encoder_layer_2[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.steer_encoder_layer_2[0].bias, a=lb, b=ub)

        combined_input_size = 1 + acc_hidden_sizes[1] + steer_hidden_sizes[1]
        self.lstm_encoder = nn.LSTM(
            combined_input_size + 2,
            lstm_hidden_size + lstm_acc_hidden_size + lstm_steer_hidden_size,
            num_layers=num_layers_encoder,
            batch_first=True,
        )
        nn.init.uniform_(self.lstm_encoder.weight_hh_l0, a=lb, b=ub)
        nn.init.uniform_(self.lstm_encoder.weight_ih_l0, a=lb, b=ub)
        nn.init.uniform_(self.lstm_encoder.bias_hh_l0, a=lb, b=ub)
        nn.init.uniform_(self.lstm_encoder.bias_ih_l0, a=lb, b=ub)

        # Decoder
        self.acc_layer_1 = nn.Sequential(
            nn.Linear(
                len(self.acc_layer_input_indices) + lstm_acc_hidden_size, acc_hidden_sizes[0]
            ),
            nn.ReLU(),
        )
        nn.init.uniform_(self.acc_layer_1[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.acc_layer_1[0].bias, a=lb, b=ub)
        self.steer_layer_1 = nn.Sequential(
            nn.Linear(
                len(self.steer_layer_input_indices) + lstm_steer_hidden_size, steer_hidden_sizes[0]
            ),
            nn.ReLU(),
        )
        nn.init.uniform_(self.steer_layer_1[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.steer_layer_1[0].bias, a=lb, b=ub)
        self.acc_layer_2 = nn.Sequential(
            nn.Linear(acc_hidden_sizes[0], acc_hidden_sizes[1]), nn.ReLU()
        )
        nn.init.uniform_(self.acc_layer_2[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.acc_layer_2[0].bias, a=lb, b=ub)
        self.steer_layer_2 = nn.Sequential(
            nn.Linear(steer_hidden_sizes[0], steer_hidden_sizes[1]), nn.ReLU()
        )
        nn.init.uniform_(self.steer_layer_2[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.steer_layer_2[0].bias, a=lb, b=ub)

        self.lstm = nn.LSTM(combined_input_size, lstm_hidden_size, batch_first=True)
        nn.init.uniform_(self.lstm.weight_hh_l0, a=lb, b=ub)
        nn.init.uniform_(self.lstm.weight_ih_l0, a=lb, b=ub)
        nn.init.uniform_(self.lstm.bias_hh_l0, a=lb, b=ub)
        nn.init.uniform_(self.lstm.bias_ih_l0, a=lb, b=ub)
        self.complimentary_layer = nn.Sequential(
            nn.Linear(combined_input_size, complimentary_size), nn.ReLU()
        )
        nn.init.uniform_(self.complimentary_layer[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.complimentary_layer[0].bias, a=lb, b=ub)
        self.linear_relu = nn.Sequential(
            nn.Linear(lstm_hidden_size + complimentary_size, hidden_size),
            nn.ReLU(),
        )
        nn.init.uniform_(self.linear_relu[0].weight, a=lb, b=ub)
        nn.init.uniform_(self.linear_relu[0].bias, a=lb, b=ub)
        self.final_layer = nn.Linear(hidden_size, self.output_size)
        nn.init.uniform_(self.final_layer.weight, a=lb, b=ub)
        nn.init.uniform_(self.final_layer.bias, a=lb, b=ub)

    def forward(self, x, previous_error=None, hc=None, mode="default"):
        x_vel_scaled = x.clone()
        x_vel_scaled[:, :, self.vel_index] = (
            x_vel_scaled[:, :, self.vel_index] - self.vel_bias
        ) * self.vel_scaling
        acc_input = x_vel_scaled[:, :, self.acc_layer_input_indices]
        steer_input = x_vel_scaled[:, :, self.steer_layer_input_indices]
        if (
            mode == "default" or mode == "get_lstm_states"
        ):  # update lstm states using encoder and predict using decoder
            acc_encoder = self.acc_encoder_layer_1(acc_input[:, : -self.prediction_length])
            steer_encoder = self.steer_encoder_layer_1(steer_input[:, : -self.prediction_length])
            acc_encoder = self.acc_encoder_layer_2(acc_encoder)
            steer_encoder = self.steer_encoder_layer_2(steer_encoder)
            lstm_encoder_input = torch.cat(
                (
                    x_vel_scaled[:, : -self.prediction_length, : self.states_size - 2],
                    acc_encoder,
                    steer_encoder,
                ),
                dim=2,
            )
            _, (h, c) = self.lstm_encoder(
                torch.cat((lstm_encoder_input, previous_error), dim=2), hc
            )
            hc = (
                h[-1][:, : self.lstm_hidden_size].unsqueeze(0).contiguous(),
                c[-1][:, : self.lstm_hidden_size].unsqueeze(0).contiguous(),
            )

            acc_decoder = self.acc_layer_1(
                torch.cat(
                    (
                        acc_input[:, -self.prediction_length :],
                        h[-1][
                            :,
                            self.lstm_hidden_size : self.lstm_hidden_size
                            + self.lstm_acc_hidden_size,
                        ]
                        .unsqueeze(1)
                        .expand(-1, self.prediction_length, -1),
                    ),
                    dim=2,
                )
            )
            steer_decoder = self.steer_layer_1(
                torch.cat(
                    (
                        steer_input[:, -self.prediction_length :],
                        h[-1][:, self.lstm_hidden_size + self.lstm_acc_hidden_size :]
                        .unsqueeze(1)
                        .expand(-1, self.prediction_length, -1),
                    ),
                    dim=2,
                )
            )
            acc_decoder = self.acc_layer_2(acc_decoder)
            steer_decoder = self.steer_layer_2(steer_decoder)
            lstm_input = torch.cat(
                (
                    x_vel_scaled[:, -self.prediction_length :, : self.states_size - 2],
                    acc_decoder,
                    steer_decoder,
                ),
                dim=2,
            )
            lstm_head, _ = self.lstm(lstm_input, hc)
            complimentary_output = self.complimentary_layer(lstm_input)
            lstm_output = torch.cat((lstm_head, complimentary_output), dim=2)
            output = self.linear_relu(lstm_output)
            output = self.final_layer(output)
            if mode == "default":
                return output
            elif mode == "get_lstm_states":
                return output, (h, c)
        elif mode == "only_encoder":  # update lstm states using encoder
            acc_encoder = self.acc_encoder_layer_1(acc_input)
            steer_encoder = self.steer_encoder_layer_1(steer_input)
            acc_encoder = self.acc_encoder_layer_2(acc_encoder)
            steer_encoder = self.steer_encoder_layer_2(steer_encoder)
            lstm_encoder_input = torch.cat(
                (x_vel_scaled[:, :, : self.states_size - 2], acc_encoder, steer_encoder), dim=2
            )
            _, (h, c) = self.lstm_encoder(
                torch.cat((lstm_encoder_input, previous_error), dim=2), hc
            )
            return (h, c)
        elif mode == "predict_with_hc":  # Prediction using only decoder
            decoder_length = x.shape[1]
            acc_decoder = self.acc_layer_1(
                torch.cat(
                    (
                        acc_input,
                        hc[0][0][
                            :,
                            self.lstm_hidden_size : self.lstm_hidden_size
                            + self.lstm_acc_hidden_size,
                        ]
                        .unsqueeze(1)
                        .expand(-1, decoder_length, -1),
                    ),
                    dim=2,
                )
            )
            steer_decoder = self.steer_layer_1(
                torch.cat(
                    (
                        steer_input,
                        hc[0][0][:, self.lstm_hidden_size + self.lstm_acc_hidden_size :]
                        .unsqueeze(1)
                        .expand(-1, decoder_length, -1),
                    ),
                    dim=2,
                )
            )
            acc_decoder = self.acc_layer_2(acc_decoder)
            steer_decoder = self.steer_layer_2(steer_decoder)
            lstm_input = torch.cat(
                (x_vel_scaled[:, :, : self.states_size - 2], acc_decoder, steer_decoder), dim=2
            )
            hc_lstm_input = (
                hc[0][:, :, : self.lstm_hidden_size].contiguous(),
                hc[1][:, :, : self.lstm_hidden_size].contiguous(),
            )
            lstm, hc_new = self.lstm(lstm_input, hc_lstm_input)
            h_new = torch.cat((hc_new[0], hc[0][:, :, self.lstm_hidden_size :]), dim=2)
            c_new = torch.cat((hc_new[1], hc[1][:, :, self.lstm_hidden_size :]), dim=2)
            complimentary_output = self.complimentary_layer(lstm_input[:, :])
            lstm_output = torch.cat((lstm, complimentary_output), dim=2)
            output = self.linear_relu(lstm_output)
            output = self.final_layer(output)
            return output, (h_new, c_new)
