#pragma once
#include "Config.h"
#include "DennPointFunction.h"

namespace Denn
{
namespace CostFunction
{

	template < typename Scalar >
	Scalar safe_log(Scalar x)
	{
		const Scalar min_const_ep = 1e-10 ;
		return Denn::PointFunction::log<Scalar>(Denn::clamp(x,min_const_ep,Scalar(1.0)));
	}

	template < typename Matrix >
	Matrix cross_entropy_by_rows(const Matrix& x, const Matrix& y)
	{
		Matrix log_y = y.unaryExpr(&safe_log<typename Matrix::Scalar>);
		return -((x.array() * log_y.array()).matrix().rowwise()).sum();
	}	

	template < typename Matrix >
	typename Matrix::Scalar cross_entropy(const Matrix& x, const Matrix& y)
	{
		Matrix log_y = y.unaryExpr(&safe_log<typename Matrix::Scalar>);
		return -(x.array() * log_y.array()).sum() / (typename Matrix::Scalar)y.rows();
	}

	template < typename Matrix >
	inline Matrix& softmax(Matrix& inout_matrix)
	{
		const auto	                  N   = inout_matrix.rows();
		const typename Matrix::Scalar max = inout_matrix.maxCoeff();
#if 1
		//compute e^(M-max)
		inout_matrix = (inout_matrix.array() - max).exp();
		// M(r,n)/ SUM_r(M(n))
		for (int n = 0; n < N; n++)
		{
			//reduce
			typename Matrix::Scalar sum = inout_matrix.row(n).sum();
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

	template < typename Matrix >
	typename Matrix::Scalar softmax_cross_entropy(const Matrix& x, const Matrix& y)
	{
		Matrix y_softmax(y);
		softmax(y_softmax);
		return cross_entropy(x, y_softmax);
	}

	template < typename Matrix >
	typename Matrix::Scalar softmax_cross_entropy_with_logit(const Matrix& x, const Matrix& y)
	{
		Matrix y_softmax_logistic(y);
		//logit
		y_softmax_logistic.unaryExpr(&Denn::PointFunction::logit<typename Matrix::Scalar>);
		//soft max
		softmax(y_softmax_logistic);
		//
		return cross_entropy(x, y_softmax_logistic);
	}

	template < typename Matrix >
	Matrix rows_argmax(const Matrix& x, const Matrix& y)
	{
		//output
		Matrix output(x.rows(), 1);
		////values
		typename Matrix::Index  max_index, i;
		//max-max
		for (typename Matrix::Index j = 0; j < x.rows(); ++j)
		{
			x.row(j).maxCoeff(&i, &max_index);
			output(j) = Matrix::Scalar(max_index);
		}
		//
		return output;
	}

	template < typename Matrix >
	Matrix correct_prediction(const Matrix& x, const Matrix& y)
	{
		//output
		Matrix output(x.rows(),1);
		////values
		typename Matrix::Index  max_index_x, max_index_y, i;
		//max-max
		for (typename Matrix::Index j = 0; j < x.rows(); ++j)
		{
			x.row(j).maxCoeff(&i, &max_index_x);
			y.row(j).maxCoeff(&i, &max_index_y);
			output(j) = typename Matrix::Scalar(max_index_x == max_index_y);
		}
		//
		return output;
	}

	template < typename Matrix >
	typename Matrix::Scalar accuracy(const Matrix& x, const Matrix& y)
	{
		#if 0
		return correct_prediction <Matrix>(x, y).mean();
		#else
		using Scalar = typename Matrix::Scalar;
		//output
		Scalar output = Scalar(0.0);;
		////values
		typename Matrix::Index  max_index_x, max_index_y, i;
		//max-max
		for (typename Matrix::Index j = 0; j < x.rows(); ++j)
		{
			x.row(j).maxCoeff(&i, &max_index_x);
			y.row(j).maxCoeff(&i, &max_index_y);
			output += Scalar(max_index_x == max_index_y);
		}
		//
		return output/Scalar(x.rows());
		#endif
	}

}
}