
///////////////// NominalDynamics ///////////////////////
#include "nominal_dynamics.h"

using namespace nominal_dynamics_constants;

NominalDynamics::NominalDynamics()
{
}
NominalDynamics::~NominalDynamics()
{
}
void NominalDynamics::set_params(
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
}
void NominalDynamics::set_steer_dead_band(double steer_dead_band)
{
  steer_dead_band_ = steer_dead_band;
}
Eigen::VectorXd NominalDynamics::F_nominal(Eigen::VectorXd states, Eigen::VectorXd inputs)
{
  double vel = states[vel_index_];
  double yaw = states[yaw_index_];
  double acc = states[acc_index_];
  double steer = states[steer_index_];
  double steer_diff = inputs[1] - steer;
  if (std::abs(steer_diff) < steer_dead_band_) {
    steer_diff = 0.0;
  } else if (steer_diff > 0) {
    steer_diff -= steer_dead_band_;
  } else {
    steer_diff += steer_dead_band_;
  }
  Eigen::VectorXd states_diff = Eigen::VectorXd(state_size_);
  states_diff[x_index_] = vel * std::cos(yaw) * control_dt_;
  states_diff[y_index_] = vel * std::sin(yaw) * control_dt_;
  states_diff[vel_index_] = acc * control_dt_;
  states_diff[yaw_index_] = vel * std::tan(steer) / wheel_base_ * control_dt_;
  states_diff[acc_index_] = (inputs[0] - acc) * (1.0 - std::exp(-control_dt_ / acc_time_constant_));
  states_diff[steer_index_] = steer_diff * (1.0 - std::exp(-control_dt_ / steer_time_constant_));
  return states + states_diff;
}
Eigen::MatrixXd NominalDynamics::F_nominal_with_diff(Eigen::VectorXd states, Eigen::VectorXd inputs)
{
  Eigen::VectorXd state_next = F_nominal(states, inputs);
  double vel = states[vel_index_];
  double yaw = states[yaw_index_];
  double steer = states[steer_index_];
  Eigen::MatrixXd dF_d_states = Eigen::MatrixXd::Identity(state_size_, state_size_);
  Eigen::MatrixXd dF_d_inputs = Eigen::MatrixXd::Zero(state_size_, inputs.size());
  dF_d_states(x_index_, vel_index_) = std::cos(yaw) * control_dt_;
  dF_d_states(x_index_, yaw_index_) = -vel * std::sin(yaw) * control_dt_;
  dF_d_states(y_index_, vel_index_) = std::sin(yaw) * control_dt_;
  dF_d_states(y_index_, yaw_index_) = vel * std::cos(yaw) * control_dt_;
  dF_d_states(vel_index_, acc_index_) = control_dt_;
  dF_d_states(yaw_index_, vel_index_) = std::tan(steer) / wheel_base_ * control_dt_;
  dF_d_states(yaw_index_, steer_index_) =
    vel / (wheel_base_ * std::cos(steer) * std::cos(steer)) * control_dt_;
  dF_d_states(acc_index_, acc_index_) -= 1.0 - std::exp(-control_dt_ / acc_time_constant_);
  dF_d_states(steer_index_, steer_index_) -= 1.0 - std::exp(-control_dt_ / steer_time_constant_);
  dF_d_inputs(acc_index_, 0) = 1.0 - std::exp(-control_dt_ / acc_time_constant_);
  dF_d_inputs(steer_index_, 1) = 1.0 - std::exp(-control_dt_ / steer_time_constant_);
  Eigen::MatrixXd result = Eigen::MatrixXd(state_size_, 1 + state_size_ + inputs.size());
  result.col(0) = state_next;
  result.block(0, 1, state_size_, state_size_) = dF_d_states;
  result.block(0, 1 + state_size_, state_size_, inputs.size()) = dF_d_inputs;
  return result;
}
Eigen::VectorXd NominalDynamics::F_nominal_predict(Eigen::VectorXd states, Eigen::VectorXd Inputs)
{
  Eigen::VectorXd states_predict = states;
  for (int i = 0; i < predict_step_; i++) {
    states_predict = F_nominal(states_predict, Inputs.segment(2 * i, 2));
  }
  return states_predict;
}
Eigen::VectorXd NominalDynamics::F_nominal_predict_with_diff(
  Eigen::VectorXd states, Eigen::VectorXd Inputs, Eigen::MatrixXd & dF_d_states,
  Eigen::MatrixXd & dF_d_inputs)
{
  Eigen::VectorXd states_predict = states;
  dF_d_states = Eigen::MatrixXd::Identity(state_size_, state_size_);
  dF_d_inputs = Eigen::MatrixXd::Zero(state_size_, predict_step_ * 2);

  for (int i = 0; i < predict_step_; i++) {
    Eigen::MatrixXd dF_d_states_temp;
    Eigen::MatrixXd dF_d_inputs_temp;
    Eigen::MatrixXd F_nominal_with_diff_temp =
      F_nominal_with_diff(states_predict, Inputs.segment(2 * i, 2));
    dF_d_states_temp = F_nominal_with_diff_temp.block(0, 1, state_size_, state_size_);
    dF_d_inputs_temp = F_nominal_with_diff_temp.block(0, 1 + state_size_, state_size_, 2);
    dF_d_states = dF_d_states_temp * dF_d_states;
    if (i > 0) {
      dF_d_inputs.block(0, 0, state_size_, 2 * i) =
        dF_d_states_temp * dF_d_inputs.block(0, 0, state_size_, 2 * i);
    }
    dF_d_inputs.block(0, 2 * i, state_size_, 2) = dF_d_inputs_temp;
    states_predict = F_nominal_with_diff_temp.col(0);
  }
  return states_predict;
}
Eigen::MatrixXd NominalDynamics::F_nominal_for_candidates(
  Eigen::MatrixXd States, Eigen::MatrixXd Inputs)
{
  Eigen::RowVectorXd Steer_diff = Inputs.row(1).array() - States.row(steer_index_).array();
  Steer_diff = (Steer_diff.array().abs() < steer_dead_band_).select(0.0, Steer_diff);
  Steer_diff =
    (Steer_diff.array() > 0)
      .select(Steer_diff.array() - steer_dead_band_, Steer_diff.array() + steer_dead_band_);
  Eigen::MatrixXd States_diff = Eigen::MatrixXd(States.rows(), States.cols());
  States_diff.row(x_index_) =
    States.row(vel_index_).array() * States.row(yaw_index_).array().cos() * control_dt_;
  States_diff.row(y_index_) =
    States.row(vel_index_).array() * States.row(yaw_index_).array().sin() * control_dt_;
  States_diff.row(vel_index_) = States.row(acc_index_) * control_dt_;
  States_diff.row(yaw_index_) = States.row(vel_index_).array() *
                                States.row(steer_index_).array().tan() / wheel_base_ * control_dt_;
  States_diff.row(acc_index_) = (Inputs.row(0).array() - States.row(acc_index_).array()) *
                                (1.0 - std::exp(-control_dt_ / acc_time_constant_));
  States_diff.row(steer_index_) =
    Steer_diff * (1.0 - std::exp(-control_dt_ / steer_time_constant_));
  return States + States_diff;
}
Eigen::MatrixXd NominalDynamics::F_nominal_predict_for_candidates(
  Eigen::MatrixXd States, Eigen::MatrixXd Inputs)
{
  Eigen::MatrixXd States_predict = States;
  for (int i = 0; i < predict_step_; i++) {
    States_predict = F_nominal_for_candidates(States_predict, Inputs.middleRows(2 * i, 2));
  }
  return States_predict;
}
Eigen::VectorXd NominalDynamics::F_with_input_history(
  const Eigen::VectorXd states, const Eigen::VectorXd & acc_input_history_concat,
  const Eigen::VectorXd & steer_input_history_concat)
{
  Eigen::VectorXd Inputs = Eigen::VectorXd(2 * predict_step_);
  for (int i = 0; i < predict_step_; i++) {
    Inputs[2 * i] = acc_input_history_concat[acc_queue_size_ + i - acc_delay_step_];
    Inputs[2 * i + 1] = steer_input_history_concat[steer_queue_size_ + i - steer_delay_step_];
  }
  return F_nominal_predict(states, Inputs);
}
Eigen::VectorXd NominalDynamics::F_with_input_history(
  const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
  Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs)
{
  Eigen::VectorXd Inputs = Eigen::VectorXd(2 * predict_step_);
  acc_input_history_concat = Eigen::VectorXd(acc_queue_size_ + predict_step_);
  steer_input_history_concat = Eigen::VectorXd(steer_queue_size_ + predict_step_);
  acc_input_history_concat.head(acc_queue_size_) = acc_input_history;
  steer_input_history_concat.head(steer_queue_size_) = steer_input_history;
  for (int i = 0; i < predict_step_; i++) {
    acc_input_history_concat[acc_queue_size_ + i] =
      acc_input_history_concat[acc_queue_size_ + i - 1] + d_inputs[0] * control_dt_;
    steer_input_history_concat[steer_queue_size_ + i] =
      steer_input_history_concat[steer_queue_size_ + i - 1] + d_inputs[1] * control_dt_;
    Inputs[2 * i] = acc_input_history_concat[acc_queue_size_ + i - acc_delay_step_];
    Inputs[2 * i + 1] = steer_input_history_concat[steer_queue_size_ + i - steer_delay_step_];
  }
  acc_input_history = acc_input_history_concat.tail(acc_queue_size_);
  steer_input_history = steer_input_history_concat.tail(steer_queue_size_);
  return F_nominal_predict(states, Inputs);
}
Eigen::VectorXd NominalDynamics::F_with_input_history(
  const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs)
{
  Eigen::VectorXd acc_input_history_concat = Eigen::VectorXd(acc_queue_size_ + predict_step_);
  Eigen::VectorXd steer_input_history_concat = Eigen::VectorXd(steer_queue_size_ + predict_step_);
  return F_with_input_history(
    states, acc_input_history, steer_input_history, acc_input_history_concat,
    steer_input_history_concat, d_inputs);
}
Eigen::VectorXd NominalDynamics::F_with_input_history_and_diff(
  const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
  Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs,
  Eigen::MatrixXd & A, Eigen::MatrixXd & B)
{
  A = Eigen::MatrixXd::Zero(state_size_, state_size_ + acc_queue_size_ + steer_queue_size_);
  B = Eigen::MatrixXd::Zero(state_size_, 2);
  Eigen::VectorXd Inputs = Eigen::VectorXd(2 * predict_step_);
  acc_input_history_concat = Eigen::VectorXd(acc_queue_size_ + predict_step_);
  steer_input_history_concat = Eigen::VectorXd(steer_queue_size_ + predict_step_);
  acc_input_history_concat.head(acc_queue_size_) = acc_input_history;
  steer_input_history_concat.head(steer_queue_size_) = steer_input_history;
  Eigen::MatrixXd dF_d_states, dF_d_inputs;
  for (int i = 0; i < predict_step_; i++) {
    acc_input_history_concat[acc_queue_size_ + i] =
      acc_input_history_concat[acc_queue_size_ + i - 1] + d_inputs[0] * control_dt_;
    steer_input_history_concat[steer_queue_size_ + i] =
      steer_input_history_concat[steer_queue_size_ + i - 1] + d_inputs[1] * control_dt_;
    Inputs[2 * i] = acc_input_history_concat[acc_queue_size_ + i - acc_delay_step_];
    Inputs[2 * i + 1] = steer_input_history_concat[steer_queue_size_ + i - steer_delay_step_];
  }
  acc_input_history = acc_input_history_concat.tail(acc_queue_size_);
  steer_input_history = steer_input_history_concat.tail(steer_queue_size_);
  Eigen::VectorXd states_predict =
    F_nominal_predict_with_diff(states, Inputs, dF_d_states, dF_d_inputs);
  A.block(0, 0, state_size_, state_size_) = dF_d_states;
  for (int i = 0; i < predict_step_; i++) {
    A.col(state_size_ + acc_queue_size_ - std::max(acc_delay_step_ - i, 1)) +=
      dF_d_inputs.col(2 * i);
    A.col(state_size_ + acc_queue_size_ + steer_queue_size_ - std::max(steer_delay_step_ - i, 1)) +=
      dF_d_inputs.col(2 * i + 1);
    if (i - acc_delay_step_ >= 0) {
      B.col(0) += (i - acc_delay_step_ + 1) * dF_d_inputs.col(2 * i) * control_dt_;
    }
    if (i - steer_delay_step_ >= 0) {
      B.col(1) += (i - steer_delay_step_ + 1) * dF_d_inputs.col(2 * i + 1) * control_dt_;
    }
  }
  return states_predict;
}
Eigen::VectorXd NominalDynamics::F_with_input_history_and_diff(
  const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
  Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs, Eigen::MatrixXd & A,
  Eigen::MatrixXd & B)
{
  Eigen::VectorXd acc_input_history_concat = Eigen::VectorXd(acc_queue_size_ + predict_step_);
  Eigen::VectorXd steer_input_history_concat = Eigen::VectorXd(steer_queue_size_ + predict_step_);
  return F_with_input_history_and_diff(
    states, acc_input_history, steer_input_history, acc_input_history_concat,
    steer_input_history_concat, d_inputs, A, B);
}
Eigen::MatrixXd NominalDynamics::F_with_input_history_for_candidates(
  const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
  Eigen::MatrixXd & Steer_input_history, Eigen::MatrixXd & Acc_input_history_concat,
  Eigen::MatrixXd & Steer_input_history_concat, const Eigen::MatrixXd & D_inputs)
{
  Acc_input_history_concat = Eigen::MatrixXd(acc_queue_size_ + predict_step_, States.cols());
  Steer_input_history_concat = Eigen::MatrixXd(steer_queue_size_ + predict_step_, States.cols());
  Acc_input_history_concat.topRows(acc_queue_size_) = Acc_input_history;
  Steer_input_history_concat.topRows(steer_queue_size_) = Steer_input_history;

  Eigen::MatrixXd Inputs = Eigen::MatrixXd(2 * predict_step_, States.cols());

  for (int i = 0; i < predict_step_; i++) {
    Acc_input_history_concat.row(acc_queue_size_ + i) =
      Acc_input_history_concat.row(acc_queue_size_ + i - 1) + D_inputs.row(0) * control_dt_;

    Steer_input_history_concat.row(steer_queue_size_ + i) =
      Steer_input_history_concat.row(steer_queue_size_ + i - 1) + D_inputs.row(1) * control_dt_;

    Inputs.row(2 * i) = Acc_input_history_concat.row(acc_queue_size_ + i - acc_delay_step_);

    Inputs.row(2 * i + 1) =
      Steer_input_history_concat.row(steer_queue_size_ + i - steer_delay_step_);
  }

  Acc_input_history = Acc_input_history_concat.bottomRows(acc_queue_size_);
  Steer_input_history = Steer_input_history_concat.bottomRows(steer_queue_size_);
  return F_nominal_predict_for_candidates(States, Inputs);
}

Eigen::MatrixXd NominalDynamics::F_with_input_history_for_candidates(
  const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
  Eigen::MatrixXd & Steer_input_history, const Eigen::MatrixXd & D_inputs)
{
  Eigen::MatrixXd Acc_input_history_concat =
    Eigen::MatrixXd(acc_queue_size_ + predict_step_, States.cols());
  Eigen::MatrixXd Steer_input_history_concat =
    Eigen::MatrixXd(steer_queue_size_ + predict_step_, States.cols());
  return F_with_input_history_for_candidates(
    States, Acc_input_history, Steer_input_history, Acc_input_history_concat,
    Steer_input_history_concat, D_inputs);
}
