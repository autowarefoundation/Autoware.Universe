#ifndef TRANSFORM_VEHICLE_ADAPTOR_MODEL_H_
#define TRANSFORM_VEHICLE_ADAPTOR_MODEL_H_

#include "vehicle_adaptor_utils.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#include <vector>
class TransformModelToEigen
{
private:
  Eigen::MatrixXd weight_acc_encoder_layer_1_;
  Eigen::MatrixXd weight_steer_encoder_layer_1_;
  Eigen::MatrixXd weight_acc_encoder_layer_2_;
  Eigen::MatrixXd weight_steer_encoder_layer_2_;

  Eigen::MatrixXd weight_acc_layer_1_;
  Eigen::MatrixXd weight_steer_layer_1_;
  Eigen::MatrixXd weight_acc_layer_2_;
  Eigen::MatrixXd weight_steer_layer_2_;
  std::vector<Eigen::MatrixXd> weight_lstm_encoder_ih_;
  std::vector<Eigen::MatrixXd> weight_lstm_encoder_hh_;
  Eigen::MatrixXd weight_lstm_ih_;
  Eigen::MatrixXd weight_lstm_hh_;
  std::vector<Eigen::MatrixXd> weight_complimentary_layer_ = {};
  std::vector<Eigen::MatrixXd> weight_linear_relu_ = {};
  std::vector<Eigen::MatrixXd> weight_final_layer_ = {};

  Eigen::VectorXd bias_acc_encoder_layer_1_;
  Eigen::VectorXd bias_steer_encoder_layer_1_;
  Eigen::VectorXd bias_acc_encoder_layer_2_;
  Eigen::VectorXd bias_steer_encoder_layer_2_;

  Eigen::VectorXd bias_acc_layer_1_;
  Eigen::VectorXd bias_steer_layer_1_;
  Eigen::VectorXd bias_acc_layer_2_;
  Eigen::VectorXd bias_steer_layer_2_;
  std::vector<Eigen::VectorXd> bias_lstm_encoder_ih_;
  std::vector<Eigen::VectorXd> bias_lstm_encoder_hh_;
  Eigen::VectorXd bias_lstm_ih_;
  Eigen::VectorXd bias_lstm_hh_;
  std::vector<Eigen::VectorXd> bias_complimentary_layer_ = {};
  std::vector<Eigen::VectorXd> bias_linear_relu_ = {};
  std::vector<Eigen::VectorXd> bias_final_layer_ = {};

  int vel_index_ = 0;
  int acc_index_ = 1;
  int steer_index_ = 2;
  int acc_queue_size_ = 15;
  int steer_queue_size_ = 15;
  int predict_step_ = 3;
  int acc_input_start_index_ = 3;
  int steer_input_start_index_ = 3 + acc_queue_size_ + predict_step_;
  int acc_input_end_index_ = 3 + acc_queue_size_ + predict_step_ - 1;
  int steer_input_end_index_ = 3 + acc_queue_size_ + steer_queue_size_ + 2 * predict_step_ - 1;

  double vel_scaling_;
  double vel_bias_;

  int model_num_ = 0;
  int h_dim_;
  int h_dim_full_;
  int h_dim_acc_;
  int h_dim_steer_;
  int num_layers_encoder_;

public:
  TransformModelToEigen();
  virtual ~TransformModelToEigen();
  void set_params(
    const Eigen::MatrixXd & weight_acc_encoder_layer_1,
    const Eigen::MatrixXd & weight_steer_encoder_layer_1,
    const Eigen::MatrixXd & weight_acc_encoder_layer_2,
    const Eigen::MatrixXd & weight_steer_encoder_layer_2,
    const Eigen::MatrixXd & weight_acc_layer_1, const Eigen::MatrixXd & weight_steer_layer_1,
    const Eigen::MatrixXd & weight_acc_layer_2, const Eigen::MatrixXd & weight_steer_layer_2,
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
    const double vel_scaling, const double vel_bias);
  void clear_params();
  void update_lstm(
    const Eigen::VectorXd & x, const std::vector<Eigen::VectorXd> & h_lstm,
    const std::vector<Eigen::VectorXd> & c_lstm, std::vector<Eigen::VectorXd> & h_lstm_next,
    std::vector<Eigen::VectorXd> & c_lstm_next, const Eigen::Vector2d & acc_steer_error);
  void error_prediction(
    const Eigen::VectorXd & x, const Eigen::VectorXd & h_lstm, const Eigen::VectorXd & c_lstm,
    Eigen::VectorXd & h_lstm_next, Eigen::VectorXd & c_lstm_next, Eigen::VectorXd & u_acc_layer_1,
    Eigen::VectorXd & u_steer_layer_1, Eigen::VectorXd & u_acc_layer_2,
    Eigen::VectorXd & u_steer_layer_2, Eigen::VectorXd & u_i_lstm, Eigen::VectorXd & u_f_lstm,
    Eigen::VectorXd & u_g_lstm, Eigen::VectorXd & u_o_lstm, Eigen::VectorXd & i_lstm,
    Eigen::VectorXd & f_lstm, Eigen::VectorXd & g_lstm, Eigen::VectorXd & o_lstm,
    std::vector<Eigen::VectorXd> & u_complimentary_layer,
    std::vector<Eigen::VectorXd> & u_linear_relu, Eigen::VectorXd & y);
  void error_prediction(
    const Eigen::VectorXd & x, const Eigen::VectorXd & h_lstm, const Eigen::VectorXd & c_lstm,
    Eigen::VectorXd & h_lstm_next, Eigen::VectorXd & c_lstm_next, Eigen::VectorXd & y);
  void error_prediction_with_diff(
    const Eigen::VectorXd & x, const Eigen::VectorXd & h_lstm, const Eigen::VectorXd & c_lstm,
    Eigen::VectorXd & h_lstm_next, Eigen::VectorXd & c_lstm_next, Eigen::VectorXd & y,
    Eigen::MatrixXd & dy_dx, Eigen::MatrixXd & dy_dhc, Eigen::MatrixXd & dhc_dhc,
    Eigen::MatrixXd & dhc_dx);
};
#endif  // TRANSFORM_VEHICLE_ADAPTOR_MODEL_H_
