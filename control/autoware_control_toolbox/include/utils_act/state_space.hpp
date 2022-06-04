/*
 * Copyright 2021 - 2022 Autoware Foundation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AUTOWARE_CONTROL_TOOLBOX_STATE_SPACE_HPP
#define AUTOWARE_CONTROL_TOOLBOX_STATE_SPACE_HPP

#include "visibility_control.hpp"
#include <vector>
#include "act_utils.hpp"
#include "act_utils_eigen.hpp"
#include "act_definitions.hpp"
#include "transfer_functions.hpp"
#include "balance.hpp"

namespace ns_control_toolbox
{

	/**
	 * @brief Stores the state space model matrices either discrete, or continuous.
	 * */

	struct ss_system
	{
		ss_system() = default;

		explicit ss_system(Eigen::MatrixXd Am,
		                   Eigen::MatrixXd Bm,
		                   Eigen::MatrixXd Cm,
		                   Eigen::MatrixXd Dm);

		Eigen::MatrixXd A{};
		Eigen::MatrixXd B{};
		Eigen::MatrixXd C{};
		Eigen::MatrixXd D{};

	};


	/**
	 * @brief tf2ss Converts a transfer function representation in to a state-space form.
	 * We assume the system is SISO type.
	 *
	 * */

	struct ACT_PUBLIC tf2ss
	{

		// Constructors
		tf2ss();

		explicit tf2ss(tf const& sys_tf, const double& Ts = 0.1);

		tf2ss(std::vector<double> const& num, std::vector<double> const& den, const double& Ts = 0.1);


		// Public methods
		// Currently only Tustin - Bilinear discretization is implemented.
		void discretisize(double const& Ts);

		void print() const;

		void print_discrete_system() const;


		[[nodiscard]] ss_system getSystem_continuous() const;

		[[nodiscard]] ss_system getSystem_discrete() const;

		virtual void getABCD_continuous(Eigen::MatrixXd& A, Eigen::MatrixXd& B,
		                                Eigen::MatrixXd& C, Eigen::MatrixXd& D) const;

		virtual void getABCD_discrete(Eigen::MatrixXd& Ad, Eigen::MatrixXd& Bd,
		                              Eigen::MatrixXd& Cd, Eigen::MatrixXd& Dd) const;

		// Getters for the system matrices.
		// Discrete time state-space matrices.
		[[nodiscard]] Eigen::MatrixXd Ad() const
		{
			return Ad_;
		}

		[[nodiscard]] Eigen::MatrixXd Bd() const
		{
			return Bd_;
		}

		[[nodiscard]] Eigen::MatrixXd Cd() const
		{
			return Cd_;
		}

		[[nodiscard]] Eigen::MatrixXd Dd() const
		{
			return Dd_;
		}


		// Continuous time state-space matrices.
		[[nodiscard]] Eigen::MatrixXd A() const
		{
			return A_;
		}

		[[nodiscard]] Eigen::MatrixXd B() const
		{
			return B_;
		}

		[[nodiscard]] Eigen::MatrixXd C() const
		{
			return C_;
		}

		[[nodiscard]] Eigen::MatrixXd D() const
		{
			return D_;
		}

		// Class methods.

		/**
		 * @brief Compute the system continuous time system matrices
		 * */
		void computeSystemMatrices(std::vector<double> const& num, std::vector<double> const& den);


	private:

		double Ts_{};

		// Data members
		// Continuous time state-space model
		Eigen::MatrixXd A_{};
		Eigen::MatrixXd B_{};
		Eigen::MatrixXd C_{};
		Eigen::MatrixXd D_{};

		// Discrete time state-space model
		Eigen::MatrixXd Ad_{};
		Eigen::MatrixXd Bd_{};
		Eigen::MatrixXd Cd_{};
		Eigen::MatrixXd Dd_{};

	};


	// Templated tf2ss
	template<int Norder>
	struct ss : public tf2ss
	{
		using tf2ss::tf2ss;

		using state_vec_type = Eigen::Matrix<double, Norder, 1>;
		using mat_Atype = Eigen::Matrix<double, Norder, Norder>;
		using mat_Btype = Eigen::Matrix<double, Norder, 1>;
		using mat_Ctype = Eigen::Matrix<double, 1, Norder>;
		using mat_Dtype = Eigen::Matrix<double, 1, 1>;

		void getSSc(mat_Atype& A, mat_Btype& B,
		            mat_Ctype& C, mat_Dtype& D) const;

		void getSSd(mat_Atype& Ad, mat_Btype& Bd,
		            mat_Ctype& Cd, mat_Dtype& Dd) const;

	private:

		double Ts_{};

		// Data members
		// Continuous time state-space model
		mat_Atype A_{};
		mat_Btype B_{};
		mat_Ctype C_{};
		mat_Dtype D_{};

		// Discrete time state-space model
		mat_Atype Ad_{};
		mat_Btype Bd_{};
		mat_Ctype Cd_{};
		mat_Dtype Dd_{};

	};

	template<int Norder>
	void ss<Norder>::getSSc(mat_Atype& A, mat_Btype& B,
	                        mat_Ctype& C, mat_Dtype& D) const
	{
		A = A_;
		B = B_;
		C = C_;
		D = D_;
	}

	template<int Norder>
	void ss<Norder>::getSSd(mat_Atype& Ad, mat_Btype& Bd,
	                        mat_Ctype& Cd, mat_Dtype& Dd) const
	{
		Ad = mat_Atype{ Ad_ };
		ns_eigen_utils::printEigenMat(Ad_);
		ns_eigen_utils::printEigenMat(Ad);


		Bd = mat_Btype{ Bd_ };
		Cd = mat_Ctype{ Cd_ };
		Dd = mat_Dtype{ Dd_ };
	}

	template<int nx, int ny>
	using mat_type_t = Eigen::Matrix<double, nx, ny>;

} // namespace ns_control_toolbox
#endif //AUTOWARE_CONTROL_TOOLBOX_STATE_SPACE_HPP
