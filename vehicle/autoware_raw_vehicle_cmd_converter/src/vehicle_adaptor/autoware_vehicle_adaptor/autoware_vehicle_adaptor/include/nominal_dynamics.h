#ifndef NOMINAL_DYNAMICS_H_
#define NOMINAL_DYNAMICS_H_

#include <Eigen/Core>
#include <Eigen/Dense>

namespace nominal_dynamics_constants
{
constexpr int x_index_ = 0;
constexpr int y_index_ = 1;
constexpr int vel_index_ = 2;
constexpr int yaw_index_ = 3;
constexpr int acc_index_ = 4;
constexpr int steer_index_ = 5;
constexpr int state_size_ = 6;
}  // namespace nominal_dynamics_constants

class NominalDynamics
{
private:
  double wheel_base_ = 2.79;
  double acc_time_delay_ = 0.1;
  double steer_time_delay_ = 0.27;
  double acc_time_constant_ = 0.1;
  double steer_time_constant_ = 0.24;
  int acc_queue_size_ = 15;
  int steer_queue_size_ = 15;

  int acc_input_start_index_ = 6;
  int acc_input_end_index_ = 6 + acc_queue_size_ - 1;
  int steer_input_start_index_ = 6 + acc_queue_size_;
  int steer_input_end_index_ = 6 + acc_queue_size_ + steer_queue_size_ - 1;
  double control_dt_ = 0.033;
  int predict_step_ = 3;
  double predict_dt_ = control_dt_ * predict_step_;

  int acc_delay_step_ = std::min(int(std::round(acc_time_delay_ / control_dt_)), acc_queue_size_);
  int steer_delay_step_ =
    std::min(int(std::round(steer_time_delay_ / control_dt_)), steer_queue_size_);
  double steer_dead_band_ = 0.0;

public:
  NominalDynamics();
  virtual ~NominalDynamics();
  void set_params(
    double wheel_base, double acc_time_delay, double steer_time_delay, double acc_time_constant,
    double steer_time_constant, int acc_queue_size, int steer_queue_size, double control_dt,
    int predict_step);
  void set_steer_dead_band(double steer_dead_band);
  Eigen::VectorXd F_nominal(Eigen::VectorXd states, Eigen::VectorXd inputs);
  Eigen::MatrixXd F_nominal_with_diff(Eigen::VectorXd states, Eigen::VectorXd inputs);
  Eigen::VectorXd F_nominal_predict(Eigen::VectorXd states, Eigen::VectorXd Inputs);
  Eigen::VectorXd F_nominal_predict_with_diff(
    Eigen::VectorXd states, Eigen::VectorXd Inputs, Eigen::MatrixXd & dF_d_states,
    Eigen::MatrixXd & dF_d_inputs);
  Eigen::MatrixXd F_nominal_for_candidates(Eigen::MatrixXd States, Eigen::MatrixXd Inputs);
  Eigen::MatrixXd F_nominal_predict_for_candidates(Eigen::MatrixXd States, Eigen::MatrixXd Inputs);
  Eigen::VectorXd F_with_input_history(
    const Eigen::VectorXd states, const Eigen::VectorXd & acc_input_history_concat,
    const Eigen::VectorXd & steer_input_history_concat);
  Eigen::VectorXd F_with_input_history(
    const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
    Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs);
  Eigen::VectorXd F_with_input_history(
    const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs);
  Eigen::VectorXd F_with_input_history_and_diff(
    const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
    Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs,
    Eigen::MatrixXd & A, Eigen::MatrixXd & B);
  Eigen::VectorXd F_with_input_history_and_diff(
    const Eigen::VectorXd states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs, Eigen::MatrixXd & A,
    Eigen::MatrixXd & B);
  Eigen::MatrixXd F_with_input_history_for_candidates(
    const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
    Eigen::MatrixXd & Steer_input_history, Eigen::MatrixXd & Acc_input_history_concat,
    Eigen::MatrixXd & Steer_input_history_concat, const Eigen::MatrixXd & D_inputs);
  Eigen::MatrixXd F_with_input_history_for_candidates(
    const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
    Eigen::MatrixXd & Steer_input_history, const Eigen::MatrixXd & D_inputs);
};

#endif  // NOMINAL_DYNAMICS_H_
