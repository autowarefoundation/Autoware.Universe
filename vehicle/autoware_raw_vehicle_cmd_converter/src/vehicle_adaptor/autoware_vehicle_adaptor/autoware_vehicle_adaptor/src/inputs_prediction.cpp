#include "inputs_prediction.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <cmath>
#include <sstream>

namespace py = pybind11;
InputsSchedulePrediction::InputsSchedulePrediction()
{
}
InputsSchedulePrediction::~InputsSchedulePrediction()
{
}
void InputsSchedulePrediction::set_params(
  int input_step, int output_step, double control_dt, std::string csv_dir,
  const int & adaptive_scale_index)
{
  std::cout << "csv_dir: " << csv_dir << std::endl;
  Eigen::MatrixXd weight_lstm_encoder_ih_0 = read_csv(csv_dir + "/weight_lstm_encoder_ih_0.csv");
  Eigen::MatrixXd weight_lstm_encoder_hh_0 = read_csv(csv_dir + "/weight_lstm_encoder_hh_0.csv");
  Eigen::MatrixXd weight_lstm_decoder_ih_0 = read_csv(csv_dir + "/weight_lstm_decoder_ih_0.csv");
  Eigen::MatrixXd weight_lstm_decoder_hh_0 = read_csv(csv_dir + "/weight_lstm_decoder_hh_0.csv");
  Eigen::MatrixXd weight_lstm_encoder_ih_1 = read_csv(csv_dir + "/weight_lstm_encoder_ih_1.csv");
  Eigen::MatrixXd weight_lstm_encoder_hh_1 = read_csv(csv_dir + "/weight_lstm_encoder_hh_1.csv");
  Eigen::MatrixXd weight_lstm_decoder_ih_1 = read_csv(csv_dir + "/weight_lstm_decoder_ih_1.csv");
  Eigen::MatrixXd weight_lstm_decoder_hh_1 = read_csv(csv_dir + "/weight_lstm_decoder_hh_1.csv");
  Eigen::MatrixXd weight_post_decoder_0 = read_csv(csv_dir + "/weight_post_decoder_0.csv");
  Eigen::MatrixXd weight_post_decoder_1 = read_csv(csv_dir + "/weight_post_decoder_1.csv");
  Eigen::MatrixXd weight_final_layer = read_csv(csv_dir + "/weight_final_layer.csv");
  Eigen::VectorXd bias_pre_encoder_0 = read_csv(csv_dir + "/bias_pre_encoder_0.csv").col(0);
  Eigen::VectorXd bias_pre_encoder_1 = read_csv(csv_dir + "/bias_pre_encoder_1.csv").col(0);
  Eigen::VectorXd bias_lstm_encoder_ih_0 = read_csv(csv_dir + "/bias_lstm_encoder_ih_0.csv").col(0);
  Eigen::VectorXd bias_lstm_encoder_hh_0 = read_csv(csv_dir + "/bias_lstm_encoder_hh_0.csv").col(0);
  Eigen::VectorXd bias_lstm_decoder_ih_0 = read_csv(csv_dir + "/bias_lstm_decoder_ih_0.csv").col(0);
  Eigen::VectorXd bias_lstm_decoder_hh_0 = read_csv(csv_dir + "/bias_lstm_decoder_hh_0.csv").col(0);
  Eigen::VectorXd bias_lstm_encoder_ih_1 = read_csv(csv_dir + "/bias_lstm_encoder_ih_1.csv").col(0);
  Eigen::VectorXd bias_lstm_encoder_hh_1 = read_csv(csv_dir + "/bias_lstm_encoder_hh_1.csv").col(0);
  Eigen::VectorXd bias_lstm_decoder_ih_1 = read_csv(csv_dir + "/bias_lstm_decoder_ih_1.csv").col(0);
  Eigen::VectorXd bias_lstm_decoder_hh_1 = read_csv(csv_dir + "/bias_lstm_decoder_hh_1.csv").col(0);
  Eigen::VectorXd bias_post_decoder_0 = read_csv(csv_dir + "/bias_post_decoder_0.csv").col(0);
  Eigen::VectorXd bias_post_decoder_1 = read_csv(csv_dir + "/bias_post_decoder_1.csv").col(0);
  Eigen::VectorXd bias_final_layer = read_csv(csv_dir + "/bias_final_layer.csv").col(0);
  Eigen::VectorXd adaptive_scale;
  if (adaptive_scale_index < 0) {
    adaptive_scale = read_csv(csv_dir + "/adaptive_scale.csv").col(0);
  } else {
    adaptive_scale =
      read_csv(
        csv_dir + "/adaptive_scale_" + std::to_string(adaptive_scale_index) + "/adaptive_scale.csv")
        .col(0);
  }
  Eigen::MatrixXd vel_params = read_csv(csv_dir + "/vel_params.csv");
  double vel_scaling = vel_params(0, 0);
  double vel_bias = vel_params(1, 0);
  Eigen::MatrixXd limit = read_csv(csv_dir + "/limit.csv");
  double input_rate_lim = limit(0, 0);
  set_NN_params(
    weight_lstm_encoder_ih_0, weight_lstm_encoder_hh_0, weight_lstm_decoder_ih_0,
    weight_lstm_decoder_hh_0, weight_lstm_encoder_ih_1, weight_lstm_encoder_hh_1,
    weight_lstm_decoder_ih_1, weight_lstm_decoder_hh_1, weight_post_decoder_0,
    weight_post_decoder_1, weight_final_layer, bias_lstm_encoder_ih_0, bias_lstm_encoder_hh_0,
    bias_lstm_decoder_ih_0, bias_lstm_decoder_hh_0, bias_lstm_encoder_ih_1, bias_lstm_encoder_hh_1,
    bias_lstm_decoder_ih_1, bias_lstm_decoder_hh_1, bias_post_decoder_0, bias_post_decoder_1,
    bias_final_layer, adaptive_scale, input_step, output_step, input_rate_lim, vel_scaling,
    vel_bias, control_dt);
  initialized_ = false;
}
void InputsSchedulePrediction::set_NN_params(
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
  const Eigen::VectorXd & bias_final_layer, const Eigen::VectorXd & adaptive_scale, int input_step,
  int output_step, double input_rate_lim, double vel_scaling, double vel_bias, double control_dt)
{
  weight_lstm_encoder_ih_0_ = weight_lstm_encoder_ih_0;
  weight_lstm_encoder_hh_0_ = weight_lstm_encoder_hh_0;
  weight_lstm_decoder_ih_0_ = weight_lstm_decoder_ih_0;
  weight_lstm_decoder_hh_0_ = weight_lstm_decoder_hh_0;
  weight_lstm_encoder_ih_1_ = weight_lstm_encoder_ih_1;
  weight_lstm_encoder_hh_1_ = weight_lstm_encoder_hh_1;
  weight_lstm_decoder_ih_1_ = weight_lstm_decoder_ih_1;
  weight_lstm_decoder_hh_1_ = weight_lstm_decoder_hh_1;
  weight_post_decoder_0_ = weight_post_decoder_0;
  weight_post_decoder_1_ = weight_post_decoder_1;
  weight_final_layer_ = weight_final_layer;
  bias_lstm_encoder_ih_0_ = bias_lstm_encoder_ih_0;
  bias_lstm_encoder_hh_0_ = bias_lstm_encoder_hh_0;
  bias_lstm_decoder_ih_0_ = bias_lstm_decoder_ih_0;
  bias_lstm_decoder_hh_0_ = bias_lstm_decoder_hh_0;
  bias_lstm_encoder_ih_1_ = bias_lstm_encoder_ih_1;
  bias_lstm_encoder_hh_1_ = bias_lstm_encoder_hh_1;
  bias_lstm_decoder_ih_1_ = bias_lstm_decoder_ih_1;
  bias_lstm_decoder_hh_1_ = bias_lstm_decoder_hh_1;
  bias_post_decoder_0_ = bias_post_decoder_0;
  bias_post_decoder_1_ = bias_post_decoder_1;
  bias_final_layer_ = bias_final_layer;
  adaptive_scale_ = adaptive_scale;
  input_step_ = input_step;
  output_step_ = output_step;
  vel_scaling_ = vel_scaling;
  vel_bias_ = vel_bias;
  input_rate_lim_ = input_rate_lim;

  h_dim_ = weight_lstm_encoder_hh_0.cols();
  augmented_input_size_ = weight_lstm_decoder_ih_0.cols() - 1;
  control_dt_ = control_dt;
}
void InputsSchedulePrediction::update_encoder_cells(
  const Eigen::VectorXd & x, Eigen::VectorXd & h_lstm_encoder_0, Eigen::VectorXd & h_lstm_encoder_1,
  Eigen::VectorXd & c_lstm_encoder_0, Eigen::VectorXd & c_lstm_encoder_1)
{
  Eigen::VectorXd x_scaled = x;
  x_scaled[vel_index_] = (x[vel_index_] - vel_bias_) * vel_scaling_;
  Eigen::VectorXd i_f_g_o_0 = weight_lstm_encoder_ih_0_ * x_scaled + bias_lstm_encoder_ih_0_ +
                              weight_lstm_encoder_hh_0_ * h_lstm_encoder_0 +
                              bias_lstm_encoder_hh_0_;
  Eigen::VectorXd i_lstm_encoder_0 = sigmoid(i_f_g_o_0.segment(0, h_dim_));
  Eigen::VectorXd f_lstm_encoder_0 = sigmoid(i_f_g_o_0.segment(h_dim_, h_dim_));
  Eigen::VectorXd g_lstm_encoder_0 = tanh(i_f_g_o_0.segment(2 * h_dim_, h_dim_));
  Eigen::VectorXd o_lstm_encoder_0 = sigmoid(i_f_g_o_0.segment(3 * h_dim_, h_dim_));
  c_lstm_encoder_0 = f_lstm_encoder_0.array() * c_lstm_encoder_0.array() +
                     i_lstm_encoder_0.array() * g_lstm_encoder_0.array();
  h_lstm_encoder_0 = o_lstm_encoder_0.array() * tanh(c_lstm_encoder_0).array();

  Eigen::VectorXd i_f_g_o_1 =
    weight_lstm_encoder_ih_1_ * h_lstm_encoder_0 + bias_lstm_encoder_ih_1_ +
    weight_lstm_encoder_hh_1_ * h_lstm_encoder_1 + bias_lstm_encoder_hh_1_;
  Eigen::VectorXd i_lstm_encoder_1 = sigmoid(i_f_g_o_1.segment(0, h_dim_));
  Eigen::VectorXd f_lstm_encoder_1 = sigmoid(i_f_g_o_1.segment(h_dim_, h_dim_));
  Eigen::VectorXd g_lstm_encoder_1 = tanh(i_f_g_o_1.segment(2 * h_dim_, h_dim_));
  Eigen::VectorXd o_lstm_encoder_1 = sigmoid(i_f_g_o_1.segment(3 * h_dim_, h_dim_));
  c_lstm_encoder_1 = f_lstm_encoder_1.array() * c_lstm_encoder_1.array() +
                     i_lstm_encoder_1.array() * g_lstm_encoder_1.array();
  h_lstm_encoder_1 = o_lstm_encoder_1.array() * tanh(c_lstm_encoder_1).array();
}
void InputsSchedulePrediction::get_encoder_cells(
  const std::vector<Eigen::VectorXd> & x_seq, Eigen::VectorXd & h_lstm_encoder_0,
  Eigen::VectorXd & h_lstm_encoder_1, Eigen::VectorXd & c_lstm_encoder_0,
  Eigen::VectorXd & c_lstm_encoder_1)
{
  h_lstm_encoder_0 = Eigen::VectorXd::Zero(h_dim_);
  c_lstm_encoder_0 = Eigen::VectorXd::Zero(h_dim_);
  h_lstm_encoder_1 = Eigen::VectorXd::Zero(h_dim_);
  c_lstm_encoder_1 = Eigen::VectorXd::Zero(h_dim_);
  for (int i = 0; i < int(x_seq.size()); i++) {
    update_encoder_cells(
      x_seq[i], h_lstm_encoder_0, h_lstm_encoder_1, c_lstm_encoder_0, c_lstm_encoder_1);
  }
}
Eigen::VectorXd InputsSchedulePrediction::update_decoder_cells(
  const Eigen::VectorXd & decoder_input, Eigen::VectorXd & h_lstm_decoder_0,
  Eigen::VectorXd & h_lstm_decoder_1, Eigen::VectorXd & c_lstm_decoder_0,
  Eigen::VectorXd & c_lstm_decoder_1)
{
  Eigen::VectorXd i_f_g_o_0 = weight_lstm_decoder_ih_0_ * decoder_input + bias_lstm_decoder_ih_0_ +
                              weight_lstm_decoder_hh_0_ * h_lstm_decoder_0 +
                              bias_lstm_decoder_hh_0_;
  Eigen::VectorXd i_lstm_decoder_0 = sigmoid(i_f_g_o_0.segment(0, h_dim_));
  Eigen::VectorXd f_lstm_decoder_0 = sigmoid(i_f_g_o_0.segment(h_dim_, h_dim_));
  Eigen::VectorXd g_lstm_decoder_0 = tanh(i_f_g_o_0.segment(2 * h_dim_, h_dim_));
  Eigen::VectorXd o_lstm_decoder_0 = sigmoid(i_f_g_o_0.segment(3 * h_dim_, h_dim_));
  c_lstm_decoder_0 = f_lstm_decoder_0.array() * c_lstm_decoder_0.array() +
                     i_lstm_decoder_0.array() * g_lstm_decoder_0.array();
  h_lstm_decoder_0 = o_lstm_decoder_0.array() * tanh(c_lstm_decoder_0).array();
  Eigen::VectorXd i_f_g_o_1 =
    weight_lstm_decoder_ih_1_ * h_lstm_decoder_0 + bias_lstm_decoder_ih_1_ +
    weight_lstm_decoder_hh_1_ * h_lstm_decoder_1 + bias_lstm_decoder_hh_1_;
  Eigen::VectorXd i_lstm_decoder_1 = sigmoid(i_f_g_o_1.segment(0, h_dim_));
  Eigen::VectorXd f_lstm_decoder_1 = sigmoid(i_f_g_o_1.segment(h_dim_, h_dim_));
  Eigen::VectorXd g_lstm_decoder_1 = tanh(i_f_g_o_1.segment(2 * h_dim_, h_dim_));
  Eigen::VectorXd o_lstm_decoder_1 = sigmoid(i_f_g_o_1.segment(3 * h_dim_, h_dim_));
  c_lstm_decoder_1 = f_lstm_decoder_1.array() * c_lstm_decoder_1.array() +
                     i_lstm_decoder_1.array() * g_lstm_decoder_1.array();
  h_lstm_decoder_1 = o_lstm_decoder_1.array() * tanh(c_lstm_decoder_1).array();
  Eigen::VectorXd h_decoder =
    relu(weight_post_decoder_0_ * h_lstm_decoder_1 + bias_post_decoder_0_);
  Eigen::VectorXd h_decoder_scaled = h_decoder.array() * adaptive_scale_.array();
  h_decoder_scaled = relu(weight_post_decoder_1_ * h_decoder_scaled + bias_post_decoder_1_);
  Eigen::VectorXd output = weight_final_layer_ * h_decoder_scaled + bias_final_layer_;
  return output;
}
std::vector<double> InputsSchedulePrediction::predict(const std::vector<Eigen::VectorXd> & x_seq)
{
  Eigen::VectorXd h_lstm_encoder_0, h_lstm_encoder_1, c_lstm_encoder_0, c_lstm_encoder_1;
  get_encoder_cells(x_seq, h_lstm_encoder_0, h_lstm_encoder_1, c_lstm_encoder_0, c_lstm_encoder_1);
  // Eigen::VectorXd decoder_output = (x_seq[x_seq.size() - 1].tail(2) - x_seq[x_seq.size() -
  // 2].tail(2)) / control_dt_;
  Eigen::VectorXd output(augmented_input_size_ + 1);
  for (int i = 0; i < augmented_input_size_; i++) {
    output[i] = x_seq[x_seq.size() - augmented_input_size_ + i][1];
  }
  output[augmented_input_size_] =
    (x_seq[x_seq.size() - 1][1] - x_seq[x_seq.size() - 2][1]) / control_dt_;
  Eigen::VectorXd h_lstm_decoder_0 = h_lstm_encoder_0;
  Eigen::VectorXd c_lstm_decoder_0 = c_lstm_encoder_0;
  Eigen::VectorXd h_lstm_decoder_1 = h_lstm_encoder_1;
  Eigen::VectorXd c_lstm_decoder_1 = c_lstm_encoder_1;
  std::vector<double> output_seq;
  for (int i = 0; i < output_step_; i++) {
    Eigen::VectorXd decoder_output = tanh(update_decoder_cells(
      output, h_lstm_decoder_0, h_lstm_decoder_1, c_lstm_decoder_0, c_lstm_decoder_1));
    decoder_output[0] = input_rate_lim_ * decoder_output[0];
    output.head(augmented_input_size_ - 1) = output.segment(1, augmented_input_size_ - 1);
    output[augmented_input_size_ - 1] += decoder_output[0] * control_dt_;
    output.tail(1) = decoder_output;
    output_seq.push_back(output[augmented_input_size_ - 1]);
  }
  return output_seq;
}
std::vector<double> InputsSchedulePrediction::get_inputs_schedule_predicted(
  const Eigen::VectorXd & x, double timestamp)
{
  if (!initialized_) {
    x_seq_obs_.resize(input_step_);
    timestamps_obs_.resize(input_step_);
    for (int i = 0; i < input_step_; i++) {
      x_seq_obs_[i] = x;
      timestamps_obs_[i] = timestamp - control_dt_ * (input_step_ - i - 1);
    }
    initialized_ = true;
  } else {
    x_seq_obs_.push_back(x);
    timestamps_obs_.push_back(timestamp);
    if (timestamp - timestamps_obs_[1] > control_dt_ * (input_step_ - 1)) {
      x_seq_obs_.erase(x_seq_obs_.begin());
      timestamps_obs_.erase(timestamps_obs_.begin());
    }
  }
  std::vector<double> timestamp_for_prediction(input_step_);
  for (int i = 0; i < input_step_; i++) {
    timestamp_for_prediction[i] = timestamp - control_dt_ * (input_step_ - i - 1);
  }
  std::vector<Eigen::VectorXd> x_seq =
    interpolate_vector(x_seq_obs_, timestamps_obs_, timestamp_for_prediction);
  return predict(x_seq);
}
void InputsSchedulePrediction::send_initialized_flag()
{
  initialized_ = false;
}

PYBIND11_MODULE(inputs_prediction, m)
{
  py::class_<InputsSchedulePrediction>(m, "InputsSchedulePrediction")
    .def(py::init())
    .def("set_params", &InputsSchedulePrediction::set_params)
    .def("get_inputs_schedule_predicted", &InputsSchedulePrediction::get_inputs_schedule_predicted)
    .def("send_initialized_flag", &InputsSchedulePrediction::send_initialized_flag);
}
