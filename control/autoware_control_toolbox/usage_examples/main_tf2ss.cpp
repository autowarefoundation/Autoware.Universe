//
// Created by ali on 17/2/22.
//

#include <fmt/core.h>
#include <cassert>
#include "utils_act/act_utils.hpp"
#include "autoware_control_toolbox.hpp"
#include "utils_act/state_space.hpp"

int main()
{

	double Ts = 0.1; // sampling time for discretization.
	std::vector<double> num{ 1. };
	std::vector<double> den{ 5.039e-07, 0.00019, 0.02387, 1 };

	// With a num, den
	ns_control_toolbox::tf sys_tf(num, den);

	// Print sys
	sys_tf.print();

	//  Default constructor example
	// ns_control_toolbox::tf2ss sys_ss; args : sys, Ts with Ts is defaulted = 0.1
	ns_control_toolbox::tf2ss sys_ss1(sys_tf);
	sys_ss1.print();

	// Constructor with num and den
	// args : sys, Ts with Ts is defaulted = 0.1
	ns_control_toolbox::tf2ss sys_ss2(num, den, Ts);
	sys_ss2.print();

	ns_utils::print("SS.A \n");
	ns_eigen_utils::printEigenMat(sys_ss2.A());

	ns_utils::print("SS.Ad \n");
	ns_eigen_utils::printEigenMat(sys_ss2.Ad());

	// DISCRETIZATION : Note sstf2 automatically discretisize
	Ts = 0.05;
	sys_ss2.print_discrete_system();

	// Test defaulted.
	ns_utils::print("Discretization with a given Ts when constructing");
	auto sys_ss3 = ns_control_toolbox::tf2ss(sys_tf, Ts);
	sys_ss3.print_discrete_system();


	// Simulate one-step.
	const int orderN = 4;
	ns_control_toolbox::state_type_t<orderN> x0_;
	x0_.setOnes();


	return 0;
}