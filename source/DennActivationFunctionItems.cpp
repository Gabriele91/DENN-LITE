#include "Config.h"
#include "DennConstants.h"
#include "DennPointFunction.h"
#include "DennActivationFunction.h"
#include "DennCostFunction.h"

namespace Denn
{
    //ActivationFunction linear (none)
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
	REGISTERED_ACTIVATION_FUNCTION("linear", linear<Matrix>, dx_linear<Matrix>)

	//ActivationFunction (local)
	//Now it is used to the loss function, it is unnecessary (only for back compatibility)
	REGISTERED_ACTIVATION_FUNCTION("softmax", CostFunction::implace_softmax_row_samples<Matrix>) 

	//ActivationFunction (point)
	#define POINTF_AS_ACTIVEF(ufunc)\
    template < typename Matrix >\
    Matrix& ufunc(Matrix& inout_matrix)\
	{\
		inout_matrix = inout_matrix.unaryExpr(&Denn::PointFunction:: ufunc <typename Matrix::Scalar>);\
        return inout_matrix;\
	}

	POINTF_AS_ACTIVEF(sigmoid)
	POINTF_AS_ACTIVEF(dx_sigmoid)
	REGISTERED_ACTIVATION_FUNCTION("sigmoid", sigmoid<Matrix>, dx_sigmoid<Matrix>)

	POINTF_AS_ACTIVEF(logit)
	POINTF_AS_ACTIVEF(dx_logit)
    REGISTERED_ACTIVATION_FUNCTION("logit", logit<Matrix>, dx_logit<Matrix>)

	POINTF_AS_ACTIVEF(log)
	POINTF_AS_ACTIVEF(dx_log)
	REGISTERED_ACTIVATION_FUNCTION("log", log<Matrix>, dx_log<Matrix>)

	POINTF_AS_ACTIVEF(relu)
	POINTF_AS_ACTIVEF(dx_relu)
	REGISTERED_ACTIVATION_FUNCTION("relu", relu<Matrix>, dx_relu<Matrix>)
		
	POINTF_AS_ACTIVEF(tanh)
	POINTF_AS_ACTIVEF(dx_tanh)
	REGISTERED_ACTIVATION_FUNCTION("tanh", tanh<Matrix>, dx_tanh<Matrix>)

	POINTF_AS_ACTIVEF(binary)
    REGISTERED_ACTIVATION_FUNCTION("binary", binary<Matrix>)
		
}