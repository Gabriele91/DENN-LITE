#include "DennActivationFunction.h"
#include "DennPerceptronLayer.h"
#include "DennDump.h"

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
		  ActivationFunction active_function
		, size_t features
		, size_t clazz
	)
	{
		set_activation_function(active_function);
		m_weights = Matrix::Zero(features, clazz);
		m_baias = Matrix::Zero(1, clazz);
	}

	PerceptronLayer::PerceptronLayer(const PerceptronLayer& lpptr)
	{
		m_weights = lpptr.m_weights;
		m_baias = lpptr.m_baias;
		m_activation_function = lpptr.m_activation_function;
		//test
		if (lpptr.m_context.get())
		{
			m_context = std::make_unique<BPContext>(*lpptr.m_context);
		}
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
	Matrix PerceptronLayer::apply(const Matrix& input) const
	{
		//get output
		Matrix layer_output = (input * m_weights).rowwise() + Eigen::Map<RowVector>((Scalar*)m_baias.data(), m_baias.cols()*m_baias.rows());
		//activation function?
		if (m_activation_function) return m_activation_function(layer_output);
		else                   return layer_output;
	}
	//////////////////////////////////////////////////
	//utils
	void PerceptronLayer::free_context()
	{
		m_context = nullptr;
	}
	PerceptronLayer::BPContext& PerceptronLayer::context()
	{
		if (!m_context)
		{
			m_context = std::make_unique<BPContext>();
			m_context->m_dweights = Matrix::Zero(m_weights.rows(), m_weights.cols());
			m_context->m_dbaias = Matrix::Zero(m_baias.rows(), m_baias.cols());
		}
		return *m_context;
	}
	Matrix& PerceptronLayer::output()
	{ 
		return context().m_output;
	}
	Matrix& PerceptronLayer::input()
	{
		return context().m_input;
	}
	Matrix& PerceptronLayer::dW()
	{
		return context().m_dweights;
	}
	Matrix& PerceptronLayer::dB()
	{
		return context().m_dbaias;
	}
	const PerceptronLayer::BPContext& PerceptronLayer::context() const
	{
		if (!m_context) m_context = std::make_unique<BPContext>();
		return *m_context;
	}
	const Matrix& PerceptronLayer::output() const
	{
		return context().m_output;
	}
	const Matrix& PerceptronLayer::input() const
	{
		return context().m_input;
	}
	const Matrix& PerceptronLayer::dW() const
	{
		return context().m_dweights;
	}
	const Matrix& PerceptronLayer::dB() const
	{
		return context().m_dbaias;
	}
	//////////////////////////////////////////////////
	Matrix PerceptronLayer::compute_delta(const Matrix& err) const
	{		
		//delta
		if (!m_activation_function || !m_activation_function.exists_function_derivate())
			return std::move(err.cwiseProduct(output()));
		//else
		Matrix output_(output());
		//compute
		output_ = m_activation_function.derive(output_);
		//cp
		return std::move(err.cwiseProduct(output_));
	}
	//Backpropagation stuff
	Matrix PerceptronLayer::feedforward(const Matrix& input_)
	{
		//save
		input() = input_;
		//compute
		output() = (input() * m_weights).rowwise() + MapRowVector(m_baias.data(), m_baias.cols()*m_baias.rows());
		//applay activation function
		if (m_activation_function)
		{
			return m_activation_function(output());
		}
		//else, return vanilla output
		return output();
	}
	Matrix PerceptronLayer::backpropagate(const Matrix& err, Scalar eta, Scalar momentum)
	{
		//delta
		Matrix delta = compute_delta(err).transpose();
		//get pref error
		Matrix prev_err = (m_weights * delta).transpose();
		//delta
		const int n_data = err.rows();
		//compute dW
		Matrix current_dW = (delta * input()).transpose() / n_data;
		Matrix current_db = delta.rowwise().mean().transpose();
		//update
		dW() = momentum * dW() + eta * current_dW;
		dB() = momentum * dB() + eta * current_db;
		//sgd
		m_weights -= dW();
		m_baias   -= dB();
		//magic print
		#if 0
		std::cout << m_weights.sum() << std::endl;
		std::cout << m_baias.sum() << std::endl;
		std::cout << "-" << std::endl;
		std::cout << input().sum() << std::endl;
		std::cout << output().sum() << std::endl;
		std::cout << "-------" << std::endl;
		#endif
		//back error
		return std::move(prev_err);
	}
	//////////////////////////////////////////////////
	ActivationFunction PerceptronLayer::get_activation_function() 
	{
		return m_activation_function;
	}
	void PerceptronLayer::set_activation_function(ActivationFunction active_function)
	{
		m_activation_function = ActivationFunctionFactory::name_of(active_function) == "linear" ? nullptr : active_function;
	}
    //////////////////////////////////////////////////
	size_t PerceptronLayer::size() const
	{
		return 2;
	}
	Matrix& PerceptronLayer::operator[](size_t i)
	{
		denn_assert(i < 2);
		if (i & 0x1) return  m_baias;  //1
		else		 return  m_weights;//0
	}
	const Matrix& PerceptronLayer::operator[](size_t i) const
	{
		denn_assert(i < 2);
		if (i & 0x1) return  m_baias;  //1
		else		 return  m_weights;//0
	}
	//////////////////////////////////////////////////
}
