#include "PerceptronLayer.h"

namespace Denn
{
	///////////////////////////////////////
	PerceptronLayer::PerceptronLayer
	(
		  int features
		, int clazz
	)
	{
		m_weights.resize(features, clazz);
		m_baias.resize(1, clazz);
	}

	PerceptronLayer::PerceptronLayer
	(
		ActiveFunction::Ptr<Matrix> active_function
		, size_t features
		, size_t clazz
	)
	{
		m_active_function = active_function;
		m_weights.resize(features, clazz);
		m_baias.resize(1, clazz);
	}
	//////////////////////////////////////////////////
	Matrix& PerceptronLayer::weights() { return m_weights; }
	Matrix& PerceptronLayer::baias() { return m_baias; }
	//////////////////////////////////////////////////
	const Matrix& PerceptronLayer::weights() const { return m_weights; }
	const Matrix& PerceptronLayer::baias()   const { return m_baias; }
	//////////////////////////////////////////////////
	Layer::SPtr PerceptronLayer::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<PerceptronLayer>(*this));
	}
	//////////////////////////////////////////////////
	Matrix PerceptronLayer::apply(const Matrix& input) 
	{
		//get output
		//Matrix layer_output = (input * m_weights).rowwise() + RowVector(Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows()));
		Matrix layer_output = (input * m_weights).rowwise() + Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows());
		//Matrix layer_output = Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows()) + (input * m_weights).rowwise();
		//activation function?
		if (m_active_function) return m_active_function(layer_output);
		else                   return layer_output;
	}
	//////////////////////////////////////////////////
	size_t PerceptronLayer::size() const
	{
		return 2;
	}
	Matrix& PerceptronLayer::operator[](size_t i)
	{
		if (i & 0x1) return  m_baias;
		else		 return  m_weights;
	}
	const Matrix& PerceptronLayer::operator[](size_t i) const
	{
		if (i & 0x1) return  m_baias;
		else		 return  m_weights;
	}
	//////////////////////////////////////////////////
}