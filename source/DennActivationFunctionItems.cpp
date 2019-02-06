#include "Config.h"
#include "DennConstants.h"
#include "DennPointFunction.h"
#include "DennActivationFunction.h"
#include "DennCostFunction.h"

namespace Denn
{
	void identity_activate(const Matrix& Z, Matrix& A)
	{
		A.noalias() = Z;
	}
	void identity_jacobian(const Matrix& Z, const Matrix& A, const Matrix& F, Matrix& G)
	{
		G.noalias() = F;
	}
	REGISTERED_ACTIVATION_FUNCTION("linear", identity_activate, identity_jacobian)

	void softmax_activate(const Matrix& Z, Matrix& A)
	{
		A.array() = (Z.rowwise() - Z.colwise().maxCoeff()).array().exp();
		RowArray colsums = A.colwise().sum();
		A.array().rowwise() /= colsums;
	}
	void softmax_jacobian(const Matrix& Z, const Matrix& A, const Matrix& F, Matrix& G)
	{
		RowArray a_dot_f = A.cwiseProduct(F).colwise().sum();
		G.array() = A.array() * (F.array().rowwise() - a_dot_f);
	}
	REGISTERED_ACTIVATION_FUNCTION("softmax", softmax_activate, softmax_jacobian)
		
	void sigmoid_activate(const Matrix& Z, Matrix& A)
	{
		A.array() = Scalar(1) / (Scalar(1) + (-Z.array()).exp());
	}
	void sigmoid_jacobian(const Matrix& Z, const Matrix& A, const Matrix& F, Matrix& G)
	{
		G.array() = A.array() * (Scalar(1) - A.array()) * F.array();
	}
	REGISTERED_ACTIVATION_FUNCTION("sigmoid", sigmoid_activate, sigmoid_jacobian)
		
	void relu_activate(const Matrix& Z, Matrix& A)
	{
		A.array() = Z.array().cwiseMax(Scalar(0));
	}
	void relu_jacobian(const Matrix& Z, const Matrix& A, const Matrix& F, Matrix& G)
	{
		G.array() = (A.array() > Scalar(0)).select(F, Scalar(0));
	}
	REGISTERED_ACTIVATION_FUNCTION("relu", relu_activate, relu_jacobian)
}