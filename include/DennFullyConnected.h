#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

namespace Denn
{

	class FullyConnected : public Layer
	{
	public:
		///////////////////////////////////////
		FullyConnected
		(
			  ActivationFunction active_function
			, Shape inshape
			, Shape outshape
		);
		FullyConnected
		(
			  ActivationFunction active_function
			, int features
			, int clazz
		);
		//////////////////////////////////////////////////
		virtual Layer::SPtr copy() const override;
		//////////////////////////////////////////////////
		virtual void activation(ActivationFunction) override;
		virtual ActivationFunction activation() const override;
		//////////////////////////////////////////////////
		virtual const Matrix& feedforward(const Matrix& input) override;
		virtual const Matrix& backpropagate(const Matrix& prev_layer_data, const Matrix& next_layer_data) override;
		//////////////////////////////////////////////////
		virtual void update(const Optimizer& optimize) override;
		//////////////////////////////////////////////////
		virtual const Matrix& ff_output() override;
		virtual const Matrix& bp_output() override;
		//////////////////////////////////////////////////
		virtual size_t size() const operator_override;
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////
	protected:    
		//weight
		Matrix m_weight;  // Weight parameters, W(in_size x out_size)
		Matrix m_bias;    // Bias parameters, b(out_size x 1)
		//backpropagation
		Matrix m_dw;      // Derivative of weights
		Matrix m_db;      // Derivative of bias
		Matrix m_z;       // Linear term, z = W' * in + b
		Matrix m_a;       // Output of this layer, a = act(z)
		Matrix m_din;     // Derivative of the input of this layer.
						  // Note that input of this layer is also the output of previous layer
		//activation
		ActivationFunction m_activation_function;
	};
}