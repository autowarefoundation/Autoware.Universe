#include <Eigen/Core>
#include <Eigen/Dense>

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace py = pybind11;

class ActuationMap2D
{
private:
  Eigen::MatrixXd actuation_map_matrix_;
  std::vector<double> actuation_map_state_;
  std::vector<double> actuation_map_cmd_;

public:
  ActuationMap2D(std::string actuation_map_path)
  {
    std::vector<std::vector<double>> actuation_map;

    std::ifstream actuation_map_file(actuation_map_path);
    if (!actuation_map_file.is_open()) {
      throw std::runtime_error("Failed to open actuation map file");
    }

    std::string line;
    bool first_line = true;
    while (std::getline(actuation_map_file, line)) {
      std::vector<double> actuation_map_row;
      std::istringstream iss(line);
      std::string value;

      bool first_column = true;

      while (std::getline(iss, value, ',')) {
        if (first_line) {
          if (!first_column) {
            actuation_map_state_.push_back(std::stod(value));
          }
          first_column = false;
        } else {
          if (!first_column) {
            actuation_map_row.push_back(std::stod(value));
          } else {
            actuation_map_cmd_.push_back(std::stod(value));
            first_column = false;
          }
        }
      }
      if (!first_line) {
        actuation_map.push_back(actuation_map_row);
      }
      first_line = false;
    }
    actuation_map_file.close();
    actuation_map_matrix_ = Eigen::MatrixXd::Zero(actuation_map.size(), actuation_map[0].size());
    for (int i = 0; i < int(actuation_map.size()); i++) {
      for (int j = 0; j < int(actuation_map[0].size()); j++) {
        actuation_map_matrix_(i, j) = actuation_map[i][j];
      }
    }
  }
  virtual ~ActuationMap2D() {}
  double get_sim_actuation(double state, double cmd)
  {
    int state_index = 0;
    int cmd_index = 0;
    double state_ratio = 0.0;
    double cmd_ratio = 0.0;
    for (int i = 0; i < int(actuation_map_state_.size()); i++) {
      if (i == int(actuation_map_state_.size()) - 1 && state >= actuation_map_state_[i]) {
        state_index = i - 1;
        state_ratio = 1.0;
        break;
      }
      if (state >= actuation_map_state_[i] && state <= actuation_map_state_[i + 1]) {
        state_index = i;
        state_ratio = (state - actuation_map_state_[i]) /
                      (actuation_map_state_[i + 1] - actuation_map_state_[i]);
        break;
      }
    }
    for (int i = 0; i < int(actuation_map_cmd_.size()); i++) {
      if (i == int(actuation_map_cmd_.size()) - 1 && cmd >= actuation_map_cmd_[i]) {
        cmd_index = i - 1;
        cmd_ratio = 1.0;
        break;
      }
      if (cmd >= actuation_map_cmd_[i] && cmd <= actuation_map_cmd_[i + 1]) {
        cmd_index = i;
        cmd_ratio =
          (cmd - actuation_map_cmd_[i]) / (actuation_map_cmd_[i + 1] - actuation_map_cmd_[i]);
        break;
      }
    }
    return (1 - cmd_ratio) * (1 - state_ratio) * actuation_map_matrix_(cmd_index, state_index) +
           cmd_ratio * (1 - state_ratio) * actuation_map_matrix_(cmd_index + 1, state_index) +
           (1 - cmd_ratio) * state_ratio * actuation_map_matrix_(cmd_index, state_index + 1) +
           cmd_ratio * state_ratio * actuation_map_matrix_(cmd_index + 1, state_index + 1);
  }
  double get_actuation_cmd(double state, double input)
  {
    int state_index = 0;
    double state_ratio = 0.0;
    for (int i = 0; i < int(actuation_map_state_.size()); i++) {
      if (i == int(actuation_map_state_.size()) - 1 && state >= actuation_map_state_[i]) {
        state_index = i - 1;
        state_ratio = 1.0;
        break;
      }
      if (state >= actuation_map_state_[i] && state <= actuation_map_state_[i + 1]) {
        state_index = i;
        state_ratio = (state - actuation_map_state_[i]) /
                      (actuation_map_state_[i + 1] - actuation_map_state_[i]);
        break;
      }
    }
    Eigen::VectorXd actuation_cmd_to_input = Eigen::VectorXd::Zero(actuation_map_cmd_.size());
    actuation_cmd_to_input = actuation_map_matrix_.col(state_index) * (1 - state_ratio) +
                             actuation_map_matrix_.col(state_index + 1) * state_ratio;
    int cmd_index = -1;
    for (int i = 0; i < int(actuation_map_cmd_.size()); i++) {
      if (i == int(actuation_map_cmd_.size()) - 1) {
        if (
          actuation_cmd_to_input[i] >= actuation_cmd_to_input[i - 1] &&
          input >= actuation_cmd_to_input[i]) {
          cmd_index = i;
        } else if (
          actuation_cmd_to_input[i] <= actuation_cmd_to_input[i - 1] &&
          input <= actuation_cmd_to_input[i]) {
          cmd_index = i;
        }
      } else if (
        (input >= actuation_cmd_to_input[i] && input <= actuation_cmd_to_input[i + 1]) ||
        (input <= actuation_cmd_to_input[i] && input >= actuation_cmd_to_input[i + 1])) {
        cmd_index = i;
        break;
      }
    }
    if (cmd_index == -1) {
      return actuation_map_cmd_[0];
    } else if (cmd_index == int(actuation_map_cmd_.size()) - 1) {
      return actuation_map_cmd_[actuation_map_cmd_.size() - 1];
    } else {
      return (input - actuation_cmd_to_input[cmd_index]) /
               (actuation_cmd_to_input[cmd_index + 1] - actuation_cmd_to_input[cmd_index]) *
               (actuation_map_cmd_[cmd_index + 1] - actuation_map_cmd_[cmd_index]) +
             actuation_map_cmd_[cmd_index];
    }
    return 0.0;
  }
  bool is_map_used(double state, double input)
  {
    double minimum_input = 0.0;
    if (state <= actuation_map_state_[0]) {
      minimum_input = actuation_map_matrix_(0, 0);
    } else if (state >= actuation_map_state_[actuation_map_state_.size() - 1]) {
      minimum_input = actuation_map_matrix_(0, actuation_map_matrix_.cols() - 1);
    } else {
      for (int i = 0; i < int(actuation_map_state_.size()) - 1; i++) {
        if (state >= actuation_map_state_[i] && state <= actuation_map_state_[i + 1]) {
          minimum_input = (state - actuation_map_state_[i]) /
                            (actuation_map_state_[i + 1] - actuation_map_state_[i]) *
                            (actuation_map_matrix_(0, i + 1) - actuation_map_matrix_(0, i)) +
                          actuation_map_matrix_(0, i);
          break;
        }
      }
    }
    if (input < minimum_input) {
      return false;
    } else {
      return true;
    }
  }
};

PYBIND11_MODULE(actuation_map_2d, m)
{
  py::class_<ActuationMap2D>(m, "ActuationMap2D")
    .def(py::init<std::string>())
    .def("get_sim_actuation", &ActuationMap2D::get_sim_actuation)
    .def("get_actuation_cmd", &ActuationMap2D::get_actuation_cmd)
    .def("is_map_used", &ActuationMap2D::is_map_used);
}
