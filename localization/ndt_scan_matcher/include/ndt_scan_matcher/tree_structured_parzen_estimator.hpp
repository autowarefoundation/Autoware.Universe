// Copyright 2023 Autoware Foundation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NDT_SCAN_MATCHER__TREE_STRUCTURED_PARZEN_ESTIMATOR_HPP_
#define NDT_SCAN_MATCHER__TREE_STRUCTURED_PARZEN_ESTIMATOR_HPP_

/*
A implementation of tree-structured parzen estimator
Search double variables in [-1, 1)
*/

#include <cstdint>
#include <random>
#include <vector>

class TreeStructuredParzenEstimator
{
public:
  using Input = std::vector<double>;
  using Score = double;
  struct Trial
  {
    Input input;
    Score score;
  };

  enum Direction {
    MINIMIZE = 0,
    MAXIMIZE = 1,
  };

  TreeStructuredParzenEstimator() = delete;
  TreeStructuredParzenEstimator(
    const Direction direction, const int64_t n_startup_trials, std::vector<bool> is_loop_variable);
  void add_trial(const Trial & trial);
  Input get_next_input();

private:
  static constexpr double BASE_STDDEV_COEFF = 0.2;
  static constexpr double MAX_GOOD_RATE = 0.10;
  static constexpr double MAX_VALUE = 1.0;
  static constexpr double MIN_VALUE = -1.0;
  static constexpr int64_t N_EI_CANDIDATES = 100;
  static constexpr double PRIOR_WEIGHT = 0.0;

  double acquisition_function(const Input & input);
  double log_gaussian_pdf(const Input & input, const Input & mu, const Input & sigma) const;
  static std::vector<double> get_weights(const int64_t n);
  static double normalize_loop_variable(const double value);

  std::mt19937_64 engine_;
  std::uniform_real_distribution<double> dist_uniform_;
  std::normal_distribution<double> dist_normal_;

  std::vector<Trial> trials_;
  int64_t above_num_;
  const Direction direction_;
  const int64_t n_startup_trials_;
  const int64_t input_dimension_;
  const std::vector<bool> is_loop_variable_;
  Input base_stddev_;
};

#endif  // NDT_SCAN_MATCHER__TREE_STRUCTURED_PARZEN_ESTIMATOR_HPP_
