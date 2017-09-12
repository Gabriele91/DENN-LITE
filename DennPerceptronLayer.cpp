#include "DennActiveFunction.h"
#include "DennPerceptronLayer.h"

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
		  ActiveFunction active_function
		, size_t features
		, size_t clazz
	)
	{
		m_active_function = ActiveFunctionFactory::name_of(active_function) == "linear" ? nullptr : active_function;
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
		Matrix layer_output = (input * m_weights).rowwise() + Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows());
		//activation function?
		if (m_active_function) return m_active_function(layer_output);
		else                   return layer_output;
	}
	Matrix PerceptronLayer::feedforward(const Matrix& input, Matrix& ff_out)
	{		
		//get output
		ff_out = (input * m_weights).rowwise() + Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows());
		//activation function?
		if (m_active_function) return m_active_function(Matrix(ff_out));
		else                   return ff_out;
	}
	Matrix PerceptronLayer::backpropagate_delta(const Matrix& bp_delta, const Matrix& ff_out)
	{
		//////////////////////////////////////////////////////////////////////
		Matrix g(ff_out);
		//derivate of active function
		if (m_active_function.exists_function_derivate()) m_active_function.derive(g); // g := D(f(x))
		else											  g.fill(1.0); //f(x) = x, g := D(f(x)) => 1.0
		//////////////////////////////////////////////////////////////////////
		//return (m_weights.transpose() * bp_delta).colwise().cwiseProduct(g);
		//return g.asDiagonal() * (bp_delta * m_weights.transpose());
		return (bp_delta * m_weights.transpose()).cwiseProduct(g);
		//////////////////////////////////////////////////////////////////////
	}
	std::vector<Matrix> PerceptronLayer::backpropagate_gradient(const Matrix& bp_delta, const Matrix& ff_out, size_t input_samples, Scalar lambda)
	{
		// add regularization to weights, bias weights are not regularized
		Matrix dEdW = (ff_out.transpose() * bp_delta + lambda*m_weights) / Scalar(input_samples);
		//Matrix dEdW = ((lambda*m_weights).colwise() + bp_delta.transpose() * ff_out) / Scalar(input_samples);
		Matrix dEdb =  (bp_delta.colwise().sum() / Scalar(input_samples));
		//J = Scalar(0.5) * lambda * m_weights.array().square().sum() / Scalar(input_samples);
		return std::vector<Matrix>{dEdW /* this[0] = w */, dEdb /* this[1] = b */};
	}
	//////////////////////////////////////////////////
	size_t PerceptronLayer::size() const
	{
		return 2;
	}
	Matrix& PerceptronLayer::operator[](size_t i)
	{
		assert(i < 2);
		if (i & 0x1) return  m_baias;  //1
		else		 return  m_weights;//0
	}
	const Matrix& PerceptronLayer::operator[](size_t i) const
	{
		assert(i < 2);
		if (i & 0x1) return  m_baias;  //1
		else		 return  m_weights;//0
	}
	//////////////////////////////////////////////////
}