#include "vehicle_adaptor_compensator.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <yaml-cpp/yaml.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace py = pybind11;
using namespace Proxima;
using namespace vehicle_adaptor_constants;

///////////////// TrainedDynamics ///////////////////////
TrainedDynamics::TrainedDynamics()
{
  YAML::Node optimization_param_node =
    YAML::LoadFile(get_param_dir_path() + "/optimization_param.yaml");
  min_gradient_acc_ =
    optimization_param_node["optimization_parameter"]["min_gradient"]["min_gradient_acc"]
      .as<double>();
  min_gradient_steer_ =
    optimization_param_node["optimization_parameter"]["min_gradient"]["min_gradient_steer"]
      .as<double>();
}
TrainedDynamics::~TrainedDynamics()
{
}
void TrainedDynamics::set_vehicle_params(
  double wheel_base, double acc_time_delay, double steer_time_delay, double acc_time_constant,
  double steer_time_constant, int acc_queue_size, int steer_queue_size, double control_dt,
  int predict_step)
{
  wheel_base_ = wheel_base;
  acc_time_delay_ = acc_time_delay;
  steer_time_delay_ = steer_time_delay;
  acc_time_constant_ = acc_time_constant;
  steer_time_constant_ = steer_time_constant;
  acc_queue_size_ = acc_queue_size;
  steer_queue_size_ = steer_queue_size;

  predict_step_ = predict_step;
  acc_input_end_index_ = state_size_ + acc_queue_size_ - 1;
  steer_input_start_index_ = state_size_ + acc_queue_size_;
  steer_input_end_index_ = state_size_ + acc_queue_size_ + steer_queue_size_ - 1;
  control_dt_ = control_dt;
  predict_dt_ = control_dt_ * predict_step_;

  acc_delay_step_ = std::min(int(std::round(acc_time_delay_ / control_dt_)), acc_queue_size_);
  steer_delay_step_ = std::min(int(std::round(steer_time_delay_ / control_dt_)), steer_queue_size_);
  nominal_dynamics_.set_params(
    wheel_base, acc_time_delay, steer_time_delay, acc_time_constant, steer_time_constant,
    acc_queue_size, steer_queue_size, control_dt, predict_step);
}
void TrainedDynamics::set_NN_params(
  const Eigen::MatrixXd & weight_acc_encoder_layer_1,
  const Eigen::MatrixXd & weight_steer_encoder_layer_1,
  const Eigen::MatrixXd & weight_acc_encoder_layer_2,
  const Eigen::MatrixXd & weight_steer_encoder_layer_2, const Eigen::MatrixXd & weight_acc_layer_1,
  const Eigen::MatrixXd & weight_steer_layer_1, const Eigen::MatrixXd & weight_acc_layer_2,
  const Eigen::MatrixXd & weight_steer_layer_2,
  const std::vector<Eigen::MatrixXd> & weight_lstm_encoder_ih,
  const std::vector<Eigen::MatrixXd> & weight_lstm_encoder_hh,
  const Eigen::MatrixXd & weight_lstm_ih, const Eigen::MatrixXd & weight_lstm_hh,
  const Eigen::MatrixXd & weight_complimentary_layer, const Eigen::MatrixXd & weight_linear_relu,
  const Eigen::MatrixXd & weight_final_layer, const Eigen::VectorXd & bias_acc_encoder_layer_1,
  const Eigen::VectorXd & bias_steer_encoder_layer_1,
  const Eigen::VectorXd & bias_acc_encoder_layer_2,
  const Eigen::VectorXd & bias_steer_encoder_layer_2, const Eigen::VectorXd & bias_acc_layer_1,
  const Eigen::VectorXd & bias_steer_layer_1, const Eigen::VectorXd & bias_acc_layer_2,
  const Eigen::VectorXd & bias_steer_layer_2,
  const std::vector<Eigen::VectorXd> & bias_lstm_encoder_ih,
  const std::vector<Eigen::VectorXd> & bias_lstm_encoder_hh, const Eigen::VectorXd & bias_lstm_ih,
  const Eigen::VectorXd & bias_lstm_hh, const Eigen::VectorXd & bias_complimentary_layer,
  const Eigen::VectorXd & bias_linear_relu, const Eigen::VectorXd & bias_final_layer,
  const double vel_scaling, const double vel_bias,
  const std::vector<std::string> state_component_predicted)
{
  transform_model_to_eigen_.set_params(
    weight_acc_encoder_layer_1, weight_steer_encoder_layer_1, weight_acc_encoder_layer_2,
    weight_steer_encoder_layer_2, weight_acc_layer_1, weight_steer_layer_1, weight_acc_layer_2,
    weight_steer_layer_2, weight_lstm_encoder_ih, weight_lstm_encoder_hh, weight_lstm_ih,
    weight_lstm_hh, weight_complimentary_layer, weight_linear_relu, weight_final_layer,
    bias_acc_encoder_layer_1, bias_steer_encoder_layer_1, bias_acc_encoder_layer_2,
    bias_steer_encoder_layer_2, bias_acc_layer_1, bias_steer_layer_1, bias_acc_layer_2,
    bias_steer_layer_2, bias_lstm_encoder_ih, bias_lstm_encoder_hh, bias_lstm_ih, bias_lstm_hh,
    bias_complimentary_layer, bias_linear_relu, bias_final_layer, vel_scaling, vel_bias);
  h_dim_full_ = weight_lstm_encoder_hh[0].cols();
  h_dim_ = weight_lstm_hh.cols();
  state_component_predicted_ = state_component_predicted;
  state_component_predicted_index_ = std::vector<int>(state_component_predicted_.size());
  for (int i = 0; i < int(state_component_predicted_.size()); i++) {
    for (int j = 0; j < int(all_state_name_.size()); j++) {
      if (state_component_predicted_[i] == all_state_name_[j]) {
        state_component_predicted_index_[i] = j;
      }
    }
  }
}
void TrainedDynamics::clear_NN_params()
{
  transform_model_to_eigen_.clear_params();
}
void TrainedDynamics::set_sg_filter_params(int degree, int window_size)
{
  filter_diff_NN_.set_sg_filter_params(
    degree, window_size, state_size_, h_dim_, acc_queue_size_, steer_queue_size_, predict_step_,
    control_dt_);
}
Eigen::VectorXd TrainedDynamics::nominal_prediction(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
  const Eigen::VectorXd & steer_input_history_concat)
{
  return nominal_dynamics_.F_with_input_history(
    states, acc_input_history_concat, steer_input_history_concat);
}
void TrainedDynamics::update_lstm_states(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
  const Eigen::VectorXd & steer_input_history_concat, std::vector<Eigen::VectorXd> & h_lstm,
  std::vector<Eigen::VectorXd> & c_lstm, const Eigen::Vector2d & acc_steer_error)
{
  Eigen::VectorXd NN_input =
    Eigen::VectorXd::Zero(3 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_);
  NN_input << states[vel_index_], states[acc_index_], states[steer_index_],
    acc_input_history_concat, steer_input_history_concat;
  std::vector<Eigen::VectorXd> h_lstm_next, c_lstm_next;
  transform_model_to_eigen_.update_lstm(
    NN_input, h_lstm, c_lstm, h_lstm_next, c_lstm_next, acc_steer_error);
  h_lstm = h_lstm_next;
  c_lstm = c_lstm_next;
}
Eigen::VectorXd TrainedDynamics::F_with_model_for_calc_controller_prediction_error(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
  const Eigen::VectorXd & steer_input_history_concat, Eigen::VectorXd & h_lstm,
  Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error, const int horizon)
{
  Eigen::VectorXd states_next = nominal_dynamics_.F_with_input_history(
    states, acc_input_history_concat, steer_input_history_concat);

  Eigen::VectorXd NN_input =
    Eigen::VectorXd::Zero(3 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_);
  NN_input << states[vel_index_], states[acc_index_], states[steer_index_],
    acc_input_history_concat, steer_input_history_concat;

  Eigen::VectorXd h_lstm_next, c_lstm_next, NN_output;
  transform_model_to_eigen_.error_prediction(
    NN_input, h_lstm, c_lstm, h_lstm_next, c_lstm_next, NN_output);
  if (horizon == 0) {
    previous_error = error_decay_rate_ * previous_error + (1 - error_decay_rate_) * NN_output;
  } else {
    previous_error = double_power(error_decay_rate_, predict_step_) * previous_error +
                     (1.0 - double_power(error_decay_rate_, predict_step_)) * NN_output;
  }
  double yaw = states[yaw_index_];
  for (int i = 0; i < int(state_component_predicted_index_.size()); i++) {
    if (state_component_predicted_index_[i] == x_index_) {
      states_next[x_index_] += previous_error[i] * predict_dt_ * std::cos(yaw) -
                               previous_error[i + 1] * predict_dt_ * std::sin(yaw);
    } else if (state_component_predicted_index_[i] == y_index_) {
      states_next[y_index_] += previous_error[i - 1] * predict_dt_ * std::sin(yaw) +
                               previous_error[i] * predict_dt_ * std::cos(yaw);
    } else {
      states_next[state_component_predicted_index_[i]] += previous_error[i] * predict_dt_;
    }
  }
  h_lstm = h_lstm_next;
  c_lstm = c_lstm_next;

  return states_next;
}
Eigen::VectorXd TrainedDynamics::F_with_model(
  const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
  Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs,
  Eigen::VectorXd & h_lstm, Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error,
  const int horizon)
{
  Eigen::VectorXd states_next = nominal_dynamics_.F_with_input_history(
    states, acc_input_history, steer_input_history, acc_input_history_concat,
    steer_input_history_concat, d_inputs);

  Eigen::VectorXd NN_input =
    Eigen::VectorXd::Zero(3 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_);
  NN_input << states[vel_index_], states[acc_index_], states[steer_index_],
    acc_input_history_concat, steer_input_history_concat;

  Eigen::VectorXd h_lstm_next, c_lstm_next, NN_output;
  transform_model_to_eigen_.error_prediction(
    NN_input, h_lstm, c_lstm, h_lstm_next, c_lstm_next, NN_output);
  if (horizon == 0) {
    previous_error = error_decay_rate_ * previous_error + (1 - error_decay_rate_) * NN_output;
  } else {
    previous_error = double_power(error_decay_rate_, predict_step_) * previous_error +
                     (1.0 - double_power(error_decay_rate_, predict_step_)) * NN_output;
  }
  double yaw = states[yaw_index_];
  for (int i = 0; i < int(state_component_predicted_index_.size()); i++) {
    if (state_component_predicted_index_[i] == x_index_) {
      states_next[x_index_] += previous_error[i] * predict_dt_ * std::cos(yaw) -
                               previous_error[i + 1] * predict_dt_ * std::sin(yaw);
    } else if (state_component_predicted_index_[i] == y_index_) {
      states_next[y_index_] += previous_error[i - 1] * predict_dt_ * std::sin(yaw) +
                               previous_error[i] * predict_dt_ * std::cos(yaw);
    } else {
      states_next[state_component_predicted_index_[i]] += previous_error[i] * predict_dt_;
    }
  }
  h_lstm = h_lstm_next;
  c_lstm = c_lstm_next;
  return states_next;
}
Eigen::VectorXd TrainedDynamics::F_with_model(
  const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs, Eigen::VectorXd & h_lstm,
  Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error, const int horizon)
{
  Eigen::VectorXd acc_input_history_concat, steer_input_history_concat;
  Eigen::VectorXd states_next = F_with_model(
    states, acc_input_history, steer_input_history, acc_input_history_concat,
    steer_input_history_concat, d_inputs, h_lstm, c_lstm, previous_error, horizon);
  return states_next;
}
Eigen::VectorXd TrainedDynamics::F_with_model_diff(
  const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
  Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs,
  Eigen::VectorXd & h_lstm, Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error,
  const int horizon, Eigen::MatrixXd & A, Eigen::MatrixXd & B, Eigen::MatrixXd & C)
{
  Eigen::VectorXd states_next = nominal_dynamics_.F_with_input_history_and_diff(
    states, acc_input_history, steer_input_history, acc_input_history_concat,
    steer_input_history_concat, d_inputs, A, B);
  Eigen::VectorXd NN_input =
    Eigen::VectorXd::Zero(3 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_);
  NN_input << states[vel_index_], states[acc_index_], states[steer_index_],
    acc_input_history_concat, steer_input_history_concat;
  Eigen::VectorXd h_lstm_next, c_lstm_next, NN_output;
  Eigen::MatrixXd dF_d_states_with_history, dF_dhc, dhc_dhc, dhc_d_states_with_history;
  transform_model_to_eigen_.error_prediction_with_diff(
    NN_input, h_lstm, c_lstm, h_lstm_next, c_lstm_next, NN_output, dF_d_states_with_history, dF_dhc,
    dhc_dhc, dhc_d_states_with_history);
  double yaw = states[yaw_index_];
  C = Eigen::MatrixXd::Zero(
    2 * h_dim_ + states.size(),
    2 * h_dim_ + states.size() + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_);
  for (int i = 0; i < int(state_component_predicted_index_.size()); i++) {
    if (state_component_predicted_index_[i] == x_index_) {
      C(2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 2) +=
        dF_d_states_with_history(i, 0) * predict_dt_ * std::cos(yaw);
      C(2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 2) -=
        dF_d_states_with_history(i + 1, 0) * predict_dt_ * std::sin(yaw);
      C.block(
        2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 4, 1,
        states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) +=
        dF_d_states_with_history.block(
          i, 1, 1,
          states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) *
        predict_dt_ * std::cos(yaw);
      C.block(
        2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 4, 1,
        states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) -=
        dF_d_states_with_history.block(
          i + 1, 1, 1,
          states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) *
        predict_dt_ * std::sin(yaw);
      C.block(2 * h_dim_ + state_component_predicted_index_[i], 0, 1, 2 * h_dim_) +=
        dF_dhc.row(i) * predict_dt_ * std::cos(yaw);
      C.block(2 * h_dim_ + state_component_predicted_index_[i], 0, 1, 2 * h_dim_) -=
        dF_dhc.row(i + 1) * predict_dt_ * std::sin(yaw);
    } else if (state_component_predicted_index_[i] == y_index_) {
      C(2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 2) +=
        dF_d_states_with_history(i - 1, 0) * predict_dt_ * std::sin(yaw);
      C(2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 2) +=
        dF_d_states_with_history(i, 0) * predict_dt_ * std::cos(yaw);
      C.block(
        2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 4, 1,
        states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) +=
        dF_d_states_with_history.block(
          i - 1, 1, 1,
          states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) *
        predict_dt_ * std::sin(yaw);
      C.block(
        2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 4, 1,
        states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) +=
        dF_d_states_with_history.block(
          i, 1, 1,
          states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) *
        predict_dt_ * std::cos(yaw);
      C.block(2 * h_dim_ + state_component_predicted_index_[i], 0, 1, 2 * h_dim_) +=
        dF_dhc.row(i - 1) * predict_dt_ * std::sin(yaw);
      C.block(2 * h_dim_ + state_component_predicted_index_[i], 0, 1, 2 * h_dim_) +=
        dF_dhc.row(i) * predict_dt_ * std::cos(yaw);
    } else {
      C(2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 2) +=
        dF_d_states_with_history(i, 0) * predict_dt_;
      C.block(
        2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + 4, 1,
        states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) +=
        dF_d_states_with_history.block(
          i, 1, 1,
          states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) *
        predict_dt_;
      C.block(2 * h_dim_ + state_component_predicted_index_[i], 0, 1, 2 * h_dim_) +=
        dF_dhc.row(i) * predict_dt_;
    }
  }
  C.block(0, 0, 2 * h_dim_, 2 * h_dim_) = dhc_dhc;

  C.block(0, 2 * h_dim_ + 2, 2 * h_dim_, 1) = dhc_d_states_with_history.col(0);
  C.block(
    0, 2 * h_dim_ + 4, 2 * h_dim_,
    states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_) =
    dhc_d_states_with_history.rightCols(
      states.size() - 3 - 1 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_);

  Eigen::VectorXd d_steer_d_steer_input =
    C.block(
       2 * h_dim_ + steer_index_, 2 * h_dim_ + steer_input_start_index_ + predict_step_, 1,
       steer_queue_size_ + predict_step_)
      .transpose();
  d_steer_d_steer_input.head(steer_queue_size_) +=
    A.block(steer_index_, steer_input_start_index_, 1, steer_queue_size_).transpose();
  Eigen::VectorXd d_acc_d_acc_input =
    C.block(2 * h_dim_ + acc_index_, 2 * h_dim_ + states.size(), 1, acc_queue_size_ + predict_step_)
      .transpose();
  d_acc_d_acc_input.head(acc_queue_size_) +=
    A.block(acc_index_, states.size(), 1, acc_queue_size_).transpose();
  double steer_diff = d_steer_d_steer_input.sum();
  double acc_diff = d_acc_d_acc_input.sum();

  if (steer_diff < min_gradient_steer_) {
    int max_d_steer_index;
    d_steer_d_steer_input.array().maxCoeff(&max_d_steer_index);
    max_d_steer_index = std::min(max_d_steer_index, steer_queue_size_);
    for (int i = 0; i < predict_step_; i++) {
      C(2 * h_dim_ + steer_index_,
        2 * h_dim_ + steer_input_start_index_ + predict_step_ + max_d_steer_index + i) +=
        (min_gradient_steer_ - steer_diff) / predict_step_;
    }
  }
  if (acc_diff < min_gradient_acc_) {
    int max_d_acc_index;
    d_acc_d_acc_input.array().maxCoeff(&max_d_acc_index);
    max_d_acc_index = std::min(max_d_acc_index, acc_queue_size_);
    for (int i = 0; i < predict_step_; i++) {
      C(2 * h_dim_ + acc_index_,
        2 * h_dim_ + states.size() + acc_queue_size_ + predict_step_ + max_d_acc_index + i) +=
        (min_gradient_acc_ - acc_diff) / predict_step_;
    }
  }
  if (horizon == 0) {
    previous_error = error_decay_rate_ * previous_error + (1 - error_decay_rate_) * NN_output;
  } else {
    previous_error = double_power(error_decay_rate_, predict_step_) * previous_error +
                     (1 - double_power(error_decay_rate_, predict_step_)) * NN_output;
  }
  h_lstm = h_lstm_next;
  c_lstm = c_lstm_next;
  for (int i = 0; i < int(state_component_predicted_index_.size()); i++) {
    if (state_component_predicted_index_[i] == x_index_) {
      states_next[x_index_] += previous_error[i] * predict_dt_ * std::cos(yaw) -
                               previous_error[i + 1] * predict_dt_ * std::sin(yaw);
      C(2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + yaw_index_) +=
        -previous_error[i] * predict_dt_ * std::sin(yaw) -
        previous_error[i + 1] * predict_dt_ * std::cos(yaw);
    } else if (state_component_predicted_index_[i] == y_index_) {
      states_next[y_index_] += previous_error[i - 1] * predict_dt_ * std::sin(yaw) +
                               previous_error[i] * predict_dt_ * std::cos(yaw);
      C(2 * h_dim_ + state_component_predicted_index_[i], 2 * h_dim_ + yaw_index_) +=
        previous_error[i - 1] * predict_dt_ * std::cos(yaw) -
        previous_error[i] * predict_dt_ * std::sin(yaw);
    } else {
      states_next[state_component_predicted_index_[i]] += previous_error[i] * predict_dt_;
    }
  }
  return states_next;
}
Eigen::VectorXd TrainedDynamics::F_with_model_diff(
  const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs, Eigen::VectorXd & h_lstm,
  Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error, const int horizon,
  Eigen::MatrixXd & A, Eigen::MatrixXd & B, Eigen::MatrixXd & C)
{
  Eigen::VectorXd acc_input_history_concat, steer_input_history_concat;
  Eigen::VectorXd states_next = F_with_model_diff(
    states, acc_input_history, steer_input_history, acc_input_history_concat,
    steer_input_history_concat, d_inputs, h_lstm, c_lstm, previous_error, horizon, A, B, C);
  return states_next;
}
Eigen::MatrixXd TrainedDynamics::F_with_model_for_candidates(
  const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
  Eigen::MatrixXd & Steer_input_history, Eigen::MatrixXd & Acc_input_history_concat,
  Eigen::MatrixXd & Steer_input_history_concat, const Eigen::MatrixXd & D_inputs,
  Eigen::MatrixXd & H_lstm, Eigen::MatrixXd & C_lstm, Eigen::MatrixXd & Previous_error,
  const int horizon)
{
  Eigen::MatrixXd States_next = nominal_dynamics_.F_with_input_history_for_candidates(
    States, Acc_input_history, Steer_input_history, Acc_input_history_concat,
    Steer_input_history_concat, D_inputs);
  Eigen::MatrixXd NN_input = Eigen::MatrixXd::Zero(
    3 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_, States.cols());
  NN_input.row(0) = States.row(vel_index_);
  NN_input.row(1) = States.row(acc_index_);
  NN_input.row(2) = States.row(steer_index_),
  NN_input.middleRows(3, acc_queue_size_ + predict_step_) = Acc_input_history_concat;
  NN_input.middleRows(3 + acc_queue_size_ + predict_step_, steer_queue_size_ + predict_step_) =
    Steer_input_history_concat;

  for (int i = 0; i < States.rows(); i++) {
    Eigen::VectorXd h_lstm = H_lstm.col(i);
    Eigen::VectorXd c_lstm = C_lstm.col(i);
    Eigen::VectorXd NN_output;
    Eigen::VectorXd h_lstm_next, c_lstm_next;
    transform_model_to_eigen_.error_prediction(
      NN_input.col(i), h_lstm, c_lstm, h_lstm_next, c_lstm_next, NN_output);
    if (horizon == 0) {
      Previous_error.col(i) =
        error_decay_rate_ * Previous_error.col(i) + (1.0 - error_decay_rate_) * NN_output;
    } else {
      Previous_error.col(i) =
        double_power(error_decay_rate_, predict_step_) * Previous_error.col(i) +
        (1.0 - double_power(error_decay_rate_, predict_step_)) * NN_output;
    }
    H_lstm.col(i) = h_lstm_next;
    C_lstm.col(i) = c_lstm_next;
  }
  Eigen::RowVectorXd yaw = States.row(yaw_index_);
  for (int i = 0; i < int(state_component_predicted_index_.size()); i++) {
    if (state_component_predicted_index_[i] == x_index_) {
      States_next.row(x_index_) +=
        (Previous_error.row(i).array() * predict_dt_ * yaw.array().cos() -
         Previous_error.row(i + 1).array() * predict_dt_ * yaw.array().sin())
          .matrix();
    } else if (state_component_predicted_index_[i] == y_index_) {
      States_next.row(y_index_) +=
        (Previous_error.row(i - 1).array() * predict_dt_ * yaw.array().sin() +
         Previous_error.row(i).array() * predict_dt_ * yaw.array().cos())
          .matrix();
    } else {
      States_next.row(state_component_predicted_index_[i]) += Previous_error.row(i) * predict_dt_;
    }
  }
  return States_next;
}
Eigen::MatrixXd TrainedDynamics::F_with_model_for_candidates(
  const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
  Eigen::MatrixXd & Steer_input_history, const Eigen::MatrixXd & D_inputs, Eigen::MatrixXd & H_lstm,
  Eigen::MatrixXd & C_lstm, Eigen::MatrixXd & Previous_error, const int horizon)
{
  Eigen::MatrixXd Acc_input_history_concat, Steer_input_history_concat;
  Eigen::MatrixXd States_next = F_with_model_for_candidates(
    States, Acc_input_history, Steer_input_history, Acc_input_history_concat,
    Steer_input_history_concat, D_inputs, H_lstm, C_lstm, Previous_error, horizon);
  return States_next;
}
void TrainedDynamics::calc_forward_trajectory_with_diff(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
  const Eigen::VectorXd & steer_input_history,
  const std::vector<Eigen::VectorXd> & d_inputs_schedule, const Eigen::VectorXd & h_lstm,
  const Eigen::VectorXd & c_lstm, const Eigen::VectorXd & previous_error,
  std::vector<Eigen::VectorXd> & states_prediction, std::vector<Eigen::MatrixXd> & dF_d_states,
  std::vector<Eigen::MatrixXd> & dF_d_inputs, std::vector<Eigen::Vector2d> & inputs_schedule)
{
  int horizon_len = d_inputs_schedule.size();
  Eigen::VectorXd previous_error_tmp = previous_error;
  states_prediction = std::vector<Eigen::VectorXd>(horizon_len + 1);
  dF_d_states = std::vector<Eigen::MatrixXd>(horizon_len);
  dF_d_inputs = std::vector<Eigen::MatrixXd>(horizon_len);
  inputs_schedule = std::vector<Eigen::Vector2d>(horizon_len);
  std::vector<Eigen::MatrixXd> A_vec(horizon_len), B_vec(horizon_len), C_vec(horizon_len);
  states_prediction[0] = states;
  Eigen::VectorXd acc_input_history_tmp, steer_input_history_tmp, h_lstm_tmp, c_lstm_tmp;

  acc_input_history_tmp = acc_input_history;
  steer_input_history_tmp = steer_input_history;
  h_lstm_tmp = h_lstm;
  c_lstm_tmp = c_lstm;
  for (int i = 0; i < horizon_len; i++) {
    Eigen::Vector2d d_inputs = d_inputs_schedule[i];
    Eigen::MatrixXd A, B, C;
    states_prediction[i + 1] = F_with_model_diff(
      states_prediction[i], acc_input_history_tmp, steer_input_history_tmp, d_inputs, h_lstm_tmp,
      c_lstm_tmp, previous_error_tmp, i, A, B, C);
    inputs_schedule[i][0] = acc_input_history_tmp[acc_input_history_tmp.size() - predict_step_];
    inputs_schedule[i][1] = steer_input_history_tmp[steer_input_history_tmp.size() - predict_step_];

    A_vec[i] = A;
    B_vec[i] = B;
    C_vec[i] = C;
  }
  filter_diff_NN_.fit_transform_for_NN_diff(A_vec, B_vec, C_vec, dF_d_states, dF_d_inputs);
}
///////////////// AdaptorILQR ///////////////////////

AdaptorILQR::AdaptorILQR()
{
  set_params();
}
AdaptorILQR::~AdaptorILQR()
{
}
void AdaptorILQR::set_params()
{
  YAML::Node optimization_param_node =
    YAML::LoadFile(get_param_dir_path() + "/optimization_param.yaml");
  bool add_position_to_ilqr =
    optimization_param_node["optimization_parameter"]["ilqr"]["add_position_to_ilqr"].as<bool>();
  bool add_yaw_to_ilqr =
    optimization_param_node["optimization_parameter"]["ilqr"]["add_yaw_to_ilqr"].as<bool>();
  if (add_yaw_to_ilqr) {
    state_component_ilqr_ = {"vel", "yaw", "acc", "steer"};
  }
  if (add_position_to_ilqr) {
    state_component_ilqr_.insert(state_component_ilqr_.begin(), "y");
    state_component_ilqr_.insert(state_component_ilqr_.begin(), "x");
  }
  controller_acc_input_weight_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_acc_input_weight_target_table"]
                             .as<std::vector<double>>();
  controller_longitudinal_coef_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_longitudinal_coef_target_table"]
                             .as<std::vector<double>>();
  controller_vel_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_vel_error_domain_table"]
                             .as<std::vector<double>>();
  controller_acc_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_acc_error_domain_table"]
                             .as<std::vector<double>>();

  controller_steer_input_weight_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_steer_input_weight_target_table"]
                             .as<std::vector<double>>();
  controller_lateral_coef_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_lateral_coef_target_table"]
                             .as<std::vector<double>>();
  controller_yaw_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_yaw_error_domain_table"]
                             .as<std::vector<double>>();
  controller_steer_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["controller_steer_error_domain_table"]
                             .as<std::vector<double>>();

  acc_rate_input_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["acc_rate_input_table"]
      .as<std::vector<double>>();
  acc_rate_cost_coef_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["acc_rate_cost_coef_table"]
      .as<std::vector<double>>();
  x_coef_by_acc_rate_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["x_coef_by_acc_rate_table"]
      .as<std::vector<double>>();
  vel_coef_by_acc_rate_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["vel_coef_by_acc_rate_table"]
      .as<std::vector<double>>();
  acc_coef_by_acc_rate_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["acc_coef_by_acc_rate_table"]
      .as<std::vector<double>>();

  steer_rate_input_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["steer_rate_input_table"]
      .as<std::vector<double>>();
  steer_rate_cost_coef_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["steer_rate_cost_coef_table"]
      .as<std::vector<double>>();
  y_coef_by_steer_rate_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["y_coef_by_steer_rate_table"]
      .as<std::vector<double>>();
  yaw_coef_by_steer_rate_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["yaw_coef_by_steer_rate_table"]
      .as<std::vector<double>>();
  steer_coef_by_steer_rate_table_ = optimization_param_node["optimization_parameter"]["cost_tables"]
                                                           ["steer_coef_by_steer_rate_table"]
                                                             .as<std::vector<double>>();

  vel_for_steer_rate_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["vel_for_steer_rate_table"]
      .as<std::vector<double>>();
  steer_rate_cost_coef_by_vel_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]
                           ["steer_rate_cost_coef_by_vel_table"]
                             .as<std::vector<double>>();

  x_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["x_error_domain_table"]
      .as<std::vector<double>>();
  x_error_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["x_error_target_table"]
      .as<std::vector<double>>();
  y_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["y_error_domain_table"]
      .as<std::vector<double>>();
  y_error_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["y_error_target_table"]
      .as<std::vector<double>>();
  vel_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["vel_error_domain_table"]
      .as<std::vector<double>>();
  vel_error_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["vel_error_target_table"]
      .as<std::vector<double>>();
  yaw_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["yaw_error_domain_table"]
      .as<std::vector<double>>();
  yaw_error_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["yaw_error_target_table"]
      .as<std::vector<double>>();
  acc_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["acc_error_domain_table"]
      .as<std::vector<double>>();
  acc_error_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["acc_error_target_table"]
      .as<std::vector<double>>();
  steer_error_domain_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["steer_error_domain_table"]
      .as<std::vector<double>>();
  steer_error_target_table_ =
    optimization_param_node["optimization_parameter"]["cost_tables"]["steer_error_target_table"]
      .as<std::vector<double>>();
}
void AdaptorILQR::set_states_cost(
  double x_cost, double y_cost, double vel_cost, double yaw_cost, double acc_cost,
  double steer_cost)
{
  x_cost_ = x_cost;
  y_cost_ = y_cost;
  vel_cost_ = vel_cost;
  yaw_cost_ = yaw_cost;
  steer_cost_ = steer_cost;
  acc_cost_ = acc_cost;
}
void AdaptorILQR::set_inputs_cost(double acc_rate_cost, double steer_rate_cost)
{
  steer_rate_cost_ = steer_rate_cost;
  acc_rate_cost_ = acc_rate_cost;
}
void AdaptorILQR::set_rate_cost(double acc_rate_rate_cost, double steer_rate_rate_cost)
{
  steer_rate_rate_cost_ = steer_rate_rate_cost;
  acc_rate_rate_cost_ = acc_rate_rate_cost;
}
void AdaptorILQR::set_intermediate_cost(
  double x_intermediate_cost, double y_intermediate_cost, double vel_intermediate_cost,
  double yaw_intermediate_cost, double acc_intermediate_cost, double steer_intermediate_cost,
  int intermediate_cost_index)
{
  x_intermediate_cost_ = x_intermediate_cost;
  y_intermediate_cost_ = y_intermediate_cost;
  vel_intermediate_cost_ = vel_intermediate_cost;
  yaw_intermediate_cost_ = yaw_intermediate_cost;
  steer_intermediate_cost_ = steer_intermediate_cost;
  acc_intermediate_cost_ = acc_intermediate_cost;
  intermediate_cost_index_ = intermediate_cost_index;
}

void AdaptorILQR::set_terminal_cost(
  double x_terminal_cost, double y_terminal_cost, double vel_terminal_cost,
  double yaw_terminal_cost, double acc_terminal_cost, double steer_terminal_cost)
{
  x_terminal_cost_ = x_terminal_cost;
  y_terminal_cost_ = y_terminal_cost;
  vel_terminal_cost_ = vel_terminal_cost;
  yaw_terminal_cost_ = yaw_terminal_cost;
  steer_terminal_cost_ = steer_terminal_cost;
  acc_terminal_cost_ = acc_terminal_cost;
}
void AdaptorILQR::set_vehicle_params(
  double wheel_base, double acc_time_delay, double steer_time_delay, double acc_time_constant,
  double steer_time_constant, int acc_queue_size, int steer_queue_size, double control_dt,
  int predict_step)
{
  wheel_base_ = wheel_base;
  acc_time_delay_ = acc_time_delay;
  steer_time_delay_ = steer_time_delay;
  acc_time_constant_ = acc_time_constant;
  steer_time_constant_ = steer_time_constant;
  acc_queue_size_ = acc_queue_size;
  steer_queue_size_ = steer_queue_size;

  predict_step_ = predict_step;
  acc_input_end_index_ = 6 + acc_queue_size_ - 1;
  steer_input_start_index_ = 6 + acc_queue_size_;
  steer_input_end_index_ = 6 + acc_queue_size_ + steer_queue_size_ - 1;
  control_dt_ = control_dt;
  predict_dt_ = control_dt_ * predict_step_;

  acc_delay_step_ = std::min(int(std::round(acc_time_delay_ / control_dt_)), acc_queue_size_);
  steer_delay_step_ = std::min(int(std::round(steer_time_delay_ / control_dt_)), steer_queue_size_);
  trained_dynamics_.set_vehicle_params(
    wheel_base, acc_time_delay, steer_time_delay, acc_time_constant, steer_time_constant,
    acc_queue_size, steer_queue_size, control_dt, predict_step);
}
void AdaptorILQR::set_horizon_len(int horizon_len)
{
  horizon_len_ = horizon_len;
}
void AdaptorILQR::set_NN_params(
  const Eigen::MatrixXd & weight_acc_encoder_layer_1,
  const Eigen::MatrixXd & weight_steer_encoder_layer_1,
  const Eigen::MatrixXd & weight_acc_encoder_layer_2,
  const Eigen::MatrixXd & weight_steer_encoder_layer_2, const Eigen::MatrixXd & weight_acc_layer_1,
  const Eigen::MatrixXd & weight_steer_layer_1, const Eigen::MatrixXd & weight_acc_layer_2,
  const Eigen::MatrixXd & weight_steer_layer_2,
  const std::vector<Eigen::MatrixXd> & weight_lstm_encoder_ih,
  const std::vector<Eigen::MatrixXd> & weight_lstm_encoder_hh,
  const Eigen::MatrixXd & weight_lstm_ih, const Eigen::MatrixXd & weight_lstm_hh,
  const Eigen::MatrixXd & weight_complimentary_layer, const Eigen::MatrixXd & weight_linear_relu,
  const Eigen::MatrixXd & weight_final_layer, const Eigen::VectorXd & bias_acc_encoder_layer_1,
  const Eigen::VectorXd & bias_steer_encoder_layer_1,
  const Eigen::VectorXd & bias_acc_encoder_layer_2,
  const Eigen::VectorXd & bias_steer_encoder_layer_2, const Eigen::VectorXd & bias_acc_layer_1,
  const Eigen::VectorXd & bias_steer_layer_1, const Eigen::VectorXd & bias_acc_layer_2,
  const Eigen::VectorXd & bias_steer_layer_2,
  const std::vector<Eigen::VectorXd> & bias_lstm_encoder_ih,
  const std::vector<Eigen::VectorXd> & bias_lstm_encoder_hh, const Eigen::VectorXd & bias_lstm_ih,
  const Eigen::VectorXd & bias_lstm_hh, const Eigen::VectorXd & bias_complimentary_layer,
  const Eigen::VectorXd & bias_linear_relu, const Eigen::VectorXd & bias_final_layer,
  const double vel_scaling, const double vel_bias,
  const std::vector<std::string> state_component_predicted)
{
  trained_dynamics_.set_NN_params(
    weight_acc_encoder_layer_1, weight_steer_encoder_layer_1, weight_acc_encoder_layer_2,
    weight_steer_encoder_layer_2, weight_acc_layer_1, weight_steer_layer_1, weight_acc_layer_2,
    weight_steer_layer_2, weight_lstm_encoder_ih, weight_lstm_encoder_hh, weight_lstm_ih,
    weight_lstm_hh, weight_complimentary_layer, weight_linear_relu, weight_final_layer,
    bias_acc_encoder_layer_1, bias_steer_encoder_layer_1, bias_acc_encoder_layer_2,
    bias_steer_encoder_layer_2, bias_acc_layer_1, bias_steer_layer_1, bias_acc_layer_2,
    bias_steer_layer_2, bias_lstm_encoder_ih, bias_lstm_encoder_hh, bias_lstm_ih, bias_lstm_hh,
    bias_complimentary_layer, bias_linear_relu, bias_final_layer, vel_scaling, vel_bias,
    state_component_predicted);
  h_dim_ = weight_lstm_hh.cols();
  num_state_component_ilqr_ = int(state_component_ilqr_.size());
  state_component_ilqr_index_ = std::vector<int>(num_state_component_ilqr_);
  for (int i = 0; i < int(state_component_ilqr_.size()); i++) {
    for (int j = 0; j < int(all_state_name_.size()); j++) {
      if (state_component_ilqr_[i] == all_state_name_[j]) {
        state_component_ilqr_index_[i] = j;
      }
    }
  }
}
void AdaptorILQR::clear_NN_params()
{
  trained_dynamics_.clear_NN_params();
}
void AdaptorILQR::set_sg_filter_params(int degree, int window_size)
{
  trained_dynamics_.set_sg_filter_params(degree, window_size);
}
Eigen::VectorXd AdaptorILQR::nominal_prediction(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
  const Eigen::VectorXd & steer_input_history_concat)
{
  return trained_dynamics_.nominal_prediction(
    states, acc_input_history_concat, steer_input_history_concat);
}
void AdaptorILQR::update_lstm_states(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
  const Eigen::VectorXd & steer_input_history_concat, std::vector<Eigen::VectorXd> & h_lstm,
  std::vector<Eigen::VectorXd> & c_lstm, const Eigen::Vector2d & previous_error)
{
  trained_dynamics_.update_lstm_states(
    states, acc_input_history_concat, steer_input_history_concat, h_lstm, c_lstm, previous_error);
}
void AdaptorILQR::calc_forward_trajectory_with_cost(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
  const Eigen::VectorXd & steer_input_history,
  const std::vector<Eigen::MatrixXd> & D_inputs_schedule, const Eigen::VectorXd & h_lstm,
  const Eigen::VectorXd & c_lstm, const Eigen::VectorXd & previous_error,
  std::vector<Eigen::MatrixXd> & states_prediction, const std::vector<Eigen::VectorXd> & states_ref,
  const std::vector<Eigen::VectorXd> & d_input_ref, const std::vector<Eigen::Vector2d> & inputs_ref,
  double x_weight_coef, double y_weight_coef, double vel_weight_coef, double yaw_weight_coef,
  double acc_weight_coef, double steer_weight_coef, double acc_input_weight,
  double steer_input_weight, double acc_rate_weight_coef, double steer_rate_weight_coef,
  Eigen::VectorXd & Cost)
{
  int sample_size = D_inputs_schedule[0].cols();
  Cost = Eigen::VectorXd::Zero(sample_size);
  Eigen::VectorXd X_Cost = Eigen::VectorXd::Zero(sample_size);
  Eigen::VectorXd Y_Cost = Eigen::VectorXd::Zero(sample_size);
  Eigen::VectorXd Vel_Cost = Eigen::VectorXd::Zero(sample_size);
  Eigen::VectorXd Yaw_Cost = Eigen::VectorXd::Zero(sample_size);
  Eigen::VectorXd Acc_Cost = Eigen::VectorXd::Zero(sample_size);
  Eigen::VectorXd Steer_Cost = Eigen::VectorXd::Zero(sample_size);
  Eigen::RowVectorXd Max_X_Diff = Eigen::RowVectorXd::Zero(sample_size);
  Eigen::RowVectorXd Max_Y_Diff = Eigen::RowVectorXd::Zero(sample_size);
  Eigen::RowVectorXd Max_Vel_Diff = Eigen::RowVectorXd::Zero(sample_size);
  Eigen::RowVectorXd Max_Yaw_Diff = Eigen::RowVectorXd::Zero(sample_size);
  Eigen::RowVectorXd Max_Acc_Diff = Eigen::RowVectorXd::Zero(sample_size);
  Eigen::RowVectorXd Max_Steer_Diff = Eigen::RowVectorXd::Zero(sample_size);

  states_prediction = std::vector<Eigen::MatrixXd>(horizon_len_ + 1);
  states_prediction[0] = states.replicate(1, sample_size);
  Eigen::MatrixXd Previous_error = previous_error.replicate(1, sample_size);
  Eigen::MatrixXd Acc_input_history = acc_input_history.replicate(1, sample_size);
  Eigen::MatrixXd Steer_input_history = steer_input_history.replicate(1, sample_size);
  Eigen::MatrixXd H_lstm = h_lstm.replicate(1, sample_size);
  Eigen::MatrixXd C_lstm = c_lstm.replicate(1, sample_size);
  for (int i = 0; i < horizon_len_; i++) {
    Eigen::MatrixXd D_inputs = D_inputs_schedule[i];
    double x_cost = x_cost_;
    double y_cost = y_cost_;
    double vel_cost = vel_cost_;
    double yaw_cost = yaw_cost_;
    double acc_cost = acc_cost_;
    double steer_cost = steer_cost_;
    if (i == intermediate_cost_index_) {
      x_cost = x_intermediate_cost_;
      y_cost = y_intermediate_cost_;
      vel_cost = vel_intermediate_cost_;
      yaw_cost = yaw_intermediate_cost_;
      acc_cost = acc_intermediate_cost_;
      steer_cost = steer_intermediate_cost_;
    }
    double yaw_target = states_ref[i][yaw_index_];
    Eigen::Matrix2d rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
    Eigen::MatrixXd xy_diff = states_prediction[i].block(x_index_, 0, 2, sample_size).colwise() -
                              states_ref[i].segment(x_index_, 2);
    xy_diff = rotation_matrix * xy_diff;
    Max_X_Diff = xy_diff.row(0).cwiseAbs().cwiseMax(Max_X_Diff);
    Max_Y_Diff = xy_diff.row(1).cwiseAbs().cwiseMax(Max_Y_Diff);
    X_Cost += 0.5 * x_cost * xy_diff.row(0).array().square().matrix().transpose();
    Y_Cost += 0.5 * y_cost * xy_diff.row(1).array().square().matrix().transpose();
    Max_Vel_Diff = (states_prediction[i].row(vel_index_).array() - states_ref[i][vel_index_])
                     .cwiseAbs()
                     .matrix()
                     .cwiseMax(Max_Vel_Diff);
    Vel_Cost += 0.5 * vel_cost *
                (states_prediction[i].row(vel_index_).array() - states_ref[i][vel_index_])
                  .square()
                  .matrix()
                  .transpose();
    Max_Yaw_Diff = (states_prediction[i].row(yaw_index_).array() - states_ref[i][yaw_index_])
                     .cwiseAbs()
                     .matrix()
                     .cwiseMax(Max_Yaw_Diff);
    Yaw_Cost += 0.5 * yaw_cost *
                (states_prediction[i].row(yaw_index_).array() - states_ref[i][yaw_index_])
                  .square()
                  .matrix()
                  .transpose();
    Max_Acc_Diff = (states_prediction[i].row(acc_index_).array() - states_ref[i][acc_index_])
                     .cwiseAbs()
                     .matrix()
                     .cwiseMax(Max_Acc_Diff);
    Acc_Cost += 0.5 * acc_cost *
                (states_prediction[i].row(acc_index_).array() - states_ref[i][acc_index_])
                  .square()
                  .matrix()
                  .transpose();
    Max_Steer_Diff = (states_prediction[i].row(steer_index_).array() - states_ref[i][steer_index_])
                       .cwiseAbs()
                       .matrix()
                       .cwiseMax(Max_Steer_Diff);
    Steer_Cost += 0.5 * steer_cost *
                  (states_prediction[i].row(steer_index_).array() - states_ref[i][steer_index_])
                    .square()
                    .matrix()
                    .transpose();

    Cost += 0.5 * acc_rate_weight_coef * acc_rate_cost_ *
            (D_inputs.row(0).array() - d_input_ref[i][0]).square().matrix().transpose();

    Cost += 0.5 * steer_rate_weight_coef * steer_rate_cost_ *
            (D_inputs.row(1).array() - d_input_ref[i][1]).square().matrix().transpose();
    if (i == 0) {
      double prev_acc_rate =
        (acc_input_history[acc_queue_size_ - 1] - acc_input_history[acc_queue_size_ - 2]) /
        control_dt_;
      double prev_steer_rate =
        (steer_input_history[steer_queue_size_ - 1] - steer_input_history[steer_queue_size_ - 2]) /
        control_dt_;
      Cost += 0.5 * acc_rate_rate_cost_ *
              (D_inputs.row(0).array() - prev_acc_rate).square().matrix().transpose();
      Cost += 0.5 * steer_rate_rate_cost_ *
              (D_inputs.row(1).array() - prev_steer_rate).square().matrix().transpose();
    }
    if (i > 0) {
      Cost += 0.5 * acc_rate_rate_cost_ *
              (D_inputs.row(0).array() - D_inputs_schedule[i - 1].row(0).array())
                .square()
                .matrix()
                .transpose();
      Cost += 0.5 * steer_rate_rate_cost_ *
              (D_inputs.row(1).array() - D_inputs_schedule[i - 1].row(1).array())
                .square()
                .matrix()
                .transpose();
      Cost +=
        0.5 * acc_input_weight *
        (Acc_input_history.row(acc_queue_size_ - predict_step_).array() - inputs_ref[i - 1][0])
          .square()
          .matrix()
          .transpose();
      Cost +=
        0.5 * steer_input_weight *
        (Steer_input_history.row(steer_queue_size_ - predict_step_).array() - inputs_ref[i - 1][1])
          .square()
          .matrix()
          .transpose();
    }
    states_prediction[i + 1] = trained_dynamics_.F_with_model_for_candidates(
      states_prediction[i], Acc_input_history, Steer_input_history, D_inputs, H_lstm, C_lstm,
      Previous_error, i);
  }
  for (int i = 0; i < sample_size; i++) {
    for (int j = 0; j < num_state_component_ilqr_; j++) {
      if (state_component_ilqr_[j] == "x") {
        Cost[i] +=
          std::min(
            x_weight_coef,
            calc_table_value(Max_X_Diff[i], x_error_domain_table_, x_error_target_table_)) *
          X_Cost[i];
      } else if (state_component_ilqr_[j] == "y") {
        Cost[i] +=
          std::min(
            y_weight_coef,
            calc_table_value(Max_Y_Diff[i], y_error_domain_table_, y_error_target_table_)) *
          Y_Cost[i];
      } else if (state_component_ilqr_[j] == "vel") {
        Cost[i] +=
          std::min(
            vel_weight_coef,
            calc_table_value(Max_Vel_Diff[i], vel_error_domain_table_, vel_error_target_table_)) *
          Vel_Cost[i];
      } else if (state_component_ilqr_[j] == "yaw") {
        Cost[i] +=
          std::min(
            yaw_weight_coef,
            calc_table_value(Max_Yaw_Diff[i], yaw_error_domain_table_, yaw_error_target_table_)) *
          Yaw_Cost[i];
      } else if (state_component_ilqr_[j] == "acc") {
        Cost[i] +=
          std::min(
            acc_weight_coef,
            calc_table_value(Max_Acc_Diff[i], acc_error_domain_table_, acc_error_target_table_)) *
          Acc_Cost[i];
      } else if (state_component_ilqr_[j] == "steer") {
        Cost[i] += std::min(
                     steer_weight_coef,
                     calc_table_value(
                       Max_Steer_Diff[i], steer_error_domain_table_, steer_error_target_table_)) *
                   Steer_Cost[i];
      }
    }
  }

  double yaw_target = states_ref[horizon_len_][yaw_index_];
  Eigen::Matrix2d rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
  Eigen::Matrix2d xy_cost_matrix_sqrt =
    rotation_matrix.transpose() *
    Eigen::DiagonalMatrix<double, 2>(
      std::sqrt(x_weight_coef * x_terminal_cost_), std::sqrt(y_weight_coef * y_terminal_cost_)) *
    rotation_matrix;
  Eigen::MatrixXd xy_diff =
    states_prediction[horizon_len_].block(x_index_, 0, 2, sample_size).colwise() -
    states_ref[horizon_len_].segment(x_index_, 2);
  for (int j = 0; j < num_state_component_ilqr_; j++) {
    if (state_component_ilqr_[j] == "x") {
      Cost += 0.5 * (xy_cost_matrix_sqrt * xy_diff).colwise().squaredNorm().transpose();
    } else if (state_component_ilqr_[j] == "vel") {
      Cost += 0.5 * vel_weight_coef * vel_terminal_cost_ *
              (states_prediction[horizon_len_].row(vel_index_).array() -
               states_ref[horizon_len_][vel_index_])
                .square()
                .matrix()
                .transpose();
    } else if (state_component_ilqr_[j] == "yaw") {
      Cost += 0.5 * yaw_weight_coef * yaw_terminal_cost_ *
              (states_prediction[horizon_len_].row(yaw_index_).array() -
               states_ref[horizon_len_][yaw_index_])
                .square()
                .matrix()
                .transpose();
    } else if (state_component_ilqr_[j] == "acc") {
      Cost += 0.5 * acc_weight_coef * acc_terminal_cost_ *
              (states_prediction[horizon_len_].row(acc_index_).array() -
               states_ref[horizon_len_][acc_index_])
                .square()
                .matrix()
                .transpose();
    } else if (state_component_ilqr_[j] == "steer") {
      Cost += 0.5 * steer_weight_coef * steer_terminal_cost_ *
              (states_prediction[horizon_len_].row(steer_index_).array() -
               states_ref[horizon_len_][steer_index_])
                .square()
                .matrix()
                .transpose();
    }
  }
}
void AdaptorILQR::calc_inputs_ref_info(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
  const Eigen::VectorXd & steer_input_history, const Eigen::VectorXd & h_lstm,
  const Eigen::VectorXd & c_lstm, const Eigen::VectorXd & previous_error,
  const std::vector<Eigen::VectorXd> & states_ref,
  const Eigen::VectorXd & acc_controller_input_schedule,
  const Eigen::VectorXd & steer_controller_input_schedule,
  const std::vector<Eigen::VectorXd> & states_prediction, std::vector<Eigen::Vector2d> & inputs_ref,
  const double & acc_input_change_rate, const double & steer_input_change_rate,
  double & x_weight_coef, double & y_weight_coef, double & vel_weight_coef,
  double & yaw_weight_coef, double & acc_weight_coef, double & steer_weight_coef,
  double & acc_input_weight, double & steer_input_weight, double & acc_rate_weight_coef,
  double & steer_rate_weight_coef, double & initial_prediction_x_weight_coef,
  double & initial_prediction_y_weight_coef, double & initial_prediction_vel_weight_coef,
  double & initial_prediction_yaw_weight_coef, double & initial_prediction_acc_weight_coef,
  double & initial_prediction_steer_weight_coef)
{  // edit here to change the adaptive cost
  Eigen::VectorXd acc_input_history_concat(acc_queue_size_ + predict_step_);
  Eigen::VectorXd steer_input_history_concat(steer_queue_size_ + predict_step_);
  acc_input_history_concat.head(acc_queue_size_) = acc_input_history;
  steer_input_history_concat.head(steer_queue_size_) = steer_input_history;
  acc_input_history_concat.tail(predict_step_) = acc_controller_input_schedule.head(predict_step_);
  steer_input_history_concat.tail(predict_step_) =
    steer_controller_input_schedule.head(predict_step_);
  std::vector<Eigen::VectorXd> states_prediction_by_controller_inputs(horizon_len_ + 1);
  states_prediction_by_controller_inputs[0] = states;
  Eigen::VectorXd previous_error_tmp = previous_error;
  Eigen::VectorXd h_lstm_tmp = h_lstm;
  Eigen::VectorXd c_lstm_tmp = c_lstm;
  inputs_ref = std::vector<Eigen::Vector2d>(horizon_len_);
  double max_vel_error = 0.0;
  double max_acc_error = 0.0;
  double max_steer_error = 0.0;
  double max_yaw_error = 0.0;
  double initial_prediction_max_x_error = 0.0;
  double initial_prediction_max_y_error = 0.0;
  double initial_prediction_max_vel_error = 0.0;
  double initial_prediction_max_acc_error = 0.0;
  double initial_prediction_max_steer_error = 0.0;
  double initial_prediction_max_yaw_error = 0.0;
  for (int i = 0; i < horizon_len_; i++) {
    if (i > 0) {
      acc_input_history_concat.head(acc_queue_size_) =
        acc_input_history_concat.tail(acc_queue_size_);
      steer_input_history_concat.head(steer_queue_size_) =
        steer_input_history_concat.tail(steer_queue_size_);
      acc_input_history_concat.tail(predict_step_) =
        acc_controller_input_schedule.segment(i * predict_step_, predict_step_);
      steer_input_history_concat.tail(predict_step_) =
        steer_controller_input_schedule.segment(i * predict_step_, predict_step_);
    }
    inputs_ref[i][0] = acc_controller_input_schedule[i * predict_step_];
    inputs_ref[i][1] = steer_controller_input_schedule[i * predict_step_];
    states_prediction_by_controller_inputs[i + 1] =
      trained_dynamics_.F_with_model_for_calc_controller_prediction_error(
        states_prediction_by_controller_inputs[i], acc_input_history_concat,
        steer_input_history_concat, h_lstm_tmp, c_lstm_tmp, previous_error_tmp, i);
    double vel_error = std::abs(
      states_ref[i + 1][vel_index_] - states_prediction_by_controller_inputs[i + 1][vel_index_]);
    double acc_error = std::abs(
      states_ref[i + 1][acc_index_] - states_prediction_by_controller_inputs[i + 1][acc_index_]);
    double steer_error = std::abs(
      states_ref[i + 1][steer_index_] -
      states_prediction_by_controller_inputs[i + 1][steer_index_]);
    double yaw_error = std::abs(
      states_ref[i + 1][yaw_index_] - states_prediction_by_controller_inputs[i + 1][yaw_index_]);
    if (vel_error > max_vel_error) {
      max_vel_error = vel_error;
    }
    if (acc_error > max_acc_error) {
      max_acc_error = acc_error;
    }
    if (steer_error > max_steer_error) {
      max_steer_error = steer_error;
    }
    if (yaw_error > max_yaw_error) {
      max_yaw_error = yaw_error;
    }

    double yaw_target = states_ref[i][yaw_index_];
    Eigen::Matrix2d rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
    Eigen::Vector2d xy_diff =
      rotation_matrix * Eigen::Vector2d(
                          states_prediction[i][x_index_] - states_ref[i][x_index_],
                          states_prediction[i][y_index_] - states_ref[i][y_index_]);
    double initial_prediction_x_error = std::abs(xy_diff[0]);
    double initial_prediction_y_error = std::abs(xy_diff[1]);
    double initial_prediction_vel_error =
      std::abs(states_prediction[i][vel_index_] - states_ref[i][vel_index_]);
    double initial_prediction_yaw_error =
      std::abs(states_prediction[i][yaw_index_] - states_ref[i][yaw_index_]);
    double initial_prediction_acc_error =
      std::abs(states_prediction[i][acc_index_] - states_ref[i][acc_index_]);
    double initial_prediction_steer_error =
      std::abs(states_prediction[i][steer_index_] - states_ref[i][steer_index_]);
    if (initial_prediction_x_error > initial_prediction_max_x_error) {
      initial_prediction_max_x_error = initial_prediction_x_error;
    }
    if (initial_prediction_y_error > initial_prediction_max_y_error) {
      initial_prediction_max_y_error = initial_prediction_y_error;
    }
    if (initial_prediction_vel_error > initial_prediction_max_vel_error) {
      initial_prediction_max_vel_error = initial_prediction_vel_error;
    }
    if (initial_prediction_yaw_error > initial_prediction_max_yaw_error) {
      initial_prediction_max_yaw_error = initial_prediction_yaw_error;
    }
    if (initial_prediction_acc_error > initial_prediction_max_acc_error) {
      initial_prediction_max_acc_error = initial_prediction_acc_error;
    }
    if (initial_prediction_steer_error > initial_prediction_max_steer_error) {
      initial_prediction_max_steer_error = initial_prediction_steer_error;
    }
  }
  double longitudinal_coef = std::max(
    calc_table_value(
      max_vel_error, controller_vel_error_domain_table_,
      controller_longitudinal_coef_target_table_),
    calc_table_value(
      max_acc_error, controller_acc_error_domain_table_,
      controller_longitudinal_coef_target_table_));
  double lateral_coef = std::max(
    calc_table_value(
      max_steer_error, controller_steer_error_domain_table_, controller_lateral_coef_target_table_),
    calc_table_value(
      max_yaw_error, controller_yaw_error_domain_table_, controller_lateral_coef_target_table_));
  x_weight_coef = std::min(
    longitudinal_coef,
    calc_table_value(acc_input_change_rate, acc_rate_input_table_, x_coef_by_acc_rate_table_));
  y_weight_coef = std::min(
    lateral_coef, calc_table_value(
                    steer_input_change_rate, steer_rate_input_table_, y_coef_by_steer_rate_table_));
  vel_weight_coef = std::min(
    longitudinal_coef,
    calc_table_value(acc_input_change_rate, acc_rate_input_table_, vel_coef_by_acc_rate_table_));
  yaw_weight_coef = std::min(
    lateral_coef,
    calc_table_value(
      steer_input_change_rate, steer_rate_input_table_, yaw_coef_by_steer_rate_table_));
  acc_weight_coef = std::min(
    longitudinal_coef,
    calc_table_value(acc_input_change_rate, acc_rate_input_table_, acc_coef_by_acc_rate_table_));
  steer_weight_coef = std::min(
    lateral_coef,
    calc_table_value(
      steer_input_change_rate, steer_rate_input_table_, steer_coef_by_steer_rate_table_));

  acc_input_weight = std::min(
    calc_table_value(
      max_vel_error, controller_vel_error_domain_table_, controller_acc_input_weight_target_table_),
    calc_table_value(
      max_acc_error, controller_acc_error_domain_table_,
      controller_acc_input_weight_target_table_));
  steer_input_weight = std::min(
    calc_table_value(
      max_steer_error, controller_steer_error_domain_table_,
      controller_steer_input_weight_target_table_),
    calc_table_value(
      max_yaw_error, controller_yaw_error_domain_table_,
      controller_steer_input_weight_target_table_));
  acc_rate_weight_coef =
    calc_table_value(acc_input_change_rate, acc_rate_input_table_, acc_rate_cost_coef_table_) *
    acc_rate_cost_;
  steer_rate_weight_coef =
    std::max(
      calc_table_value(
        states_ref[0][vel_index_], vel_for_steer_rate_table_, steer_rate_cost_coef_by_vel_table_),
      calc_table_value(
        steer_input_change_rate, steer_rate_input_table_, steer_rate_cost_coef_table_)) *
    steer_rate_cost_;

  initial_prediction_x_weight_coef =
    calc_table_value(initial_prediction_max_x_error, x_error_domain_table_, x_error_target_table_);
  initial_prediction_y_weight_coef =
    calc_table_value(initial_prediction_max_y_error, y_error_domain_table_, y_error_target_table_);
  initial_prediction_vel_weight_coef = calc_table_value(
    initial_prediction_max_vel_error, vel_error_domain_table_, vel_error_target_table_);
  initial_prediction_yaw_weight_coef = calc_table_value(
    initial_prediction_max_yaw_error, yaw_error_domain_table_, yaw_error_target_table_);
  initial_prediction_acc_weight_coef = calc_table_value(
    initial_prediction_max_acc_error, acc_error_domain_table_, acc_error_target_table_);
  initial_prediction_steer_weight_coef = calc_table_value(
    initial_prediction_max_steer_error, steer_error_domain_table_, steer_error_target_table_);
}
Eigen::MatrixXd AdaptorILQR::extract_dF_d_state(const Eigen::MatrixXd & dF_d_state_with_history)
{
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(
    2 * h_dim_ + num_state_component_ilqr_,
    2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ + steer_queue_size_);
  //(dF_d_state_with_history.rows() -3 , dF_d_state_with_history.cols()-3);
  result.block(0, 0, 2 * h_dim_, 2 * h_dim_) =
    dF_d_state_with_history.block(0, 0, 2 * h_dim_, 2 * h_dim_);
  for (int i = 0; i < num_state_component_ilqr_; i++) {
    result.block(2 * h_dim_ + i, 0, 1, 2 * h_dim_) =
      dF_d_state_with_history.block(2 * h_dim_ + state_component_ilqr_index_[i], 0, 1, 2 * h_dim_);
    result.block(0, 2 * h_dim_ + i, 2 * h_dim_, 1) =
      dF_d_state_with_history.block(0, 2 * h_dim_ + state_component_ilqr_index_[i], 2 * h_dim_, 1);
    result.block(
      2 * h_dim_ + i, 2 * h_dim_ + num_state_component_ilqr_, 1,
      acc_queue_size_ + steer_queue_size_) =
      dF_d_state_with_history.block(
        2 * h_dim_ + state_component_ilqr_index_[i], 2 * h_dim_ + state_size_, 1,
        acc_queue_size_ + steer_queue_size_);
    for (int j = 0; j < num_state_component_ilqr_; j++) {
      result(2 * h_dim_ + i, 2 * h_dim_ + j) = dF_d_state_with_history(
        2 * h_dim_ + state_component_ilqr_index_[i], 2 * h_dim_ + state_component_ilqr_index_[j]);
    }
  }
  return result;
}
Eigen::MatrixXd AdaptorILQR::extract_dF_d_input(const Eigen::MatrixXd & dF_d_input)
{
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(2 * h_dim_ + num_state_component_ilqr_, 2);
  result.block(0, 0, 2 * h_dim_, 2) = dF_d_input.block(0, 0, 2 * h_dim_, 2);
  for (int i = 0; i < num_state_component_ilqr_; i++) {
    result.block(2 * h_dim_ + i, 0, 1, 2) =
      dF_d_input.block(2 * h_dim_ + state_component_ilqr_index_[i], 0, 1, 2);
  }
  return result;
}

Eigen::MatrixXd AdaptorILQR::right_action_by_state_diff_with_history(
  const Eigen::MatrixXd & Mat, const Eigen::MatrixXd & dF_d_state_with_history)
{
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(Mat.rows(), dF_d_state_with_history.cols());
  result = Mat.leftCols(num_state_component_ilqr_ + 2 * h_dim_) * dF_d_state_with_history;
  result.middleCols(
    num_state_component_ilqr_ + 2 * h_dim_ + predict_step_, acc_queue_size_ - predict_step_) +=
    Mat.middleCols(num_state_component_ilqr_ + 2 * h_dim_, acc_queue_size_ - predict_step_);
  result.middleCols(
    num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + predict_step_,
    steer_queue_size_ - predict_step_) +=
    Mat.middleCols(
      num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_, steer_queue_size_ - predict_step_);
  for (int i = 0; i < predict_step_; i++) {
    result.col(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - 1) +=
      Mat.col(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - predict_step_ + i);
    result.col(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_ - 1) +=
      Mat.col(
        num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_ -
        predict_step_ + i);
  }
  return result;
}
Eigen::MatrixXd AdaptorILQR::left_action_by_state_diff_with_history(
  const Eigen::MatrixXd & dF_d_state_with_history, const Eigen::MatrixXd & Mat)
{
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(dF_d_state_with_history.cols(), Mat.cols());
  result =
    dF_d_state_with_history.transpose() * Mat.topRows(num_state_component_ilqr_ + 2 * h_dim_);
  result.middleRows(
    num_state_component_ilqr_ + 2 * h_dim_ + predict_step_, acc_queue_size_ - predict_step_) +=
    Mat.middleRows(num_state_component_ilqr_ + 2 * h_dim_, acc_queue_size_ - predict_step_);
  result.middleRows(
    num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + predict_step_,
    steer_queue_size_ - predict_step_) +=
    Mat.middleRows(
      num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_, steer_queue_size_ - predict_step_);
  for (int i = 0; i < predict_step_; i++) {
    result.row(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - 1) +=
      Mat.row(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - predict_step_ + i);
    result.row(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_ - 1) +=
      Mat.row(
        num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_ -
        predict_step_ + i);
  }
  return result;
}
Eigen::MatrixXd AdaptorILQR::right_action_by_input_diff(
  const Eigen::MatrixXd & Mat, const Eigen::MatrixXd & dF_d_input)
{
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(Mat.rows(), dF_d_input.cols());
  result = Mat.leftCols(num_state_component_ilqr_ + 2 * h_dim_) * dF_d_input;
  for (int i = 0; i < predict_step_; i++) {
    result.col(0) +=
      (i + 1) *
      Mat.col(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - predict_step_ + i) *
      control_dt_;
    result.col(1) += (i + 1) *
                     Mat.col(
                       num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ +
                       steer_queue_size_ - predict_step_ + i) *
                     control_dt_;
  }
  return result;
}
Eigen::MatrixXd AdaptorILQR::left_action_by_input_diff(
  const Eigen::MatrixXd & dF_d_input, const Eigen::MatrixXd & Mat)
{
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(dF_d_input.cols(), Mat.cols());
  result = dF_d_input.transpose() * Mat.topRows(num_state_component_ilqr_ + 2 * h_dim_);
  for (int i = 0; i < predict_step_; i++) {
    result.row(0) +=
      (i + 1) *
      Mat.row(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - predict_step_ + i) *
      control_dt_;
    result.row(1) += (i + 1) *
                     Mat.row(
                       num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ +
                       steer_queue_size_ - predict_step_ + i) *
                     control_dt_;
  }
  return result;
}
void AdaptorILQR::compute_ilqr_coefficients(
  const std::vector<Eigen::MatrixXd> & dF_d_states,
  const std::vector<Eigen::MatrixXd> & dF_d_inputs,
  const std::vector<Eigen::VectorXd> & states_prediction,
  const std::vector<Eigen::VectorXd> & d_inputs_schedule,
  const std::vector<Eigen::VectorXd> & states_ref, const std::vector<Eigen::VectorXd> & d_input_ref,
  const double prev_acc_rate, const double prev_steer_rate,
  const std::vector<Eigen::Vector2d> & inputs_ref, const double x_weight_coef,
  const double y_weight_coef, const double vel_weight_coef, const double yaw_weight_coef,
  const double acc_weight_coef, const double steer_weight_coef, const double acc_input_weight,
  const double steer_input_weight, const double acc_rate_weight_coef,
  const double steer_rate_weight_coef, const std::vector<Eigen::Vector2d> & inputs_schedule,
  std::vector<Eigen::MatrixXd> & K, std::vector<Eigen::VectorXd> & k)
{
  K = std::vector<Eigen::MatrixXd>(horizon_len_);
  k = std::vector<Eigen::VectorXd>(horizon_len_);
  Eigen::MatrixXd P = Eigen::MatrixXd::Zero(
    num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_,
    num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_);
  Eigen::VectorXd w = Eigen::VectorXd::Zero(
    num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_);
  double yaw_target = states_ref[horizon_len_][yaw_index_];
  Eigen::Matrix2d rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
  Eigen::Matrix2d xy_cost_matrix =
    rotation_matrix.transpose() *
    Eigen::DiagonalMatrix<double, 2>(
      Eigen::Vector2d(x_weight_coef * x_terminal_cost_, y_weight_coef * y_terminal_cost_)) *
    rotation_matrix;
  Eigen::Vector2d xy_diff = Eigen::Vector2d(
    states_prediction[horizon_len_][x_index_] - states_ref[horizon_len_][x_index_],
    states_prediction[horizon_len_][y_index_] - states_ref[horizon_len_][y_index_]);
  for (int j = 0; j < num_state_component_ilqr_; j++) {
    if (state_component_ilqr_[j] == "x") {
      P.block(2 * h_dim_ + j, 2 * h_dim_ + j, 2, 2) = xy_cost_matrix;
      w.segment(2 * h_dim_ + j, 2) = xy_cost_matrix * xy_diff;
    } else if (state_component_ilqr_[j] == "vel") {
      P(2 * h_dim_ + j, 2 * h_dim_ + j) = vel_weight_coef * vel_terminal_cost_;
      w(2 * h_dim_ + j) =
        vel_weight_coef * vel_terminal_cost_ *
        (states_prediction[horizon_len_][vel_index_] - states_ref[horizon_len_][vel_index_]);
    } else if (state_component_ilqr_[j] == "yaw") {
      P(2 * h_dim_ + j, 2 * h_dim_ + j) = yaw_weight_coef * yaw_terminal_cost_;
      w(2 * h_dim_ + j) =
        yaw_weight_coef * yaw_terminal_cost_ *
        (states_prediction[horizon_len_][yaw_index_] - states_ref[horizon_len_][yaw_index_]);
    } else if (state_component_ilqr_[j] == "acc") {
      P(2 * h_dim_ + j, 2 * h_dim_ + j) = acc_weight_coef * acc_terminal_cost_;
      w(2 * h_dim_ + j) =
        acc_weight_coef * acc_terminal_cost_ *
        (states_prediction[horizon_len_][acc_index_] - states_ref[horizon_len_][acc_index_]);
    } else if (state_component_ilqr_[j] == "steer") {
      P(2 * h_dim_ + j, 2 * h_dim_ + j) = steer_weight_coef * steer_terminal_cost_;
      w(2 * h_dim_ + j) =
        steer_weight_coef * steer_terminal_cost_ *
        (states_prediction[horizon_len_][steer_index_] - states_ref[horizon_len_][steer_index_]);
    }
  }

  P(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ - predict_step_,
    2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ - predict_step_) = acc_input_weight;
  P(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ + steer_queue_size_ - predict_step_,
    2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ + steer_queue_size_ - predict_step_) =
    steer_input_weight;
  w(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ - predict_step_) =
    acc_input_weight * (inputs_schedule[horizon_len_ - 1][0] - inputs_ref[horizon_len_ - 1][0]);
  w(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ + steer_queue_size_ - predict_step_) =
    steer_input_weight * (inputs_schedule[horizon_len_ - 1][1] - inputs_ref[horizon_len_ - 1][1]);

  for (int i = horizon_len_ - 1; i >= 0; i--) {
    Eigen::MatrixXd Pi1Ai =
      right_action_by_state_diff_with_history(P, extract_dF_d_state(dF_d_states[i]));

    Eigen::MatrixXd G = left_action_by_input_diff(extract_dF_d_input(dF_d_inputs[i]), Pi1Ai);

    Eigen::MatrixXd H = left_action_by_input_diff(
      extract_dF_d_input(dF_d_inputs[i]),
      right_action_by_input_diff(P, extract_dF_d_input(dF_d_inputs[i])));

    H(0, 0) += acc_rate_weight_coef * acc_rate_cost_;
    H(1, 1) += steer_rate_weight_coef * steer_rate_cost_;
    if (i == 0) {
      H(0, 0) += acc_rate_rate_cost_;
      H(1, 1) += steer_rate_rate_cost_;
    }
    Eigen::MatrixXd invH = H.inverse();

    Eigen::VectorXd g_ = left_action_by_input_diff(extract_dF_d_input(dF_d_inputs[i]), w);
    g_(0) += acc_rate_weight_coef * acc_rate_cost_ * (d_inputs_schedule[i][0] - d_input_ref[i][0]);
    g_(1) +=
      steer_rate_weight_coef * steer_rate_cost_ * (d_inputs_schedule[i][1] - d_input_ref[i][1]);
    if (i == 0) {
      g_(0) += acc_rate_rate_cost_ * (d_inputs_schedule[i][0] - prev_acc_rate);
      g_(1) += steer_rate_rate_cost_ * (d_inputs_schedule[i][1] - prev_steer_rate);
    }

    k[i] = invH * g_;
    K[i] = invH * G;
    P = left_action_by_state_diff_with_history(extract_dF_d_state(dF_d_states[i]), Pi1Ai) -
        G.transpose() * K[i];
    double x_cost = x_cost_;
    double y_cost = y_cost_;
    double vel_cost = vel_cost_;
    double yaw_cost = yaw_cost_;
    double acc_cost = acc_cost_;
    double steer_cost = steer_cost_;
    if (i == intermediate_cost_index_) {
      x_cost = x_intermediate_cost_;
      y_cost = y_intermediate_cost_;
      vel_cost = vel_intermediate_cost_;
      yaw_cost = yaw_intermediate_cost_;
      acc_cost = acc_intermediate_cost_;
      steer_cost = steer_intermediate_cost_;
    }
    w = left_action_by_state_diff_with_history(extract_dF_d_state(dF_d_states[i]), w);
    w -= G.transpose() * k[i];
    yaw_target = states_ref[i][yaw_index_];
    rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
    xy_cost_matrix = rotation_matrix.transpose() *
                     Eigen::DiagonalMatrix<double, 2>(
                       Eigen::Vector2d(x_weight_coef * x_cost, y_weight_coef * y_cost)) *
                     rotation_matrix;
    xy_diff = Eigen::Vector2d(
      states_prediction[i][x_index_] - states_ref[i][x_index_],
      states_prediction[i][y_index_] - states_ref[i][y_index_]);
    for (int j = 0; j < num_state_component_ilqr_; j++) {
      if (state_component_ilqr_[j] == "x") {
        P.block(2 * h_dim_ + j, 2 * h_dim_ + j, 2, 2) += xy_cost_matrix;
        w.segment(2 * h_dim_ + j, 2) += xy_cost_matrix * xy_diff;
      } else if (state_component_ilqr_[j] == "vel") {
        P(2 * h_dim_ + j, 2 * h_dim_ + j) += vel_weight_coef * vel_cost;
        w(2 * h_dim_ + j) += vel_weight_coef * vel_cost *
                             (states_prediction[i][vel_index_] - states_ref[i][vel_index_]);
      } else if (state_component_ilqr_[j] == "yaw") {
        P(2 * h_dim_ + j, 2 * h_dim_ + j) += yaw_weight_coef * yaw_cost;
        w(2 * h_dim_ + j) += yaw_weight_coef * yaw_cost *
                             (states_prediction[i][yaw_index_] - states_ref[i][yaw_index_]);
      } else if (state_component_ilqr_[j] == "acc") {
        P(2 * h_dim_ + j, 2 * h_dim_ + j) += acc_weight_coef * acc_cost;
        w(2 * h_dim_ + j) += acc_weight_coef * acc_cost *
                             (states_prediction[i][acc_index_] - states_ref[i][acc_index_]);
      } else if (state_component_ilqr_[j] == "steer") {
        P(2 * h_dim_ + j, 2 * h_dim_ + j) += steer_weight_coef * steer_cost;
        w(2 * h_dim_ + j) += steer_weight_coef * steer_cost *
                             (states_prediction[i][steer_index_] - states_ref[i][steer_index_]);
      }
    }
    if (i > 0) {
      P(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ - predict_step_,
        2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ - predict_step_) +=
        acc_input_weight;
      P(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ + steer_queue_size_ -
          predict_step_,
        2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ + steer_queue_size_ -
          predict_step_) += steer_input_weight;
      w(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ - predict_step_) +=
        acc_input_weight * (inputs_schedule[i - 1][0] - inputs_ref[i - 1][0]);
      w(2 * h_dim_ + num_state_component_ilqr_ + acc_queue_size_ + steer_queue_size_ -
        predict_step_) += steer_input_weight * (inputs_schedule[i - 1][1] - inputs_ref[i - 1][1]);
    }
  }
}
std::vector<Eigen::MatrixXd> AdaptorILQR::calc_line_search_candidates(
  const std::vector<Eigen::MatrixXd> & K, const std::vector<Eigen::VectorXd> & k,
  const std::vector<Eigen::MatrixXd> & dF_d_states,
  const std::vector<Eigen::MatrixXd> & dF_d_inputs,
  const std::vector<Eigen::VectorXd> & d_inputs_schedule, const Eigen::VectorXd & ls_points)
{
  std::vector<Eigen::MatrixXd> D_inputs_schedule(horizon_len_);
  Eigen::MatrixXd D_states = Eigen::MatrixXd::Zero(
    num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_, ls_points.size());
  for (int i = 0; i < horizon_len_; i++) {
    Eigen::MatrixXd D_D_inputs = -k[i] * ls_points.transpose() - K[i] * D_states;
    D_inputs_schedule[i] = D_D_inputs + d_inputs_schedule[i].replicate(1, ls_points.size());
    Eigen::MatrixXd D_states_temp = Eigen::MatrixXd::Zero(
      num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_,
      ls_points.size());
    D_states_temp.topRows(num_state_component_ilqr_ + 2 * h_dim_) +=
      extract_dF_d_state(dF_d_states[i]) * D_states +
      extract_dF_d_input(dF_d_inputs[i]) * D_D_inputs;
    D_states_temp.middleRows(
      num_state_component_ilqr_ + 2 * h_dim_, acc_queue_size_ - predict_step_) +=
      D_states.middleRows(
        num_state_component_ilqr_ + 2 * h_dim_ + predict_step_, acc_queue_size_ - predict_step_);

    D_states_temp.middleRows(
      num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_,
      steer_queue_size_ - predict_step_) +=
      D_states.middleRows(
        num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + predict_step_,
        steer_queue_size_ - predict_step_);

    for (int j = 0; j < predict_step_; j++) {
      D_states_temp.row(
        num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - predict_step_ + j) +=
        D_states.row(num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - 1);

      D_states_temp.row(
        num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_ -
        predict_step_ + j) +=
        D_states.row(
          num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_ - 1);

      D_states_temp.row(
        num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ - predict_step_ + j) +=
        (j + 1) * control_dt_ * D_D_inputs.row(0);

      D_states_temp.row(
        num_state_component_ilqr_ + 2 * h_dim_ + acc_queue_size_ + steer_queue_size_ -
        predict_step_ + j) += (j + 1) * control_dt_ * D_D_inputs.row(1);
    }
    D_states = D_states_temp;
  }
  return D_inputs_schedule;
}
void AdaptorILQR::compute_optimal_control(
  const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
  const Eigen::VectorXd & steer_input_history, std::vector<Eigen::VectorXd> & d_inputs_schedule,
  const Eigen::VectorXd & h_lstm, const Eigen::VectorXd & c_lstm,
  const std::vector<Eigen::VectorXd> & states_ref, const std::vector<Eigen::VectorXd> & d_input_ref,
  double acc_input_change_rate, double steer_input_change_rate, Eigen::VectorXd & previous_error,
  std::vector<Eigen::VectorXd> & states_prediction,
  const Eigen::VectorXd & acc_controller_input_schedule,
  const Eigen::VectorXd & steer_controller_input_schedule, double & acc_input, double & steer_input)
{
  std::vector<Eigen::MatrixXd> dF_d_states, dF_d_inputs;
  std::vector<Eigen::Vector2d> inputs_schedule;
  trained_dynamics_.calc_forward_trajectory_with_diff(
    states, acc_input_history, steer_input_history, d_inputs_schedule, h_lstm, c_lstm,
    previous_error, states_prediction, dF_d_states, dF_d_inputs, inputs_schedule);

  std::vector<Eigen::Vector2d> inputs_ref;
  double x_weight_coef, y_weight_coef, vel_weight_coef, yaw_weight_coef, acc_weight_coef,
    steer_weight_coef;
  double acc_input_weight, steer_input_weight;
  double acc_rate_weight_coef, steer_rate_weight_coef;
  double initial_prediction_x_weight_coef, initial_prediction_y_weight_coef,
    initial_prediction_vel_weight_coef, initial_prediction_yaw_weight_coef,
    initial_prediction_acc_weight_coef, initial_prediction_steer_weight_coef;

  calc_inputs_ref_info(
    states, acc_input_history, steer_input_history, h_lstm, c_lstm, previous_error, states_ref,
    acc_controller_input_schedule, steer_controller_input_schedule, states_prediction, inputs_ref,
    acc_input_change_rate, steer_input_change_rate, x_weight_coef, y_weight_coef, vel_weight_coef,
    yaw_weight_coef, acc_weight_coef, steer_weight_coef, acc_input_weight, steer_input_weight,
    acc_rate_weight_coef, steer_rate_weight_coef, initial_prediction_x_weight_coef,
    initial_prediction_y_weight_coef, initial_prediction_vel_weight_coef,
    initial_prediction_yaw_weight_coef, initial_prediction_acc_weight_coef,
    initial_prediction_steer_weight_coef);

  std::vector<Eigen::MatrixXd> K;
  std::vector<Eigen::VectorXd> k;
  double prev_acc_rate =
    (acc_input_history[acc_queue_size_ - 1] - acc_input_history[acc_queue_size_ - 2]) / control_dt_;
  double prev_steer_rate =
    (steer_input_history[steer_queue_size_ - 1] - steer_input_history[steer_queue_size_ - 2]) /
    control_dt_;
  compute_ilqr_coefficients(
    dF_d_states, dF_d_inputs, states_prediction, d_inputs_schedule, states_ref, d_input_ref,
    prev_acc_rate, prev_steer_rate, inputs_ref,
    std::min(x_weight_coef, initial_prediction_x_weight_coef),
    std::min(y_weight_coef, initial_prediction_y_weight_coef),
    std::min(vel_weight_coef, initial_prediction_vel_weight_coef),
    std::min(yaw_weight_coef, initial_prediction_yaw_weight_coef),
    std::min(acc_weight_coef, initial_prediction_acc_weight_coef),
    std::min(steer_weight_coef, initial_prediction_steer_weight_coef), acc_input_weight,
    steer_input_weight, acc_rate_weight_coef, steer_rate_weight_coef, inputs_schedule, K, k);
  Eigen::VectorXd ls_points = Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);

  std::vector<Eigen::MatrixXd> D_inputs_schedule =
    calc_line_search_candidates(K, k, dF_d_states, dF_d_inputs, d_inputs_schedule, ls_points);
  Eigen::VectorXd Cost;
  std::vector<Eigen::MatrixXd> States_prediction;
  calc_forward_trajectory_with_cost(
    states, acc_input_history, steer_input_history, D_inputs_schedule, h_lstm, c_lstm,
    previous_error, States_prediction, states_ref, d_input_ref, inputs_ref, x_weight_coef,
    y_weight_coef, vel_weight_coef, yaw_weight_coef, acc_weight_coef, steer_weight_coef,
    acc_input_weight, steer_input_weight, acc_rate_weight_coef, steer_rate_weight_coef, Cost);
  int min_index;
  Cost.minCoeff(&min_index);

  for (int i = 0; i < horizon_len_; i++) {
    d_inputs_schedule[i] = D_inputs_schedule[i].col(min_index);
    states_prediction[i + 1] = States_prediction[i + 1].col(min_index);
  }
  Eigen::VectorXd acc_input_history_copy = acc_input_history;
  Eigen::VectorXd steer_input_history_copy = steer_input_history;
  Eigen::VectorXd h_lstm_copy = h_lstm;
  Eigen::VectorXd c_lstm_copy = c_lstm;

  // update previous error
  trained_dynamics_.F_with_model(
    states, acc_input_history_copy, steer_input_history_copy, d_inputs_schedule[0], h_lstm_copy,
    c_lstm_copy, previous_error, 0);

  // get result
  acc_input = acc_input_history[acc_queue_size_ - 1] + d_inputs_schedule[0][0] * control_dt_;
  steer_input = steer_input_history[steer_queue_size_ - 1] + d_inputs_schedule[0][1] * control_dt_;
}

///////////////// VehicleAdaptor ///////////////////////

VehicleAdaptor::VehicleAdaptor()
{
  set_params();
}
VehicleAdaptor::~VehicleAdaptor()
{
}
void VehicleAdaptor::set_params()
{
  adaptor_ilqr_.set_params();
  std::string param_dir_path = get_param_dir_path();
  YAML::Node nominal_param_node = YAML::LoadFile(param_dir_path + "/nominal_param.yaml");
  wheel_base_ = nominal_param_node["nominal_parameter"]["vehicle_info"]["wheel_base"].as<double>();
  acc_time_delay_ =
    nominal_param_node["nominal_parameter"]["acceleration"]["acc_time_delay"].as<double>();
  acc_time_constant_ =
    nominal_param_node["nominal_parameter"]["acceleration"]["acc_time_constant"].as<double>();
  steer_time_delay_ =
    nominal_param_node["nominal_parameter"]["steering"]["steer_time_delay"].as<double>();
  steer_time_constant_ =
    nominal_param_node["nominal_parameter"]["steering"]["steer_time_constant"].as<double>();

  YAML::Node optimization_param_node = YAML::LoadFile(param_dir_path + "/optimization_param.yaml");
  steer_dead_band_ =
    optimization_param_node["optimization_parameter"]["steering"]["steer_dead_band"].as<double>();
  x_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["x_cost"].as<double>();
  y_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["y_cost"].as<double>();
  vel_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["vel_cost"].as<double>();
  yaw_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["yaw_cost"].as<double>();
  acc_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["acc_cost"].as<double>();
  steer_cost_ = optimization_param_node["optimization_parameter"]["weight_parameter"]["steer_cost"]
                  .as<double>();

  acc_rate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["acc_rate_cost"]
      .as<double>();
  steer_rate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["steer_rate_cost"]
      .as<double>();
  steer_rate_cost_ /= (wheel_base_ * wheel_base_);

  double acc_rate_rate_cost =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["acc_rate_rate_cost"]
      .as<double>();
  double steer_rate_rate_cost =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["steer_rate_rate_cost"]
      .as<double>();
  acc_rate_rate_cost /= (wheel_base_ * wheel_base_);

  x_terminal_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["x_terminal_cost"]
      .as<double>();
  y_terminal_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["y_terminal_cost"]
      .as<double>();
  vel_terminal_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["vel_terminal_cost"]
      .as<double>();
  yaw_terminal_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["yaw_terminal_cost"]
      .as<double>();
  acc_terminal_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["acc_terminal_cost"]
      .as<double>();
  steer_terminal_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["steer_terminal_cost"]
      .as<double>();

  past_acc_input_change_decay_rate_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]
                           ["past_acc_input_change_decay_rate"]
                             .as<double>();
  past_steer_input_change_decay_rate_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]
                           ["past_steer_input_change_decay_rate"]
                             .as<double>();
  acc_input_change_window_size_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]
                           ["acc_input_change_window_size"]
                             .as<int>();
  steer_input_change_window_size_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]
                           ["steer_input_change_window_size"]
                             .as<int>();
  future_acc_input_change_decay_rate_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]
                           ["future_acc_input_change_decay_rate"]
                             .as<double>();
  future_steer_input_change_decay_rate_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]
                           ["future_steer_input_change_decay_rate"]
                             .as<double>();

  x_intermediate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["x_intermediate_cost"]
      .as<double>();
  y_intermediate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["y_intermediate_cost"]
      .as<double>();
  vel_intermediate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["vel_intermediate_cost"]
      .as<double>();
  yaw_intermediate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["yaw_intermediate_cost"]
      .as<double>();
  acc_intermediate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["acc_intermediate_cost"]
      .as<double>();
  steer_intermediate_cost_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]["steer_intermediate_cost"]
      .as<double>();
  intermediate_cost_index_ =
    optimization_param_node["optimization_parameter"]["weight_parameter"]
                           ["intermediate_cost_index_predict_by_polynomial_regression"]
                             .as<int>();

  control_dt_ =
    optimization_param_node["optimization_parameter"]["setting"]["control_dt"].as<double>();
  predict_step_ =
    optimization_param_node["optimization_parameter"]["setting"]["predict_step"].as<int>();
  predict_dt_ = control_dt_ * predict_step_;
  horizon_len_ = optimization_param_node["optimization_parameter"]["setting"]
                                        ["horizon_len_predict_by_polynomial_regression"]
                                          .as<int>();

  controller_acc_input_history_len_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["controller_acc_input_history_len"]
                             .as<int>();
  controller_steer_input_history_len_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["controller_steer_input_history_len"]
                             .as<int>();
  deg_controller_acc_input_history_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["deg_controller_acc_input_history"]
                             .as<int>();
  deg_controller_steer_input_history_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["deg_controller_steer_input_history"]
                             .as<int>();
  lam_controller_acc_input_history_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["lam_controller_acc_input_history"]
                             .as<std::vector<double>>();
  lam_controller_steer_input_history_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["lam_controller_steer_input_history"]
                             .as<std::vector<double>>();
  oldest_sample_weight_controller_acc_input_history_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["oldest_sample_weight_controller_acc_input_history"]
                             .as<double>();
  oldest_sample_weight_controller_steer_input_history_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["oldest_sample_weight_controller_steer_input_history"]
                             .as<double>();
  acc_polynomial_prediction_len_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["acc_polynomial_prediction_len"]
                             .as<int>();
  steer_polynomial_prediction_len_ =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["steer_polynomial_prediction_len"]
                             .as<int>();

  acc_linear_extrapolation_len_ =
    optimization_param_node["optimization_parameter"]["linear_extrapolation"]
                           ["acc_linear_extrapolation_len"]
                             .as<int>();
  steer_linear_extrapolation_len_ =
    optimization_param_node["optimization_parameter"]["linear_extrapolation"]
                           ["steer_linear_extrapolation_len"]
                             .as<int>();
  past_len_for_acc_linear_extrapolation_ =
    optimization_param_node["optimization_parameter"]["linear_extrapolation"]
                           ["past_len_for_acc_linear_extrapolation"]
                             .as<int>();
  past_len_for_steer_linear_extrapolation_ =
    optimization_param_node["optimization_parameter"]["linear_extrapolation"]
                           ["past_len_for_steer_linear_extrapolation"]
                             .as<int>();

  use_sg_for_d_inputs_schedule_ =
    optimization_param_node["optimization_parameter"]["sg_filter"]["use_sg_for_d_inputs_schedule"]
      .as<bool>();
  sg_window_size_for_d_inputs_schedule_ =
    optimization_param_node["optimization_parameter"]["sg_filter"]
                           ["sg_window_size_for_d_inputs_schedule"]
                             .as<int>();
  sg_deg_for_d_inputs_schedule_ =
    optimization_param_node["optimization_parameter"]["sg_filter"]["sg_deg_for_d_inputs_schedule"]
      .as<int>();
  sg_window_size_for_NN_diff_ =
    optimization_param_node["optimization_parameter"]["sg_filter"]["sg_window_size_for_NN_diff"]
      .as<int>();
  sg_deg_for_NN_diff_ =
    optimization_param_node["optimization_parameter"]["sg_filter"]["sg_deg_for_NN_diff"].as<int>();

  YAML::Node trained_model_param_node =
    YAML::LoadFile(param_dir_path + "/trained_model_param.yaml");
  acc_queue_size_ =
    trained_model_param_node["trained_model_parameter"]["queue_size"]["acc_queue_size"].as<int>();
  steer_queue_size_ =
    trained_model_param_node["trained_model_parameter"]["queue_size"]["steer_queue_size"].as<int>();
  update_lstm_len_ =
    trained_model_param_node["trained_model_parameter"]["lstm"]["update_lstm_len"].as<int>();

  acc_delay_step_ = std::min(int(std::round(acc_time_delay_ / control_dt_)), acc_queue_size_);
  steer_delay_step_ = std::min(int(std::round(steer_time_delay_ / control_dt_)), steer_queue_size_);

  YAML::Node controller_param_node = YAML::LoadFile(param_dir_path + "/controller_param.yaml");

  double acc_time_delay_controller =
    controller_param_node["controller_parameter"]["acceleration"]["acc_time_delay"].as<double>();
  double acc_time_constant_controller =
    controller_param_node["controller_parameter"]["acceleration"]["acc_time_constant"].as<double>();
  double steer_time_delay_controller =
    controller_param_node["controller_parameter"]["steering"]["steer_time_delay"].as<double>();
  double steer_time_constant_controller =
    controller_param_node["controller_parameter"]["steering"]["steer_time_constant"].as<double>();
  acc_delay_step_controller_ =
    std::min(int(std::round(acc_time_delay_controller / control_dt_)), acc_queue_size_);
  steer_delay_step_controller_ =
    std::min(int(std::round(steer_time_delay_controller / control_dt_)), steer_queue_size_);

  nominal_dynamics_controller_.set_params(
    wheel_base_, acc_time_delay_controller, steer_time_delay_controller,
    acc_time_constant_controller, steer_time_constant_controller, acc_queue_size_,
    steer_queue_size_, control_dt_, predict_step_);
  nominal_dynamics_controller_.set_steer_dead_band(steer_dead_band_);

  mix_ratio_vel_target_table_ =
    optimization_param_node["optimization_parameter"]["mix_ratio"]["mix_ratio_vel_target_table"]
      .as<std::vector<double>>();
  mix_ratio_vel_domain_table_ =
    optimization_param_node["optimization_parameter"]["mix_ratio"]["mix_ratio_vel_domain_table"]
      .as<std::vector<double>>();
  mix_ratio_time_target_table_ =
    optimization_param_node["optimization_parameter"]["mix_ratio"]["mix_ratio_time_target_table"]
      .as<std::vector<double>>();
  mix_ratio_time_domain_table_ =
    optimization_param_node["optimization_parameter"]["mix_ratio"]["mix_ratio_time_domain_table"]
      .as<std::vector<double>>();

  adaptor_ilqr_.set_vehicle_params(
    wheel_base_, acc_time_delay_, steer_time_delay_, acc_time_constant_, steer_time_constant_,
    acc_queue_size_, steer_queue_size_, control_dt_, predict_step_);

  polynomial_reg_for_predict_acc_input_.set_params(
    deg_controller_acc_input_history_, controller_acc_input_history_len_,
    lam_controller_acc_input_history_);
  polynomial_reg_for_predict_acc_input_.set_oldest_sample_weight(
    oldest_sample_weight_controller_acc_input_history_);
  bool ignore_intercept_acc_prediction =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["ignore_intercept_acc_prediction"]
                             .as<bool>();
  if (ignore_intercept_acc_prediction) {
    polynomial_reg_for_predict_acc_input_.set_ignore_intercept();
  }
  polynomial_reg_for_predict_acc_input_.calc_coef_matrix();
  polynomial_reg_for_predict_acc_input_.calc_prediction_matrix(predict_step_ * horizon_len_ - 1);
  polynomial_reg_for_predict_steer_input_.set_params(
    deg_controller_steer_input_history_, controller_steer_input_history_len_,
    lam_controller_steer_input_history_);
  polynomial_reg_for_predict_steer_input_.set_oldest_sample_weight(
    oldest_sample_weight_controller_steer_input_history_);
  bool ignore_intercept_steer_prediction =
    optimization_param_node["optimization_parameter"]["polynomial_regression"]
                           ["ignore_intercept_steer_prediction"]
                             .as<bool>();
  if (ignore_intercept_steer_prediction) {
    polynomial_reg_for_predict_steer_input_.set_ignore_intercept();
  }
  polynomial_reg_for_predict_steer_input_.calc_coef_matrix();
  polynomial_reg_for_predict_steer_input_.calc_prediction_matrix(predict_step_ * horizon_len_ - 1);

  adaptor_ilqr_.set_states_cost(x_cost_, y_cost_, vel_cost_, yaw_cost_, acc_cost_, steer_cost_);
  adaptor_ilqr_.set_inputs_cost(acc_rate_cost_, steer_rate_cost_);
  adaptor_ilqr_.set_terminal_cost(
    x_terminal_cost_, y_terminal_cost_, vel_terminal_cost_, yaw_terminal_cost_, acc_terminal_cost_,
    steer_terminal_cost_);
  adaptor_ilqr_.set_intermediate_cost(
    x_intermediate_cost_, y_intermediate_cost_, vel_intermediate_cost_, yaw_intermediate_cost_,
    acc_intermediate_cost_, steer_intermediate_cost_, intermediate_cost_index_);
  adaptor_ilqr_.set_rate_cost(acc_rate_rate_cost, steer_rate_rate_cost);

  reflect_controller_d_input_ratio_ =
    optimization_param_node["optimization_parameter"]["ilqr"]["reflect_controller_d_input_ratio"]
      .as<double>();
  use_controller_inputs_as_target_ =
    optimization_param_node["optimization_parameter"]["ilqr"]["use_controller_inputs_as_target"]
      .as<bool>();
  input_filter_mode_ =
    optimization_param_node["optimization_parameter"]["input_filter"]["input_filter_mode"]
      .as<std::string>();
  if (input_filter_mode_ == "butterworth") {
    butterworth_filter_.set_params();
  }
  use_controller_steer_input_schedule_ =
    optimization_param_node["optimization_parameter"]["autoware_alignment"]
                           ["use_controller_steer_input_schedule"]
                             .as<bool>();
  use_vehicle_adaptor_ =
    optimization_param_node["optimization_parameter"]["autoware_alignment"]["use_vehicle_adaptor"]
      .as<bool>();
  use_offline_features_autoware_ =
    optimization_param_node["optimization_parameter"]["autoware_alignment"]["use_offline_features"]
      .as<bool>();

  acc_input_schedule_prediction_mode_ =
    optimization_param_node["optimization_parameter"]["inputs_schedule_prediction_mode"]
                           ["acc_input_schedule_prediction_mode"]
                             .as<std::string>();
  steer_input_schedule_prediction_mode_ =
    optimization_param_node["optimization_parameter"]["inputs_schedule_prediction_mode"]
                           ["steer_input_schedule_prediction_mode"]
                             .as<std::string>();

  acc_input_schedule_prediction_len_ =
    optimization_param_node["optimization_parameter"]["inputs_schedule_prediction_NN"]
                           ["acc_input_schedule_prediction_len"]
                             .as<int>();
  steer_input_schedule_prediction_len_ =
    optimization_param_node["optimization_parameter"]["inputs_schedule_prediction_NN"]
                           ["steer_input_schedule_prediction_len"]
                             .as<int>();
  acc_input_schedule_prediction_len_ =
    std::min(acc_input_schedule_prediction_len_, horizon_len_ * predict_step_ - 1);
  steer_input_schedule_prediction_len_ =
    std::min(steer_input_schedule_prediction_len_, horizon_len_ * predict_step_ - 1);
  int controller_acc_input_history_len =
    optimization_param_node["optimization_parameter"]["inputs_schedule_prediction_NN"]
                           ["controller_acc_input_history_len"]
                             .as<int>();
  int controller_steer_input_history_len =
    optimization_param_node["optimization_parameter"]["inputs_schedule_prediction_NN"]
                           ["controller_steer_input_history_len"]
                             .as<int>();
  int adaptive_scale_index =
    optimization_param_node["optimization_parameter"]["inputs_schedule_prediction_NN"]
                           ["adaptive_scale_index"]
                             .as<int>();
  if (acc_input_schedule_prediction_mode_ == "NN" && !acc_input_schedule_prediction_initialized_) {
    acc_input_schedule_prediction_.set_params(
      controller_acc_input_history_len, acc_input_schedule_prediction_len_, control_dt_,
      "inputs_schedule_prediction_model/acc_schedule_predictor", adaptive_scale_index);
    acc_input_schedule_prediction_initialized_ = true;
  }
  if (
    steer_input_schedule_prediction_mode_ == "NN" &&
    !steer_input_schedule_prediction_initialized_) {
    steer_input_schedule_prediction_.set_params(
      controller_steer_input_history_len, steer_input_schedule_prediction_len_, control_dt_,
      "inputs_schedule_prediction_model/steer_schedule_predictor", adaptive_scale_index);
    steer_input_schedule_prediction_initialized_ = true;
  }
  double lambda_smooth_acc_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["lambda_smooth_acc_ref_smoother"]
                             .as<double>();
  double terminal_lambda_smooth_acc_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["terminal_lambda_smooth_acc_ref_smoother"]
                             .as<double>();

  double lambda_decay_acc_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["lambda_decay_acc_ref_smoother"]
                             .as<double>();
  double lambda_terminal_decay_acc_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["lambda_terminal_decay_acc_ref_smoother"]
                             .as<double>();
  bool use_acc_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]["use_acc_ref_smoother"]
      .as<bool>();

  acc_input_ref_smoother_.set_params(
    control_dt_, lambda_smooth_acc_ref_smoother, terminal_lambda_smooth_acc_ref_smoother,
    lambda_decay_acc_ref_smoother, lambda_terminal_decay_acc_ref_smoother);
  acc_input_ref_smoother_.set_use_smoother(use_acc_ref_smoother);

  double lambda_smooth_steer_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["lambda_smooth_steer_ref_smoother"]
                             .as<double>();
  double terminal_lambda_smooth_steer_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["terminal_lambda_smooth_steer_ref_smoother"]
                             .as<double>();

  double lambda_decay_steer_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["lambda_decay_steer_ref_smoother"]
                             .as<double>();
  double lambda_terminal_decay_steer_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["lambda_terminal_decay_steer_ref_smoother"]
                             .as<double>();
  bool use_steer_ref_smoother =
    optimization_param_node["optimization_parameter"]["inputs_ref_smoother"]
                           ["use_steer_ref_smoother"]
                             .as<bool>();
  steer_input_ref_smoother_.set_params(
    control_dt_, lambda_smooth_steer_ref_smoother, terminal_lambda_smooth_steer_ref_smoother,
    lambda_decay_steer_ref_smoother, lambda_terminal_decay_steer_ref_smoother);
  steer_input_ref_smoother_.set_use_smoother(use_steer_ref_smoother);

  if (acc_input_schedule_prediction_mode_ == "NN") {
    acc_input_ref_smoother_.set_prediction_len(acc_input_schedule_prediction_len_);
  } else if (acc_input_schedule_prediction_mode_ == "linear_extrapolation") {
    acc_input_ref_smoother_.set_prediction_len(acc_linear_extrapolation_len_);
  } else {
    acc_input_ref_smoother_.set_prediction_len(acc_polynomial_prediction_len_);
  }
  if (steer_input_schedule_prediction_mode_ == "NN") {
    steer_input_ref_smoother_.set_prediction_len(steer_input_schedule_prediction_len_);
  } else if (steer_input_schedule_prediction_mode_ == "linear_extrapolation") {
    steer_input_ref_smoother_.set_prediction_len(steer_linear_extrapolation_len_);
  } else {
    steer_input_ref_smoother_.set_prediction_len(steer_polynomial_prediction_len_);
  }

  srand(static_cast<unsigned>(time(0)));
}
void VehicleAdaptor::set_NN_params(
  const Eigen::MatrixXd & weight_acc_encoder_layer_1,
  const Eigen::MatrixXd & weight_steer_encoder_layer_1,
  const Eigen::MatrixXd & weight_acc_encoder_layer_2,
  const Eigen::MatrixXd & weight_steer_encoder_layer_2, const Eigen::MatrixXd & weight_acc_layer_1,
  const Eigen::MatrixXd & weight_steer_layer_1, const Eigen::MatrixXd & weight_acc_layer_2,
  const Eigen::MatrixXd & weight_steer_layer_2,
  const std::vector<Eigen::MatrixXd> & weight_lstm_encoder_ih,
  const std::vector<Eigen::MatrixXd> & weight_lstm_encoder_hh,
  const Eigen::MatrixXd & weight_lstm_ih, const Eigen::MatrixXd & weight_lstm_hh,
  const Eigen::MatrixXd & weight_complimentary_layer, const Eigen::MatrixXd & weight_linear_relu,
  const Eigen::MatrixXd & weight_final_layer, const Eigen::VectorXd & bias_acc_encoder_layer_1,
  const Eigen::VectorXd & bias_steer_encoder_layer_1,
  const Eigen::VectorXd & bias_acc_encoder_layer_2,
  const Eigen::VectorXd & bias_steer_encoder_layer_2, const Eigen::VectorXd & bias_acc_layer_1,
  const Eigen::VectorXd & bias_steer_layer_1, const Eigen::VectorXd & bias_acc_layer_2,
  const Eigen::VectorXd & bias_steer_layer_2,
  const std::vector<Eigen::VectorXd> & bias_lstm_encoder_ih,
  const std::vector<Eigen::VectorXd> & bias_lstm_encoder_hh, const Eigen::VectorXd & bias_lstm_ih,
  const Eigen::VectorXd & bias_lstm_hh, const Eigen::VectorXd & bias_complimentary_layer,
  const Eigen::VectorXd & bias_linear_relu, const Eigen::VectorXd & bias_final_layer,
  const double vel_scaling, const double vel_bias,
  const std::vector<std::string> state_component_predicted)
{
  adaptor_ilqr_.set_NN_params(
    weight_acc_encoder_layer_1, weight_steer_encoder_layer_1, weight_acc_encoder_layer_2,
    weight_steer_encoder_layer_2, weight_acc_layer_1, weight_steer_layer_1, weight_acc_layer_2,
    weight_steer_layer_2, weight_lstm_encoder_ih, weight_lstm_encoder_hh, weight_lstm_ih,
    weight_lstm_hh, weight_complimentary_layer, weight_linear_relu, weight_final_layer,
    bias_acc_encoder_layer_1, bias_steer_encoder_layer_1, bias_acc_encoder_layer_2,
    bias_steer_encoder_layer_2, bias_acc_layer_1, bias_steer_layer_1, bias_acc_layer_2,
    bias_steer_layer_2, bias_lstm_encoder_ih, bias_lstm_encoder_hh, bias_lstm_ih, bias_lstm_hh,
    bias_complimentary_layer, bias_linear_relu, bias_final_layer, vel_scaling, vel_bias,
    state_component_predicted);
  h_dim_full_ = weight_lstm_encoder_hh[0].cols();
  h_dim_ = weight_lstm_hh.cols();
  num_layers_encoder_ = weight_lstm_encoder_ih.size();
  NN_prediction_target_dim_ = state_component_predicted.size();
}
void VehicleAdaptor::set_offline_features(const Eigen::VectorXd & offline_features)
{
  offline_features_ = offline_features;
  use_offline_features_ = true;
}
void VehicleAdaptor::set_offline_features_from_csv(std::string csv_dir)
{
  std::cout << "csv_dir: " << csv_dir << std::endl;
  Eigen::VectorXd offline_features = read_csv(csv_dir + "/offline_features.csv").col(0);
  set_offline_features(offline_features);
}
void VehicleAdaptor::set_NN_params_from_csv(std::string csv_dir)
{
  double vel_scaling, vel_bias;
  std::cout << "csv_dir: " << csv_dir << std::endl;
  Eigen::MatrixXd model_info = read_csv(csv_dir + "/model_info.csv");
  num_layers_encoder_ = model_info(0, 0);
  Eigen::MatrixXd weight_acc_encoder_layer_1 =
    read_csv(csv_dir + "/weight_acc_encoder_layer_1.csv");
  Eigen::MatrixXd weight_steer_encoder_layer_1 =
    read_csv(csv_dir + "/weight_steer_encoder_layer_1.csv");
  Eigen::MatrixXd weight_acc_encoder_layer_2 =
    read_csv(csv_dir + "/weight_acc_encoder_layer_2.csv");
  Eigen::MatrixXd weight_steer_encoder_layer_2 =
    read_csv(csv_dir + "/weight_steer_encoder_layer_2.csv");

  Eigen::MatrixXd weight_acc_layer_1 = read_csv(csv_dir + "/weight_acc_layer_1.csv");
  Eigen::MatrixXd weight_steer_layer_1 = read_csv(csv_dir + "/weight_steer_layer_1.csv");
  Eigen::MatrixXd weight_acc_layer_2 = read_csv(csv_dir + "/weight_acc_layer_2.csv");
  Eigen::MatrixXd weight_steer_layer_2 = read_csv(csv_dir + "/weight_steer_layer_2.csv");

  Eigen::MatrixXd weight_lstm_ih = read_csv(csv_dir + "/weight_lstm_ih.csv");
  Eigen::MatrixXd weight_lstm_hh = read_csv(csv_dir + "/weight_lstm_hh.csv");
  Eigen::MatrixXd weight_complimentary_layer =
    read_csv(csv_dir + "/weight_complimentary_layer.csv");
  Eigen::MatrixXd weight_linear_relu = read_csv(csv_dir + "/weight_linear_relu.csv");
  Eigen::MatrixXd weight_final_layer = read_csv(csv_dir + "/weight_final_layer.csv");

  Eigen::VectorXd bias_acc_encoder_layer_1 =
    read_csv(csv_dir + "/bias_acc_encoder_layer_1.csv").col(0);
  Eigen::VectorXd bias_steer_encoder_layer_1 =
    read_csv(csv_dir + "/bias_steer_encoder_layer_1.csv").col(0);
  Eigen::VectorXd bias_acc_encoder_layer_2 =
    read_csv(csv_dir + "/bias_acc_encoder_layer_2.csv").col(0);
  Eigen::VectorXd bias_steer_encoder_layer_2 =
    read_csv(csv_dir + "/bias_steer_encoder_layer_2.csv").col(0);

  Eigen::VectorXd bias_acc_layer_1 = read_csv(csv_dir + "/bias_acc_layer_1.csv").col(0);
  Eigen::VectorXd bias_steer_layer_1 = read_csv(csv_dir + "/bias_steer_layer_1.csv").col(0);
  Eigen::VectorXd bias_acc_layer_2 = read_csv(csv_dir + "/bias_acc_layer_2.csv").col(0);
  Eigen::VectorXd bias_steer_layer_2 = read_csv(csv_dir + "/bias_steer_layer_2.csv").col(0);
  Eigen::VectorXd bias_lstm_ih = read_csv(csv_dir + "/bias_lstm_ih.csv").col(0);
  Eigen::VectorXd bias_lstm_hh = read_csv(csv_dir + "/bias_lstm_hh.csv").col(0);
  Eigen::VectorXd bias_complimentary_layer =
    read_csv(csv_dir + "/bias_complimentary_layer.csv").col(0);
  Eigen::VectorXd bias_linear_relu = read_csv(csv_dir + "/bias_linear_relu.csv").col(0);
  Eigen::VectorXd bias_final_layer = read_csv(csv_dir + "/bias_final_layer.csv").col(0);
  std::vector<Eigen::MatrixXd> weight_lstm_encoder_ih(num_layers_encoder_);
  std::vector<Eigen::MatrixXd> weight_lstm_encoder_hh(num_layers_encoder_);
  std::vector<Eigen::VectorXd> bias_lstm_encoder_ih(num_layers_encoder_);
  std::vector<Eigen::VectorXd> bias_lstm_encoder_hh(num_layers_encoder_);
  for (int i = 0; i < num_layers_encoder_; i++) {
    weight_lstm_encoder_ih[i] =
      read_csv(csv_dir + "/weight_lstm_encoder_ih_" + std::to_string(i) + ".csv");
    weight_lstm_encoder_hh[i] =
      read_csv(csv_dir + "/weight_lstm_encoder_hh_" + std::to_string(i) + ".csv");
    bias_lstm_encoder_ih[i] =
      read_csv(csv_dir + "/bias_lstm_encoder_ih_" + std::to_string(i) + ".csv").col(0);
    bias_lstm_encoder_hh[i] =
      read_csv(csv_dir + "/bias_lstm_encoder_hh_" + std::to_string(i) + ".csv").col(0);
  }
  Eigen::MatrixXd vel_params = read_csv(csv_dir + "/vel_scale.csv");

  vel_scaling = vel_params(0, 0);
  vel_bias = vel_params(1, 0);
  std::vector<std::string> state_component_predicted =
    read_string_csv(csv_dir + "/state_component_predicted.csv");
  set_NN_params(
    weight_acc_encoder_layer_1, weight_steer_encoder_layer_1, weight_acc_encoder_layer_2,
    weight_steer_encoder_layer_2, weight_acc_layer_1, weight_steer_layer_1, weight_acc_layer_2,
    weight_steer_layer_2, weight_lstm_encoder_ih, weight_lstm_encoder_hh, weight_lstm_ih,
    weight_lstm_hh, weight_complimentary_layer, weight_linear_relu, weight_final_layer,
    bias_acc_encoder_layer_1, bias_steer_encoder_layer_1, bias_acc_encoder_layer_2,
    bias_steer_encoder_layer_2, bias_acc_layer_1, bias_steer_layer_1, bias_acc_layer_2,
    bias_steer_layer_2, bias_lstm_encoder_ih, bias_lstm_encoder_hh, bias_lstm_ih, bias_lstm_hh,
    bias_complimentary_layer, bias_linear_relu, bias_final_layer, vel_scaling, vel_bias,
    state_component_predicted);
}
void VehicleAdaptor::clear_NN_params()
{
  adaptor_ilqr_.clear_NN_params();
}
void VehicleAdaptor::set_controller_d_inputs_schedule(
  const Eigen::VectorXd & acc_controller_d_inputs_schedule,
  const Eigen::VectorXd & steer_controller_d_inputs_schedule)
{  // called when controller inputs increments schedule are available
  acc_controller_d_inputs_schedule_ = acc_controller_d_inputs_schedule;
  steer_controller_d_inputs_schedule_ = steer_controller_d_inputs_schedule;
  if (!initialized_) {
    set_params();
    states_ref_mode_ = "controller_d_inputs_schedule";
    YAML::Node optimization_param_node =
      YAML::LoadFile(get_param_dir_path() + "/optimization_param.yaml");
    horizon_len_ = optimization_param_node["optimization_parameter"]["setting"]
                                          ["horizon_len_controller_d_inputs_schedule"]
                                            .as<int>();
    horizon_len_ = std::min(horizon_len_, int(acc_controller_d_inputs_schedule.size()));
    horizon_len_ = std::min(horizon_len_, int(steer_controller_d_inputs_schedule.size()));
    intermediate_cost_index_ =
      optimization_param_node["optimization_parameter"]["weight_parameter"]
                             ["intermediate_cost_index_controller_d_inputs_schedule"]
                               .as<int>();
    intermediate_cost_index_ = std::min(intermediate_cost_index_, horizon_len_ - 1);
    adaptor_ilqr_.set_intermediate_cost(
      x_intermediate_cost_, y_intermediate_cost_, vel_intermediate_cost_, yaw_intermediate_cost_,
      acc_intermediate_cost_, steer_intermediate_cost_, intermediate_cost_index_);
  }
}
void VehicleAdaptor::set_controller_d_steer_schedule(
  const Eigen::VectorXd & steer_controller_d_inputs_schedule)
{  // called when controller steer inputs increments schedule are available
  steer_controller_d_inputs_schedule_ = steer_controller_d_inputs_schedule;
  if (!initialized_) {
    set_params();
    states_ref_mode_ = "controller_d_steer_schedule";
  }
}
void VehicleAdaptor::set_controller_steer_input_schedule(
  double timestamp, const std::vector<double> & steer_controller_input_schedule)
{  // called when controller steer input schedule are available
  if (int(steer_controller_input_schedule.size()) < horizon_len_ + 1) {
    std::cerr << "steer_controller_input_schedule size is smaller than horizon_len" << std::endl;
    return;
  }
  Eigen::VectorXd steer_schedule(steer_controller_input_schedule.size());
  std::vector<double> timestamp_horizon(steer_controller_input_schedule.size());
  std::vector<double> timestamp_new(steer_controller_input_schedule.size() - 1);
  for (int i = 0; i < int(steer_controller_input_schedule.size()); i++) {
    steer_schedule[i] = steer_controller_input_schedule[i];
    timestamp_horizon[i] = i * predict_dt_;
    if (i < int(steer_controller_input_schedule.size()) - 1) {
      timestamp_new[i] = (i + 1) * predict_dt_ - control_dt_;
    }
  }
  Eigen::VectorXd steer_controller_input_schedule_interpolated =
    interpolate_eigen(steer_schedule, timestamp_horizon, timestamp_new);
  Eigen::VectorXd steer_controller_d_inputs_schedule(
    steer_controller_input_schedule_interpolated.size());
  for (int i = 0; i < int(steer_controller_input_schedule_interpolated.size()) - 1; i++) {
    steer_controller_d_inputs_schedule[i + 1] =
      (steer_controller_input_schedule_interpolated[i + 1] -
       steer_controller_input_schedule_interpolated[i]) /
      predict_dt_;
  }
  if (!initialized_) {
    steer_controller_d_inputs_schedule[0] =
      (steer_controller_input_schedule_interpolated[0] - steer_controller_input_schedule[0]) /
      (predict_dt_ - control_dt_);
  } else {
    double prev_timestamp = time_stamp_obs_[time_stamp_obs_.size() - 1];
    double prev_steer_controller_input =
      steer_controller_input_history_obs_[steer_controller_input_history_obs_.size() - 1];
    steer_controller_d_inputs_schedule[0] =
      (steer_controller_input_schedule_interpolated[0] - prev_steer_controller_input) /
      (predict_dt_ - control_dt_ + timestamp - prev_timestamp);
  }
  set_controller_d_steer_schedule(steer_controller_d_inputs_schedule);
}
void VehicleAdaptor::set_controller_prediction(
  const Eigen::VectorXd & x_controller_prediction, const Eigen::VectorXd & y_controller_prediction,
  const Eigen::VectorXd & vel_controller_prediction,
  const Eigen::VectorXd & yaw_controller_prediction,
  const Eigen::VectorXd & acc_controller_prediction,
  const Eigen::VectorXd & steer_controller_prediction)
{  // called when controller prediction trajectory are available
  x_controller_prediction_ = x_controller_prediction;
  y_controller_prediction_ = y_controller_prediction;
  vel_controller_prediction_ = vel_controller_prediction;
  yaw_controller_prediction_ = yaw_controller_prediction;
  acc_controller_prediction_ = acc_controller_prediction;
  steer_controller_prediction_ = steer_controller_prediction;
  if (!initialized_) {
    set_params();
    states_ref_mode_ = "controller_prediction";
    YAML::Node optimization_param_node =
      YAML::LoadFile(get_param_dir_path() + "/optimization_param.yaml");
    horizon_len_ = optimization_param_node["optimization_parameter"]["setting"]
                                          ["horizon_len_controller_prediction"]
                                            .as<int>();
    horizon_len_ = std::min(horizon_len_, int(acc_controller_prediction.size()) - 1);
    horizon_len_ = std::min(horizon_len_, int(steer_controller_prediction.size()) - 1);
    intermediate_cost_index_ =
      optimization_param_node["optimization_parameter"]["weight_parameter"]
                             ["intermediate_cost_index_controller_prediction"]
                               .as<int>();
    intermediate_cost_index_ = std::min(intermediate_cost_index_, horizon_len_ - 1);
    adaptor_ilqr_.set_intermediate_cost(
      x_intermediate_cost_, y_intermediate_cost_, vel_intermediate_cost_, yaw_intermediate_cost_,
      acc_intermediate_cost_, steer_intermediate_cost_, intermediate_cost_index_);
  }
}
void VehicleAdaptor::set_controller_steer_prediction(
  const Eigen::VectorXd & steer_controller_prediction)
{  // called when controller steer prediction trajectory are available
  steer_controller_prediction_ = steer_controller_prediction;
  steer_controller_prediction_aided_ = true;
}

Eigen::VectorXd VehicleAdaptor::get_adjusted_inputs(
  double time_stamp, const Eigen::VectorXd & states, const double acc_controller_input,
  const double steer_controller_input)
{
  int controller_acc_input_history_len = std::max(
    {controller_acc_input_history_len_, acc_delay_step_ + 1,
     past_len_for_acc_linear_extrapolation_ + 1});
  int controller_steer_input_history_len = std::max(
    {controller_steer_input_history_len_, steer_delay_step_ + 1,
     past_len_for_steer_linear_extrapolation_ + 1});
  if (!initialized_) {  // initialize
    start_time_ = time_stamp;
    if (states_ref_mode_ == "input_schedule_prediction") {
      set_params();
    }
    previous_error_ = Eigen::VectorXd::Zero(NN_prediction_target_dim_);

    adaptor_ilqr_.set_sg_filter_params(sg_deg_for_NN_diff_, sg_window_size_for_NN_diff_);
    adaptor_ilqr_.set_horizon_len(horizon_len_);

    max_queue_size_ = std::max(
      {acc_queue_size_ + 1, steer_queue_size_ + 1, controller_acc_input_history_len,
       controller_steer_input_history_len, predict_step_ * update_lstm_len_ + 2});
    time_stamp_obs_ = std::vector<double>(max_queue_size_);
    for (int i = 0; i < max_queue_size_; i++) {
      time_stamp_obs_[i] = time_stamp - (max_queue_size_ - i - 1) * control_dt_;
    }

    acc_input_history_ = acc_controller_input * Eigen::VectorXd::Ones(acc_queue_size_);
    steer_input_history_ = steer_controller_input * Eigen::VectorXd::Ones(steer_queue_size_);

    acc_controller_input_history_ =
      acc_controller_input * Eigen::VectorXd::Ones(controller_acc_input_history_len);
    steer_controller_input_history_ =
      steer_controller_input * Eigen::VectorXd::Ones(controller_steer_input_history_len);
    d_inputs_schedule_ = std::vector<Eigen::VectorXd>(horizon_len_, Eigen::VectorXd::Zero(2));

    state_history_lstm_ = std::vector<Eigen::VectorXd>(predict_step_ * update_lstm_len_, states);

    acc_input_history_lstm_ =
      std::vector<Eigen::VectorXd>(predict_step_ * update_lstm_len_ + 1, acc_input_history_);
    steer_input_history_lstm_ =
      std::vector<Eigen::VectorXd>(predict_step_ * update_lstm_len_ + 1, steer_input_history_);

    acc_input_history_obs_ = std::vector<double>(max_queue_size_ - 1, acc_controller_input);
    steer_input_history_obs_ = std::vector<double>(max_queue_size_ - 1, steer_controller_input);
    acc_controller_input_history_obs_ = std::vector<double>(max_queue_size_, acc_controller_input);
    steer_controller_input_history_obs_ =
      std::vector<double>(max_queue_size_, steer_controller_input);
    state_history_lstm_obs_ = std::vector<Eigen::VectorXd>(max_queue_size_ - 1, states);
    acc_input_history_lstm_obs_ =
      std::vector<Eigen::VectorXd>(max_queue_size_ - 1, acc_input_history_);
    steer_input_history_lstm_obs_ =
      std::vector<Eigen::VectorXd>(max_queue_size_ - 1, steer_input_history_);

    sg_window_size_for_d_inputs_schedule_ =
      std::min(sg_window_size_for_d_inputs_schedule_, int(horizon_len_ / 2) - 1);
    sg_filter_for_d_inputs_schedule_.set_params(
      sg_deg_for_d_inputs_schedule_, sg_window_size_for_d_inputs_schedule_);
    sg_filter_for_d_inputs_schedule_.calc_sg_filter_weight();

    initialized_ = true;
    past_acc_input_change_ = 0.0;
    past_steer_input_change_ = 0.0;
  }  // end of initialization
  else {  // already initialized case

    std::vector<double> time_stamp_acc_input_history(acc_queue_size_);
    for (int i = 0; i < acc_queue_size_; i++) {
      time_stamp_acc_input_history[i] = time_stamp - (acc_queue_size_ - i) * control_dt_;
    }
    std::vector<double> time_stamp_steer_input_history(steer_queue_size_);
    for (int i = 0; i < steer_queue_size_; i++) {
      time_stamp_steer_input_history[i] = time_stamp - (steer_queue_size_ - i) * control_dt_;
    }

    std::vector<double> time_stamp_controller_acc_input_history(controller_acc_input_history_len);
    for (int i = 0; i < controller_acc_input_history_len; i++) {
      time_stamp_controller_acc_input_history[i] =
        time_stamp - (controller_acc_input_history_len - i - 1) * control_dt_;
    }
    std::vector<double> time_stamp_controller_steer_input_history(
      controller_steer_input_history_len);
    for (int i = 0; i < controller_steer_input_history_len; i++) {
      time_stamp_controller_steer_input_history[i] =
        time_stamp - (controller_steer_input_history_len - i - 1) * control_dt_;
    }
    std::vector<double> time_stamp_state_history_lstm(predict_step_ * update_lstm_len_);
    for (int i = 0; i < predict_step_ * update_lstm_len_; i++) {
      time_stamp_state_history_lstm[i] =
        time_stamp - (predict_step_ * update_lstm_len_ - i) * control_dt_;
    }
    std::vector<double> time_stamp_input_history_lstm(predict_step_ * update_lstm_len_ + 1);
    for (int i = 0; i < predict_step_ * update_lstm_len_ + 1; i++) {
      time_stamp_input_history_lstm[i] =
        time_stamp - (predict_step_ * update_lstm_len_ + 1 - i) * control_dt_;
    }
    Eigen::Map<Eigen::VectorXd> acc_input_history_obs_eigen(
      acc_input_history_obs_.data(), acc_input_history_obs_.size());
    Eigen::Map<Eigen::VectorXd> steer_input_history_obs_eigen(
      steer_input_history_obs_.data(), steer_input_history_obs_.size());

    acc_input_history_ =
      interpolate_eigen(acc_input_history_obs_eigen, time_stamp_obs_, time_stamp_acc_input_history);
    steer_input_history_ = interpolate_eigen(
      steer_input_history_obs_eigen, time_stamp_obs_, time_stamp_steer_input_history);

    state_history_lstm_ =
      interpolate_vector(state_history_lstm_obs_, time_stamp_obs_, time_stamp_state_history_lstm);
    acc_input_history_lstm_ = interpolate_vector(
      acc_input_history_lstm_obs_, time_stamp_obs_, time_stamp_input_history_lstm);
    steer_input_history_lstm_ = interpolate_vector(
      steer_input_history_lstm_obs_, time_stamp_obs_, time_stamp_input_history_lstm);

    time_stamp_obs_.push_back(time_stamp);
    acc_controller_input_history_obs_.push_back(acc_controller_input);
    steer_controller_input_history_obs_.push_back(steer_controller_input);

    if (time_stamp - time_stamp_obs_[1] > max_queue_size_ * control_dt_) {
      time_stamp_obs_.erase(time_stamp_obs_.begin());
      acc_input_history_obs_.erase(acc_input_history_obs_.begin());
      steer_input_history_obs_.erase(steer_input_history_obs_.begin());
      acc_controller_input_history_obs_.erase(acc_controller_input_history_obs_.begin());
      steer_controller_input_history_obs_.erase(steer_controller_input_history_obs_.begin());
      state_history_lstm_obs_.erase(state_history_lstm_obs_.begin());
      acc_input_history_lstm_obs_.erase(acc_input_history_lstm_obs_.begin());
      steer_input_history_lstm_obs_.erase(steer_input_history_lstm_obs_.begin());
    }

    Eigen::Map<Eigen::VectorXd> acc_controller_input_history_obs_eigen(
      acc_controller_input_history_obs_.data(), acc_controller_input_history_obs_.size());
    Eigen::Map<Eigen::VectorXd> steer_controller_input_history_obs_eigen(
      steer_controller_input_history_obs_.data(), steer_controller_input_history_obs_.size());

    acc_controller_input_history_ = interpolate_eigen(
      acc_controller_input_history_obs_eigen, time_stamp_obs_,
      time_stamp_controller_acc_input_history);
    steer_controller_input_history_ = interpolate_eigen(
      steer_controller_input_history_obs_eigen, time_stamp_obs_,
      time_stamp_controller_steer_input_history);

    past_acc_input_change_ = past_acc_input_change_decay_rate_ * past_acc_input_change_;
    past_acc_input_change_ +=
      (1 - past_acc_input_change_decay_rate_) *
      std::abs(
        acc_controller_input_history_[acc_controller_input_history_.size() - 1] -
        acc_controller_input_history_
          [acc_controller_input_history_.size() - 1 - acc_input_change_window_size_]) /
      (acc_input_change_window_size_ * control_dt_);
    past_steer_input_change_ = past_steer_input_change_decay_rate_ * past_steer_input_change_;
    past_steer_input_change_ +=
      (1 - past_steer_input_change_decay_rate_) *
      std::abs(
        steer_controller_input_history_[steer_controller_input_history_.size() - 1] -
        steer_controller_input_history_
          [steer_controller_input_history_.size() - 1 - steer_input_change_window_size_]) /
      (steer_input_change_window_size_ * control_dt_);
    if (use_sg_for_d_inputs_schedule_) {
      d_inputs_schedule_ = sg_filter_for_d_inputs_schedule_.sg_filter(d_inputs_schedule_);
    }
  }

  // update lstm states //
  Eigen::VectorXd h_lstm = Eigen::VectorXd::Zero(h_dim_full_);
  Eigen::VectorXd c_lstm = Eigen::VectorXd::Zero(h_dim_full_);
  std::vector<Eigen::VectorXd> h_lstm_encoder(
    num_layers_encoder_, Eigen::VectorXd::Zero(h_dim_full_));
  std::vector<Eigen::VectorXd> c_lstm_encoder(
    num_layers_encoder_, Eigen::VectorXd::Zero(h_dim_full_));
  Eigen::Vector2d acc_steer_error = Eigen::Vector2d::Zero();

  if (use_offline_features_) {
    h_lstm = offline_features_.head(h_dim_full_);
    c_lstm = offline_features_.tail(h_dim_full_);
  }

  for (int i = 0; i < update_lstm_len_; i++) {
    Eigen::VectorXd states_tmp = state_history_lstm_[predict_step_ * i];
    Eigen::VectorXd acc_input_history_concat =
      Eigen::VectorXd::Zero(acc_queue_size_ + predict_step_);
    Eigen::VectorXd steer_input_history_concat =
      Eigen::VectorXd::Zero(steer_queue_size_ + predict_step_);
    acc_input_history_concat.head(acc_queue_size_) = acc_input_history_lstm_[predict_step_ * i];
    steer_input_history_concat.head(steer_queue_size_) =
      steer_input_history_lstm_[predict_step_ * i];

    for (int j = 0; j < predict_step_; j++) {
      acc_input_history_concat[acc_queue_size_ + j] =
        acc_input_history_lstm_[predict_step_ * i + j + 1][acc_queue_size_ - 1];
      steer_input_history_concat[steer_queue_size_ + j] =
        steer_input_history_lstm_[predict_step_ * i + j + 1][steer_queue_size_ - 1];
    }

    if (i < update_lstm_len_ - 1) {
      acc_steer_error = (state_history_lstm_[predict_step_ * (i + 1)] -
                         adaptor_ilqr_.nominal_prediction(
                           states_tmp, acc_input_history_concat, steer_input_history_concat))
                          .tail(2) /
                        predict_dt_;
    } else {
      acc_steer_error =
        (states - adaptor_ilqr_.nominal_prediction(
                    states_tmp, acc_input_history_concat, steer_input_history_concat))
          .tail(2) /
        predict_dt_;
    }
    adaptor_ilqr_.update_lstm_states(
      states_tmp, acc_input_history_concat, steer_input_history_concat, h_lstm_encoder,
      c_lstm_encoder, acc_steer_error);
  }
  h_lstm = h_lstm_encoder[num_layers_encoder_ - 1];
  c_lstm = c_lstm_encoder[num_layers_encoder_ - 1];

  ///////// calculate states_ref /////////
  std::vector<Eigen::VectorXd> states_ref(horizon_len_ + 1);

  // calculate controller input history with schedule //

  Eigen::VectorXd acc_controller_input_history_with_schedule =
    Eigen::VectorXd::Zero(controller_acc_input_history_len + predict_step_ * horizon_len_ - 1);
  Eigen::VectorXd steer_controller_input_history_with_schedule =
    Eigen::VectorXd::Zero(controller_steer_input_history_len + predict_step_ * horizon_len_ - 1);
  acc_controller_input_history_with_schedule.head(controller_acc_input_history_len) =
    acc_controller_input_history_;
  steer_controller_input_history_with_schedule.head(controller_steer_input_history_len) =
    steer_controller_input_history_;

  Eigen::VectorXd acc_controller_inputs_prediction_by_NN(horizon_len_ * predict_step_ - 1);
  Eigen::VectorXd steer_controller_inputs_prediction_by_NN(horizon_len_ * predict_step_ - 1);
  Eigen::VectorXd acc_input_schedule_predictor_states(2);
  Eigen::VectorXd steer_input_schedule_predictor_states(2);
  acc_input_schedule_predictor_states << states[vel_index_], acc_controller_input;
  steer_input_schedule_predictor_states << states[vel_index_], steer_controller_input;
  if (acc_input_schedule_prediction_mode_ == "NN") {
    std::vector<double> controller_acc_input_prediction_by_NN =
      acc_input_schedule_prediction_.get_inputs_schedule_predicted(
        acc_input_schedule_predictor_states, time_stamp);
    for (int i = 0; i < acc_input_schedule_prediction_len_; i++) {
      acc_controller_inputs_prediction_by_NN[i] = controller_acc_input_prediction_by_NN[i];
    }
    acc_controller_inputs_prediction_by_NN.head(acc_input_schedule_prediction_len_) =
      acc_input_ref_smoother_.get_smoothed_inputs_ref(
        acc_controller_input,
        acc_controller_inputs_prediction_by_NN.head(acc_input_schedule_prediction_len_));
    for (int i = acc_input_schedule_prediction_len_; i < horizon_len_ * predict_step_ - 1; i++) {
      acc_controller_inputs_prediction_by_NN[i] =
        acc_controller_inputs_prediction_by_NN[acc_input_schedule_prediction_len_ - 1];
    }
  }
  if (steer_input_schedule_prediction_mode_ == "NN") {
    std::vector<double> controller_steer_input_prediction_by_NN =
      steer_input_schedule_prediction_.get_inputs_schedule_predicted(
        steer_input_schedule_predictor_states, time_stamp);
    for (int i = 0; i < steer_input_schedule_prediction_len_; i++) {
      steer_controller_inputs_prediction_by_NN[i] = controller_steer_input_prediction_by_NN[i];
    }
    steer_controller_inputs_prediction_by_NN.head(steer_input_schedule_prediction_len_) =
      steer_input_ref_smoother_.get_smoothed_inputs_ref(
        steer_controller_input,
        steer_controller_inputs_prediction_by_NN.head(steer_input_schedule_prediction_len_));
    for (int i = steer_input_schedule_prediction_len_; i < horizon_len_ * predict_step_ - 1; i++) {
      steer_controller_inputs_prediction_by_NN[i] =
        steer_controller_inputs_prediction_by_NN[steer_input_schedule_prediction_len_ - 1];
    }
  }

  if (states_ref_mode_ == "controller_d_steer_schedule") {
    for (int i = 0; i < int(d_inputs_schedule_.size()); i++) {
      d_inputs_schedule_[i][1] = (1 - reflect_controller_d_input_ratio_) * d_inputs_schedule_[i][1];
      d_inputs_schedule_[i][1] +=
        reflect_controller_d_input_ratio_ * steer_controller_d_inputs_schedule_[i];
    }
    past_acc_input_change_weight_ = 1.0;
    past_steer_input_change_weight_ = 0.5;

    Eigen::VectorXd acc_controller_input_prediction(horizon_len_ * predict_step_ - 1);
    if (acc_input_schedule_prediction_mode_ == "NN") {
      acc_controller_input_prediction = acc_controller_inputs_prediction_by_NN;
    } else if (acc_input_schedule_prediction_mode_ == "linear_extrapolation") {
      acc_controller_input_prediction.head(acc_linear_extrapolation_len_) =
        (acc_controller_input_history_[acc_controller_input_history_.size() - 1] -
         acc_controller_input_history_
           [acc_controller_input_history_.size() - past_len_for_acc_linear_extrapolation_ - 1]) /
          past_len_for_acc_linear_extrapolation_ *
          Eigen::VectorXd::LinSpaced(
            acc_linear_extrapolation_len_, 1, acc_linear_extrapolation_len_) +
        acc_controller_input_history_[acc_controller_input_history_.size() - 1] *
          Eigen::VectorXd::Ones(acc_linear_extrapolation_len_);
      acc_controller_input_prediction.head(acc_linear_extrapolation_len_) =
        acc_input_ref_smoother_.get_smoothed_inputs_ref(
          acc_controller_input,
          acc_controller_input_prediction.head(acc_linear_extrapolation_len_));
      acc_controller_input_prediction.tail(
        horizon_len_ * predict_step_ - acc_linear_extrapolation_len_ - 1) =
        acc_controller_input_prediction[acc_linear_extrapolation_len_ - 1] *
        Eigen::VectorXd::Ones(horizon_len_ * predict_step_ - acc_linear_extrapolation_len_ - 1);
    } else {
      acc_controller_input_prediction.head(acc_polynomial_prediction_len_) =
        polynomial_reg_for_predict_acc_input_
          .predict(acc_controller_input_history_.tail(controller_acc_input_history_len_))
          .head(acc_polynomial_prediction_len_);
      acc_controller_input_prediction.head(acc_polynomial_prediction_len_) =
        acc_input_ref_smoother_.get_smoothed_inputs_ref(
          acc_controller_input,
          acc_controller_input_prediction.head(acc_polynomial_prediction_len_));
      acc_controller_input_prediction.tail(
        horizon_len_ * predict_step_ - acc_polynomial_prediction_len_ - 1) =
        acc_controller_input_prediction[acc_polynomial_prediction_len_ - 1] *
        Eigen::VectorXd::Ones(horizon_len_ * predict_step_ - acc_polynomial_prediction_len_ - 1);
    }
    acc_controller_input_history_with_schedule.tail(predict_step_ * horizon_len_ - 1) =
      acc_controller_input_prediction;
    for (int i = 0; i < horizon_len_; i++) {
      for (int j = 0; j < predict_step_; j++) {
        if (i != 0 || j != 0) {
          steer_controller_input_history_with_schedule
            [controller_steer_input_history_len_ - 1 + i * predict_step_ + j] =
              steer_controller_input_history_with_schedule
                [controller_steer_input_history_len_ - 1 + i * predict_step_ + j - 1] +
              control_dt_ * steer_controller_d_inputs_schedule_[i];
        }
      }
    }
  } else if (states_ref_mode_ == "controller_d_inputs_schedule") {
    for (int i = 0; i < int(d_inputs_schedule_.size()); i++) {
      d_inputs_schedule_[i] = (1 - reflect_controller_d_input_ratio_) * d_inputs_schedule_[i];
      d_inputs_schedule_[i][0] +=
        reflect_controller_d_input_ratio_ * acc_controller_d_inputs_schedule_[i];
      d_inputs_schedule_[i][1] +=
        reflect_controller_d_input_ratio_ * steer_controller_d_inputs_schedule_[i];
    }
    past_acc_input_change_weight_ = 0.5;
    past_steer_input_change_weight_ = 0.5;

    for (int i = 0; i < horizon_len_; i++) {
      for (int j = 0; j < predict_step_; j++) {
        if (i != 0 || j != 0) {
          acc_controller_input_history_with_schedule
            [controller_acc_input_history_len - 1 + i * predict_step_ + j] =
              acc_controller_input_history_with_schedule
                [controller_acc_input_history_len - 1 + i * predict_step_ + j - 1] +
              control_dt_ * acc_controller_d_inputs_schedule_[i];
          steer_controller_input_history_with_schedule
            [controller_steer_input_history_len - 1 + i * predict_step_ + j] =
              steer_controller_input_history_with_schedule
                [controller_steer_input_history_len - 1 + i * predict_step_ + j - 1] +
              control_dt_ * steer_controller_d_inputs_schedule_[i];
        }
      }
    }
  } else {
    if (states_ref_mode_ == "input_schedule_prediction") {
      past_acc_input_change_weight_ = 1.0;
      past_steer_input_change_weight_ = 1.0;
    } else {  // states_ref_mode_ == "controller_prediction"
      past_acc_input_change_weight_ = 0.5;
      past_steer_input_change_weight_ = 0.5;
    }

    Eigen::VectorXd acc_controller_input_prediction(horizon_len_ * predict_step_ - 1);
    Eigen::VectorXd steer_controller_input_prediction(horizon_len_ * predict_step_ - 1);
    if (acc_input_schedule_prediction_mode_ == "NN") {
      acc_controller_input_prediction = acc_controller_inputs_prediction_by_NN;
    } else if (acc_input_schedule_prediction_mode_ == "linear_extrapolation") {
      acc_controller_input_prediction.head(acc_linear_extrapolation_len_) =
        (acc_controller_input_history_[acc_controller_input_history_.size() - 1] -
         acc_controller_input_history_
           [acc_controller_input_history_.size() - past_len_for_acc_linear_extrapolation_ - 1]) /
          past_len_for_acc_linear_extrapolation_ *
          Eigen::VectorXd::LinSpaced(
            acc_linear_extrapolation_len_, 1, acc_linear_extrapolation_len_) +
        acc_controller_input_history_[acc_controller_input_history_.size() - 1] *
          Eigen::VectorXd::Ones(acc_linear_extrapolation_len_);
      acc_controller_input_prediction.head(acc_linear_extrapolation_len_) =
        acc_input_ref_smoother_.get_smoothed_inputs_ref(
          acc_controller_input,
          acc_controller_input_prediction.head(acc_linear_extrapolation_len_));
      acc_controller_input_prediction.tail(
        horizon_len_ * predict_step_ - acc_linear_extrapolation_len_ - 1) =
        acc_controller_input_prediction[acc_linear_extrapolation_len_ - 1] *
        Eigen::VectorXd::Ones(horizon_len_ * predict_step_ - acc_linear_extrapolation_len_ - 1);
    } else {
      acc_controller_input_prediction.head(acc_polynomial_prediction_len_) =
        polynomial_reg_for_predict_acc_input_
          .predict(acc_controller_input_history_.tail(controller_acc_input_history_len_))
          .head(acc_polynomial_prediction_len_);
      acc_controller_input_prediction.head(acc_polynomial_prediction_len_) =
        acc_input_ref_smoother_.get_smoothed_inputs_ref(
          acc_controller_input,
          acc_controller_input_prediction.head(acc_polynomial_prediction_len_));
      acc_controller_input_prediction.tail(
        horizon_len_ * predict_step_ - acc_polynomial_prediction_len_ - 1) =
        acc_controller_input_prediction[acc_polynomial_prediction_len_ - 1] *
        Eigen::VectorXd::Ones(horizon_len_ * predict_step_ - acc_polynomial_prediction_len_ - 1);
    }
    if (steer_input_schedule_prediction_mode_ == "NN") {
      steer_controller_input_prediction = steer_controller_inputs_prediction_by_NN;
    } else if (steer_input_schedule_prediction_mode_ == "linear_extrapolation") {
      steer_controller_input_prediction.head(steer_linear_extrapolation_len_) =
        (steer_controller_input_history_[steer_controller_input_history_.size() - 1] -
         steer_controller_input_history_
           [steer_controller_input_history_.size() - past_len_for_steer_linear_extrapolation_ -
            1]) /
          past_len_for_steer_linear_extrapolation_ *
          Eigen::VectorXd::LinSpaced(
            steer_linear_extrapolation_len_, 1, steer_linear_extrapolation_len_) +
        steer_controller_input_history_[steer_controller_input_history_.size() - 1] *
          Eigen::VectorXd::Ones(steer_linear_extrapolation_len_);
      steer_controller_input_prediction.head(steer_linear_extrapolation_len_) =
        steer_input_ref_smoother_.get_smoothed_inputs_ref(
          steer_controller_input,
          steer_controller_input_prediction.head(steer_linear_extrapolation_len_));
      steer_controller_input_prediction.tail(
        horizon_len_ * predict_step_ - steer_linear_extrapolation_len_ - 1) =
        steer_controller_input_prediction[steer_linear_extrapolation_len_ - 1] *
        Eigen::VectorXd::Ones(horizon_len_ * predict_step_ - steer_linear_extrapolation_len_ - 1);
    } else {
      steer_controller_input_prediction.head(steer_polynomial_prediction_len_) =
        polynomial_reg_for_predict_steer_input_
          .predict(steer_controller_input_history_.tail(controller_steer_input_history_len_))
          .head(steer_polynomial_prediction_len_);
      steer_controller_input_prediction.head(steer_polynomial_prediction_len_) =
        steer_input_ref_smoother_.get_smoothed_inputs_ref(
          steer_controller_input,
          steer_controller_input_prediction.head(steer_polynomial_prediction_len_));
      steer_controller_input_prediction.tail(
        horizon_len_ * predict_step_ - steer_polynomial_prediction_len_ - 1) =
        steer_controller_input_prediction[steer_polynomial_prediction_len_ - 1] *
        Eigen::VectorXd::Ones(horizon_len_ * predict_step_ - steer_polynomial_prediction_len_ - 1);
    }
    acc_controller_input_history_with_schedule.tail(predict_step_ * horizon_len_ - 1) =
      acc_controller_input_prediction;
    steer_controller_input_history_with_schedule.tail(predict_step_ * horizon_len_ - 1) =
      steer_controller_input_prediction;
  }
  // calculate states_ref by controller inputs history with schedule //
  if (
    states_ref_mode_ == "input_schedule_prediction" ||
    states_ref_mode_ == "controller_d_inputs_schedule" ||
    states_ref_mode_ == "controller_d_steer_schedule") {
    states_ref[0] = states;
    Eigen::VectorXd states_ref_tmp = states;
    for (int i = 0; i < horizon_len_; i++) {
      for (int j = 0; j < predict_step_; j++) {
        Eigen::Vector2d inputs_tmp;
        inputs_tmp << acc_controller_input_history_with_schedule
            [controller_acc_input_history_len + i * predict_step_ + j - acc_delay_step_controller_ -
             1],
          steer_controller_input_history_with_schedule
            [controller_steer_input_history_len + i * predict_step_ + j -
             steer_delay_step_controller_ - 1];
        states_ref_tmp = nominal_dynamics_controller_.F_nominal(states_ref_tmp, inputs_tmp);
        if (steer_controller_prediction_aided_) {
          states_ref_tmp[steer_index_] =
            (predict_step_ - j - 1) * steer_controller_prediction_[i] / predict_step_ +
            (j + 1) * steer_controller_prediction_[i + 1] / predict_step_;
        }
      }
      states_ref[i + 1] = states_ref_tmp;
    }
  }
  if (states_ref_mode_ == "controller_prediction") {  // calculate states_ref by controller
                                                      // prediction

    states_ref = std::vector<Eigen::VectorXd>(horizon_len_ + 1, states);
    for (int i = 0; i < horizon_len_ + 1; i++) {
      states_ref[i][x_index_] = x_controller_prediction_[i];
      states_ref[i][y_index_] = y_controller_prediction_[i];
      states_ref[i][vel_index_] = vel_controller_prediction_[i];
      states_ref[i][yaw_index_] = yaw_controller_prediction_[i];
      states_ref[i][acc_index_] = acc_controller_prediction_[i];
      states_ref[i][steer_index_] = steer_controller_prediction_[i];
    }
  }

  std::vector<Eigen::VectorXd> d_input_ref(horizon_len_, Eigen::VectorXd::Zero(2));
  double acc_input, steer_input;
  if (states_ref_mode_ == "controller_d_inputs_schedule" and use_controller_inputs_as_target_) {
    for (int i = 0; i < horizon_len_; i++) {
      d_input_ref[i][0] = acc_controller_d_inputs_schedule_[i];
      d_input_ref[i][1] = steer_controller_d_inputs_schedule_[i];
    }
  }

  // calculate future inputs change rate
  Eigen::VectorXd acc_controller_input_schedule =
    acc_controller_input_history_with_schedule.tail(predict_step_ * horizon_len_);
  Eigen::VectorXd steer_controller_input_schedule =
    steer_controller_input_history_with_schedule.tail(predict_step_ * horizon_len_);

  double future_acc_input_change_rate = 0.0;
  double future_steer_input_change_rate = 0.0;
  double future_acc_input_change_rate_coef = 1.0;
  double future_steer_input_change_rate_coef = 1.0;
  double future_acc_input_change_rate_coef_sum = 0.0;
  double future_steer_input_change_rate_coef_sum = 0.0;
  for (int i = 0; i < predict_step_ * horizon_len_ - 1; i++) {
    future_acc_input_change_rate +=
      future_acc_input_change_rate_coef *
      std::abs(acc_controller_input_schedule[i + 1] - acc_controller_input_schedule[i]) /
      control_dt_;
    future_steer_input_change_rate +=
      future_steer_input_change_rate_coef *
      std::abs(steer_controller_input_schedule[i + 1] - steer_controller_input_schedule[i]) /
      control_dt_;
    future_acc_input_change_rate_coef_sum += future_acc_input_change_rate_coef;
    future_steer_input_change_rate_coef_sum += future_steer_input_change_rate_coef;
    future_acc_input_change_rate_coef *= future_acc_input_change_decay_rate_;
    future_steer_input_change_rate_coef *= future_steer_input_change_decay_rate_;
  }
  future_acc_input_change_rate /= future_acc_input_change_rate_coef_sum;
  future_steer_input_change_rate /= future_steer_input_change_rate_coef_sum;

  // calculate inputs change rate
  double acc_input_change_rate =
    past_acc_input_change_weight_ * past_acc_input_change_ +
    (1.0 - past_acc_input_change_weight_) * future_acc_input_change_rate;
  double steer_input_change_rate =
    past_steer_input_change_weight_ * past_steer_input_change_ +
    (1.0 - past_steer_input_change_weight_) * future_steer_input_change_rate;

  adaptor_ilqr_.compute_optimal_control(
    states, acc_input_history_, steer_input_history_, d_inputs_schedule_, h_lstm, c_lstm,
    states_ref, d_input_ref, acc_input_change_rate, steer_input_change_rate, previous_error_,
    states_prediction_, acc_controller_input_schedule, steer_controller_input_schedule, acc_input,
    steer_input);

  if (input_filter_mode_ == "butterworth") {
    Eigen::Vector2d inputs_tmp = Eigen::Vector2d(acc_input, steer_input);
    inputs_tmp = butterworth_filter_.apply(inputs_tmp);
    acc_input = inputs_tmp[0];
    steer_input = inputs_tmp[1];
  }
  double mix_ratio_vel =
    calc_table_value(states[vel_index_], mix_ratio_vel_domain_table_, mix_ratio_vel_target_table_);
  double mix_ratio_time = calc_table_value(
    time_stamp - start_time_, mix_ratio_time_domain_table_, mix_ratio_time_target_table_);
  double mix_ratio = mix_ratio_vel * mix_ratio_time;
  acc_input = (1 - mix_ratio) * acc_controller_input + mix_ratio * acc_input;
  steer_input = (1 - mix_ratio) * steer_controller_input + mix_ratio * steer_input;

  state_history_lstm_obs_.push_back(states);
  acc_input_history_obs_.push_back(acc_input);
  steer_input_history_obs_.push_back(steer_input);
  acc_input_history_lstm_obs_.push_back(acc_input_history_);
  steer_input_history_lstm_obs_.push_back(steer_input_history_);
  return Eigen::Vector2d(acc_input, steer_input);
}
Eigen::MatrixXd VehicleAdaptor::get_states_prediction()
{
  Eigen::MatrixXd states_prediction_matrix(states_prediction_[0].size(), states_prediction_.size());
  for (int i = 0; i < int(states_prediction_.size()); i++) {
    states_prediction_matrix.col(i) = states_prediction_[i];
  }
  return states_prediction_matrix;
}
Eigen::MatrixXd VehicleAdaptor::get_d_inputs_schedule()
{
  Eigen::MatrixXd d_inputs_schedule_matrix(d_inputs_schedule_[0].size(), d_inputs_schedule_.size());
  for (int i = 0; i < int(d_inputs_schedule_.size()); i++) {
    d_inputs_schedule_matrix.col(i) = d_inputs_schedule_[i];
  }
  return d_inputs_schedule_matrix;
}
void VehicleAdaptor::send_initialized_flag()
{
  initialized_ = false;
  states_ref_mode_ = "input_schedule_prediction";
}

PYBIND11_MODULE(vehicle_adaptor_compensator, m)
{
  m.def("rotate_data", &rotate_data);
  py::class_<NominalDynamics>(m, "NominalDynamics")
    .def(py::init())
    .def("set_params", &NominalDynamics::set_params)
    .def("F_nominal", &NominalDynamics::F_nominal)
    .def("F_nominal_with_diff", &NominalDynamics::F_nominal_with_diff)
    .def("F_nominal_predict", &NominalDynamics::F_nominal_predict)
    .def("F_nominal_predict_with_diff", &NominalDynamics::F_nominal_predict_with_diff);
  py::class_<VehicleAdaptor>(m, "VehicleAdaptor")
    .def(py::init())
    .def("set_NN_params", &VehicleAdaptor::set_NN_params)
    .def("set_NN_params_from_csv", &VehicleAdaptor::set_NN_params_from_csv)
    .def("set_offline_features_from_csv", &VehicleAdaptor::set_offline_features_from_csv)
    .def("clear_NN_params", &VehicleAdaptor::clear_NN_params)
    .def("set_controller_d_inputs_schedule", &VehicleAdaptor::set_controller_d_inputs_schedule)
    .def("set_controller_d_steer_schedule", &VehicleAdaptor::set_controller_d_steer_schedule)
    .def("set_controller_prediction", &VehicleAdaptor::set_controller_prediction)
    .def("set_controller_steer_prediction", &VehicleAdaptor::set_controller_steer_prediction)
    .def("get_adjusted_inputs", &VehicleAdaptor::get_adjusted_inputs)
    .def("get_states_prediction", &VehicleAdaptor::get_states_prediction)
    .def("get_d_inputs_schedule", &VehicleAdaptor::get_d_inputs_schedule)
    .def("send_initialized_flag", &VehicleAdaptor::send_initialized_flag);
}
