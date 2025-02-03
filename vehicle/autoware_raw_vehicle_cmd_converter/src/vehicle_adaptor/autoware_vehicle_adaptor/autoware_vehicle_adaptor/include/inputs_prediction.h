#ifndef INPUTS_PREDICTION_H_
#define INPUTS_PREDICTION_H_

#include "vehicle_adaptor_utils.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

class InputsSchedulePrediction
{
private:
  Eigen::MatrixXd weight_lstm_encoder_ih_0_;
  Eigen::MatrixXd weight_lstm_encoder_hh_0_;
  Eigen::MatrixXd weight_lstm_decoder_ih_0_;
  Eigen::MatrixXd weight_lstm_decoder_hh_0_;
  Eigen::MatrixXd weight_lstm_encoder_ih_1_;
  Eigen::MatrixXd weight_lstm_encoder_hh_1_;
  Eigen::MatrixXd weight_lstm_decoder_ih_1_;
  Eigen::MatrixXd weight_lstm_decoder_hh_1_;
  Eigen::MatrixXd weight_post_decoder_0_;
  Eigen::MatrixXd weight_post_decoder_1_;
  Eigen::MatrixXd weight_final_layer_;
  Eigen::VectorXd bias_lstm_encoder_ih_0_;
  Eigen::VectorXd bias_lstm_encoder_hh_0_;
  Eigen::VectorXd bias_lstm_decoder_ih_0_;
  Eigen::VectorXd bias_lstm_decoder_hh_0_;
  Eigen::VectorXd bias_lstm_encoder_ih_1_;
  Eigen::VectorXd bias_lstm_encoder_hh_1_;
  Eigen::VectorXd bias_lstm_decoder_ih_1_;
  Eigen::VectorXd bias_lstm_decoder_hh_1_;
  Eigen::VectorXd bias_post_decoder_0_;
  Eigen::VectorXd bias_post_decoder_1_;
  Eigen::VectorXd bias_final_layer_;
  Eigen::VectorXd adaptive_scale_;
  double vel_scaling_, vel_bias_;

  int vel_index_ = 0;
  int predict_step_ = 3;
  int input_step_, output_step_;
  int h_dim_;
  double control_dt_ = 0.033;
  bool initialized_ = false;
  std::vector<Eigen::VectorXd> x_seq_obs_;
  int input_rate_lim_;
  std::vector<double> timestamps_obs_;
  Eigen::VectorXd limit_scaling_;
  int augmented_input_size_;

public:
  InputsSchedulePrediction();
  virtual ~InputsSchedulePrediction();
  void set_params(
    int input_step, int output_step, double control_dt, std::string csv_dir,
    const int & adaptive_scale_index = -1);
  void set_NN_params(
    const Eigen::MatrixXd & weight_lstm_encoder_ih_0,
    const Eigen::MatrixXd & weight_lstm_encoder_hh_0,
    const Eigen::MatrixXd & weight_lstm_decoder_ih_0,
    const Eigen::MatrixXd & weight_lstm_decoder_hh_0,
    const Eigen::MatrixXd & weight_lstm_encoder_ih_1,
    const Eigen::MatrixXd & weight_lstm_encoder_hh_1,
    const Eigen::MatrixXd & weight_lstm_decoder_ih_1,
    const Eigen::MatrixXd & weight_lstm_decoder_hh_1, const Eigen::MatrixXd & weight_post_decoder_0,
    const Eigen::MatrixXd & weight_post_decoder_1, const Eigen::MatrixXd & weight_final_layer,
    const Eigen::VectorXd & bias_lstm_encoder_ih_0, const Eigen::VectorXd & bias_lstm_encoder_hh_0,
    const Eigen::VectorXd & bias_lstm_decoder_ih_0, const Eigen::VectorXd & bias_lstm_decoder_hh_0,
    const Eigen::VectorXd & bias_lstm_encoder_ih_1, const Eigen::VectorXd & bias_lstm_encoder_hh_1,
    const Eigen::VectorXd & bias_lstm_decoder_ih_1, const Eigen::VectorXd & bias_lstm_decoder_hh_1,
    const Eigen::VectorXd & bias_post_decoder_0, const Eigen::VectorXd & bias_post_decoder_1,
    const Eigen::VectorXd & bias_final_layer, const Eigen::VectorXd & adaptive_scale,
    int input_step, int output_step, double input_rate_lim, double vel_scaling, double vel_bias,
    double control_dt);
  void update_encoder_cells(
    const Eigen::VectorXd & x, Eigen::VectorXd & h_lstm_encoder_0,
    Eigen::VectorXd & h_lstm_encoder_1, Eigen::VectorXd & c_lstm_encoder_0,
    Eigen::VectorXd & c_lstm_encoder_1);
  void get_encoder_cells(
    const std::vector<Eigen::VectorXd> & x_seq, Eigen::VectorXd & h_lstm_encoder_0,
    Eigen::VectorXd & c_lstm_encoder_0, Eigen::VectorXd & h_lstm_encoder_1,
    Eigen::VectorXd & c_lstm_encoder_1);
  Eigen::VectorXd update_decoder_cells(
    const Eigen::VectorXd & decoder_input, Eigen::VectorXd & h_lstm_decoder_0,
    Eigen::VectorXd & c_lstm_decoder_0, Eigen::VectorXd & h_lstm_decoder_1,
    Eigen::VectorXd & c_lstm_decoder_1);
  std::vector<double> predict(const std::vector<Eigen::VectorXd> & x_seq);
  std::vector<double> get_inputs_schedule_predicted(const Eigen::VectorXd & x, double timestamp);
  void send_initialized_flag();
};
#endif  // INPUTS_PREDICTION_H_
