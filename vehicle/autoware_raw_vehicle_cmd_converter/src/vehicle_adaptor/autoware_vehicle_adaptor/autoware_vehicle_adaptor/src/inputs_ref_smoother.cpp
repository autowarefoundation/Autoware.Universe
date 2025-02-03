#include "inputs_ref_smoother.h"

InputsRefSmoother::InputsRefSmoother()
{
}
InputsRefSmoother::~InputsRefSmoother()
{
}
void InputsRefSmoother::set_params(
  const double control_dt, const double lambda_smooth, const double terminal_lambda_smooth,
  const double lambda_decay, const double lambda_terminal_decay)
{
  control_dt_ = control_dt;
  lambda_smooth_ = lambda_smooth;
  terminal_lambda_smooth_ = terminal_lambda_smooth;
  lambda_decay_ = lambda_decay;
  lambda_terminal_decay_ = lambda_terminal_decay;
  initialized_ = false;
}
void InputsRefSmoother::set_prediction_len(const int prediction_len)
{
  prediction_len_ = prediction_len;
  params_set_ = true;
}
void InputsRefSmoother::set_use_smoother(const bool use_smoother)
{
  use_smoother_ = use_smoother;
}
void InputsRefSmoother::calc_matrices()
{
  if (!params_set_) {
    std::cerr << "Error: Parameters not set" << std::endl;
    return;
  }
  integration_matrix_ = control_dt_ * Eigen::MatrixXd::Zero(prediction_len_, prediction_len_);
  for (int i = 0; i < prediction_len_; i++) {
    for (int j = 0; j <= i; j++) {
      integration_matrix_(i, j) = control_dt_;
    }
  }
  Eigen::VectorXd lambda_smooth_vector =
    Eigen::VectorXd::LinSpaced(prediction_len_, lambda_smooth_, terminal_lambda_smooth_);
  Eigen::MatrixXd lambda_smooth_matrix = lambda_smooth_vector.asDiagonal();

  terminal_integration_vector_ = integration_matrix_.row(prediction_len_ - 1).transpose();
  A_ = (1 + lambda_decay_) * integration_matrix_.transpose() * integration_matrix_ +
       lambda_smooth_matrix
       //+ lambda_smooth_ * Eigen::MatrixXd::Identity(prediction_len_, prediction_len_)
       + lambda_terminal_decay_ * terminal_integration_vector_ *
           terminal_integration_vector_.transpose();
  A_inv_ = A_.llt().solve(Eigen::MatrixXd::Identity(prediction_len_, prediction_len_));
  initialized_ = true;
}
Eigen::VectorXd InputsRefSmoother::get_smoothed_inputs_ref(
  const double initial_input, const Eigen::VectorXd & inputs_ref)
{
  if (!use_smoother_) {
    return inputs_ref;
  }
  if (!initialized_) {
    calc_matrices();
  }
  Eigen::VectorXd d_inputs_ref =
    A_inv_ *
    (integration_matrix_.transpose() *
       (inputs_ref - (1 + lambda_decay_) * initial_input * Eigen::VectorXd::Ones(prediction_len_)) -
     lambda_terminal_decay_ * terminal_integration_vector_ * initial_input);
  Eigen::VectorXd inputs_ref_smoothed = integration_matrix_ * d_inputs_ref;
  inputs_ref_smoothed = inputs_ref_smoothed.array() + initial_input;
  return inputs_ref_smoothed;
}
