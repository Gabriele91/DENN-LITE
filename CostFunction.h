#pragma once
#include "Config.h"
#include "ActiveFunction.h"

namespace Denn
{
namespace CostFunction
{

	template < typename Matrix >
	Matrix cross_entropy_by_rows(const Matrix& x, const Matrix& y)
	{
		const typename Matrix::Scalar const_ep = 1e-10;
		Matrix log_y = (y.array() + const_ep).matrix().unaryExpr(&Denn::PointFunction::log<typename Matrix::Scalar>);
		return -((x.array() * log_y.array()).matrix().rowwise()).sum();
	}	

	template < typename Matrix >
	typename Matrix::Scalar cross_entropy(const Matrix& x, const Matrix& y)
	{
		const typename Matrix::Scalar const_ep = 1e-10 ;
		Matrix log_y = (y.array() + const_ep).matrix().unaryExpr(&Denn::PointFunction::log<typename Matrix::Scalar>);
		return -(x.array() * log_y.array()).sum() / (typename Matrix::Scalar)y.rows();
	}

	template < typename Matrix >
	typename Matrix::Scalar softmax_cross_entropy(const Matrix& x, const Matrix& y)
	{
		Matrix y_softmax(y);
		ActiveFunction::softmax(y_softmax);
		return cross_entropy(x, y_softmax);
	}

	template < typename Matrix >
	typename Matrix::Scalar softmax_cross_entropy_with_logit(const Matrix& x, const Matrix& y)
	{
		Matrix y_softmax_logistic(y);
		ActiveFunction::logit(y_softmax_logistic);
		ActiveFunction::softmax(y_softmax_logistic);
		return cross_entropy(x, y_softmax_logistic);
	}

	template < typename Matrix >
	Matrix rows_argmax(const Matrix& x, const Matrix& y)
	{
		//output
		Matrix output(x.rows(), 1);
		////values
		typename Matrix::Index  max_index;
		//max-max
		for (Matrix::Index j = 0; j < x.rows(); ++j)
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
		for (Matrix::Index j = 0; j < x.rows(); ++j)
		{
			x.row(j).maxCoeff(&i, &max_index_x);
			y.row(j).maxCoeff(&i, &max_index_y);
			output(j) = Matrix::Scalar(max_index_x == max_index_y);
		}
		//
		return output;
	}

	template < typename Matrix >
	typename Matrix::Scalar accuracy(const Matrix& x, const Matrix& y)
	{
		return correct_prediction <Matrix>(x, y).mean();
	}

}
}