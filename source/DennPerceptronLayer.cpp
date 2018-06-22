#include "DennActivationFunction.h"
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
		  ActivationFunction active_function
		, size_t features
		, size_t clazz
	)
	{
		set_activation_function(active_function);
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
	Matrix PerceptronLayer::apply(const Matrix& input) const
	{
		//get output
		Matrix layer_output = (input * m_weights).rowwise() + Eigen::Map<RowVector>((Scalar*)m_baias.data(), m_baias.cols()*m_baias.rows());
		//activation function?
		if (m_activation_function) return m_activation_function(layer_output);
		else                   return layer_output;
	}
	//////////////////////////////////////////////////
	Matrix PerceptronLayer::feedforward(const Matrix& input, Matrix& l_out)
	{
		//get output
		l_out = (input * m_weights).rowwise() + Eigen::Map<RowVector>(m_baias.data(), m_baias.cols()*m_baias.rows());
		//activation function?
		if (m_activation_function)
		{
			Matrix out_matrix(l_out);
            m_activation_function(out_matrix);
			return out_matrix;
		} 
		//return the same
		return l_out; 
	}
	Matrix PerceptronLayer::backpropagate_delta(const Matrix& loss)
    {
		Matrix delta = m_weights * loss;
		return delta;
    }
    Matrix PerceptronLayer::backpropagate_derive(const Matrix& delta, const Matrix& l_out)
    {
		#define CP(x,y) (x).cwiseProduct(y)
		#define CAN_DERIVE (m_activation_function && m_activation_function.exists_function_derivate())
        //////////////////////////////////////////////////////////////////////
        if (CAN_DERIVE)
        {
			//copy x
            Matrix x_(l_out); 
            //derivate of active function x':= D_f(x)
			x_ = m_activation_function.derive(x_); //inplace
			// x' * delta
            return CP(x_.transpose(), delta);
        }
		return delta;
        //////////////////////////////////////////////////////////////////////
    }
    std::vector<Matrix> PerceptronLayer::backpropagate_gradient(const Matrix& delta, const Matrix& l_in, Scalar regular)
    {
        //add regular factor
        if (regular != Scalar(0.0))
		{
			return std::vector<Matrix>
			{
				(delta * l_in).transpose() + regular * m_weights,
				delta.transpose().colwise().sum()
			};
		} 
        //return
        return std::vector<Matrix>
		{
			(delta * l_in).transpose(),
			delta.transpose().colwise().sum()
		};
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
	Layer::VMatrix PerceptronLayer::apply(const std::vector<Matrix>& inputs) const
	{
		VMatrix vout; vout.reserve(inputs.size());
		for(auto& input : inputs) vout.push_back(apply(input));
		return vout;
	}
	Layer::VMatrix PerceptronLayer::feedforward(const VMatrix& inputs, VMatrix& linear_outs)
	{
		//alloc output
		VMatrix vout; 
		vout.reserve(inputs.size());
		linear_outs.resize(inputs.size());
		//compute
		for(size_t i=0; i!=inputs.size() ;++i) vout.push_back(feedforward(inputs[i],linear_outs[i]));
		//return
		return vout;
	}
	Layer::VMatrix PerceptronLayer::backpropagate_delta(const VMatrix& vloss)
	{
		//alloc
		VMatrix vout;
		vout.reserve(vloss.size());
		//compute
		for(auto& loss : vloss) vout.push_back(backpropagate_delta(loss));
		return vout;
	}
	Layer::VMatrix PerceptronLayer::backpropagate_derive(const VMatrix& deltas, const VMatrix& linear_outs)
	{
		//alloc
		VMatrix vout; 
		vout.reserve(deltas.size());
		//compute
		for(size_t i=0; i!=deltas.size() ;++i) vout.push_back(backpropagate_derive(deltas[i],linear_outs[i]));
		return vout;
	}
	Layer::VVMatrix PerceptronLayer::backpropagate_gradient(const VMatrix& deltas, const VMatrix& linear_inputs, Scalar regular)
	{
		//alloc
		VVMatrix vout; 
		vout.reserve(deltas.size());
		//compute
		for(size_t i=0; i!=deltas.size() ;++i) vout.push_back(backpropagate_gradient(deltas[i],linear_inputs[i],regular));
		return vout;
	}
}
