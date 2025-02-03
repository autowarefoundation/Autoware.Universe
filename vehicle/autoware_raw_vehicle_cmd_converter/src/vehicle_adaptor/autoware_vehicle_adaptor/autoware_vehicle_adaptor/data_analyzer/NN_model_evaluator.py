from autoware_vehicle_adaptor.param import parameters
from autoware_vehicle_adaptor.training import add_data_from_csv
from autoware_vehicle_adaptor.training import train_error_prediction_NN_with_offline_data
from autoware_vehicle_adaptor.training import training_utils
import matplotlib.pyplot as plt
import numpy as np
import simplejson as json
import torch

past_length = parameters.past_length


class NNModelEvaluator(
    train_error_prediction_NN_with_offline_data.train_error_prediction_NN_with_offline_data
):
    def __init__(self):
        super().__init__()

    def reset_model(self):
        self.model = None
        self.model_for_initial_hidden = None
        self.models = None

    def set_model(self, model):
        self.model = model

    def set_model_for_initial_hidden(self, model_for_initial_hidden):
        self.model_for_initial_hidden = model_for_initial_hidden

    def set_models(self, models):
        self.models = models

    def evaluate(self, window_size=10, json_save_dir=None, save_plt_dir=None, show_plt=False):
        if self.X_test_list is None:
            print("No test data")
            return
        if self.model is None and self.models is None:
            print("No model")
            return
        if self.model_for_initial_hidden is None:
            X_test_np, Y_test_np = training_utils.TrainErrorPredictionNNFunctions.get_sequence_data(
                self.X_test_list, self.Y_test_list, self.division_indices_test
            )
            X_test = torch.tensor(X_test_np, dtype=torch.float32)
            Y_test = torch.tensor(Y_test_np, dtype=torch.float32)
            if self.models is None:
                Y_pred_np = training_utils.TrainErrorPredictionNNFunctions.get_Y_pred_np(
                    self.model, X_test, Y_test
                )
                model_prediction_error = training_utils.TrainErrorPredictionNNFunctions.get_model_signed_prediction_error(
                    self.model, X_test, Y_test, window_size
                )
            else:
                Y_pred_np_0 = training_utils.TrainErrorPredictionNNFunctions.get_Y_pred_np(
                    self.models[0], X_test, Y_test
                )
                Y_pred_np = Y_pred_np_0 / len(self.models)
                for i in range(1, len(self.models)):
                    Y_pred_np_i = training_utils.TrainErrorPredictionNNFunctions.get_Y_pred_np(
                        self.models[i], X_test, Y_test
                    )
                    Y_pred_np += Y_pred_np_i / len(self.models)
                model_prediction_error = training_utils.TrainErrorPredictionNNFunctions.get_models_signed_prediction_error(
                    self.models, X_test, Y_test, window_size
                )
        else:
            X_test_np, Y_test_np, indices_test = (
                training_utils.TrainErrorPredictionNNWithOfflineData.get_sequence_data(
                    self.X_test_list, self.Y_test_list, self.division_indices_test
                )
            )
            X_test = torch.tensor(X_test_np, dtype=torch.float32)
            Y_test = torch.tensor(Y_test_np, dtype=torch.float32)
            if self.models is None:
                Y_pred_np = training_utils.TrainErrorPredictionNNWithOfflineData.get_Y_pred_np(
                    self.model,
                    self.model_for_initial_hidden,
                    X_test,
                    Y_test,
                    self.offline_data_dict_test,
                    indices_test,
                )
                model_prediction_error = training_utils.TrainErrorPredictionNNWithOfflineData.get_model_signed_prediction_error(
                    self.model,
                    self.model_for_initial_hidden,
                    X_test,
                    Y_test,
                    self.offline_data_dict_test,
                    indices_test,
                    window_size,
                )
            else:
                Y_pred_np_0 = training_utils.TrainErrorPredictionNNWithOfflineData.get_Y_pred_np(
                    self.models[0],
                    self.model_for_initial_hidden,
                    X_test,
                    Y_test,
                    self.offline_data_dict_test,
                    indices_test,
                )
                Y_pred_np = Y_pred_np_0 / len(self.models)
                for i in range(1, len(self.models)):
                    Y_pred_np_i = (
                        training_utils.TrainErrorPredictionNNWithOfflineData.get_Y_pred_np(
                            self.models[i],
                            self.model_for_initial_hidden,
                            X_test,
                            Y_test,
                            self.offline_data_dict_test,
                            indices_test,
                        )
                    )
                    Y_pred_np += Y_pred_np_i / len(self.models)
                model_prediction_error = training_utils.TrainErrorPredictionNNWithOfflineData.get_models_signed_prediction_error(
                    self.models,
                    self.model_for_initial_hidden,
                    X_test,
                    Y_test,
                    self.offline_data_dict_test,
                    indices_test,
                    window_size,
                )
        if self.models is None:
            state_component_predicted = self.model.state_component_predicted
        else:
            state_component_predicted = self.models[0].state_component_predicted
        nominal_prediction_error = training_utils.get_nominal_signed_prediction_error(
            X_test, window_size
        )
        # model_prediction_error = model_prediction_error.detach().to("cpu").numpy()
        # nominal_prediction_error = nominal_prediction_error.detach().to("cpu").numpy()
        nominal_mae = np.abs(Y_test_np[:, past_length:]).mean(axis=(0, 1))
        model_mae = np.abs(Y_test_np[:, past_length:] - Y_pred_np).mean(axis=(0, 1))
        mae_ratio = model_mae / nominal_mae
        nominal_mse = np.square(Y_test_np[:, past_length:]).mean(axis=(0, 1))
        model_mse = np.square(Y_test_np[:, past_length:] - Y_pred_np).mean(axis=(0, 1))
        mse_ratio = model_mse / nominal_mse
        nominal_prediction_mae = np.abs(nominal_prediction_error).mean(axis=0)
        model_prediction_mae = np.abs(model_prediction_error).mean(axis=0)
        prediction_mae_ratio = model_prediction_mae / nominal_prediction_mae
        nominal_prediction_mse = np.square(nominal_prediction_error).mean(axis=0)
        model_prediction_mse = np.square(model_prediction_error).mean(axis=0)
        prediction_mse_ratio = model_prediction_mse / nominal_prediction_mse

        results_json = {}
        for i in range(len(state_component_predicted)):
            results_json[state_component_predicted[i] + "_nominal_mae"] = nominal_mae[i]
            results_json[state_component_predicted[i] + "_model_mae"] = model_mae[i]
            results_json[state_component_predicted[i] + "_mae_ratio"] = mae_ratio[i]
            results_json[state_component_predicted[i] + "_nominal_mse"] = nominal_mse[i]
            results_json[state_component_predicted[i] + "_model_mse"] = model_mse[i]
            results_json[state_component_predicted[i] + "_mse_ratio"] = mse_ratio[i]

        results_json["acc_nominal_" + str(window_size) + "_step_prediction_mae"] = (
            nominal_prediction_mae[0]
        )
        results_json["acc_model_" + str(window_size) + "_step_prediction_mae"] = (
            model_prediction_mae[0]
        )
        results_json["acc_" + str(window_size) + "_step_prediction_mae_ratio"] = (
            prediction_mae_ratio[0]
        )
        results_json["acc_nominal_" + str(window_size) + "_step_prediction_mse"] = (
            nominal_prediction_mse[0]
        )
        results_json["acc_model_" + str(window_size) + "_step_prediction_mse"] = (
            model_prediction_mse[0]
        )
        results_json["acc_" + str(window_size) + "_step_prediction_mse_ratio"] = (
            prediction_mse_ratio[0]
        )

        results_json["steer_nominal_" + str(window_size) + "_step_prediction_mae"] = (
            nominal_prediction_mae[1]
        )
        results_json["steer_model_" + str(window_size) + "_step_prediction_mae"] = (
            model_prediction_mae[1]
        )
        results_json["steer_" + str(window_size) + "_step_prediction_mae_ratio"] = (
            prediction_mae_ratio[1]
        )
        results_json["steer_nominal_" + str(window_size) + "_step_prediction_mse"] = (
            nominal_prediction_mse[1]
        )
        results_json["steer_model_" + str(window_size) + "_step_prediction_mse"] = (
            model_prediction_mse[1]
        )
        results_json["steer_" + str(window_size) + "_step_prediction_mse_ratio"] = (
            prediction_mse_ratio[1]
        )

        print(results_json)

        if json_save_dir is not None:
            with open(json_save_dir + "/results.json", "w") as f:
                json.dump(results_json, f, indent=4)
        if save_plt_dir is not None or show_plt:
            fig, axes = plt.subplots(
                nrows=2, ncols=len(state_component_predicted), figsize=(24, 15), tight_layout=True
            )
            fig.suptitle("Prediction Error")
            for i in range(len(state_component_predicted)):
                axes[0, i].plot(Y_test_np[:, past_length + window_size, i], label="Nominal")
                axes[0, i].plot(
                    Y_test_np[:, past_length + window_size, i] - Y_pred_np[:, window_size, i],
                    label="Model",
                )
                axes[0, i].set_title(state_component_predicted[i])
                axes[0, i].legend()
            axes[1, len(state_component_predicted) - 2].plot(
                nominal_prediction_error[:, 0], label="Nominal"
            )
            axes[1, len(state_component_predicted) - 2].plot(
                model_prediction_error[:, 0], label="Model"
            )
            axes[1, len(state_component_predicted) - 2].set_title(
                str(window_size) + "Step Acc Prediction Error"
            )
            axes[1, len(state_component_predicted) - 2].legend()
            axes[1, len(state_component_predicted) - 1].plot(
                nominal_prediction_error[:, 1], label="Nominal"
            )
            axes[1, len(state_component_predicted) - 1].plot(
                model_prediction_error[:, 1], label="Model"
            )
            axes[1, len(state_component_predicted) - 1].set_title(
                str(window_size) + "Step Steer Prediction Error"
            )
            axes[1, len(state_component_predicted) - 1].legend()
            if save_plt_dir is not None:
                plt.savefig(save_plt_dir + "/prediction_error.png")
            if show_plt:
                plt.show()
            plt.close()
