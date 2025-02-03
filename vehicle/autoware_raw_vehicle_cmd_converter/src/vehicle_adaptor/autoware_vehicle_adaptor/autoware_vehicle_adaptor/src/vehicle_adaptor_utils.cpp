#include "vehicle_adaptor_utils.h"

#include <yaml-cpp/yaml.h>

Eigen::VectorXd tanh(const Eigen::VectorXd & v)
{
  return v.array().tanh();
}
Eigen::VectorXd d_tanh(const Eigen::VectorXd & v)
{
  return 1 / (v.array().cosh() * v.array().cosh());
}
Eigen::VectorXd sigmoid(const Eigen::VectorXd & v)
{
  return 0.5 * (0.5 * v).array().tanh() + 0.5;
}
Eigen::VectorXd relu(const Eigen::VectorXd & x)
{
  Eigen::VectorXd x_ = x;
  for (int i = 0; i < x.size(); i++) {
    if (x[i] < 0) {
      x_[i] = 0;
    }
  }
  return x_;
}
Eigen::VectorXd d_relu(const Eigen::VectorXd & x)
{
  Eigen::VectorXd result = Eigen::VectorXd::Ones(x.size());
  for (int i = 0; i < x.size(); i++) {
    if (x[i] < 0) {
      result[i] = 0;
    }
  }
  return result;
}
Eigen::MatrixXd d_relu_product(const Eigen::MatrixXd & m, const Eigen::VectorXd & x)
{
  Eigen::MatrixXd result = Eigen::MatrixXd::Zero(m.rows(), m.cols());
  for (int i = 0; i < m.cols(); i++) {
    if (x[i] >= 0) {
      result.col(i) = m.col(i);
    }
  }
  return result;
}
Eigen::MatrixXd d_tanh_product(const Eigen::MatrixXd & m, const Eigen::VectorXd & x)
{
  Eigen::MatrixXd result = Eigen::MatrixXd(m.rows(), m.cols());
  for (int i = 0; i < m.cols(); i++) {
    result.col(i) = m.col(i) / (std::cosh(x[i]) * std::cosh(x[i]));
  }
  return result;
}
Eigen::VectorXd d_tanh_product_vec(const Eigen::VectorXd & v, const Eigen::VectorXd & x)
{
  Eigen::VectorXd result = Eigen::VectorXd(v.size());
  for (int i = 0; i < v.size(); i++) {
    result[i] = v[i] / (std::cosh(x[i]) * std::cosh(x[i]));
  }
  return result;
}
Eigen::MatrixXd d_sigmoid_product(const Eigen::MatrixXd & m, const Eigen::VectorXd & x)
{
  Eigen::MatrixXd result = Eigen::MatrixXd(m.rows(), m.cols());
  for (int i = 0; i < m.cols(); i++) {
    result.col(i) = 0.25 * m.col(i) / (std::cosh(0.5 * x[i]) * std::cosh(0.5 * x[i]));
  }
  return result;
}
Eigen::VectorXd d_sigmoid_product_vec(const Eigen::VectorXd & v, const Eigen::VectorXd & x)
{
  Eigen::VectorXd result = Eigen::VectorXd(v.size());
  for (int i = 0; i < v.size(); i++) {
    result[i] = 0.25 * v[i] / (std::cosh(0.5 * x[i]) * std::cosh(0.5 * x[i]));
  }
  return result;
}

Eigen::VectorXd rotate_data(Eigen::VectorXd states, double yaw)
{
  Eigen::VectorXd states_rotated = states;
  double cos_yaw = std::cos(yaw);
  double sin_yaw = std::sin(yaw);
  states_rotated[0] = states[0] * cos_yaw + states[1] * sin_yaw;
  states_rotated[1] = -states[1] * sin_yaw + states[0] * cos_yaw;
  return states_rotated;
}

Eigen::VectorXd vector_power(Eigen::VectorXd vec, int power)
{
  Eigen::VectorXd result = vec;
  for (int i = 0; i < power - 1; i++) {
    result = result.array() * vec.array();
  }
  return result;
}

double double_power(double val, int power)
{
  double result = val;
  for (int i = 0; i < power - 1; i++) {
    result = result * val;
  }
  return result;
}
double calc_table_value(
  double domain_value, std::vector<double> domain_table, std::vector<double> target_table)
{
  if (domain_value <= domain_table[0]) {
    return target_table[0];
  }
  if (domain_value >= domain_table[domain_table.size() - 1]) {
    return target_table[target_table.size() - 1];
  }
  for (int i = 0; i < int(domain_table.size()) - 1; i++) {
    if (domain_value >= domain_table[i] && domain_value <= domain_table[i + 1]) {
      return target_table[i] + (target_table[i + 1] - target_table[i]) /
                                 (domain_table[i + 1] - domain_table[i]) *
                                 (domain_value - domain_table[i]);
    }
  }
  return 0.0;
}

std::string get_param_dir_path()
{
  std::string build_path = BUILD_PATH;
  std::string param_dir_path = build_path + "/autoware_vehicle_adaptor/param";
  return param_dir_path;
}
Eigen::MatrixXd read_csv(std::string file_path)
{
  std::string build_path = BUILD_PATH;
  std::string file_path_abs = build_path + "/" + file_path;
  std::vector<std::vector<double>> csv_data;
  std::ifstream ifs(file_path_abs);
  if (!ifs) {
    std::cerr << "Failed to open file." << std::endl;
    return Eigen::MatrixXd(0, 0);
  }
  std::string line;
  while (std::getline(ifs, line)) {
    std::istringstream stream(line);
    std::string field;
    std::vector<double> fields;
    while (std::getline(stream, field, ',')) {
      fields.push_back(std::stod(field));
    }
    csv_data.push_back(fields);
  }
  ifs.close();
  Eigen::MatrixXd data = Eigen::MatrixXd(csv_data.size(), csv_data[0].size());
  for (int i = 0; i < int(csv_data.size()); i++) {
    for (int j = 0; j < int(csv_data[0].size()); j++) {
      data(i, j) = csv_data[i][j];
    }
  }
  return data;
}
std::vector<std::string> read_string_csv(std::string file_path)
{
  std::string build_path = BUILD_PATH;
  std::string file_path_abs = build_path + "/" + file_path;
  std::vector<std::string> csv_data;
  std::ifstream ifs(file_path_abs);
  if (!ifs) {
    std::cerr << "Failed to open file." << std::endl;
    return csv_data;
  }
  std::string line;
  while (std::getline(ifs, line)) {
    std::stringstream stream(line);
    std::string field;
    while (std::getline(stream, field, ',')) {
      field.erase(field.find_last_not_of(" \n\r\t") + 1);
      field.erase(0, field.find_first_not_of(" \n\r\t"));
      csv_data.push_back(field);
    }
  }
  ifs.close();
  return csv_data;
}
Eigen::VectorXd interpolate_eigen(
  Eigen::VectorXd y, std::vector<double> time_stamp_obs, std::vector<double> time_stamp_new)
{
  Eigen::VectorXd y_new = Eigen::VectorXd(time_stamp_new.size());
  int lower_bound_index = 0;
  for (int i = 0; i < int(time_stamp_new.size()); i++) {
    if (time_stamp_new[i] >= time_stamp_obs[time_stamp_obs.size() - 1]) {
      y_new[i] =
        y[time_stamp_obs.size() - 1] +
        (y[time_stamp_obs.size() - 1] - y[time_stamp_obs.size() - 2]) /
          (time_stamp_obs[time_stamp_obs.size() - 1] - time_stamp_obs[time_stamp_obs.size() - 2]) *
          (time_stamp_new[i] - time_stamp_obs[time_stamp_obs.size() - 1]);
      continue;
    }
    for (int j = lower_bound_index; j < int(time_stamp_obs.size()) - 1; j++) {
      if (time_stamp_new[i] >= time_stamp_obs[j] && time_stamp_new[i] <= time_stamp_obs[j + 1]) {
        y_new[i] = y[j] + (y[j + 1] - y[j]) / (time_stamp_obs[j + 1] - time_stamp_obs[j]) *
                            (time_stamp_new[i] - time_stamp_obs[j]);
        lower_bound_index = j;
        break;
      }
    }
  }
  return y_new;
}
std::vector<Eigen::VectorXd> interpolate_vector(
  std::vector<Eigen::VectorXd> y, std::vector<double> time_stamp_obs,
  std::vector<double> time_stamp_new)
{
  std::vector<Eigen::VectorXd> y_new;
  int lower_bound_index = 0;
  for (int i = 0; i < int(time_stamp_new.size()); i++) {
    if (time_stamp_new[i] >= time_stamp_obs[time_stamp_obs.size() - 1]) {
      y_new.push_back(
        y[time_stamp_obs.size() - 1] +
        (y[time_stamp_obs.size() - 1] - y[time_stamp_obs.size() - 2]) /
          (time_stamp_obs[time_stamp_obs.size() - 1] - time_stamp_obs[time_stamp_obs.size() - 2]) *
          (time_stamp_new[i] - time_stamp_obs[time_stamp_obs.size() - 1]));
      continue;
    }
    for (int j = lower_bound_index; j < int(time_stamp_obs.size()) - 1; j++) {
      if (time_stamp_new[i] >= time_stamp_obs[j] && time_stamp_new[i] <= time_stamp_obs[j + 1]) {
        y_new.push_back(
          y[j] + (y[j + 1] - y[j]) / (time_stamp_obs[j + 1] - time_stamp_obs[j]) *
                   (time_stamp_new[i] - time_stamp_obs[j]));
        lower_bound_index = j;
        break;
      }
    }
  }
  return y_new;
}

///////////////// Polynomial Regression ///////////////////////

PolynomialRegressionPredictor::PolynomialRegressionPredictor()
{
}
PolynomialRegressionPredictor::~PolynomialRegressionPredictor()
{
}
void PolynomialRegressionPredictor::set_params(
  int degree, int num_samples, std::vector<double> lambda)
{
  degree_ = degree;
  num_samples_ = num_samples;
  lambda_ = lambda;
}
void PolynomialRegressionPredictor::set_oldest_sample_weight(double oldest_sample_weight)
{
  oldest_sample_weight_ = oldest_sample_weight;
}
void PolynomialRegressionPredictor::set_ignore_intercept()
{
  ignore_intercept_ = true;
}
void PolynomialRegressionPredictor::calc_coef_matrix()
{
  if (ignore_intercept_) {
    Eigen::VectorXd time_vector =
      Eigen::VectorXd::LinSpaced(num_samples_ - 1, -(num_samples_ - 1), -1.0);
    Eigen::MatrixXd time_matrix = Eigen::MatrixXd::Zero(num_samples_ - 1, degree_);

    time_matrix.col(0) = time_vector;

    for (int i = 1; i < degree_; i++) {
      time_matrix.col(i) = time_matrix.col(i - 1).array() * time_vector.array();
    }
    Eigen::VectorXd weight_vector =
      Eigen::VectorXd::LinSpaced(num_samples_ - 1, oldest_sample_weight_, 1.0);
    Eigen::MatrixXd weight_matrix = weight_vector.asDiagonal();

    Eigen::MatrixXd regularization_matrix = Eigen::MatrixXd::Identity(degree_, degree_);
    for (int i = 0; i < degree_; i++) {
      regularization_matrix(i, i) = lambda_[i];
    }
    coef_matrix_ =
      (time_matrix.transpose() * weight_matrix * time_matrix + regularization_matrix).inverse() *
      time_matrix.transpose() * weight_matrix;
  } else {
    Eigen::VectorXd time_vector =
      Eigen::VectorXd::LinSpaced(num_samples_, -(num_samples_ - 1), 0.0);
    Eigen::MatrixXd time_matrix = Eigen::MatrixXd::Ones(num_samples_, degree_ + 1);
    for (int i = 1; i < degree_; i++) {
      time_matrix.col(i) = time_matrix.col(i - 1).array() * time_vector.array();
    }
    Eigen::VectorXd weight_vector =
      Eigen::VectorXd::LinSpaced(num_samples_, oldest_sample_weight_, 1.0);
    Eigen::MatrixXd weight_matrix = weight_vector.asDiagonal();

    Eigen::MatrixXd regularization_matrix = Eigen::MatrixXd::Identity(degree_ + 1, degree_ + 1);
    regularization_matrix(0, 0) = 0.0;
    for (int i = 0; i < degree_; i++) {
      regularization_matrix(i + 1, i + 1) = lambda_[i];
    }
    coef_matrix_ =
      (time_matrix.transpose() * weight_matrix * time_matrix + regularization_matrix).inverse() *
      time_matrix.transpose() * weight_matrix;
  }
}
void PolynomialRegressionPredictor::calc_prediction_matrix(int horizon_len)
{
  Eigen::VectorXd time_vector = Eigen::VectorXd::LinSpaced(horizon_len, 1.0, horizon_len);
  if (ignore_intercept_) {
    Eigen::MatrixXd prediction_time_matrix = Eigen::MatrixXd::Zero(horizon_len, degree_);
    prediction_time_matrix.col(0) = time_vector;
    for (int i = 1; i < degree_; i++) {
      prediction_time_matrix.col(i) =
        prediction_time_matrix.col(i - 1).array() * time_vector.array();
    }
    prediction_matrix_ = prediction_time_matrix * coef_matrix_;
  } else {
    Eigen::MatrixXd prediction_time_matrix = Eigen::MatrixXd::Ones(horizon_len, degree_ + 1);
    for (int i = 1; i < degree_ + 1; i++) {
      prediction_time_matrix.col(i) =
        prediction_time_matrix.col(i - 1).array() * time_vector.array();
    }
    prediction_matrix_ = prediction_time_matrix * coef_matrix_;
  }
}
Eigen::VectorXd PolynomialRegressionPredictor::predict(Eigen::VectorXd vec)
{
  if (ignore_intercept_) {
    Eigen::VectorXd vec_ = Eigen::VectorXd(vec.size() - 1);
    vec_ = vec.head(vec.size() - 1);
    vec_ = vec_.array() - vec[vec.size() - 1];
    Eigen::VectorXd prediction = prediction_matrix_ * vec_;
    prediction = prediction.array() + vec[vec.size() - 1];
    return prediction;
  } else {
    Eigen::VectorXd vec_ = Eigen::VectorXd(vec.size());
    vec_ = vec.array() - vec[vec.size() - 1];
    Eigen::VectorXd prediction = prediction_matrix_ * vec;
    prediction = prediction.array() + vec[vec.size() - 1];
    return prediction;
  }
}

///////////////// SgFilter ///////////////////////

SgFilter::SgFilter()
{
}
SgFilter::~SgFilter()
{
}
void SgFilter::set_params(int degree, int window_size)
{
  degree_ = degree;
  window_size_ = window_size;
}
void SgFilter::calc_sg_filter_weight()
{
  Eigen::VectorXd e_0 = Eigen::VectorXd::Zero(degree_ + 1);
  e_0[0] = 1.0;
  sg_vector_left_edge_ = std::vector<Eigen::VectorXd>(window_size_);
  sg_vector_right_edge_ = std::vector<Eigen::VectorXd>(window_size_);
  for (int i = 0; i < window_size_; i++) {
    Eigen::VectorXd time_vector =
      Eigen::VectorXd::LinSpaced(window_size_ + i + 1, -i, window_size_);
    Eigen::MatrixXd time_matrix = Eigen::MatrixXd::Ones(window_size_ + i + 1, degree_ + 1);
    for (int j = 1; j < degree_ + 1; j++) {
      time_matrix.col(j) = time_matrix.col(j - 1).array() * time_vector.array();
    }
    sg_vector_left_edge_[i] = time_matrix * (time_matrix.transpose() * time_matrix).inverse() * e_0;
    time_vector = Eigen::VectorXd::LinSpaced(window_size_ + i + 1, -window_size_, i);
    time_matrix = Eigen::MatrixXd::Ones(window_size_ + i + 1, degree_ + 1);
    for (int j = 1; j < degree_ + 1; j++) {
      time_matrix.col(j) = time_matrix.col(j - 1).array() * time_vector.array();
    }
    sg_vector_right_edge_[i] =
      time_matrix * (time_matrix.transpose() * time_matrix).inverse() * e_0;
  }
  Eigen::VectorXd time_vector =
    Eigen::VectorXd::LinSpaced(2 * window_size_ + 1, -window_size_, window_size_);
  Eigen::MatrixXd time_matrix = Eigen::MatrixXd::Ones(2 * window_size_ + 1, degree_ + 1);
  for (int j = 1; j < degree_ + 1; j++) {
    time_matrix.col(j) = time_matrix.col(j - 1).array() * time_vector.array();
  }
  sg_vector_center_ = time_matrix * (time_matrix.transpose() * time_matrix).inverse() * e_0;
}
// Overloaded function for std::vector<Eigen::MatrixXd>
std::vector<Eigen::MatrixXd> SgFilter::sg_filter(const std::vector<Eigen::MatrixXd> & raw_data)
{
  return sg_filter_impl(raw_data);
}

// Overloaded function for std::vector<Eigen::VectorXd>
std::vector<Eigen::VectorXd> SgFilter::sg_filter(const std::vector<Eigen::VectorXd> & raw_data)
{
  return sg_filter_impl(raw_data);
}

///////////////// FilterDiffNN ///////////////////////
FilterDiffNN::FilterDiffNN()
{
}
FilterDiffNN::~FilterDiffNN()
{
}
void FilterDiffNN::set_sg_filter_params(
  int degree, int window_size, int state_size, int h_dim, int acc_queue_size, int steer_queue_size,
  int predict_step, double control_dt)
{
  state_size_ = state_size;
  h_dim_ = h_dim;
  acc_queue_size_ = acc_queue_size;
  steer_queue_size_ = steer_queue_size;
  predict_step_ = predict_step;
  control_dt_ = control_dt;
  sg_filter_.set_params(degree, window_size);
  sg_filter_.calc_sg_filter_weight();
}
void FilterDiffNN::fit_transform_for_NN_diff(
  std::vector<Eigen::MatrixXd> A, std::vector<Eigen::MatrixXd> B, std::vector<Eigen::MatrixXd> C,
  std::vector<Eigen::MatrixXd> & dF_d_states, std::vector<Eigen::MatrixXd> & dF_d_inputs)
{
  int num_samples = A.size();
  dF_d_states = std::vector<Eigen::MatrixXd>(num_samples);
  dF_d_inputs = std::vector<Eigen::MatrixXd>(num_samples);
  std::vector<Eigen::MatrixXd> dF_d_state_with_input_history;
  dF_d_state_with_input_history = sg_filter_.sg_filter(C);
  for (int i = 0; i < num_samples; i++) {
    Eigen::MatrixXd dF_d_state_temp = Eigen::MatrixXd::Zero(
      2 * h_dim_ + state_size_, 2 * h_dim_ + state_size_ + acc_queue_size_ + steer_queue_size_);

    Eigen::MatrixXd dF_d_input_temp = Eigen::MatrixXd::Zero(2 * h_dim_ + state_size_, 2);
    Eigen::MatrixXd dF_d_state_with_input_history_tmp = dF_d_state_with_input_history[i];

    dF_d_state_temp.block(
      2 * h_dim_, 2 * h_dim_, state_size_, state_size_ + acc_queue_size_ + steer_queue_size_) =
      A[i];

    dF_d_input_temp.block(2 * h_dim_, 0, state_size_, 2) = B[i];

    dF_d_state_temp.leftCols(2 * h_dim_ + state_size_ + acc_queue_size_) +=
      dF_d_state_with_input_history_tmp.leftCols(2 * h_dim_ + state_size_ + acc_queue_size_);

    dF_d_state_temp.rightCols(steer_queue_size_) += dF_d_state_with_input_history_tmp.middleCols(
      2 * h_dim_ + state_size_ + acc_queue_size_ + predict_step_, steer_queue_size_);
    for (int j = 0; j < predict_step_; j++) {
      dF_d_state_temp.col(2 * h_dim_ + state_size_ + acc_queue_size_ - 1) +=
        dF_d_state_with_input_history_tmp.col(2 * h_dim_ + state_size_ + acc_queue_size_ + j);
      dF_d_state_temp.col(2 * h_dim_ + state_size_ + acc_queue_size_ + steer_queue_size_ - 1) +=
        dF_d_state_with_input_history_tmp.col(
          2 * h_dim_ + state_size_ + acc_queue_size_ + predict_step_ + steer_queue_size_ + j);
      dF_d_input_temp.col(0) +=
        (j + 1) *
        dF_d_state_with_input_history_tmp.col(2 * h_dim_ + state_size_ + acc_queue_size_ + j) *
        control_dt_;
      dF_d_input_temp.col(1) +=
        (j + 1) *
        dF_d_state_with_input_history_tmp.col(
          2 * h_dim_ + state_size_ + acc_queue_size_ + predict_step_ + steer_queue_size_ + j) *
        control_dt_;
    }
    dF_d_states[i] = dF_d_state_temp;
    dF_d_inputs[i] = dF_d_input_temp;
  }
}
///////////////// ButterworthFilter ///////////////////////

ButterworthFilter::ButterworthFilter()
{
  set_params();
}
ButterworthFilter::~ButterworthFilter()
{
}
void ButterworthFilter::set_params()
{
  YAML::Node butterworth_coef_node =
    YAML::LoadFile(get_param_dir_path() + "/butterworth_coef.yaml");
  order_ = butterworth_coef_node["Butterworth"]["order"].as<int>();
  a_ = butterworth_coef_node["Butterworth"]["a"].as<std::vector<double>>();
  b_ = butterworth_coef_node["Butterworth"]["b"].as<std::vector<double>>();
  initialized_ = false;
}
Eigen::VectorXd ButterworthFilter::apply(Eigen::VectorXd input_value)
{
  if (!initialized_) {
    x_ = std::vector<Eigen::VectorXd>(order_, input_value);
    y_ = std::vector<Eigen::VectorXd>(order_, input_value);
    initialized_ = true;
  }
  Eigen::VectorXd output_value = b_[0] * input_value;
  for (int i = 0; i < order_; i++) {
    output_value += b_[order_ - i] * x_[i] - a_[order_ - i] * y_[i];
  }
  x_.erase(x_.begin());
  x_.push_back(input_value);
  y_.erase(y_.begin());
  y_.push_back(output_value);
  return output_value;
}
