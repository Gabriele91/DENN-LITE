#pragma once
#include "Config.h"
#include "Constants.h"

namespace Denn
{
	template < typename ScalarType >
	using ActiveFunction = std::function< ScalarType(const ScalarType& value) >;

	template < typename ScalarType = double >
	inline ScalarType identity(const ScalarType& a)
	{
		return a;
	}

	template < typename ScalarType = double >
	inline ScalarType sigmoid(const ScalarType& a)
	{
		return ScalarType(1.0) / (ScalarType(1.0) + std::exp(-a));
	}

	template < typename ScalarType = double >
	inline ScalarType logistic(const ScalarType& a)
	{
		return ScalarType(1.0) / (ScalarType(1.0) + std::pow(Constants::e<ScalarType>(), -2 * a));
	}

	template < typename ScalarType = double >
	inline ScalarType log(const ScalarType& a)
	{
		return std::log(a);
	}

	template < typename ScalarType = double >
	inline ScalarType tanh(const ScalarType& a)
	{
		return std::tanh(a);
	}

	template < typename Matrix >
	Matrix& softmax(Matrix& inout_matrix)
	{
		const int	 N = inout_matrix.rows();
		const double max = inout_matrix.maxCoeff();
		for (int n = 0; n < N; n++)
		{
			inout_matrix.row(n) = (inout_matrix.row(n).array() - max).exp();
			inout_matrix.row(n) /= inout_matrix.row(n).sum();
		}
		return inout_matrix;
	}

	//////////////////////////////////////////////////////ALIAS
	static ActiveFunction<double> identityD = &identity<double>;
	static ActiveFunction<float>  identityF = &identity<float>;
	static ActiveFunction<double> sigmoidD  = &sigmoid <double>;
	static ActiveFunction<float>  sigmoidF  = &sigmoid <float>;
	static ActiveFunction<double> logisticD = &logistic<double>;
	static ActiveFunction<float>  logisticF = &logistic<float>;
	static ActiveFunction<double> logD		= &log<double>;
	static ActiveFunction<float>  logF	    = &log<float>;
}