#pragma once
#include "Config.h"
#include "Constants.h"

namespace Denn
{
namespace PointFunction
{
	template < typename ScalarType >
	using Ptr = std::function< ScalarType(const ScalarType& input) >;

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
		return ScalarType(1.0) / (ScalarType(1.0) + std::exp(-a));
	}	
	
	template < typename ScalarType = double >
	inline ScalarType log(const ScalarType& a)
	{
		return std::log(a);
	}

	template < typename ScalarType = double >
	inline ScalarType logit(const ScalarType& a)
	{
		return log<ScalarType>(a / (ScalarType(1.0) + a));
	}

	template < typename ScalarType = double >
	inline ScalarType tanh(const ScalarType& a)
	{
		return std::tanh(a);
	}
}

namespace ActiveFunction
{
	template < typename Matrix >
	using Ptr = std::function< Matrix& (Matrix& input) >;

	template < typename Matrix >
	inline Matrix& identity(Matrix& inout_matrix)
	{
		return inout_matrix;
	}

	template < typename Matrix >
	inline Matrix& sigmoid(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::sigmoid<typename Matrix::Scalar>);
		return inout_matrix;
	}

	template < typename Matrix >
	inline Matrix& logistic(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::logistic<typename Matrix::Scalar>);
		return inout_matrix;
	}

	template < typename Matrix >
	inline Matrix& log(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::log<typename Matrix::Scalar>);
		return inout_matrix;
	}

	template < typename Matrix >
	inline Matrix& logit(Matrix& inout_matrix)
	{
		inout_matrix.unaryExpr(&Denn::PointFunction::logit<typename Matrix::Scalar>);
		return inout_matrix;
	}

	template < typename Matrix >
	Matrix& softmax(Matrix& inout_matrix)
	{
		const int	         N   = inout_matrix.rows();
		const Matrix::Scalar max = inout_matrix.maxCoeff();
#if 1
		//compute e^(M-max)
		inout_matrix = (inout_matrix.array() - max).exp();
		// M(r,n)/ SUM_r(M(n))
		for (int n = 0; n < N; n++)
		{
			//reduce
			Matrix::Scalar sum = inout_matrix.row(n).sum();
			//no nan
			if (sum) inout_matrix.row(n) /= sum;
		}
#else  //standard
		for (int n = 0; n < N; n++)
		{
			// e^(M_r-max)/ SUM_r(e^(M_r-max))
			inout_matrix.row(n) = (inout_matrix.row(n).array() - max).exp();
			inout_matrix.row(n) /= inout_matrix.row(n).sum();
		}
#endif
		return inout_matrix;
	}

	//////////////////////////////////////////////////////ALIAS
	static Ptr<MatrixD>  identityD = &identity<MatrixD>;
	static Ptr<MatrixF>  identityF = &identity<MatrixF>;
	static Ptr<MatrixD>  sigmoidD  = &sigmoid <MatrixD>;
	static Ptr<MatrixF>  sigmoidF  = &sigmoid <MatrixF>;
	static Ptr<MatrixD>  logisticD = &logistic<MatrixD>;
	static Ptr<MatrixF>  logisticF = &logistic<MatrixF>;
	static Ptr<MatrixD>  logD	   = &log     <MatrixD>;
	static Ptr<MatrixF>  logF	   = &log     <MatrixF>;
	static Ptr<MatrixD>  logitD	   = &logit   <MatrixD>;
	static Ptr<MatrixF>  logitF	   = &logit   <MatrixF>;
	static Ptr<MatrixD>  softmaxD  = &softmax <MatrixD>;
	static Ptr<MatrixF>  softmaxF  = &softmax <MatrixF>;
}
	
}