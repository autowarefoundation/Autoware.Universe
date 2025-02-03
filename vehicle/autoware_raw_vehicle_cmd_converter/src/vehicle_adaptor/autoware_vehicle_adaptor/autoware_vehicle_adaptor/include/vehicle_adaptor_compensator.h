#ifndef VEHICLE_ADAPTOR_COMPENSATOR_H_
#define VEHICLE_ADAPTOR_COMPENSATOR_H_

#include "inputs_prediction.h"
#include "inputs_ref_smoother.h"
#include "nominal_dynamics.h"
#include "transform_vehicle_adaptor_model.h"
#include "vehicle_adaptor_utils.h"

#include <Eigen/Core>
#include <Eigen/Dense>

#include <pybind11/stl.h>

#include <vector>

namespace vehicle_adaptor_constants
{
constexpr int x_index_ = 0;
constexpr int y_index_ = 1;
constexpr int vel_index_ = 2;
constexpr int yaw_index_ = 3;
constexpr int acc_index_ = 4;
constexpr int steer_index_ = 5;
constexpr int state_size_ = 6;
static const std::vector<std::string> all_state_name_ = {"x", "y", "vel", "yaw", "acc", "steer"};

// Nominal dynamics error decay rate to blend new prediction errors with past errors
constexpr double error_decay_rate_ = 0.5;
}  // namespace vehicle_adaptor_constants

/**
 * @class TrainedDynamics
 * @brief Class representing a dynamic model for a vehicle that incorporates a combination of
 * nominal dynamics and neural network-based corrections.
 *
 * This class models vehicle dynamics by combining a nominal dynamics model with corrections derived
 * from a neural network (NN). It supports the prediction of future states, updating of LSTM states,
 * and computation of derivatives with respect to inputs and states for control purposes. The NN
 * includes LSTM layers to capture temporal dependencies, and the class leverages both a baseline
 * physics-based (nominal) model and learned corrections to improve prediction accuracy.
 */
class TrainedDynamics
{
private:
  // Nominal dynamics model used as a baseline for system dynamics
  NominalDynamics nominal_dynamics_;
  // Class for using a NN model in Eigen format
  TransformModelToEigen transform_model_to_eigen_;

  // Vehicle-specific parameters
  double wheel_base_;           // Wheelbase of the vehicle [m]
  double acc_time_delay_;       // Time delay of the acceleration input [s]
  double steer_time_delay_;     // Time delay of the steering input [s]
  double acc_time_constant_;    // Time constant of the acceleration input [s]
  double steer_time_constant_;  // Time constant of the steering input [s]
  int
    acc_delay_step_;  // Number of delay steps for acceleration (acc_delay_step_ =
                      // std::min(int(std::round(acc_time_delay_ / control_dt_)), acc_queue_size_))
  int steer_delay_step_;  // Number of delay steps for steering (steer_delay_step_ =
                          // std::min(int(std::round(steer_time_delay_ / control_dt_)),
                          // steer_queue_size_))

  // Input queue sizes for control inputs
  int acc_queue_size_;    // Size of acceleration input queue
  int steer_queue_size_;  // Size of steering input queue

  // Indices related to input queue
  int acc_input_start_index_;    // Start index of acceleration input in the input queue
  int acc_input_end_index_;      // End index of acceleration input in the input queue
  int steer_input_start_index_;  // Start index of steering input in the input queue
  int steer_input_end_index_;    // End index of steering input in the input queue

  // Control parameters
  double control_dt_;  // Control time step [s]
  int predict_step_;   // Number of control steps per one step of the model prediction (not equal to
                       // horizon length)
  double predict_dt_;  // Prediction time step [s] (predict_dt = control_dt * predict_step)

  // NN model
  FilterDiffNN filter_diff_NN_;  // Filter for differentiating NN model output
  int h_dim_;                    // Dimension of the hidden state of the LSTM decoder
  int h_dim_full_;               // Dimension of the hidden state of the LSTM encoder

  // Minimum values to ensure that the derivatives do not vanish in optimization
  double min_gradient_acc_;  // Minimum allowable derivative value for acceleration in optimization
  double min_gradient_steer_;  // Minimum allowable derivative value for steering in optimization

  // Names of state components for easier reference
  std::vector<std::string>
    state_component_predicted_;                       // Components of state predicted by the model
  std::vector<int> state_component_predicted_index_;  // Indices of the predicted state components
public:
  /**
   * @brief Constructor for TrainedDynamics.
   */
  TrainedDynamics();

  /**
   * @brief Destructor for TrainedDynamics.
   */
  virtual ~TrainedDynamics();

  /**
   * @brief Set vehicle-specific parameters.
   * @param wheel_base Wheelbase of the vehicle [m]
   * @param acc_time_delay Acceleration input time delay [s]
   * @param steer_time_delay Steering input time delay [s]
   * @param acc_time_constant Acceleration input time constant [s]
   * @param steer_time_constant Steering input time constant [s]
   * @param acc_queue_size Acceleration input queue size
   * @param steer_queue_size Steering input queue size
   * @param control_dt Control time step [s]
   * @param predict_step Number of control steps per model prediction step
   */
  void set_vehicle_params(
    double wheel_base, double acc_time_delay, double steer_time_delay, double acc_time_constant,
    double steer_time_constant, int acc_queue_size, int steer_queue_size, double control_dt,
    int predict_step);

  /**
   * @brief Set the neural network parameters (weights and biases) required for the LSTM-based
   * correction model.
   * @param weight_acc_encoder_layer_1, weight_steer_encoder_layer_1, ... various weights for
   * encoding and decoding layers
   * @param bias_acc_encoder_layer_1, bias_steer_encoder_layer_1, ... corresponding biases
   * @param vel_scaling Velocity scaling factor for input normalization
   * @param vel_bias Velocity bias for input normalization
   * @param state_component_predicted List of state components predicted by the NN
   */
  void set_NN_params(
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
    const double vel_scaling, const double vel_bias,
    const std::vector<std::string> state_component_predicted);

  /**
   * @brief Clear all neural network parameters.
   */
  void clear_NN_params();

  /**
   * @brief Set filter parameters for the Savitzky-Golay filter used for smoothing derivatives.
   * @param degree Polynomial degree for the Savitzky-Golay filter
   * @param window_size Window size for the Savitzky-Golay filter
   */
  void set_sg_filter_params(int degree, int window_size);

  /**
   * @brief Perform nominal prediction of the system state using only the nominal (physics-based)
   * dynamics model.
   *
   * This function predicts the vehicle's future state after prediction_step_ steps
   * (i.e., after control_dt_ * prediction_step_ seconds) using only the nominal (physics-based)
   * vehicle dynamics model.
   *
   * @param states Current state vector
   * @param acc_input_history_concat Concatenated acceleration input history including the upcoming
   * input sequence for prediction_step_ steps.
   * @param steer_input_history_concat Concatenated steering input history including the upcoming
   * input sequence for prediction_step_ steps.
   * @return Predicted next state vector without NN corrections
   */
  Eigen::VectorXd nominal_prediction(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
    const Eigen::VectorXd & steer_input_history_concat);

  /**
   * @brief Update LSTM hidden and cell states.
   * @param states Current state vector
   * @param acc_input_history_concat Concatenated acceleration input history including the upcoming
input sequence for prediction_step_ steps.
   * @param steer_input_history_concat Concatenated steering input history including the upcoming
input sequence for prediction_step_ steps.
   * @param h_lstm LSTM hidden states (modified in place)
   * @param c_lstm LSTM cell states (modified in place)
   * @param acc_steer_error The error in the previous acceleration and steering states under the
nominal dynamics.
   *
　 * In this function, the LSTM states (h_lstm and c_lstm) are updated based on the current
 　* vehicle states and input histories. The acc_steer_error parameter represents the error
   * of the previous acceleration and steering states under the nominal dynamics. This error
   * is used to adjust the LSTM's internal states, allowing the neural network to account for
   * recent deviations from the nominal behavior and potentially improve future predictions.
   */
  void update_lstm_states(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
    const Eigen::VectorXd & steer_input_history_concat, std::vector<Eigen::VectorXd> & h_lstm,
    std::vector<Eigen::VectorXd> & c_lstm, const Eigen::Vector2d & acc_steer_error);
  /**
   * @brief Compute the predicted next state for calculating controller prediction error using the
   * full model (nominal + NN).
   * @param states Current state vector
   * @param acc_input_history_concat Concatenated acceleration input history including the upcoming
   * input sequence for prediction_step_ steps.
   * @param steer_input_history_concat Concatenated steering input history including the upcoming
   * input sequence for prediction_step_ steps.
   * @param h_lstm LSTM hidden state (modified in place)
   * @param c_lstm LSTM cell state (modified in place)
   * @param previous_error Previous prediction error
   * @param horizon Prediction horizon
   * @return Predicted next state vector with corrections
   *
   * The controller prediction, which is not used in this function,
   * is not necessarily the same as the nominal prediction (base model).
   */
  Eigen::VectorXd F_with_model_for_calc_controller_prediction_error(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
    const Eigen::VectorXd & steer_input_history_concat, Eigen::VectorXd & h_lstm,
    Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error, const int horizon);
  /**
   * @brief Compute the predicted next state using the full model (nominal + NN) given current
   * inputs and states.
   * @param states Current state vector
   * @param acc_input_history Acceleration input history (modified in place)
   * @param steer_input_history Steering input history (modified in place)
   * @param acc_input_history_concat Concatenated acceleration input history including the upcoming
   * input sequence for prediction_step_ steps (modified in place).
   * @param steer_input_history_concat Concatenated steering input history including the upcoming
   * input sequence for prediction_step_ steps (modified in place).
   * @param d_inputs Input increments (acceleration, steering)
   * @param h_lstm LSTM hidden state (modified in place)
   * @param c_lstm LSTM cell state (modified in place)
   * @param previous_error Previous prediction error
   * @param horizon Prediction horizon
   * @return Predicted next state vector
   */
  Eigen::VectorXd F_with_model(
    const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
    Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs,
    Eigen::VectorXd & h_lstm, Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error,
    const int horizon);
  /**
   * @brief Overloaded version of F_with_model that does not require concatenated histories as
   * arguments.
   */
  Eigen::VectorXd F_with_model(
    const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs,
    Eigen::VectorXd & h_lstm, Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error,
    const int horizon);
  /**
   * @brief Compute the predicted next state and linearization matrices (A, B, C) using the full
   * model (with NN corrections).
   * @param states Current state vector
   * @param acc_input_history Acceleration input history (modified in place)
   * @param steer_input_history Steering input history (modified in place)
   * @param acc_input_history_concat Concatenated acceleration input history including the upcoming
   * input sequence for prediction_step_ steps (modified in place).
   * @param steer_input_history_concat Concatenated steering input history including the upcoming
   * input sequence for prediction_step_ steps (modified in place).
   * @param d_inputs Input increments
   * @param h_lstm LSTM hidden state (modified in place)
   * @param c_lstm LSTM cell state (modified in place)
   * @param previous_error Previous prediction error
   * @param horizon Prediction horizon
   * @param A State Jacobian matrix
   * @param B Input Jacobian matrix
   * @param C Additional Jacobian (related to the LSTM hidden state and cell state)
   * @return Predicted next state vector
   */
  Eigen::VectorXd F_with_model_diff(
    const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, Eigen::VectorXd & acc_input_history_concat,
    Eigen::VectorXd & steer_input_history_concat, const Eigen::Vector2d & d_inputs,
    Eigen::VectorXd & h_lstm, Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error,
    const int horizon, Eigen::MatrixXd & A, Eigen::MatrixXd & B, Eigen::MatrixXd & C);
  /**
   * @brief Overloaded version of F_with_model_diff that does not require concatenated histories as
   * arguments.
   */
  Eigen::VectorXd F_with_model_diff(
    const Eigen::VectorXd & states, Eigen::VectorXd & acc_input_history,
    Eigen::VectorXd & steer_input_history, const Eigen::Vector2d & d_inputs,
    Eigen::VectorXd & h_lstm, Eigen::VectorXd & c_lstm, Eigen::VectorXd & previous_error,
    const int horizon, Eigen::MatrixXd & A, Eigen::MatrixXd & B, Eigen::MatrixXd & C);
  /**
   * @brief Compute predictions for multiple candidate trajectories.
   * @param States Matrix of current state vectors for multiple candidate trajectories
   * @param Acc_input_history Matrix of acceleration input histories for multiple candidate
   * trajectories
   * @param Steer_input_history Matrix of steering input histories for multiple candidate
   * trajectories
   * @param Acc_input_history_concat Matrix of concatenated acceleration input histories for
   * multiple candidate trajectories
   * @param Steer_input_history_concat Matrix of concatenated steering input histories for multiple
   * candidate trajectories
   * @param D_inputs Matrix of input increments for multiple candidate trajectories
   * @param H_lstm Matrix of LSTM hidden states for multiple candidate trajectories
   * @param C_lstm Matrix of LSTM cell states for multiple candidate trajectories
   * @param Previous_error Matrix of previous prediction errors for multiple candidate trajectories
   * @param horizon Prediction horizon
   * @return Matrix of predicted state vectors for multiple candidate trajectories
   */
  Eigen::MatrixXd F_with_model_for_candidates(
    const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
    Eigen::MatrixXd & Steer_input_history, Eigen::MatrixXd & Acc_input_history_concat,
    Eigen::MatrixXd & Steer_input_history_concat, const Eigen::MatrixXd & D_inputs,
    Eigen::MatrixXd & H_lstm, Eigen::MatrixXd & C_lstm, Eigen::MatrixXd & Previous_error,
    const int horizon);
  /**
   * @brief Overloaded version of F_with_model_for_candidates that does not require concatenated
   * histories as arguments.
   */
  Eigen::MatrixXd F_with_model_for_candidates(
    const Eigen::MatrixXd & States, Eigen::MatrixXd & Acc_input_history,
    Eigen::MatrixXd & Steer_input_history, const Eigen::MatrixXd & D_inputs,
    Eigen::MatrixXd & H_lstm, Eigen::MatrixXd & C_lstm, Eigen::MatrixXd & Previous_error,
    const int horizon);
  /**
   * @brief Calculate a forward trajectory and its linearizations over a schedule of input
   * increments, useful for iLQR.
   * @param states Initial state
   * @param acc_input_history Current acceleration input history
   * @param steer_input_history Current steering input history
   * @param d_inputs_schedule Schedule of input increments over a horizon
   * @param h_lstm LSTM hidden state
   * @param c_lstm LSTM cell state
   * @param previous_error Previous prediction error
   * @param states_prediction Output vector of predicted states at each step
   * @param dF_d_states Output vector of state Jacobians at each step
   * @param dF_d_inputs Output vector of input Jacobians at each step
   * @param inputs_schedule Output vector of input schedules calculated from the input increments
   * (d_inputs_schedule) and the previous inputs (last step of acc_input_history and
   * steer_input_history)
   */
  void calc_forward_trajectory_with_diff(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
    const Eigen::VectorXd & steer_input_history,
    const std::vector<Eigen::VectorXd> & d_inputs_schedule, const Eigen::VectorXd & h_lstm,
    const Eigen::VectorXd & c_lstm, const Eigen::VectorXd & previous_error,
    std::vector<Eigen::VectorXd> & states_prediction, std::vector<Eigen::MatrixXd> & dF_d_states,
    std::vector<Eigen::MatrixXd> & dF_d_inputs, std::vector<Eigen::Vector2d> & inputs_schedule);
};

/**
 * @class AdaptorILQR
 * @brief Class that performs optimization using ILQR with the TrainedDynamics model for Vehicle
 * Adaptor.
 *
 * This class is responsible for setting cost parameters, computing cost,
 * and ultimately determining optimal control sequences that minimize the defined cost.
 * It leverages the TrainedDynamics instance for state predictions and linearization.
 */
class AdaptorILQR
{
private:
  // Vehicle-specific parameters
  double wheel_base_;           // Wheelbase of the vehicle [m]
  double acc_time_delay_;       // Time delay of the acceleration input [s]
  double steer_time_delay_;     // Time delay of the steering input [s]
  double acc_time_constant_;    // Time constant of the acceleration input [s]
  double steer_time_constant_;  // Time constant of the steering input [s]
  int
    acc_delay_step_;  // Number of delay steps for acceleration (acc_delay_step_ =
                      // std::min(int(std::round(acc_time_delay_ / control_dt_)), acc_queue_size_))
  int steer_delay_step_;  // Number of delay steps for steering (steer_delay_step_ =
                          // std::min(int(std::round(steer_time_delay_ / control_dt_)),
                          // steer_queue_size_))

  // Input queue sizes for control inputs
  int acc_queue_size_;    // Size of acceleration input queue
  int steer_queue_size_;  // Size of steering input queue

  // Indices related to input queue
  int acc_input_start_index_;    // Start index of acceleration input in the input queue
  int acc_input_end_index_;      // End index of acceleration input in the input queue
  int steer_input_start_index_;  // Start index of steering input in the input queue
  int steer_input_end_index_;    // End index of steering input in the input queue

  // Control parameters
  double control_dt_;  // Control time step [s]
  int predict_step_;   // Number of control steps per one step of the model prediction (not equal to
                       // horizon length)
  double predict_dt_;  // Prediction time step [s] (predict_dt = control_dt * predict_step)

  // NN model
  int horizon_len_;                   // Length of the prediction horizon
  int h_dim_;                         // Dimension of the hidden state of the LSTM decoder
  TrainedDynamics trained_dynamics_;  // Trained dynamics model

  // Cost parameters
  // stage cost
  double x_cost_;
  double y_cost_;
  double vel_cost_;
  double yaw_cost_;
  double acc_cost_;
  double steer_cost_;

  // terminal cost
  double x_terminal_cost_;
  double y_terminal_cost_;
  double vel_terminal_cost_;
  double yaw_terminal_cost_;
  double acc_terminal_cost_;
  double steer_terminal_cost_;

  // intermediate cost (emphasis on the cost at the intermediate step)
  int intermediate_cost_index_;
  double x_intermediate_cost_;
  double y_intermediate_cost_;
  double vel_intermediate_cost_;
  double yaw_intermediate_cost_;
  double steer_intermediate_cost_;
  double acc_intermediate_cost_;

  // input cost
  double acc_rate_cost_;
  double steer_rate_cost_;
  double acc_rate_rate_cost_;
  double steer_rate_rate_cost_;

  // tables for adaptive cost change
  std::vector<double> controller_acc_input_weight_target_table_,
    controller_longitudinal_coef_target_table_, controller_vel_error_domain_table_,
    controller_acc_error_domain_table_;
  std::vector<double> controller_steer_input_weight_target_table_,
    controller_lateral_coef_target_table_, controller_yaw_error_domain_table_,
    controller_steer_error_domain_table_;

  std::vector<double> vel_for_steer_rate_table_, steer_rate_cost_coef_by_vel_table_;

  std::vector<double> acc_rate_input_table_;
  std::vector<double> acc_rate_cost_coef_table_;
  std::vector<double> x_coef_by_acc_rate_table_, vel_coef_by_acc_rate_table_,
    acc_coef_by_acc_rate_table_;

  std::vector<double> steer_rate_input_table_;
  std::vector<double> steer_rate_cost_coef_table_;
  std::vector<double> y_coef_by_steer_rate_table_, yaw_coef_by_steer_rate_table_,
    steer_coef_by_steer_rate_table_;

  std::vector<double> x_error_domain_table_, y_error_domain_table_, vel_error_domain_table_,
    yaw_error_domain_table_, acc_error_domain_table_, steer_error_domain_table_;
  std::vector<double> x_error_target_table_, y_error_target_table_, vel_error_target_table_,
    yaw_error_target_table_, acc_error_target_table_, steer_error_target_table_;

  // states which are used for ilqr
  std::vector<std::string> state_component_ilqr_;  // Components of state used for iLQR
  std::vector<int> state_component_ilqr_index_;    // Indices of the state components used for iLQR
  int num_state_component_ilqr_;                   // Number of state components used for iLQR

public:
  /**
   * @brief Default constructor for AdaptorILQR.
   */
  AdaptorILQR();

  /**
   * @brief Destructor for AdaptorILQR.
   */
  virtual ~AdaptorILQR();

  /**
   * @brief Set general parameters from yaml file.
   */
  void set_params();

  /**
   * @brief Set the weights for state costs during the stage cost.
   * @param x_cost Weight on longitudinal error
   * @param y_cost Weight on lateral error
   * @param vel_cost Weight on velocity error
   * @param yaw_cost Weight on yaw error
   * @param acc_cost Weight on acceleration state deviation
   * @param steer_cost Weight on steering state deviation
   */
  void set_states_cost(
    double x_cost, double y_cost, double vel_cost, double yaw_cost, double acc_cost,
    double steer_cost);

  /**
   * @brief Set the input costs related to input increments (rate).
   * @param acc_rate_cost Cost on acceleration rate
   * @param steer_rate_cost Cost on steering rate
   */
  void set_inputs_cost(double acc_rate_cost, double steer_rate_cost);

  /**
   * @brief Set the second derivative costs (rate of rate) for inputs.
   * @param acc_rate_rate_cost Cost on change of acceleration rate
   * @param steer_rate_rate_cost Cost on change of steering rate
   */
  void set_rate_cost(double acc_rate_rate_cost, double steer_rate_rate_cost);

  /**
   * @brief Set intermediate cost parameters for a specific step within the horizon.
   */
  void set_intermediate_cost(
    double x_intermediate_cost, double y_intermediate_cost, double vel_intermediate_cost,
    double yaw_intermediate_cost, double acc_intermediate_cost, double steer_intermediate_cost,
    int intermediate_cost_index);

  /**
   * @brief Set terminal cost parameters applied at the final step of the horizon.
   */
  void set_terminal_cost(
    double x_terminal_cost, double y_terminal_cost, double vel_terminal_cost,
    double yaw_terminal_cost, double acc_terminal_cost, double steer_terminal_cost);

  /**
   * @brief Set vehicle parameters and horizon information, and link them with the trained dynamics
   * model.
   */
  void set_vehicle_params(
    double wheel_base, double acc_time_delay, double steer_time_delay, double acc_time_constant,
    double steer_time_constant, int acc_queue_size, int steer_queue_size, double control_dt,
    int predict_step);
  /**
   * @brief Set the horizon length for the iLQR optimization.
   */
  void set_horizon_len(int horizon_len);
  /**
   * @brief Set NN parameters for the underlying TrainedDynamics instance.
   */
  void set_NN_params(
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
    const double vel_scaling, const double vel_bias,
    const std::vector<std::string> state_component_predicted);

  /**
   * @brief Clear NN parameters from the underlying TrainedDynamics model.
   */
  void clear_NN_params();

  /**
   * @brief Set Savitzky-Golay filter parameters in the underlying TrainedDynamics model.
   */
  void set_sg_filter_params(int degree, int window_size);

  /**
   * @brief Obtain nominal prediction from TrainedDynamics.
   */
  Eigen::VectorXd nominal_prediction(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
    const Eigen::VectorXd & steer_input_history_concat);

  /**
   * @brief Update LSTM states in the underlying TrainedDynamics model.
   */
  void update_lstm_states(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history_concat,
    const Eigen::VectorXd & steer_input_history_concat, std::vector<Eigen::VectorXd> & h_lstm,
    std::vector<Eigen::VectorXd> & c_lstm, const Eigen::Vector2d & previous_error);

  /**
   * @brief Calculate forward trajectory with cost. This method integrates forward simulation and
   * cost accumulation.
   * @param states Initial state
   * @param acc_input_history Current acceleration input history
   * @param steer_input_history Current steering input history
   * @param D_inputs_schedule Schedule of input increments over a horizon for several candidate
   * trajectories
   * @param h_lstm LSTM hidden state
   * @param c_lstm LSTM cell state
   * @param previous_error Previous prediction error
   * @param states_prediction Output vector of predicted states at each step
   * @param states_ref Reference states for cost calculation
   * @param d_input_ref Reference input increments for cost calculation
   * @param inputs_ref Reference inputs for cost calculation
   * @param x_weight_coef Weight coefficient for longitudinal error
   * @param y_weight_coef Weight coefficient for lateral error
   * @param vel_weight_coef Weight coefficient for velocity error
   * @param yaw_weight_coef Weight coefficient for yaw error
   * @param acc_weight_coef Weight coefficient for acceleration error
   * @param steer_weight_coef Weight coefficient for steering error
   * @param acc_input_weight Weight coefficient for acceleration input
   * @param steer_input_weight Weight coefficient for steering input
   * @param acc_rate_weight_coef Weight coefficient for acceleration rate
   * @param steer_rate_weight_coef Weight coefficient for steering rate
   * @param Cost Output vector of cost values for each candidate trajectory
   */
  void calc_forward_trajectory_with_cost(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
    const Eigen::VectorXd & steer_input_history,
    const std::vector<Eigen::MatrixXd> & D_inputs_schedule, const Eigen::VectorXd & h_lstm,
    const Eigen::VectorXd & c_lstm, const Eigen::VectorXd & previous_error,
    std::vector<Eigen::MatrixXd> & states_prediction,
    const std::vector<Eigen::VectorXd> & states_ref,
    const std::vector<Eigen::VectorXd> & d_input_ref,
    const std::vector<Eigen::Vector2d> & inputs_ref, double x_weight_coef, double y_weight_coef,
    double vel_weight_coef, double yaw_weight_coef, double acc_weight_coef,
    double steer_weight_coef, double acc_input_weight, double steer_input_weight,
    double acc_rate_weight_coef, double steer_rate_weight_coef, Eigen::VectorXd & Cost);
  /**
   * @brief Calculate several infomation from the input schedule for iLQR.
   */
  void calc_inputs_ref_info(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
    const Eigen::VectorXd & steer_input_history, const Eigen::VectorXd & h_lstm,
    const Eigen::VectorXd & c_lstm, const Eigen::VectorXd & previous_error,
    const std::vector<Eigen::VectorXd> & states_ref,
    const Eigen::VectorXd & acc_controller_input_schedule,
    const Eigen::VectorXd & steer_controller_input_schedule,
    const std::vector<Eigen::VectorXd> & states_prediction,
    std::vector<Eigen::Vector2d> & inputs_ref, const double & acc_input_change_rate,
    const double & steer_input_change_rate, double & x_weight_coef, double & y_weight_coef,
    double & vel_weight_coef, double & yaw_weight_coef, double & acc_weight_coef,
    double & steer_weight_coef, double & acc_input_weight, double & steer_input_weight,
    double & acc_rate_weight_coef, double & steer_rate_weight_coef,
    double & initial_prediction_x_weight_coef, double & initial_prediction_y_weight_coef,
    double & initial_prediction_vel_weight_coef, double & initial_prediction_yaw_weight_coef,
    double & initial_prediction_acc_weight_coef, double & initial_prediction_steer_weight_coef);
  /**
   * @brief Extract some components from the derivatives of F with respect to states that is used
   * for iLQR.
   */
  Eigen::MatrixXd extract_dF_d_state(const Eigen::MatrixXd & dF_d_state_with_history);
  /**
   * @brief Extract some components from the derivatives of F with respect to inputs and states that
   * is used for iLQR.
   */
  Eigen::MatrixXd extract_dF_d_input(const Eigen::MatrixXd & dF_d_input);

  /**
   * @brief Apply right-action transformations using state difference Jacobians (with history).
   *
   * The differentiation of the transition of the history is a priori known,
   * so we can apply the right-action transformation efficiently.
   */
  Eigen::MatrixXd right_action_by_state_diff_with_history(
    const Eigen::MatrixXd & Mat, const Eigen::MatrixXd & dF_d_state_with_history);

  /**
   * @brief Apply right-action transformations using state difference Jacobians (with history).
   */
  Eigen::MatrixXd left_action_by_state_diff_with_history(
    const Eigen::MatrixXd & dF_d_state_with_history, const Eigen::MatrixXd & Mat);

  /**
   * @brief Apply right-action transformations using input difference Jacobians.
   */
  Eigen::MatrixXd right_action_by_input_diff(
    const Eigen::MatrixXd & Mat, const Eigen::MatrixXd & dF_d_input);

  /**
   * @brief Apply left-action transformations using input difference Jacobians.
   */
  Eigen::MatrixXd left_action_by_input_diff(
    const Eigen::MatrixXd & dF_d_input, const Eigen::MatrixXd & Mat);

  /**
   * @brief Compute iLQR coefficients (K, k) for the backward pass of the iLQR algorithm.
   */
  void compute_ilqr_coefficients(
    const std::vector<Eigen::MatrixXd> & dF_d_states,
    const std::vector<Eigen::MatrixXd> & dF_d_inputs,
    const std::vector<Eigen::VectorXd> & states_prediction,
    const std::vector<Eigen::VectorXd> & d_inputs_schedule,
    const std::vector<Eigen::VectorXd> & states_ref,
    const std::vector<Eigen::VectorXd> & d_input_ref, const double prev_acc_rate,
    const double prev_steer_rate, const std::vector<Eigen::Vector2d> & inputs_ref,
    const double x_weight_coef, const double y_weight_coef, const double vel_weight_coef,
    const double yaw_weight_coef, const double acc_weight_coef, const double steer_weight_coef,
    const double acc_input_weight, const double steer_input_weight,
    const double acc_rate_weight_coef, const double steer_rate_weight_coef,
    const std::vector<Eigen::Vector2d> & inputs_schedule, std::vector<Eigen::MatrixXd> & K,
    std::vector<Eigen::VectorXd> & k);

  /**
   * @brief Calculate line search candidates for the iLQR step.
   */
  std::vector<Eigen::MatrixXd> calc_line_search_candidates(
    const std::vector<Eigen::MatrixXd> & K, const std::vector<Eigen::VectorXd> & k,
    const std::vector<Eigen::MatrixXd> & dF_d_states,
    const std::vector<Eigen::MatrixXd> & dF_d_inputs,
    const std::vector<Eigen::VectorXd> & d_inputs_schedule, const Eigen::VectorXd & ls_points);

  /**
   * @brief Compute the optimal control input by performing iLQR (forward-backward passes) and
   * applying the resulting policy.
   * @param states Current state vector
   * @param acc_input_history Current acceleration input history
   * @param steer_input_history Current steering input history
   * @param d_inputs_schedule Schedule of input increments over a horizon
   * @param h_lstm LSTM hidden state
   * @param c_lstm LSTM cell state
   * @param states_ref Reference states for cost calculation
   * @param d_input_ref Reference input increments for cost calculation
   * @param acc_input_change_rate Rate of change for the acceleration input calculated from the
   * controller intputs
   * @param steer_input_change_rate Rate of change for the steering input calculated from the
   * controller intputs
   * @param previous_error Previous prediction error
   * @param states_prediction Output vector of predicted states at each step
   * @param acc_controller_input_schedule Schedule of acceleration inputs for the controller
   * @param steer_controller_input_schedule Schedule of steering inputs for the controller
   * @param acc_input Output acceleration input
   * @param steer_input Output steering input
   */
  void compute_optimal_control(
    const Eigen::VectorXd & states, const Eigen::VectorXd & acc_input_history,
    const Eigen::VectorXd & steer_input_history, std::vector<Eigen::VectorXd> & d_inputs_schedule,
    const Eigen::VectorXd & h_lstm, const Eigen::VectorXd & c_lstm,
    const std::vector<Eigen::VectorXd> & states_ref,
    const std::vector<Eigen::VectorXd> & d_input_ref, double acc_input_change_rate,
    double steer_input_change_rate, Eigen::VectorXd & previous_error,
    std::vector<Eigen::VectorXd> & states_prediction,
    const Eigen::VectorXd & acc_controller_input_schedule,
    const Eigen::VectorXd & steer_controller_input_schedule, double & acc_input,
    double & steer_input);
};
namespace Proxima
{
class VehicleAdaptor
/**
 * @class VehicleAdaptor
 * @brief Adaptor class that interfaces a vehicle's controller inputs with trained dynamics by NN.
 */
{
private:
  // Nominal dynamics model used for the controller (not necessarily the same as base model of the
  // trained dynamics)
  NominalDynamics nominal_dynamics_controller_;
  // ILQR optimizer for the vehicle adaptor
  AdaptorILQR adaptor_ilqr_;
  // Polynomial regression predictors for acceleration and steering inputs
  PolynomialRegressionPredictor polynomial_reg_for_predict_acc_input_,
    polynomial_reg_for_predict_steer_input_;
  // NN predictors for acceleration and steering inputs
  InputsSchedulePrediction acc_input_schedule_prediction_, steer_input_schedule_prediction_;

  // Butterworth filter for smoothing the final control inputs
  ButterworthFilter butterworth_filter_;
  // Smoothers for the reference inputs
  InputsRefSmoother acc_input_ref_smoother_, steer_input_ref_smoother_;

  bool use_nonzero_initial_hidden_ = false;

  // Savitzky-Golay filters for smoothing the input increments schedule, which is the output of iLQR
  SgFilter sg_filter_for_d_inputs_schedule_;
  int sg_window_size_for_d_inputs_schedule_;  // Window size of the Savitzky-Golay filter for the
                                              // input increments schedule
  int sg_deg_for_d_inputs_schedule_;  // Degree of the Savitzky-Golay filter for smoothing the input
                                      // increments schedule
  bool use_sg_for_d_inputs_schedule_;  // Flag to enable Savitzky-Golay filter for smoothing the
                                       // input increments schedule

  // Vehicle-specific parameters
  double wheel_base_;           // Wheelbase of the vehicle [m]
  double acc_time_delay_;       // Time delay of the acceleration input [s]
  double steer_time_delay_;     // Time delay of the steering input [s]
  double acc_time_constant_;    // Time constant of the acceleration input [s]
  double steer_time_constant_;  // Time constant of the steering input [s]
  int
    acc_delay_step_;  // Number of delay steps for acceleration (acc_delay_step_ =
                      // std::min(int(std::round(acc_time_delay_ / control_dt_)), acc_queue_size_))
  int steer_delay_step_;  // Number of delay steps for steering (steer_delay_step_ =
                          // std::min(int(std::round(steer_time_delay_ / control_dt_)),
                          // steer_queue_size_))

  // Input queue sizes for control inputs
  int acc_queue_size_;    // Size of acceleration input queue
  int steer_queue_size_;  // Size of steering input queue

  // Indices related to input queue
  int acc_input_start_index_;    // Start index of acceleration input in the input queue
  int acc_input_end_index_;      // End index of acceleration input in the input queue
  int steer_input_start_index_;  // Start index of steering input in the input queue
  int steer_input_end_index_;    // End index of steering input in the input queue

  // Control parameters
  double control_dt_;  // Control time step [s]
  int predict_step_;   // Number of control steps per one step of the model prediction (not equal to
                       // horizon length)
  double predict_dt_;  // Prediction time step [s] (predict_dt = control_dt * predict_step)

  // parameters which are used for the controller
  double steer_dead_band_;           // Dead band for steering input
  int acc_delay_step_controller_;    // Number of delay steps for acceleration in the controller
  int steer_delay_step_controller_;  // Number of delay steps for steering in the controller

  // NN model
  int horizon_len_;         // Length of the prediction horizon
  int h_dim_;               // Dimension of the hidden state of the LSTM decoder
  int h_dim_full_;          // Dimension of the hidden state of the LSTM encoder
  int sg_deg_for_NN_diff_;  // Degree of the Savitzky-Golay filter for smoothing the derivatives of
                            // the NN predictions
  int sg_window_size_for_NN_diff_;  // Window size of the Savitzky-Golay filter for smoothing the
                                    // derivatives of the NN predictions
  int NN_prediction_target_dim_;    // Dimension of the target for the NN prediction
  int num_layers_encoder_;          // Number of layers in of the LSTM
  int update_lstm_len_;             // Length of history size for updating the LSTM encoder states
  bool use_offline_features_ = false;  // Flag to use offline features for the NN prediction

  // Cost parameters
  // stage cost
  double x_cost_;
  double y_cost_;
  double vel_cost_;
  double yaw_cost_;
  double acc_cost_;
  double steer_cost_;

  // terminal cost
  double x_terminal_cost_;
  double y_terminal_cost_;
  double vel_terminal_cost_;
  double yaw_terminal_cost_;
  double acc_terminal_cost_;
  double steer_terminal_cost_;

  // intermediate cost (emphasis on the cost at the intermediate step)
  int intermediate_cost_index_;
  double x_intermediate_cost_;
  double y_intermediate_cost_;
  double vel_intermediate_cost_;
  double yaw_intermediate_cost_;
  double steer_intermediate_cost_;
  double acc_intermediate_cost_;

  // input cost
  double acc_rate_cost_;
  double steer_rate_cost_;
  double acc_rate_rate_cost_;
  double steer_rate_rate_cost_;

  // parameters for the predictor by polynomial regression
  int controller_acc_input_history_len_,
    controller_steer_input_history_len_;  // Length of the history for the controller inputs
  int deg_controller_acc_input_history_,
    deg_controller_steer_input_history_;  // Degree of the polynomial regression for the controller
                                          // inputs
  std::vector<double> lam_controller_acc_input_history_,
    lam_controller_steer_input_history_;  // Regularization parameter for the polynomial regression
                                          // for the controller inputs
  double oldest_sample_weight_controller_acc_input_history_,
    oldest_sample_weight_controller_steer_input_history_;  // Weight for the oldest sample in the
                                                           // polynomial regression for the
                                                           // controller inputs
  int acc_polynomial_prediction_len_,
    steer_polynomial_prediction_len_;  // Length of the prediction for the polynomial regression for
                                       // the controller inputs

  // parameters for the predictor by linear extrapolation
  int acc_linear_extrapolation_len_;    // Length of the linear extrapolation for acceleration input
  int steer_linear_extrapolation_len_;  // Length of the linear extrapolation for steering input
  int past_len_for_acc_linear_extrapolation_;  // Length of the history for the linear extrapolation
                                               // for acceleration input
  int past_len_for_steer_linear_extrapolation_;  // Length of the history for the linear
                                                 // extrapolation for steering input

  // how to predict the states reference
  std::string states_ref_mode_ = "input_schedule_prediction";
  std::string acc_input_schedule_prediction_mode_ = "NN";
  std::string steer_input_schedule_prediction_mode_ = "NN";
  bool steer_controller_prediction_aided_ = false;
  int acc_input_schedule_prediction_len_, steer_input_schedule_prediction_len_;

  bool acc_input_schedule_prediction_initialized_ =
    false;  // Flag to check if the acceleration input schedule prediction is initialized
  bool steer_input_schedule_prediction_initialized_ =
    false;  // Flag to check if the steering input schedule prediction is initialized

  double reflect_controller_d_input_ratio_;  // Ratio of the reflected controller input increments
                                             // to the initial value of iLQR input increment
  int max_queue_size_;                       // Maximum size of the input queue
  bool use_controller_inputs_as_target_;     // Flag to use the controller input increments as the
                                             // reference for the NN prediction

  std::string input_filter_mode_ = "none";  // Mode of the input filter

  // table for the mix ratio of the vehicle adaptor and the original controller
  std::vector<double> mix_ratio_vel_target_table_, mix_ratio_vel_domain_table_;
  std::vector<double> mix_ratio_time_target_table_, mix_ratio_time_domain_table_;

  // parameters updated in the runtime
  bool initialized_ = false;  // Flag to check if the vehicle adaptor is initialized
  double start_time_;         // Start time of the vehicle adaptor

  // states and inputs history and the resulting trajectory
  std::vector<double> time_stamp_obs_;  // Time stamp of the observation
  std::vector<double> acc_input_history_obs_,
    steer_input_history_obs_;  // History of the final acceleration and steering inputs
  std::vector<double> acc_controller_input_history_obs_,
    steer_controller_input_history_obs_;  // History of the acceleration and steering inputs of the
                                          // original controller
  std::vector<Eigen::VectorXd> state_history_lstm_obs_, acc_input_history_lstm_obs_,
    steer_input_history_lstm_obs_;  // History for updating the LSTM states
  Eigen::VectorXd acc_controller_d_inputs_schedule_, steer_controller_d_inputs_schedule_;
  Eigen::VectorXd x_controller_prediction_, y_controller_prediction_, vel_controller_prediction_,
    yaw_controller_prediction_, acc_controller_prediction_, steer_controller_prediction_;
  Eigen::VectorXd acc_input_history_;
  Eigen::VectorXd steer_input_history_;
  Eigen::VectorXd acc_controller_input_history_, steer_controller_input_history_;
  std::vector<Eigen::VectorXd> states_prediction_;
  std::vector<Eigen::VectorXd> d_inputs_schedule_;
  std::vector<Eigen::VectorXd> state_history_lstm_, acc_input_history_lstm_,
    steer_input_history_lstm_;

  //  parameters for calculating the input change rate
  double past_acc_input_change_, past_steer_input_change_;
  double past_acc_input_change_decay_rate_, past_steer_input_change_decay_rate_;
  int acc_input_change_window_size_, steer_input_change_window_size_;
  double future_acc_input_change_, future_steer_input_change_;
  double future_acc_input_change_decay_rate_, future_steer_input_change_decay_rate_;
  double past_acc_input_change_weight_, past_steer_input_change_weight_;

  // parameters for NN prediction
  Eigen::VectorXd offline_features_;
  Eigen::VectorXd previous_error_;

public:
  bool use_controller_steer_input_schedule_ =
    false;  // Flag to use the controller steering input schedule
  bool
    use_vehicle_adaptor_;  // Flag to use the vehicle adaptor when the vehicle adaptor with autoware
  bool use_offline_features_autoware_;  // Flag to use offline features for the vehicle adaptor with
                                        // autoware

  /**
   * @brief Default constructor of VehicleAdaptor.
   */
  VehicleAdaptor();

  /**
   * @brief Destructor of VehicleAdaptor.
   */
  virtual ~VehicleAdaptor();

  /**
   * @brief Set general parameters from yaml file.
   */
  void set_params();

  /**
   * @brief Set neural network parameters for the underlying models used by the adaptor.
   *
   * This includes weights and biases for encoder layers, LSTM layers, and other NN layers
   * used for corrections and predictions of the dynamics model. It also includes scaling factors.
   *
   * @param weight_acc_encoder_layer_1,... Various NN weight matrices
   * @param bias_acc_encoder_layer_1,... Corresponding NN bias vectors
   * @param vel_scaling Scaling factor for velocity
   * @param vel_bias Bias term for velocity
   * @param state_component_predicted List of state components that the NN predicts
   */
  void set_NN_params(
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
    const double vel_scaling, const double vel_bias,
    const std::vector<std::string> state_component_predicted);

  /**
   * @brief Set offline features that is used for the initial state of the LSTM encoder. *
   * @param offline_features Vector of offline features.
   */
  void set_offline_features(const Eigen::VectorXd & offline_features);

  /**
   * @brief Load NN parameters from a CSV directory.
   *
   * This method reads pre-trained NN weights and biases from CSV files,
   * making it easier to load models deployed outside the source code.
   *
   * @param csv_dir Directory path containing the CSV files.
   */
  void set_NN_params_from_csv(std::string csv_dir);

  /**
   * @brief Load offline features from a CSV file.
   *
   * Similar to set_offline_features, but reads the features from a file.
   *
   * @param csv_dir Directory path containing the CSV files with offline features.
   */
  void set_offline_features_from_csv(std::string csv_dir);

  /**
   * @brief Clear the currently set NN parameters.
   *
   */
  void clear_NN_params();

  /**
   * @brief Set the controller's incremental input schedules for both acceleration and steering.
   *
   * These increments (d_inputs) are often computed by iLQR.
   *
   * @param acc_controller_d_inputs_schedule Acceleration input increments schedule.
   * @param steer_controller_d_inputs_schedule Steering input increments schedule.
   */
  void set_controller_d_inputs_schedule(
    const Eigen::VectorXd & acc_controller_d_inputs_schedule,
    const Eigen::VectorXd & steer_controller_d_inputs_schedule);

  /**
   * @brief Set only the steering incremental input schedule.
   *
   * @param steer_controller_d_inputs_schedule Steering input increments schedule.
   */
  void set_controller_d_steer_schedule(const Eigen::VectorXd & steer_controller_d_inputs_schedule);

  /**
   * @brief Set a schedule of steering inputs for the controller at a given timestamp.
   *
   * This is helpful when a series of future steering inputs is known, and we want
   * to feed them into the adaptor.
   *
   * @param timestamp Current time at which the schedule is provided.
   * @param steer_controller_input_schedule Vector of future steering inputs.
   */
  void set_controller_steer_input_schedule(
    double timestamp, const std::vector<double> & steer_controller_input_schedule);

  /**
   * @brief Set the controller's predicted states and inputs over a future horizon.
   *
   * When the controller provide a reference prediction (e.g., from a model predictive controller),
   * the adaptor can use this information.
   *
   * @param x_controller_prediction Predicted x positions
   * @param y_controller_prediction Predicted y positions
   * @param vel_controller_prediction Predicted velocities
   * @param yaw_controller_prediction Predicted yaws
   * @param acc_controller_prediction Predicted accelerations
   * @param steer_controller_prediction Predicted steering angles
   */
  void set_controller_prediction(
    const Eigen::VectorXd & x_controller_prediction,
    const Eigen::VectorXd & y_controller_prediction,
    const Eigen::VectorXd & vel_controller_prediction,
    const Eigen::VectorXd & yaw_controller_prediction,
    const Eigen::VectorXd & acc_controller_prediction,
    const Eigen::VectorXd & steer_controller_prediction);

  /**
   * @brief Set only the controller's steering prediction schedule.
   *
   * Similar to set_controller_prediction but when only the steering prediction changes.
   *
   * @param steer_controller_prediction Predicted steering angles.
   */
  void set_controller_steer_prediction(const Eigen::VectorXd & steer_controller_prediction);

  /**
   * @brief Get adjusted control inputs (acceleration and steering) at a given time and state.
   *
   * This method takes the raw controller inputs, the current time and state, and adjusts
   * them using the adaptor's logic, which involve NN corrections and iLQR optimization.
   *
   * @param time_stamp Current time
   * @param states Current vehicle states
   * @param acc_controller_input Raw acceleration input from the controller
   * @param steer_controller_input Raw steering input from the controller
   * @return Adjusted control inputs as a vector (e.g., [adjusted_acc, adjusted_steer]).
   */
  Eigen::VectorXd get_adjusted_inputs(
    double time_stamp, const Eigen::VectorXd & states, const double acc_controller_input,
    const double steer_controller_input);

  /**
   * @brief Retrieve the predicted vehicle states.
   *
   * After computations and possibly after iLQR, this method provides
   * the predicted states over the future horizon.
   *
   * @return A matrix where each column represents the predicted state at a future time step.
   */
  Eigen::MatrixXd get_states_prediction();

  /**
   * @brief Retrieve the schedule of incremental inputs (d_inputs) that were set or computed.
   *
   * This shows the incremental changes in inputs over the prediction horizon as processed by the
   * adaptor.
   *
   * @return A matrix where each column represents the increments in control inputs at a future time
   * step.
   */
  Eigen::MatrixXd get_d_inputs_schedule();

  /**
   * @brief Indicate that the VehicleAdaptor is initialized.
   *
   * This can be used to signal that all necessary parameters, models, and initial states
   * have been set, and the adaptor is ready for operation.
   */
  void send_initialized_flag();
};
}  // namespace Proxima
#endif  // VEHICLE_ADAPTOR_COMPENSATOR_H_
