#include "DennFullyConnected.h"

namespace Denn
{
	///////////////////////////////////////	
	FullyConnected::FullyConnected
	(
		  ActivationFunction active_function
		, int features
		, int clazz
	)
	: Layer("fully_connected",{ features }, { clazz })
	, m_activation_function(active_function)
	{
		//weight
		m_weight.resize(int(this->m_in_size), int(this->m_out_size));
		m_bias.resize(int(this->m_out_size), 1);
		//derivate
		m_dw.resize(int(this->m_in_size), int(this->m_out_size));
		m_db.resize(int(this->m_out_size), 1);
	}
	FullyConnected::FullyConnected
	(
		  const ActivationFunctions& active_function
		, const Shape& in
		, const Inputs& metadata
	)
	: FullyConnected(active_function[0], in.size3D(), metadata[0])
	{
	}
	//////////////////////////////////////////////////
	const Inputs FullyConnected::inputs() const
	{
		return { out_size().width() };
	}
	//////////////////////////////////////////////////
	void FullyConnected::activations(ActivationFunctions active_function)
	{
		m_activation_function = active_function[0];
	}
	ActivationFunctions FullyConnected::activations() const
	{
		return { m_activation_function };
	}
	//////////////////////////////////////////////////
	Layer::SPtr FullyConnected::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<FullyConnected>(*this));
	}

	//////////////////////////////////////////////////
	const Matrix& FullyConnected::feedforward(const Matrix& prev_layer_data)
	{
		const int nobs = prev_layer_data.cols();
		// Linear term z = W' * in + b
		m_z.resize((int)this->m_out_size, nobs);
		m_z.noalias() = m_weight.transpose().lazyProduct( prev_layer_data ); //when the dataset is bigger than W, lazyProduct is faster
		m_z.colwise() += ColVector::AlignedMapType(m_bias.data(), m_bias.size());
		// Apply activation function
		m_a.resize((int)this->m_out_size, nobs);
		m_activation_function.activate(m_z, m_a);
		//return the same
		return m_a;
	}
	const Matrix&  FullyConnected::backpropagate(const Matrix& prev_layer_data, const Matrix& next_layer_data)
    {
		const int nobs = prev_layer_data.cols();
		// After forward stage, m_z contains z = W' * in + b
		// Now we need to calculate d(L) / d(z) = [d(a) / d(z)] * [d(L) / d(a)]
		// d(L) / d(a) is computed in the next layer, contained in next_layer_data
		// The Jacobian matrix J = d(a) / d(z) is determined by the activation function
		Matrix& dLz = m_z;
		m_activation_function.jacobian(m_z, m_a, next_layer_data, dLz);

		// Now dLz contains d(L) / d(z)
		// Derivative for weights, d(L) / d(W) = [d(L) / d(z)] * in'
		m_dw.noalias() = prev_layer_data * dLz.transpose() / nobs;

		// Derivative for bias, d(L) / d(b) = d(L) / d(z)
		m_db.noalias() = dLz.rowwise().mean();

		// Compute d(L) / d_in = W * [d(L) / d(z)]
		m_din.resize((int)this->m_in_size, nobs);
		m_din.noalias() = m_weight * dLz;

		//return gradient
		return m_din;
    }
	void FullyConnected::update(const Optimizer& optimize)
	{
		
		ColVector::ConstAlignedMapType dw(m_dw.data(), m_dw.size());
		ColVector::ConstAlignedMapType db(m_db.data(), m_db.size());
		ColVector::AlignedMapType      w(m_weight.data(), m_weight.size());
		ColVector::AlignedMapType      b(m_bias.data(), m_bias.size());

		optimize.update(dw, w);
		optimize.update(db, b);
	}
	//////////////////////////////////////////////////
	const Matrix& FullyConnected::ff_output()
	{
		return m_a;
	}
	const Matrix& FullyConnected::bp_output()
	{
		return m_din;
	}
    //////////////////////////////////////////////////
	size_t FullyConnected::size() const
	{
		return 2;
	}
	Matrix& FullyConnected::operator[](size_t i)
	{
		denn_assert(i < 2);
		switch (i)
		{
		default:
		case 0: return  m_weight;
		case 1: return  m_bias;
		}
	}
	const Matrix& FullyConnected::operator[](size_t i) const
	{
		denn_assert(i < 2);
		switch (i)
		{
		default:
		case 0: return  m_weight;
		case 1: return  m_bias;
		}
	}
	//////////////////////////////////////////////////
}
