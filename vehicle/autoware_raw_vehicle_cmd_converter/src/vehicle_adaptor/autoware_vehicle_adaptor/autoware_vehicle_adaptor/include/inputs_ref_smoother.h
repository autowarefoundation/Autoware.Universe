#ifndef INPUTS_REF_SMOOTHER_H_
#define INPUTS_REF_SMOOTHER_H_

#include <Eigen/Core>
#include <Eigen/Dense>

#include <iostream>
#include <vector>

class InputsRefSmoother
{
private:
  int prediction_len_;
  Eigen::MatrixXd integration_matrix_;
  Eigen::VectorXd terminal_integration_vector_;
  Eigen::MatrixXd A_, A_inv_;
  double lambda_smooth_, lambda_decay_, lambda_terminal_decay_;
  double terminal_lambda_smooth_;
  double control_dt_;
  bool initialized_ = false;
  bool params_set_ = false;
  bool use_smoother_ = true;

public:
  InputsRefSmoother();
  virtual ~InputsRefSmoother();
  void set_params(
    const double control_dt, const double lambda_smooth, const double terminal_lambda_smooth,
    const double lambda_decay, const double lambda_terminal_decay);
  void set_prediction_len(const int prediction_len);
  void set_use_smoother(const bool use_smoother);
  void calc_matrices();
  Eigen::VectorXd get_smoothed_inputs_ref(
    const double initial_input, const Eigen::VectorXd & inputs_ref);
};
#endif  // INPUTS_REF_SMOOTHER_H_
