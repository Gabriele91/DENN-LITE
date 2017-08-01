#include "Config.h"
#include "DennConstants.h"
#include "DennPointFunction.h"
#include "DennActiveFunction.h"

namespace Denn
{
    //others ActiveFunction
	template < typename Matrix >
	Matrix& linear(Matrix& inout_matrix)
	{
		return inout_matrix;
	}
	REGISTERED_ACTIVE_FUNCTION("linear", linear<Matrix>)

	template < typename Matrix >
	Matrix& sigmoid(Matrix& inout_matrix)
	{
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction::sigmoid<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION("sigmoid", sigmoid<Matrix>)

	template < typename Matrix >
	Matrix& logit(Matrix& inout_matrix)
	{
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction::logit<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION("logit", logit<Matrix>)

	template < typename Matrix >
	Matrix& logistic(Matrix& inout_matrix)
	{
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction::logistic<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION("logistic", logistic<Matrix>)

	template < typename Matrix >
	Matrix& log(Matrix& inout_matrix)
	{
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction::log<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION("log", log<Matrix>)

	template < typename Matrix >
	Matrix& relu(Matrix& inout_matrix)
	{
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction::relu<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION("relu", relu<Matrix>)

	template < typename Matrix >
	Matrix& tanh(Matrix& inout_matrix)
	{
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction::tanh<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION("tanh", tanh<Matrix>)
	
	template < typename Matrix >
	Matrix& binary(Matrix& inout_matrix)
	{
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction::binary<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION("binary", binary<Matrix>)

}