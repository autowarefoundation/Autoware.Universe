import csv
import os

import numpy as np


def convert_model_to_csv(model, save_dir):
    model.to("cpu")
    if not os.path.exists(save_dir):
        os.makedirs(save_dir)
    np.savetxt(
        save_dir + "/weight_acc_encoder_layer_1.csv",
        model.acc_encoder_layer_1[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_steer_encoder_layer_1.csv",
        model.steer_encoder_layer_1[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_acc_encoder_layer_2.csv",
        model.acc_encoder_layer_2[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_steer_encoder_layer_2.csv",
        model.steer_encoder_layer_2[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )

    np.savetxt(
        save_dir + "/weight_acc_layer_1.csv",
        model.acc_layer_1[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_steer_layer_1.csv",
        model.steer_layer_1[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_acc_layer_2.csv",
        model.acc_layer_2[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_steer_layer_2.csv",
        model.steer_layer_2[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )

    np.savetxt(
        save_dir + "/weight_lstm_ih.csv",
        model.lstm.weight_ih_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_hh.csv",
        model.lstm.weight_hh_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_complimentary_layer.csv",
        model.complimentary_layer[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_linear_relu.csv",
        model.linear_relu[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_final_layer.csv",
        model.final_layer.weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )

    np.savetxt(
        save_dir + "/bias_acc_encoder_layer_1.csv",
        model.acc_encoder_layer_1[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_steer_encoder_layer_1.csv",
        model.steer_encoder_layer_1[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_acc_encoder_layer_2.csv",
        model.acc_encoder_layer_2[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_steer_encoder_layer_2.csv",
        model.steer_encoder_layer_2[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )

    np.savetxt(
        save_dir + "/bias_acc_layer_1.csv",
        model.acc_layer_1[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_steer_layer_1.csv",
        model.steer_layer_1[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_acc_layer_2.csv",
        model.acc_layer_2[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_steer_layer_2.csv",
        model.steer_layer_2[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_ih.csv",
        model.lstm.bias_ih_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_hh.csv",
        model.lstm.bias_hh_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_complimentary_layer.csv",
        model.complimentary_layer[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_linear_relu.csv",
        model.linear_relu[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_final_layer.csv",
        model.final_layer.bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/model_info.csv", np.array([model.num_layers_encoder]), fmt="%d", delimiter=","
    )

    for i in range(model.num_layers_encoder):
        np.savetxt(
            save_dir + "/weight_lstm_encoder_ih_" + str(i) + ".csv",
            model.lstm_encoder.__getattr__("weight_ih_l" + str(i))
            .detach()
            .numpy()
            .astype(np.float64),
            delimiter=",",
        )
        # .weight_ih_l0.detach().numpy().astype(np.float64),delimiter=',')
        np.savetxt(
            save_dir + "/weight_lstm_encoder_hh_" + str(i) + ".csv",
            model.lstm_encoder.__getattr__("weight_hh_l" + str(i))
            .detach()
            .numpy()
            .astype(np.float64),
            delimiter=",",
        )
        # .weight_hh_l0.detach().numpy().astype(np.float64),delimiter=',')
        np.savetxt(
            save_dir + "/bias_lstm_encoder_ih_" + str(i) + ".csv",
            model.lstm_encoder.__getattr__("bias_ih_l" + str(i))
            .detach()
            .numpy()
            .astype(np.float64),
            delimiter=",",
        )
        # .bias_ih_l0.detach().numpy().astype(np.float64),delimiter=',')
        np.savetxt(
            save_dir + "/bias_lstm_encoder_hh_" + str(i) + ".csv",
            model.lstm_encoder.__getattr__("bias_hh_l" + str(i))
            .detach()
            .numpy()
            .astype(np.float64),
            delimiter=",",
        )
        # .bias_hh_l0.detach().numpy().astype(np.float64),delimiter=',')

    vel_scale = np.zeros(2)
    vel_scale[0] = model.vel_scaling
    vel_scale[1] = model.vel_bias
    np.savetxt(save_dir + "/vel_scale.csv", vel_scale, delimiter=",")
    with open(save_dir + "/state_component_predicted.csv", mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(model.state_component_predicted)


def convert_inputs_schedule_model_to_csv(model, save_dir):
    if not os.path.exists(save_dir):
        os.makedirs(save_dir)
    # np.savetxt(save_dir + "/weight_pre_encoder_0.csv", model.pre_encoder[0].weight.detach().numpy().astype(np.float64),delimiter=',')
    # np.savetxt(save_dir + "/weight_pre_encoder_1.csv", model.pre_encoder[1].weight.detach().numpy().astype(np.float64),delimiter=',')
    np.savetxt(
        save_dir + "/weight_lstm_encoder_ih_0.csv",
        model.lstm_encoder.weight_ih_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_encoder_hh_0.csv",
        model.lstm_encoder.weight_hh_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_decoder_ih_0.csv",
        model.lstm_decoder.weight_ih_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_decoder_hh_0.csv",
        model.lstm_decoder.weight_hh_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_encoder_ih_1.csv",
        model.lstm_encoder.weight_ih_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_encoder_hh_1.csv",
        model.lstm_encoder.weight_hh_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_decoder_ih_1.csv",
        model.lstm_decoder.weight_ih_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_lstm_decoder_hh_1.csv",
        model.lstm_decoder.weight_hh_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_post_decoder_0.csv",
        model.post_decoder[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_post_decoder_1.csv",
        model.finalize[0].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/weight_final_layer.csv",
        model.finalize[2].weight.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    # np.savetxt(save_dir + "/bias_pre_encoder_0.csv", model.pre_encoder[0].bias.detach().numpy().astype(np.float64),delimiter=',')
    # np.savetxt(save_dir + "/bias_pre_encoder_1.csv", model.pre_encoder[1].bias.detach().numpy().astype(np.float64),delimiter=',')
    np.savetxt(
        save_dir + "/bias_lstm_encoder_ih_0.csv",
        model.lstm_encoder.bias_ih_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_encoder_hh_0.csv",
        model.lstm_encoder.bias_hh_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_decoder_ih_0.csv",
        model.lstm_decoder.bias_ih_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_decoder_hh_0.csv",
        model.lstm_decoder.bias_hh_l0.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_encoder_ih_1.csv",
        model.lstm_encoder.bias_ih_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_encoder_hh_1.csv",
        model.lstm_encoder.bias_hh_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_decoder_ih_1.csv",
        model.lstm_decoder.bias_ih_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_lstm_decoder_hh_1.csv",
        model.lstm_decoder.bias_hh_l1.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_post_decoder_0.csv",
        model.post_decoder[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_post_decoder_1.csv",
        model.finalize[0].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/bias_final_layer.csv",
        model.finalize[2].bias.detach().numpy().astype(np.float64),
        delimiter=",",
    )
    np.savetxt(
        save_dir + "/adaptive_scale.csv",
        model.post_decoder_adaptive_scales[0].detach().numpy().astype(np.float64),
        delimiter=",",
    )
    for i in range(len(model.post_decoder_adaptive_scales)):
        if not os.path.exists(save_dir + "/adaptive_scale_" + str(i)):
            os.makedirs(save_dir + "/adaptive_scale_" + str(i))
        np.savetxt(
            save_dir + "/adaptive_scale_" + str(i) + "/adaptive_scale.csv",
            model.post_decoder_adaptive_scales[i].detach().numpy().astype(np.float64),
            delimiter=",",
        )
    vel_scale = np.zeros(2)
    vel_scale[0] = model.vel_scaling
    vel_scale[1] = model.vel_bias
    np.savetxt(save_dir + "/vel_params.csv", vel_scale, delimiter=",")
    limit = np.zeros(1)
    limit[0] = model.input_rate_limit
    np.savetxt(save_dir + "/limit.csv", limit, delimiter=",")
