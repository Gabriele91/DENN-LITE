#include "Config.h"
#include "DennConstants.h"
#include "DennPointFunction.h"
#include "DennActiveFunction.h"

namespace Denn
{
    //others ActiveFunction
	template < typename Matrix >
	inline Matrix& linear(Matrix& inout_matrix)
	{
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(linear<Matrix>, "linear")

	template < typename Matrix >
	inline Matrix& sigmoid(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::sigmoid<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(sigmoid<Matrix>, "sigmoid")

	template < typename Matrix >
	inline Matrix& logit(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::logit<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(logit<Matrix>, "logit")

	template < typename Matrix >
	inline Matrix& logistic(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::logistic<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(logistic<Matrix>, "logistic")

	template < typename Matrix >
	inline Matrix& log(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::log<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(log<Matrix>, "log")

	template < typename Matrix >
	inline Matrix& relu(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::relu<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(relu<Matrix>, "relu")

	template < typename Matrix >
	inline Matrix& tanh(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::tanh<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(tanh<Matrix>, "tanh")
	
	template < typename Matrix >
	inline Matrix& binary(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::binary<typename Matrix::Scalar>);
		return inout_matrix;
	}
    REGISTERED_ACTIVE_FUNCTION(binary<Matrix>, "binary")

}