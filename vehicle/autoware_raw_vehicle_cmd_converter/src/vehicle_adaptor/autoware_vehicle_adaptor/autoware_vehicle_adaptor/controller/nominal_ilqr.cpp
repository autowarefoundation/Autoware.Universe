#include "nominal_dynamics.h"

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
Eigen::RowVectorXd calc_table_value(
  Eigen::RowVectorXd domain_values, std::vector<double> domain_table,
  std::vector<double> target_table)
{
  Eigen::RowVectorXd result = Eigen::RowVectorXd(domain_values.size());
  for (int i = 0; i < domain_values.size(); i++) {
    result[i] = calc_table_value(domain_values[i], domain_table, target_table);
  }
  return result;
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
double transform_yaw(double yaw_old, double yaw_new)
{
  int rotate_num = int(std::round((yaw_new - yaw_old) / (2 * std::numbers::pi)));
  return yaw_new - 2 * std::numbers::pi * rotate_num;
}
std::string get_param_dir_path()
{
  std::string build_path = BUILD_PATH;
  std::string param_dir_path = build_path + "/autoware_vehicle_adaptor/param/nominal_ilqr";
  return param_dir_path;
}
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
  SgFilter() {}
  virtual ~SgFilter() {}
  void set_params(int degree, int window_size)
  {
    degree_ = degree;
    window_size_ = window_size;
  }
  void calc_sg_filter_weight()
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
      sg_vector_left_edge_[i] =
        time_matrix * (time_matrix.transpose() * time_matrix).inverse() * e_0;
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
  std::vector<Eigen::MatrixXd> sg_filter(const std::vector<Eigen::MatrixXd> & raw_data)
  {
    return sg_filter_impl(raw_data);
  }
  // Overloaded function for std::vector<Eigen::VectorXd>
  std::vector<Eigen::VectorXd> sg_filter(const std::vector<Eigen::VectorXd> & raw_data)
  {
    return sg_filter_impl(raw_data);
  }
};
class NominalILQR
{
private:
  NominalDynamics nominal_dynamics_;
  int x_index_ = 0;
  int y_index_ = 1;
  int vel_index_ = 2;
  int yaw_index_ = 3;
  int acc_index_ = 4;
  int steer_index_ = 5;
  int state_size_ = 6;
  double wheel_base_ = 2.79;
  double acc_time_delay_ = 0.1;
  double steer_time_delay_ = 0.27;
  double acc_time_constant_ = 0.1;
  double steer_time_constant_ = 0.24;
  double control_dt_ = 0.033;
  int predict_step_ = 3;
  double predict_dt_ = control_dt_ * predict_step_;
  int acc_delay_step_ = int(std::round(acc_time_delay_ / control_dt_));
  int steer_delay_step_ = int(std::round(steer_time_delay_ / control_dt_));
  int acc_queue_size_ = std::max(acc_delay_step_, predict_step_ + 1);
  int steer_queue_size_ = std::max(steer_delay_step_, predict_step_ + 1);
  double steer_dead_band_ = 0.0;

  double x_cost_ = 0.0;
  double y_cost_ = 10.0;
  double vel_cost_ = 0.0;
  double yaw_cost_ = 0.0;
  double acc_cost_ = 0.0;
  double steer_cost_ = 0.0;

  double acc_rate_cost_ = 10.0;
  double steer_rate_cost_ = 1000.0;

  double x_terminal_cost_ = 1e+2;
  double y_terminal_cost_ = 1e+8;
  double vel_terminal_cost_ = 1e+6;
  double yaw_terminal_cost_ = 1e+3;
  double acc_terminal_cost_ = 1.0;
  double steer_terminal_cost_ = 1.0;

  int intermediate_cost_index_ = 25;
  double x_intermediate_cost_ = 1e+2;
  double y_intermediate_cost_ = 1e+8;
  double vel_intermediate_cost_ = 1e+2;
  double yaw_intermediate_cost_ = 1e+7;
  double acc_intermediate_cost_ = 1.0;
  double steer_intermediate_cost_ = 1.0;

  std::vector<double> vel_steer_cost_coef_table_ = {0.01, 0.01, 0.08, 0.36, 1.0};
  std::vector<double> vel_steer_table_ = {0.01, 0.1, 0.2, 0.27, 0.33};
  std::vector<double> lateral_cost_coef_table_ = {0.00001, 0.0005, 0.06, 0.27, 1.0};
  std::vector<double> lateral_error_table_ = {0.01, 0.1, 0.15, 0.175, 0.2};
  std::vector<double> yaw_cost_coef_table_ = {0.00001, 1.0};
  std::vector<double> yaw_error_table_ = {0.00001, 0.1};

  std::vector<double> steer_rate_cost_table_ = {10.0, 5.0, 1.0};
  std::vector<double> curvature_table_ = {0.01, 0.03, 0.05};
  int horizon_len_ = 50;

  std::vector<double> reference_speed_table_ = {20.0, 30.0};
  std::vector<double> steer_lim_table_ = {10.0, 10.0};
  std::vector<double> steer_rate_lim_table_ = {10.0, 10.0};
  std::vector<double> acc_lim_table_ = {20.0, 20.0};
  std::vector<double> acc_rate_lim_table_ = {20.0, 20.0};
  std::vector<double> lateral_acc_lim_table_ = {20.0, 20.0};
  std::vector<double> lateral_acc_rate_lim_table_ = {20.0, 20.0};

  double acc_lim_cost_ = 1.0;
  double acc_rate_lim_cost_ = 1.0;
  double steer_lim_cost_ = 1.0;
  double steer_rate_lim_cost_ = 1.0;

public:
  NominalILQR() {}
  virtual ~NominalILQR() {}
  void set_params_from_yaml()
  {
    YAML::Node mpc_param_node = YAML::LoadFile(get_param_dir_path() + "/mpc_param.yaml");
    x_cost_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["x_cost"].as<double>();
    y_cost_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["y_cost"].as<double>();
    vel_cost_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["vel_cost"].as<double>();
    yaw_cost_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["yaw_cost"].as<double>();
    acc_cost_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_cost"].as<double>();
    steer_cost_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_cost"].as<double>();
    x_terminal_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["x_terminal_cost"].as<double>();
    y_terminal_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["y_terminal_cost"].as<double>();
    vel_terminal_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["vel_terminal_cost"].as<double>();
    yaw_terminal_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["yaw_terminal_cost"].as<double>();
    acc_terminal_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_terminal_cost"].as<double>();
    steer_terminal_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_terminal_cost"].as<double>();
    intermediate_cost_index_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["intermediate_cost_index"].as<int>();
    x_intermediate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["x_intermediate_cost"].as<double>();
    y_intermediate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["y_intermediate_cost"].as<double>();
    vel_intermediate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["vel_intermediate_cost"].as<double>();
    yaw_intermediate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["yaw_intermediate_cost"].as<double>();
    acc_intermediate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_intermediate_cost"].as<double>();
    steer_intermediate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_intermediate_cost"].as<double>();
    acc_rate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_rate_cost"].as<double>();
    steer_rate_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_rate_cost"].as<double>();

    vel_steer_cost_coef_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["vel_steer_cost_coef_table"]
        .as<std::vector<double>>();
    vel_steer_table_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["vel_steer_table"]
                         .as<std::vector<double>>();
    lateral_cost_coef_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["lateral_cost_coef_table"]
        .as<std::vector<double>>();
    lateral_error_table_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["lateral_error_table"]
                             .as<std::vector<double>>();
    yaw_cost_coef_table_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["yaw_cost_coef_table"]
                             .as<std::vector<double>>();
    yaw_error_table_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["yaw_error_table"]
                         .as<std::vector<double>>();
    steer_rate_cost_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_rate_cost_table"]
        .as<std::vector<double>>();
    curvature_table_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["curvature_table"]
                         .as<std::vector<double>>();
    reference_speed_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["reference_speed_table"]
        .as<std::vector<double>>();
    steer_lim_table_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_lim_table"]
                         .as<std::vector<double>>();
    steer_rate_lim_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_rate_lim_table"]
        .as<std::vector<double>>();
    acc_lim_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_lim_table"].as<std::vector<double>>();
    acc_rate_lim_table_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_rate_lim_table"]
                            .as<std::vector<double>>();
    lateral_acc_lim_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["lateral_acc_lim_table"]
        .as<std::vector<double>>();
    lateral_acc_rate_lim_table_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["lateral_acc_rate_lim_table"]
        .as<std::vector<double>>();
    acc_lim_cost_ = mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_lim_cost"].as<double>();
    acc_rate_lim_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["acc_rate_lim_cost"].as<double>();
    steer_lim_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_lim_cost"].as<double>();
    steer_rate_lim_cost_ =
      mpc_param_node["mpc_parameter"]["cost_parameters"]["steer_rate_lim_cost"].as<double>();
  }
  void set_params(
    double wheel_base, double acc_time_delay, double steer_time_delay, double acc_time_constant,
    double steer_time_constant, double control_dt, int predict_step, double steer_dead_band,
    int horizon_len)
  {
    wheel_base_ = wheel_base;
    acc_time_delay_ = acc_time_delay;
    steer_time_delay_ = steer_time_delay;
    acc_time_constant_ = acc_time_constant;
    steer_time_constant_ = steer_time_constant;
    control_dt_ = control_dt;
    predict_step_ = predict_step;
    predict_dt_ = control_dt_ * predict_step_;
    acc_delay_step_ = int(std::round(acc_time_delay_ / control_dt_));
    steer_delay_step_ = int(std::round(steer_time_delay_ / control_dt_));
    acc_queue_size_ = std::max(acc_delay_step_, predict_step_ + 1);
    steer_queue_size_ = std::max(steer_delay_step_, predict_step_ + 1);
    steer_dead_band_ = steer_dead_band;
    nominal_dynamics_.set_params(
      wheel_base, acc_time_delay, steer_time_delay, acc_time_constant, steer_time_constant,
      acc_queue_size_, steer_queue_size_, control_dt, predict_step);
    nominal_dynamics_.set_steer_dead_band(steer_dead_band_);
    horizon_len_ = horizon_len;
  }

  double calc_steer_rate_cost_coef(const std::vector<Eigen::VectorXd> & states_ref)
  {
    double max_curvature = 0.0;
    for (int i = 0; i < int(states_ref.size()); i++) {
      double curvature_i = std::abs(states_ref[i][steer_index_]);
      if (curvature_i > max_curvature) {
        max_curvature = curvature_i;
      }
    }
    return calc_table_value(max_curvature, curvature_table_, steer_rate_cost_table_);
  }
  void calc_limits(
    double vel, double acc, double steer, double & steer_lim, double & steer_rate_lim_lb,
    double & steer_rate_lim_ub, double & acc_lim, double & acc_rate_lim)
  {
    steer_lim = calc_table_value(vel, reference_speed_table_, steer_lim_table_);
    double steer_rate_lim = calc_table_value(vel, reference_speed_table_, steer_rate_lim_table_);
    acc_lim = calc_table_value(vel, reference_speed_table_, acc_lim_table_);
    acc_rate_lim = calc_table_value(vel, reference_speed_table_, acc_rate_lim_table_);
    double lateral_acc_lim = calc_table_value(vel, reference_speed_table_, lateral_acc_lim_table_);
    double lateral_acc_rate_lim =
      calc_table_value(vel, reference_speed_table_, lateral_acc_rate_lim_table_);
    double lateral_acc_rate_lim_coef =
      wheel_base_ * std::cos(steer) * std::cos(steer) / (vel * vel + 1e-10);
    double lateral_acc_rate_lim_const = vel * acc * std::sin(2 * steer) / (vel * vel + 1e-10);

    steer_lim = std::min(steer_lim, std::atan(lateral_acc_lim * wheel_base_ / (vel * vel + 1e-10)));

    double steer_rate_lim_lb_by_lateral_acc_rate_lim = std::min(
      -lateral_acc_rate_lim * lateral_acc_rate_lim_coef + lateral_acc_rate_lim_const, -0.01);
    double steer_rate_lim_ub_by_lateral_acc_rate_lim =
      std::max(lateral_acc_rate_lim * lateral_acc_rate_lim_coef - lateral_acc_rate_lim_const, 0.01);
    steer_rate_lim_lb = std::max(-steer_rate_lim, steer_rate_lim_lb_by_lateral_acc_rate_lim);
    steer_rate_lim_ub = std::min(steer_rate_lim, steer_rate_lim_ub_by_lateral_acc_rate_lim);
  }
  void calc_forward_trajectory_with_diff(
    Eigen::VectorXd states, Eigen::VectorXd acc_input_history, Eigen::VectorXd steer_input_history,
    std::vector<Eigen::VectorXd> d_inputs_schedule,
    std::vector<Eigen::VectorXd> & states_prediction,
    std::vector<Eigen::MatrixXd> & dF_d_states_with_history,
    std::vector<Eigen::MatrixXd> & dF_d_inputs, std::vector<double> & acc_inputs,
    std::vector<double> & steer_inputs)
  {
    states_prediction.resize(horizon_len_ + 1);
    dF_d_states_with_history.resize(horizon_len_);
    dF_d_inputs.resize(horizon_len_);
    acc_inputs.resize(horizon_len_ + 1);
    steer_inputs.resize(horizon_len_ + 1);

    states_prediction[0] = states;
    Eigen::VectorXd tmp_acc_input_history = acc_input_history;
    Eigen::VectorXd tmp_steer_input_history = steer_input_history;
    acc_inputs[0] = tmp_acc_input_history[acc_queue_size_ - 1];
    steer_inputs[0] = tmp_steer_input_history[steer_queue_size_ - 1];
    for (int i = 0; i < horizon_len_; i++) {
      Eigen::Vector2d d_inputs = d_inputs_schedule[i];
      states_prediction[i + 1] = nominal_dynamics_.F_with_input_history_and_diff(
        states_prediction[i], tmp_acc_input_history, tmp_steer_input_history, d_inputs,
        dF_d_states_with_history[i], dF_d_inputs[i]);
      acc_inputs[i + 1] = tmp_acc_input_history[acc_queue_size_ - 1];
      steer_inputs[i + 1] = tmp_steer_input_history[steer_queue_size_ - 1];
    }
  }
  void calc_forward_trajectory_with_cost(
    Eigen::VectorXd states, Eigen::VectorXd acc_input_history, Eigen::VectorXd steer_input_history,
    std::vector<Eigen::MatrixXd> D_inputs_schedule,
    std::vector<Eigen::MatrixXd> & states_prediction, std::vector<Eigen::VectorXd> states_ref,
    std::vector<Eigen::VectorXd> d_inputs_ref, double steer_rate_cost_coef_by_curvature,
    Eigen::VectorXd & Cost)
  {
    int sample_size = D_inputs_schedule[0].cols();
    Cost = Eigen::VectorXd::Zero(sample_size);
    states_prediction.resize(horizon_len_ + 1);
    states_prediction[0] = states.replicate(1, sample_size);
    Eigen::MatrixXd Acc_input_history = acc_input_history.replicate(1, sample_size);
    Eigen::MatrixXd Steer_input_history = steer_input_history.replicate(1, sample_size);
    for (int i = 0; i < horizon_len_ + 1; i++) {
      double x_cost = x_cost_;
      double y_cost = y_cost_;
      double vel_cost = vel_cost_;
      double yaw_cost = yaw_cost_;
      double steer_cost = steer_cost_;
      double acc_cost = acc_cost_;
      if (i == intermediate_cost_index_) {
        x_cost = x_intermediate_cost_;
        y_cost = y_intermediate_cost_;
        vel_cost = vel_intermediate_cost_;
        yaw_cost = yaw_intermediate_cost_;
        steer_cost = steer_intermediate_cost_;
        acc_cost = acc_intermediate_cost_;
      }
      if (i == horizon_len_) {
        x_cost = x_terminal_cost_;
        y_cost = y_terminal_cost_;
        vel_cost = vel_terminal_cost_;
        yaw_cost = yaw_terminal_cost_;
        steer_cost = steer_terminal_cost_;
        acc_cost = acc_terminal_cost_;
      }
      double yaw_target = states_ref[i][yaw_index_];
      double vel_target_abs = states_ref[i][vel_index_];
      double steer_rate_cost_coef =
        steer_rate_cost_coef_by_curvature *
        calc_table_value(vel_target_abs, vel_steer_table_, vel_steer_cost_coef_table_);
      Eigen::Matrix2d rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
      Eigen::MatrixXd xy_diff = rotation_matrix.transpose() *
                                (states_prediction[i].block(x_index_, 0, 2, sample_size) -
                                 states_ref[i].segment(x_index_, 2).replicate(1, sample_size));
      Eigen::RowVectorXd longitudinal_error = xy_diff.row(0);
      Eigen::RowVectorXd lateral_error = xy_diff.row(1);
      Eigen::RowVectorXd lateral_error_abs = lateral_error.array().abs();
      Eigen::RowVectorXd yaw_error =
        states_prediction[i].row(yaw_index_).array() - states_ref[i][yaw_index_];
      Eigen::RowVectorXd yaw_error_abs = yaw_error.array().abs();
      Eigen::RowVectorXd lateral_cost_coef =
        calc_table_value(lateral_error_abs, lateral_error_table_, lateral_cost_coef_table_);
      Eigen::RowVectorXd yaw_cost_coef =
        calc_table_value(yaw_error_abs, yaw_error_table_, yaw_cost_coef_table_);

      Cost += 0.5 * x_cost * longitudinal_error.array().square().matrix();
      Cost +=
        0.5 * y_cost * (lateral_error.array() * lateral_cost_coef.array().sqrt()).square().matrix();
      Cost += 0.5 * steer_rate_cost_coef * vel_cost *
              (states_prediction[i].row(vel_index_).array() - states_ref[i][vel_index_])
                .square()
                .matrix();
      Cost += 0.5 * yaw_cost * (yaw_error.array() * yaw_cost_coef.array().sqrt()).square().matrix();
      Cost += 0.5 * acc_cost *
              (states_prediction[i].row(acc_index_).array() - states_ref[i][acc_index_])
                .square()
                .matrix();
      Cost += 0.5 * steer_cost *
              (states_prediction[i].row(steer_index_).array() - states_ref[i][steer_index_])
                .square()
                .matrix();
      if (i < horizon_len_) {
        Eigen::MatrixXd D_inputs = D_inputs_schedule[i];
        Cost +=
          0.5 * acc_rate_cost_ * (D_inputs.row(0).array() - d_inputs_ref[i][0]).square().matrix();
        Cost +=
          0.5 * steer_rate_cost_ * (D_inputs.row(1).array() - d_inputs_ref[i][1]).square().matrix();
        states_prediction[i + 1] = nominal_dynamics_.F_with_input_history_for_candidates(
          states_prediction[i], Acc_input_history, Steer_input_history, D_inputs);
      }
      for (int j = 0; j < sample_size; j++) {
        double steer_lim, steer_rate_lim_lb, steer_rate_lim_ub, acc_lim, acc_rate_lim;
        calc_limits(
          states_prediction[i](vel_index_, j), states_prediction[i](acc_index_, j),
          states_prediction[i](steer_index_, j), steer_lim, steer_rate_lim_lb, steer_rate_lim_ub,
          acc_lim, acc_rate_lim);
        if (Acc_input_history(acc_queue_size_ - 1, j) > acc_lim) {
          Cost[j] += 0.5 * acc_lim_cost_ * (Acc_input_history(acc_queue_size_ - 1, j) - acc_lim) *
                     (Acc_input_history(acc_queue_size_ - 1, j) - acc_lim);
        }
        if (Acc_input_history(acc_queue_size_ - 1, j) < -acc_lim) {
          Cost[j] += 0.5 * acc_lim_cost_ * (Acc_input_history(acc_queue_size_ - 1, j) + acc_lim) *
                     (Acc_input_history(acc_queue_size_ - 1, j) + acc_lim);
        }
        if (Steer_input_history(steer_queue_size_ - 1, j) > steer_lim) {
          Cost[j] += 0.5 * steer_lim_cost_ *
                     (Steer_input_history(steer_queue_size_ - 1, j) - steer_lim) *
                     (Steer_input_history(steer_queue_size_ - 1, j) - steer_lim);
        }
        if (Steer_input_history(steer_queue_size_ - 1, j) < -steer_lim) {
          Cost[j] += 0.5 * steer_lim_cost_ *
                     (Steer_input_history(steer_queue_size_ - 1, j) + steer_lim) *
                     (Steer_input_history(steer_queue_size_ - 1, j) + steer_lim);
        }
        if (i < horizon_len_) {
          Eigen::MatrixXd D_inputs = D_inputs_schedule[i];
          if (D_inputs(0, j) > acc_rate_lim) {
            Cost[j] += 0.5 * acc_rate_lim_cost_ * (D_inputs(0, j) - acc_rate_lim) *
                       (D_inputs(0, j) - acc_rate_lim);
          }
          if (D_inputs(0, j) < -acc_rate_lim) {
            Cost[j] += 0.5 * acc_rate_lim_cost_ * (D_inputs(0, j) + acc_rate_lim) *
                       (D_inputs(0, j) + acc_rate_lim);
          }
          if (D_inputs(1, j) > steer_rate_lim_ub) {
            Cost[j] += 0.5 * steer_rate_lim_cost_ * (D_inputs(1, j) - steer_rate_lim_ub) *
                       (D_inputs(1, j) - steer_rate_lim_ub);
          }
          if (D_inputs(1, j) < steer_rate_lim_lb) {
            Cost[j] += 0.5 * steer_rate_lim_cost_ * (D_inputs(1, j) - steer_rate_lim_lb) *
                       (D_inputs(1, j) - steer_rate_lim_lb);
          }
        }
      }
    }
  }
  Eigen::MatrixXd right_action_by_state_diff_with_history(
    Eigen::MatrixXd Mat, Eigen::MatrixXd dF_d_states)
  {
    Eigen::MatrixXd result = Eigen::MatrixXd::Zero(Mat.rows(), dF_d_states.cols());
    result = Mat.leftCols(state_size_) * dF_d_states;
    result.middleCols(state_size_ + predict_step_, acc_queue_size_ - predict_step_) +=
      Mat.middleCols(state_size_, acc_queue_size_ - predict_step_);
    result.middleCols(
      state_size_ + acc_queue_size_ + predict_step_, steer_queue_size_ - predict_step_) +=
      Mat.middleCols(state_size_ + acc_queue_size_, steer_queue_size_ - predict_step_);
    for (int i = 0; i < predict_step_; i++) {
      result.col(state_size_ + acc_queue_size_ - 1) +=
        Mat.col(state_size_ + acc_queue_size_ - predict_step_ + i);
      result.col(state_size_ + acc_queue_size_ + steer_queue_size_ - 1) +=
        Mat.col(state_size_ + acc_queue_size_ + steer_queue_size_ - predict_step_ + i);
    }
    return result;
  }
  Eigen::MatrixXd left_action_by_state_diff_with_history(
    Eigen::MatrixXd dF_d_states, Eigen::MatrixXd Mat)
  {
    Eigen::MatrixXd result = Eigen::MatrixXd::Zero(dF_d_states.cols(), Mat.cols());
    result = dF_d_states.transpose() * Mat.topRows(state_size_);
    result.middleRows(state_size_ + predict_step_, acc_queue_size_ - predict_step_) +=
      Mat.middleRows(state_size_, acc_queue_size_ - predict_step_);
    result.middleRows(
      state_size_ + acc_queue_size_ + predict_step_, steer_queue_size_ - predict_step_) +=
      Mat.middleRows(state_size_ + acc_queue_size_, steer_queue_size_ - predict_step_);
    for (int i = 0; i < predict_step_; i++) {
      result.row(state_size_ + acc_queue_size_ - 1) +=
        Mat.row(state_size_ + acc_queue_size_ - predict_step_ + i);
      result.row(state_size_ + acc_queue_size_ + steer_queue_size_ - 1) +=
        Mat.row(state_size_ + acc_queue_size_ + steer_queue_size_ - predict_step_ + i);
    }
    return result;
  }
  Eigen::MatrixXd right_action_by_input_diff(Eigen::MatrixXd Mat, Eigen::MatrixXd dF_d_inputs)
  {
    Eigen::MatrixXd result = Eigen::MatrixXd::Zero(Mat.rows(), dF_d_inputs.cols());
    result = Mat.leftCols(state_size_) * dF_d_inputs;
    for (int i = 0; i < predict_step_; i++) {
      result.col(0) +=
        (i + 1) * Mat.col(state_size_ + acc_queue_size_ - predict_step_ + i) * control_dt_;
      result.col(1) +=
        (i + 1) * Mat.col(state_size_ + acc_queue_size_ + steer_queue_size_ - predict_step_ + i) *
        control_dt_;
    }
    return result;
  }
  Eigen::MatrixXd left_action_by_input_diff(Eigen::MatrixXd dF_d_inputs, Eigen::MatrixXd Mat)
  {
    Eigen::MatrixXd result = Eigen::MatrixXd::Zero(dF_d_inputs.cols(), Mat.cols());
    result = dF_d_inputs.transpose() * Mat.topRows(state_size_);
    for (int i = 0; i < predict_step_; i++) {
      result.row(0) +=
        (i + 1) * Mat.row(state_size_ + acc_queue_size_ - predict_step_ + i) * control_dt_;
      result.row(1) +=
        (i + 1) * Mat.row(state_size_ + acc_queue_size_ + steer_queue_size_ - predict_step_ + i) *
        control_dt_;
    }
    return result;
  }
  void compute_ilqr_coefficients(
    const std::vector<Eigen::MatrixXd> & dF_d_states,
    const std::vector<Eigen::MatrixXd> & dF_d_inputs,
    const std::vector<Eigen::VectorXd> & states_prediction,
    const std::vector<Eigen::VectorXd> & d_inputs_schedule,
    const std::vector<Eigen::VectorXd> & states_ref,
    const std::vector<Eigen::VectorXd> & d_input_ref, double steer_rate_cost_coef_by_curvature,
    std::vector<double> acc_inputs, std::vector<double> steer_inputs,
    std::vector<Eigen::MatrixXd> & K, std::vector<Eigen::VectorXd> & k)
  {
    K = std::vector<Eigen::MatrixXd>(horizon_len_);
    k = std::vector<Eigen::VectorXd>(horizon_len_);
    Eigen::MatrixXd P = Eigen::MatrixXd::Zero(
      state_size_ + acc_queue_size_ + steer_queue_size_,
      state_size_ + acc_queue_size_ + steer_queue_size_);
    Eigen::VectorXd w = Eigen::VectorXd::Zero(state_size_ + acc_queue_size_ + steer_queue_size_);
    double yaw_target = states_ref[horizon_len_][yaw_index_];
    Eigen::Matrix2d rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
    double vel_target_abs = states_ref[horizon_len_][vel_index_];
    double steer_rate_cost_coef =
      steer_rate_cost_coef_by_curvature *
      calc_table_value(vel_target_abs, vel_steer_table_, vel_steer_cost_coef_table_);
    Eigen::Vector2d xy_diff = Eigen::Vector2d(
      states_prediction[horizon_len_][x_index_] - states_ref[horizon_len_][x_index_],
      states_prediction[horizon_len_][y_index_] - states_ref[horizon_len_][y_index_]);
    double lateral_error_abs = std::abs((rotation_matrix.transpose() * xy_diff)[1]);
    double yaw_error_abs =
      std::abs(states_prediction[horizon_len_][yaw_index_] - states_ref[horizon_len_][yaw_index_]);
    double lateral_cost_coef =
      calc_table_value(lateral_error_abs, lateral_error_table_, lateral_cost_coef_table_);
    double yaw_cost_coef = calc_table_value(yaw_error_abs, yaw_error_table_, yaw_cost_coef_table_);
    Eigen::Matrix2d xy_cost_matrix = rotation_matrix.transpose() *
                                     Eigen::DiagonalMatrix<double, 2>(Eigen::Vector2d(
                                       x_terminal_cost_, y_terminal_cost_ * lateral_cost_coef)) *
                                     rotation_matrix;

    P.block(0, 0, 2, 2) = xy_cost_matrix;
    w.head(2) = xy_cost_matrix * xy_diff;
    P(vel_index_, vel_index_) = steer_rate_cost_coef * vel_terminal_cost_;
    w(vel_index_) =
      steer_rate_cost_coef * vel_terminal_cost_ *
      (states_prediction[horizon_len_][vel_index_] - states_ref[horizon_len_][vel_index_]);
    P(yaw_index_, yaw_index_) = yaw_terminal_cost_ * yaw_cost_coef;
    w(yaw_index_) =
      yaw_terminal_cost_ *
      (states_prediction[horizon_len_][yaw_index_] - states_ref[horizon_len_][yaw_index_]) *
      yaw_cost_coef;
    P(acc_index_, acc_index_) = acc_terminal_cost_;
    w(acc_index_) = acc_terminal_cost_ * (states_prediction[horizon_len_][acc_index_] -
                                          states_ref[horizon_len_][acc_index_]);
    P(steer_index_, steer_index_) = steer_terminal_cost_;
    w(steer_index_) = steer_terminal_cost_ * (states_prediction[horizon_len_][steer_index_] -
                                              states_ref[horizon_len_][steer_index_]);

    double steer_lim, steer_rate_lim_lb, steer_rate_lim_ub, acc_lim, acc_rate_lim;
    calc_limits(
      states_prediction[horizon_len_][vel_index_], states_prediction[horizon_len_][acc_index_],
      states_prediction[horizon_len_][steer_index_], steer_lim, steer_rate_lim_lb,
      steer_rate_lim_ub, acc_lim, acc_rate_lim);
    if (acc_inputs[horizon_len_] > acc_lim) {
      P(acc_queue_size_ - 1, acc_queue_size_ - 1) += acc_lim_cost_;
      w(acc_queue_size_ - 1) += acc_lim_cost_ * (acc_inputs[horizon_len_] - acc_lim);
    }
    if (acc_inputs[horizon_len_] < -acc_lim) {
      P(acc_queue_size_ - 1, acc_queue_size_ - 1) += acc_lim_cost_;
      w(acc_queue_size_ - 1) += acc_lim_cost_ * (acc_inputs[horizon_len_] + acc_lim);
    }
    if (steer_inputs[horizon_len_] > steer_lim) {
      P(acc_queue_size_ + steer_queue_size_ - 1, acc_queue_size_ + steer_queue_size_ - 1) +=
        steer_lim_cost_;
      w(acc_queue_size_ + steer_queue_size_ - 1) +=
        steer_lim_cost_ * (steer_inputs[horizon_len_] - steer_lim);
    }
    if (steer_inputs[horizon_len_] < -steer_lim) {
      P(acc_queue_size_ + steer_queue_size_ - 1, acc_queue_size_ + steer_queue_size_ - 1) +=
        steer_lim_cost_;
      w(acc_queue_size_ + steer_queue_size_ - 1) +=
        steer_lim_cost_ * (steer_inputs[horizon_len_] + steer_lim);
    }

    for (int i = horizon_len_ - 1; i >= 0; i--) {
      calc_limits(
        states_prediction[i][vel_index_], states_prediction[i][acc_index_],
        states_prediction[i][steer_index_], steer_lim, steer_rate_lim_lb, steer_rate_lim_ub,
        acc_lim, acc_rate_lim);
      Eigen::MatrixXd Pi1Ai = right_action_by_state_diff_with_history(P, dF_d_states[i]);
      Eigen::MatrixXd G = left_action_by_input_diff(dF_d_inputs[i], Pi1Ai);
      Eigen::MatrixXd H =
        left_action_by_input_diff(dF_d_inputs[i], right_action_by_input_diff(P, dF_d_inputs[i]));

      H(0, 0) += acc_rate_cost_;
      H(1, 1) += steer_rate_cost_;

      Eigen::VectorXd g_ = left_action_by_input_diff(dF_d_inputs[i], w);
      g_(0) += acc_rate_cost_ * (d_inputs_schedule[i][0] - d_input_ref[i][0]);
      g_(1) += steer_rate_cost_ * (d_inputs_schedule[i][1] - d_input_ref[i][1]);
      if (d_inputs_schedule[i][0] > acc_rate_lim) {
        H(0, 0) += acc_rate_lim_cost_;
        g_(0) += acc_rate_lim_cost_ * (d_inputs_schedule[i][0] - acc_rate_lim);
      }
      if (d_inputs_schedule[i][0] < -acc_rate_lim) {
        H(0, 0) += acc_rate_lim_cost_;
        g_(0) += acc_rate_lim_cost_ * (d_inputs_schedule[i][0] + acc_rate_lim);
      }
      if (d_inputs_schedule[i][1] > steer_rate_lim_ub) {
        H(1, 1) += steer_rate_lim_cost_;
        g_(1) += steer_rate_lim_cost_ * (d_inputs_schedule[i][1] - steer_rate_lim_ub);
      }
      if (d_inputs_schedule[i][1] < steer_rate_lim_lb) {
        H(1, 1) += steer_rate_lim_cost_;
        g_(1) += steer_rate_lim_cost_ * (d_inputs_schedule[i][1] - steer_rate_lim_lb);
      }

      Eigen::MatrixXd invH = H.inverse();
      k[i] = invH * g_;
      K[i] = invH * G;
      P = left_action_by_state_diff_with_history(dF_d_states[i], Pi1Ai) - G.transpose() * K[i];
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
      w = left_action_by_state_diff_with_history(dF_d_states[i], w);
      w -= G.transpose() * k[i];
      yaw_target = states_ref[i][yaw_index_];
      vel_target_abs = states_ref[i][vel_index_];
      steer_rate_cost_coef =
        steer_rate_cost_coef_by_curvature *
        calc_table_value(vel_target_abs, vel_steer_table_, vel_steer_cost_coef_table_);
      rotation_matrix = Eigen::Rotation2Dd(-yaw_target).toRotationMatrix();
      xy_diff = Eigen::Vector2d(
        states_prediction[i][x_index_] - states_ref[i][x_index_],
        states_prediction[i][y_index_] - states_ref[i][y_index_]);
      lateral_error_abs = std::abs((rotation_matrix.transpose() * xy_diff)[1]);
      yaw_error_abs = std::abs(states_prediction[i][yaw_index_] - states_ref[i][yaw_index_]);
      lateral_cost_coef =
        calc_table_value(lateral_error_abs, lateral_error_table_, lateral_cost_coef_table_);
      yaw_cost_coef = calc_table_value(yaw_error_abs, yaw_error_table_, yaw_cost_coef_table_);
      xy_cost_matrix =
        rotation_matrix.transpose() *
        Eigen::DiagonalMatrix<double, 2>(Eigen::Vector2d(x_cost, y_cost * lateral_cost_coef)) *
        rotation_matrix;

      P.block(0, 0, 2, 2) += xy_cost_matrix;
      w.segment(0, 2) += xy_cost_matrix * xy_diff;
      P(vel_index_, vel_index_) += steer_rate_cost_coef * vel_cost;
      w(vel_index_) += steer_rate_cost_coef * vel_cost *
                       (states_prediction[i][vel_index_] - states_ref[i][vel_index_]);
      P(yaw_index_, yaw_index_) += yaw_cost * yaw_cost_coef;
      w(yaw_index_) +=
        yaw_cost * (states_prediction[i][yaw_index_] - states_ref[i][yaw_index_]) * yaw_cost_coef;
      P(acc_index_, acc_index_) += acc_cost;
      w(acc_index_) += acc_cost * (states_prediction[i][acc_index_] - states_ref[i][acc_index_]);
      P(steer_index_, steer_index_) += steer_cost;
      w(steer_index_) +=
        steer_cost * (states_prediction[i][steer_index_] - states_ref[i][steer_index_]);
      if (acc_inputs[i] > acc_lim) {
        P(acc_queue_size_ - 1, acc_queue_size_ - 1) += acc_lim_cost_;
        w(acc_queue_size_ - 1) += acc_lim_cost_ * (acc_inputs[i] - acc_lim);
      }
      if (acc_inputs[i] < -acc_lim) {
        P(acc_queue_size_ - 1, acc_queue_size_ - 1) += acc_lim_cost_;
        w(acc_queue_size_ - 1) += acc_lim_cost_ * (acc_inputs[i] + acc_lim);
      }
      if (steer_inputs[i] > steer_lim) {
        P(acc_queue_size_ + steer_queue_size_ - 1, acc_queue_size_ + steer_queue_size_ - 1) +=
          steer_lim_cost_;
        w(acc_queue_size_ + steer_queue_size_ - 1) +=
          steer_lim_cost_ * (steer_inputs[i] - steer_lim);
      }
      if (steer_inputs[i] < -steer_lim) {
        P(acc_queue_size_ + steer_queue_size_ - 1, acc_queue_size_ + steer_queue_size_ - 1) +=
          steer_lim_cost_;
        w(acc_queue_size_ + steer_queue_size_ - 1) +=
          steer_lim_cost_ * (steer_inputs[i] + steer_lim);
      }
    }
  }
  std::vector<Eigen::MatrixXd> calc_line_search_candidates(
    std::vector<Eigen::MatrixXd> K, std::vector<Eigen::VectorXd> k,
    std::vector<Eigen::MatrixXd> dF_d_states, std::vector<Eigen::MatrixXd> dF_d_inputs,
    std::vector<Eigen::VectorXd> d_inputs_schedule, Eigen::VectorXd ls_points)
  {
    std::vector<Eigen::MatrixXd> D_inputs_schedule(horizon_len_);
    Eigen::MatrixXd D_states =
      Eigen::MatrixXd::Zero(state_size_ + acc_queue_size_ + steer_queue_size_, ls_points.size());
    for (int i = 0; i < horizon_len_; i++) {
      Eigen::MatrixXd D_D_inputs = -k[i] * ls_points.transpose() - K[i] * D_states;
      D_inputs_schedule[i] = D_D_inputs + d_inputs_schedule[i].replicate(1, ls_points.size());
      Eigen::MatrixXd D_states_temp =
        Eigen::MatrixXd::Zero(state_size_ + acc_queue_size_ + steer_queue_size_, ls_points.size());
      D_states_temp.topRows(state_size_) += dF_d_states[i] * D_states + dF_d_inputs[i] * D_D_inputs;
      D_states_temp.middleRows(state_size_, acc_queue_size_ - predict_step_) +=
        D_states.middleRows(state_size_ + predict_step_, acc_queue_size_ - predict_step_);
      D_states_temp.middleRows(state_size_ + acc_queue_size_, steer_queue_size_ - predict_step_) +=
        D_states.middleRows(
          state_size_ + acc_queue_size_ + predict_step_, steer_queue_size_ - predict_step_);
      for (int j = 0; j < predict_step_; j++) {
        D_states_temp.row(state_size_ + acc_queue_size_ - predict_step_ + j) +=
          D_states.row(state_size_ + acc_queue_size_ - 1);
        D_states_temp.row(state_size_ + acc_queue_size_ + steer_queue_size_ - predict_step_ + j) +=
          D_states.row(state_size_ + acc_queue_size_ + steer_queue_size_ - 1);
        D_states_temp.row(state_size_ + acc_queue_size_ - predict_step_ + j) +=
          (j + 1) * control_dt_ * D_D_inputs.row(0);
        D_states_temp.row(state_size_ + acc_queue_size_ + steer_queue_size_ - predict_step_ + j) +=
          (j + 1) * control_dt_ * D_D_inputs.row(1);
      }
      D_states = D_states_temp;
    }
    return D_inputs_schedule;
  }
  void compute_optimal_control(
    Eigen::VectorXd states, Eigen::VectorXd acc_input_history, Eigen::VectorXd steer_input_history,
    std::vector<Eigen::VectorXd> & d_inputs_schedule,
    const std::vector<Eigen::VectorXd> & states_ref,
    const std::vector<Eigen::VectorXd> & d_inputs_ref,
    std::vector<Eigen::VectorXd> & states_prediction, double & acc_input, double & steer_input)
  {
    std::vector<Eigen::MatrixXd> dF_d_states, dF_d_inputs;
    std::vector<double> acc_inputs;
    std::vector<double> steer_inputs;
    calc_forward_trajectory_with_diff(
      states, acc_input_history, steer_input_history, d_inputs_schedule, states_prediction,
      dF_d_states, dF_d_inputs, acc_inputs, steer_inputs);
    std::vector<Eigen::MatrixXd> K;
    std::vector<Eigen::VectorXd> k;
    double steer_rate_cost_coef = calc_steer_rate_cost_coef(states_ref);
    compute_ilqr_coefficients(
      dF_d_states, dF_d_inputs, states_prediction, d_inputs_schedule, states_ref, d_inputs_ref,
      steer_rate_cost_coef, acc_inputs, steer_inputs, K, k);
    Eigen::VectorXd ls_points = Eigen::VectorXd::LinSpaced(11, 0.0, 1.0);

    std::vector<Eigen::MatrixXd> D_inputs_schedule =
      calc_line_search_candidates(K, k, dF_d_states, dF_d_inputs, d_inputs_schedule, ls_points);
    Eigen::VectorXd Cost;
    std::vector<Eigen::MatrixXd> States_prediction;

    calc_forward_trajectory_with_cost(
      states, acc_input_history, steer_input_history, D_inputs_schedule, States_prediction,
      states_ref, d_inputs_ref, steer_rate_cost_coef, Cost);
    int min_index;
    Cost.minCoeff(&min_index);

    for (int i = 0; i < horizon_len_; i++) {
      d_inputs_schedule[i] = D_inputs_schedule[i].col(min_index);
      states_prediction[i + 1] = States_prediction[i + 1].col(min_index);
    }
    // get result
    acc_input = acc_input_history[acc_queue_size_ - 1] + d_inputs_schedule[0][0] * control_dt_;
    steer_input =
      steer_input_history[steer_queue_size_ - 1] + d_inputs_schedule[0][1] * control_dt_;
  }
};
class NominalILQRController
{
private:
  NominalILQR nominal_ilqr_;
  int x_index_ = 0;
  int y_index_ = 1;
  int vel_index_ = 2;
  int yaw_index_ = 3;
  int acc_index_ = 4;
  int steer_index_ = 5;
  double wheel_base_ = 2.79;
  double acc_time_delay_ = 0.1;
  double steer_time_delay_ = 0.27;
  double acc_time_constant_ = 0.1;
  double steer_time_constant_ = 0.24;

  double control_dt_ = 0.033;
  int predict_step_ = 3;
  double predict_dt_ = control_dt_ * predict_step_;

  int acc_delay_step_ = int(std::round(acc_time_delay_ / control_dt_));
  int steer_delay_step_ = int(std::round(steer_time_delay_ / control_dt_));
  int acc_queue_size_ = std::max(acc_delay_step_, predict_step_ + 1);
  int steer_queue_size_ = std::max(steer_delay_step_, predict_step_ + 1);

  int acc_input_start_index_ = 6;
  int acc_input_end_index_ = 6 + acc_queue_size_ - 1;
  int steer_input_start_index_ = 6 + acc_queue_size_;
  int steer_input_end_index_ = 6 + acc_queue_size_ + steer_queue_size_ - 1;
  double steer_dead_band_ = 0.0;

  int max_queue_size_ = std::max(acc_queue_size_, steer_queue_size_);

  std::vector<Eigen::VectorXd> d_inputs_schedule_;
  std::vector<Eigen::VectorXd> states_prediction_;
  Eigen::VectorXd acc_input_history_;
  Eigen::VectorXd steer_input_history_;
  std::vector<double> acc_input_history_obs_, steer_input_history_obs_;
  std::vector<double> time_stamp_obs_;
  bool initialized_ = false;
  int horizon_len_ = 50;

  int sg_window_size_for_d_inputs_schedule_ = 10;
  int sg_deg_for_d_inputs_schedule_ = 0;
  SgFilter sg_filter_for_d_inputs_schedule_;

public:
  NominalILQRController() { set_params_from_yaml(); }
  virtual ~NominalILQRController() {}
  void set_params_from_yaml()
  {
    YAML::Node vehicle_param_node = YAML::LoadFile(get_param_dir_path() + "/vehicle_param.yaml");
    wheel_base_ =
      vehicle_param_node["vehicle_parameter"]["vehicle_info"]["wheel_base"].as<double>();
    acc_time_delay_ =
      vehicle_param_node["vehicle_parameter"]["acceleration"]["acc_time_delay"].as<double>();
    steer_time_delay_ =
      vehicle_param_node["vehicle_parameter"]["steering"]["steer_time_delay"].as<double>();
    acc_time_constant_ =
      vehicle_param_node["vehicle_parameter"]["acceleration"]["acc_time_constant"].as<double>();
    steer_time_constant_ =
      vehicle_param_node["vehicle_parameter"]["steering"]["steer_time_constant"].as<double>();
    steer_dead_band_ =
      vehicle_param_node["vehicle_parameter"]["steering"]["steer_dead_band"].as<double>();
    YAML::Node mpc_param_node = YAML::LoadFile(get_param_dir_path() + "/mpc_param.yaml");
    control_dt_ = mpc_param_node["mpc_parameter"]["setting"]["control_dt"].as<double>();
    predict_step_ = mpc_param_node["mpc_parameter"]["setting"]["predict_step"].as<int>();
    horizon_len_ = mpc_param_node["mpc_parameter"]["setting"]["horizon_len"].as<int>();
    nominal_ilqr_.set_params(
      wheel_base_, acc_time_delay_, steer_time_delay_, acc_time_constant_, steer_time_constant_,
      control_dt_, predict_step_, steer_dead_band_, horizon_len_);
    nominal_ilqr_.set_params_from_yaml();
    sg_deg_for_d_inputs_schedule_ =
      mpc_param_node["mpc_parameter"]["sg_filter"]["sg_deg_for_d_inputs_schedule"].as<int>();
    sg_window_size_for_d_inputs_schedule_ =
      mpc_param_node["mpc_parameter"]["sg_filter"]["sg_window_size_for_d_inputs_schedule"]
        .as<int>();
    acc_delay_step_ = int(std::round(acc_time_delay_ / control_dt_));
    steer_delay_step_ = int(std::round(steer_time_delay_ / control_dt_));
    acc_queue_size_ = std::max(acc_delay_step_, predict_step_ + 1);
    steer_queue_size_ = std::max(steer_delay_step_, predict_step_ + 1);
  }
  void set_params(
    double wheel_base, double acc_time_delay, double steer_time_delay, double acc_time_constant,
    double steer_time_constant, double control_dt, int predict_step, double steer_dead_band,
    int horizon_len)
  {
    nominal_ilqr_.set_params(
      wheel_base, acc_time_delay, steer_time_delay, acc_time_constant, steer_time_constant,
      control_dt, predict_step, steer_dead_band, horizon_len);
    wheel_base_ = wheel_base;
    acc_time_delay_ = acc_time_delay;
    steer_time_delay_ = steer_time_delay;
    acc_time_constant_ = acc_time_constant;
    steer_time_constant_ = steer_time_constant;
    control_dt_ = control_dt;
    predict_step_ = predict_step;
    acc_delay_step_ = int(std::round(acc_time_delay_ / control_dt_));
    steer_delay_step_ = int(std::round(steer_time_delay_ / control_dt_));
    acc_queue_size_ = std::max(acc_delay_step_, predict_step_ + 1);
    steer_queue_size_ = std::max(steer_delay_step_, predict_step_ + 1);
    steer_dead_band_ = steer_dead_band;
    horizon_len_ = horizon_len;
    acc_input_start_index_ = 6;
    acc_input_end_index_ = 6 + acc_queue_size_ - 1;
    steer_input_start_index_ = 6 + acc_queue_size_;
    steer_input_end_index_ = 6 + acc_queue_size_ + steer_queue_size_ - 1;
  }
  void transform_states_ref(
    const Eigen::VectorXd & states, const Eigen::MatrixXd & states_ref,
    std::vector<Eigen::VectorXd> & states_ref_transformed)
  {
    Eigen::Vector2d initial_position_error = states_ref.block(0, 0, 2, 1) - states.head(2);
    // double initial_vel_error = states_ref[0][vel_index_] - states[vel_index_];
    double initial_yaw_error =
      transform_yaw(states[yaw_index_], states_ref(yaw_index_, 0)) - states[yaw_index_];
    states_ref_transformed.resize(states_ref.cols());
    states_ref_transformed[0] = states_ref.col(0);
    states_ref_transformed[0].head(2) = states.head(2);
    // states_ref_transformed[0][vel_index_] = states[vel_index_];
    states_ref_transformed[0][yaw_index_] = states[yaw_index_];
    for (int i = 0; i < int(states_ref.cols()) - 1; i++) {
      Eigen::VectorXd tmp_states_ref = states_ref.col(i + 1);
      double ratio = (int(states_ref.cols()) - i - 2.0) / (int(states_ref.cols()) - 1.0);
      tmp_states_ref.head(2) -= ratio * initial_position_error;
      // tmp_states_ref[vel_index_] -= ratio * initial_vel_error;
      tmp_states_ref[yaw_index_] = transform_yaw(
        states_ref(yaw_index_, i), tmp_states_ref[yaw_index_] - ratio * initial_yaw_error);
      states_ref_transformed[i + 1] = tmp_states_ref;
    }
  }
  void initialize_d_input_schedule(
    Eigen::Vector2d acc_steer, std::vector<Eigen::VectorXd> states_ref,
    std::vector<Eigen::VectorXd> & d_input_schedule)
  {
    Eigen::Vector2d initial_error_scaled = (states_ref[0].segment(acc_index_, 2) - acc_steer) /
                                           (predict_step_ * control_dt_ * horizon_len_);
    d_input_schedule.resize(horizon_len_);
    for (int i = 0; i < horizon_len_; i++) {
      d_input_schedule[i] =
        (states_ref[i + 1].segment(acc_index_, 2) - states_ref[i].segment(acc_index_, 2)) /
          (predict_step_ * control_dt_) -
        initial_error_scaled * (horizon_len_ - i);
    }
  }
  Eigen::VectorXd compute_optimal_control(
    double time_stamp, const Eigen::VectorXd & states, const Eigen::MatrixXd & states_ref)
  {
    std::vector<Eigen::VectorXd> states_ref_transformed;
    transform_states_ref(states, states_ref, states_ref_transformed);
    if (!initialized_) {
      acc_input_history_obs_ = std::vector<double>(max_queue_size_ + 1, states[acc_index_]);
      steer_input_history_obs_ = std::vector<double>(max_queue_size_ + 1, states[steer_index_]);
      initialize_d_input_schedule(
        states.segment(acc_index_, 2), states_ref_transformed, d_inputs_schedule_);
      time_stamp_obs_.resize(max_queue_size_ + 1);
      for (int i = 0; i < max_queue_size_ + 1; i++) {
        time_stamp_obs_[i] = time_stamp - (max_queue_size_ + 1 - i) * control_dt_;
      }
      sg_window_size_for_d_inputs_schedule_ =
        std::min(sg_window_size_for_d_inputs_schedule_, int(horizon_len_ / 2) - 1);
      sg_filter_for_d_inputs_schedule_.set_params(
        sg_deg_for_d_inputs_schedule_, sg_window_size_for_d_inputs_schedule_);
      sg_filter_for_d_inputs_schedule_.calc_sg_filter_weight();
      initialized_ = true;
    } else {
      if (time_stamp - time_stamp_obs_[1] > max_queue_size_ * control_dt_) {
        acc_input_history_obs_.erase(acc_input_history_obs_.begin());
        steer_input_history_obs_.erase(steer_input_history_obs_.begin());
        time_stamp_obs_.erase(time_stamp_obs_.begin());
      }
    }
    std::vector<Eigen::VectorXd> d_inputs_ref(horizon_len_, Eigen::VectorXd::Zero(2));
    d_inputs_schedule_ = sg_filter_for_d_inputs_schedule_.sg_filter(d_inputs_schedule_);
    std::vector<double> time_stamp_acc_input_history(acc_queue_size_);
    for (int i = 0; i < acc_queue_size_; i++) {
      time_stamp_acc_input_history[i] = time_stamp - (acc_queue_size_ - i) * control_dt_;
    }
    std::vector<double> time_stamp_steer_input_history(steer_queue_size_);
    for (int i = 0; i < steer_queue_size_; i++) {
      time_stamp_steer_input_history[i] = time_stamp - (steer_queue_size_ - i) * control_dt_;
    }
    Eigen::Map<Eigen::VectorXd> acc_input_history_obs_eigen(
      acc_input_history_obs_.data(), acc_input_history_obs_.size());
    Eigen::Map<Eigen::VectorXd> steer_input_history_obs_eigen(
      steer_input_history_obs_.data(), steer_input_history_obs_.size());
    acc_input_history_ =
      interpolate_eigen(acc_input_history_obs_eigen, time_stamp_obs_, time_stamp_acc_input_history);
    steer_input_history_ = interpolate_eigen(
      steer_input_history_obs_eigen, time_stamp_obs_, time_stamp_steer_input_history);

    double acc_input, steer_input;
    nominal_ilqr_.compute_optimal_control(
      states, acc_input_history_, steer_input_history_, d_inputs_schedule_, states_ref_transformed,
      d_inputs_ref, states_prediction_, acc_input, steer_input);
    acc_input_history_obs_.push_back(acc_input);
    steer_input_history_obs_.push_back(steer_input);
    time_stamp_obs_.push_back(time_stamp);
    return Eigen::Vector2d(acc_input, steer_input);
  }
  Eigen::MatrixXd get_states_prediction()
  {
    Eigen::MatrixXd states_prediction_matrix(
      states_prediction_[0].size(), states_prediction_.size());
    for (int i = 0; i < int(states_prediction_.size()); i++) {
      states_prediction_matrix.col(i) = states_prediction_[i];
    }
    return states_prediction_matrix;
  }
  Eigen::MatrixXd get_d_inputs_schedule()
  {
    Eigen::MatrixXd d_inputs_schedule_matrix(
      d_inputs_schedule_[0].size(), d_inputs_schedule_.size());
    for (int i = 0; i < int(d_inputs_schedule_.size()); i++) {
      d_inputs_schedule_matrix.col(i) = d_inputs_schedule_[i];
    }
    return d_inputs_schedule_matrix;
  }
  void send_initialization_signal() { initialized_ = false; }
};

PYBIND11_MODULE(nominal_ilqr, m)
{
  py::class_<NominalILQRController>(m, "NominalILQRController")
    .def(py::init<>())
    .def("set_params", &NominalILQRController::set_params)
    .def("set_params_yaml", &NominalILQRController::set_params_from_yaml)
    .def("compute_optimal_control", &NominalILQRController::compute_optimal_control)
    .def("get_states_prediction", &NominalILQRController::get_states_prediction)
    .def("get_d_inputs_schedule", &NominalILQRController::get_d_inputs_schedule)
    .def("send_initialization_signal", &NominalILQRController::send_initialization_signal);
}
