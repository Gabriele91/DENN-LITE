#include "Config.h"
#include "DennConstants.h"
#include "DennPointFunction.h"
#include "DennActiveFunction.h"
#include "DennCostFunction.h"

namespace Denn
{
    //ActiveFunction linear (none)
	template < typename Matrix >
	Matrix& linear(Matrix& inout_matrix)
	{
		return inout_matrix;
	}
	template < typename Matrix >
	Matrix& dx_linear(Matrix& inout_matrix)
	{
		inout_matrix.fill(1.0);
		return inout_matrix;
	}
	REGISTERED_ACTIVE_FUNCTION("linear", linear<Matrix>, dx_linear<Matrix>)

	//ActiveFunction (local)
	REGISTERED_ACTIVE_FUNCTION("softmax", CostFunction::softmax_row_samples<Matrix>)

	//ActiveFunction (point)
	#define POINTF_AS_ACTIVEF(ufunc)\
    template < typename Matrix >\
    Matrix& ufunc(Matrix& inout_matrix)\
	{\
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction:: ufunc <typename Matrix::Scalar>);\
        return inout_matrix;\
	}

	POINTF_AS_ACTIVEF(sigmoid)
	POINTF_AS_ACTIVEF(dx_sigmoid)
	REGISTERED_ACTIVE_FUNCTION("sigmoid", sigmoid<Matrix>, dx_sigmoid<Matrix>)

	POINTF_AS_ACTIVEF(logit)
	POINTF_AS_ACTIVEF(dx_logit)
    REGISTERED_ACTIVE_FUNCTION("logit", logit<Matrix>, dx_logit<Matrix>)

	POINTF_AS_ACTIVEF(log)
	POINTF_AS_ACTIVEF(dx_log)
	REGISTERED_ACTIVE_FUNCTION("log", log<Matrix>, dx_log<Matrix>)

	POINTF_AS_ACTIVEF(relu)
	POINTF_AS_ACTIVEF(dx_relu)
	REGISTERED_ACTIVE_FUNCTION("relu", relu<Matrix>, dx_relu<Matrix>)
		
	POINTF_AS_ACTIVEF(tanh)
	POINTF_AS_ACTIVEF(dx_tanh)
	REGISTERED_ACTIVE_FUNCTION("tanh", tanh<Matrix>, dx_tanh<Matrix>)

	POINTF_AS_ACTIVEF(binary)
    REGISTERED_ACTIVE_FUNCTION("binary", binary<Matrix>)
		
}