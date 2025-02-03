#ifndef VEHICLE_ADAPTOR_UTILS_H_
#define VEHICLE_ADAPTOR_UTILS_H_
#include <Eigen/Core>
#include <Eigen/Dense>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

Eigen::VectorXd tanh(const Eigen::VectorXd & v);
Eigen::VectorXd d_tanh(const Eigen::VectorXd & v);
Eigen::VectorXd sigmoid(const Eigen::VectorXd & v);
Eigen::VectorXd relu(const Eigen::VectorXd & x);
Eigen::VectorXd d_relu(const Eigen::VectorXd & x);
Eigen::MatrixXd d_relu_product(const Eigen::MatrixXd & m, const Eigen::VectorXd & x);
Eigen::MatrixXd d_tanh_product(const Eigen::MatrixXd & m, const Eigen::VectorXd & x);
Eigen::VectorXd d_tanh_product_vec(const Eigen::VectorXd & v, const Eigen::VectorXd & x);
Eigen::MatrixXd d_sigmoid_product(const Eigen::MatrixXd & m, const Eigen::VectorXd & x);
Eigen::VectorXd d_sigmoid_product_vec(const Eigen::VectorXd & v, const Eigen::VectorXd & x);
Eigen::VectorXd rotate_data(Eigen::VectorXd states, double yaw);
Eigen::VectorXd vector_power(Eigen::VectorXd vec, int power);
double double_power(double val, int power);
double calc_table_value(
  double domain_value, std::vector<double> domain_table, std::vector<double> target_table);
std::string get_param_dir_path();
Eigen::MatrixXd read_csv(std::string file_path);
std::vector<std::string> read_string_csv(std::string file_path);
Eigen::VectorXd interpolate_eigen(
  Eigen::VectorXd y, std::vector<double> time_stamp_obs, std::vector<double> time_stamp_new);
std::vector<Eigen::VectorXd> interpolate_vector(
  std::vector<Eigen::VectorXd> y, std::vector<double> time_stamp_obs,
  std::vector<double> time_stamp_new);

class PolynomialRegressionPredictor
{
private:
  int degree_ = 2;
  int num_samples_ = 10;
  std::vector<double> lambda_ = {0.0, 0.0};
  Eigen::MatrixXd coef_matrix_, prediction_matrix_;
  bool ignore_intercept_ = false;
  double oldest_sample_weight_ = 1.0;

public:
  PolynomialRegressionPredictor();
  virtual ~PolynomialRegressionPredictor();
  void set_params(int degree, int num_samples, std::vector<double> lambda);
  void set_oldest_sample_weight(double oldest_sample_weight);
  void set_ignore_intercept();
  void calc_coef_matrix();
  void calc_prediction_matrix(int horizon_len);
  Eigen::VectorXd predict(Eigen::VectorXd vec);
};

class SgFilter
{
private:
  int degree_;
  int window_size_;
  std::vector<Eigen::VectorXd> sg_vector_left_edge_, sg_vector_right_edge_;
  Eigen::VectorXd sg_vector_center_;

  // Template function to handle both Eigen::MatrixXd and Eigen::VectorXd
  template <typename T>
  std::vector<T> sg_filter_impl(const std::vector<T> & raw_data)
  {
    int input_len = raw_data.size();
    std::vector<T> result;
    for (int i = 0; i < input_len; i++) {
      T tmp_filtered_vector = T::Zero(raw_data[0].rows(), raw_data[0].cols());
      if (i < window_size_) {
        for (int j = 0; j < i + window_size_ + 1; j++) {
          tmp_filtered_vector += sg_vector_left_edge_[i][j] * raw_data[j];
        }
      } else if (i > input_len - window_size_ - 1) {
        for (int j = 0; j < input_len - i + window_size_; j++) {
          tmp_filtered_vector +=
            sg_vector_right_edge_[input_len - i - 1][j] * raw_data[i - window_size_ + j];
        }
      } else {
        for (int j = 0; j < 2 * window_size_ + 1; j++) {
          tmp_filtered_vector += sg_vector_center_[j] * raw_data[i - window_size_ + j];
        }
      }
      result.push_back(tmp_filtered_vector);
    }
    return result;
  }

public:
  SgFilter();
  virtual ~SgFilter();
  void set_params(int degree, int window_size);
  void calc_sg_filter_weight();
  std::vector<Eigen::MatrixXd> sg_filter(const std::vector<Eigen::MatrixXd> & raw_data);
  std::vector<Eigen::VectorXd> sg_filter(const std::vector<Eigen::VectorXd> & raw_data);
};
class FilterDiffNN
{
private:
  int state_size_;
  int h_dim_;
  int acc_queue_size_;
  int steer_queue_size_;
  int predict_step_;
  double control_dt_;
  SgFilter sg_filter_;

public:
  FilterDiffNN();
  virtual ~FilterDiffNN();
  void set_sg_filter_params(
    int degree, int window_size, int state_size, int h_dim, int acc_queue_size,
    int steer_queue_size, int predict_step, double control_dt);
  void fit_transform_for_NN_diff(
    std::vector<Eigen::MatrixXd> A, std::vector<Eigen::MatrixXd> B, std::vector<Eigen::MatrixXd> C,
    std::vector<Eigen::MatrixXd> & dF_d_states, std::vector<Eigen::MatrixXd> & dF_d_inputs);
};
class ButterworthFilter
{
private:
  int order_;
  std::vector<double> a_;
  std::vector<double> b_;
  std::vector<Eigen::VectorXd> x_;
  std::vector<Eigen::VectorXd> y_;
  bool initialized_ = false;

public:
  ButterworthFilter();
  virtual ~ButterworthFilter();
  void set_params();
  Eigen::VectorXd apply(Eigen::VectorXd input_value);
};

#endif  // VEHICLE_ADAPTOR_UTILS_H_
