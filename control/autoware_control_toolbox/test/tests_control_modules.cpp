// Copyright 2022 The Autoware Foundation.
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

#include "act_test_suite.hpp"

/**
 * ------------------ Test transfer function constructor from TF factors. ----------
 * */
TEST(ACTcontrol, tffactorConstruction)
{
  // test construction from tf factors.
  auto numerator_vector = std::vector<double>{1, 0, 0};
  auto denominator_vector = std::vector<double>{1, 0, 0.2};

  // Create tf_factors which are equipped with vector - vector algebra
  ns_control_toolbox::tf_factor ntf1{numerator_vector};    // numerator of the tf
  ns_control_toolbox::tf_factor dtf2{denominator_vector};  // denominator of the df

  auto tf_from_tf_factors = ns_control_toolbox::tf(ntf1, dtf2);
  tf_from_tf_factors.print();

  auto num = tf_from_tf_factors.num();
  auto den = tf_from_tf_factors.den();

  for (size_t k = 0; k < numerator_vector.size(); ++k)
  {
    ASSERT_DOUBLE_EQ(numerator_vector[k], num[k]);
    ASSERT_DOUBLE_EQ(denominator_vector[k], den[k]);
  }
}

/**
 * Tests multiplication of two transfer functions with their constant multipliers
 * In a TF representation : a x 1 / (tau*s + 1)  a: represents the constant multiplier.
 * */
TEST(ACTcontrol, transferFunctionMultiplication)
{
  // test TF x TF multiplication

  ns_control_toolbox::tf tf1{{5}, {1, 0}, 2., 7.}; // tf = (2/7) * 5 /(s^2)
  tf1.print();

  auto const &num1 = tf1.num();
  auto const &den1 = tf1.den();

  ASSERT_DOUBLE_EQ(num1[0], 10);
  ASSERT_DOUBLE_EQ(den1[0], 7);

  ns_control_toolbox::tf tf2{{2}, {1, 1}, 3., 3.};
  tf2.print();

  auto const &num2 = tf2.num();
  auto const &den2 = tf2.den();

  ASSERT_DOUBLE_EQ(num2[0], 6);
  ASSERT_DOUBLE_EQ(den2[0], 3);
  ASSERT_DOUBLE_EQ(den2[1], 3);

  auto const &tf3 = tf1 * tf2;

  auto const &num3 = tf3.num();
  auto const &den3 = tf3.den();

  ns_utils::print("TF TF multiplication with num den constants ");
  tf3.print();

  ASSERT_DOUBLE_EQ(num3[0], 60);
  ASSERT_DOUBLE_EQ(den3[0], 21);
  ASSERT_DOUBLE_EQ(den3[1], 21);
  ASSERT_DOUBLE_EQ(den3[2], 0);

}

/**
 * Test transfer function inversion.
 * */
TEST(ACTcontrol, tfInversion)
{
  // test construction from tf factors.
  auto numerator_vector = std::vector<double>{1, 0, 0};
  auto denominator_vector = std::vector<double>{1, 0, 0.2};

  auto tf1 = ns_control_toolbox::tf(numerator_vector, denominator_vector);

  ns_utils::print("Transfer function after inversion : ");
  tf1.print();

  // Invert (num/den -> den/num)
  tf1.inv();

  ns_utils::print("Transfer function after inversion : ");
  tf1.print();

  auto num = tf1.num();
  auto den = tf1.den();

  for (size_t k = 0; k < numerator_vector.size(); ++k)
  {
    ASSERT_DOUBLE_EQ(numerator_vector[k], den[k]);
    ASSERT_DOUBLE_EQ(denominator_vector[k], num[k]);
  }
}

/**
 * Test vector scalar multiplication overloading
 * */
TEST(ACTcontrol, vectorScalarMultiplicationOverloading)
{
  auto scalar_right = 7;
  auto scalar_left = 2;

  // Test vector overloading.
  std::vector<double> ov1{1, 2, 4};
  auto ov2 = ov1 * 7.0;
  auto ov3 = 2. * ov1;

  ns_utils::print("Given a vector : ");
  ns_utils::print_container(ov1);

  ns_utils::print("The given vector is right multiplied by a scalar ");
  ns_utils::print_container(ov2);

  ns_utils::print("The given vector is left multiplied by a scalar ");
  ns_utils::print_container(ov3);

  for (size_t k = 0; k < ov1.size(); ++k)
  {
    ASSERT_DOUBLE_EQ(ov2[k], scalar_right * ov1[k]);
    ASSERT_DOUBLE_EQ(ov3[k], scalar_left * ov1[k]);
  }

  ASSERT_TRUE(true);
}

/**
 * ------------------ Test state space methods -------------------------------
 * */

/**
 *  Matlab code
 *  clc
 *   s = tf('s');
 *   tau = 0.008;
 *   norder = 2;
 *   temp_tf = 1/(tau*s + 1)^norder
 *   num = temp_tf.Numerator{1}
 *   den = temp_tf.Denominator{1}
 *   temp_ss = ss(temp_tf)
 *   [A, B, C, D] = tf2ss(1, den)
 *
 *   temp_tf =
 *
 *               1
 *   -------------------------
 *   6.4e-05 s^2 + 0.016 s + 1
 *
 *   And its state space representation. (Balanced SS matrices.)
 *   A =   1.0e+04 *
 *      [-250   -122.1]
 *      [ 128      0]
 *   B = [8, 0]' and C = [0, 15.26]
 * */

TEST(ACTcontrol, takingSquareOfTFandSS)
{
  double const tau = 0.008;
  size_t const norder = 2;

  auto temp_tfactor_den = ns_control_toolbox::tf_factor{{tau, 1.}};
  temp_tfactor_den.power(norder);

  auto temp_tf = ns_control_toolbox::tf({1.}, temp_tfactor_den());
  temp_tf.print();

  auto const &tf_den = temp_tf.den();

  ASSERT_DOUBLE_EQ(tf_den[0], 6.4e-05);
  ASSERT_DOUBLE_EQ(tf_den[1], 0.016);
  ASSERT_DOUBLE_EQ(tf_den[2], 1.);

  // STATE-SPACE
  auto ss_cont = ns_control_toolbox::tf2ss(temp_tf);
  ss_cont.print();

  auto A = ss_cont.A();
  auto B = ss_cont.B();
  auto C = ss_cont.C();
  auto D = ss_cont.D();

  ASSERT_DOUBLE_EQ(A(0, 0), -250.);
  ASSERT_DOUBLE_EQ(A(0, 1), -122.0703125);
  ASSERT_DOUBLE_EQ(A(1, 0), 128.);
  ASSERT_DOUBLE_EQ(A(1, 1), 0.);

  /**
   * http://www.ece.northwestern.edu/local-apps/matlabhelp/toolbox/control/ref/ssbal.html
   * */
  ASSERT_DOUBLE_EQ(C(1) * B(0), 122.0703125); // due to the special algorithm of matrix balancing.
  ASSERT_DOUBLE_EQ(D(0), 0.);
}