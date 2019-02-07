#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"
#include "DennUtilitiesConvolution.h"

namespace Denn
{

	class FullyConnected : public Layer
	{
	public:
		///////////////////////////////////////
		/// FullyConnected
		///
		/// \param features      Width of the input.
		/// \param clazz         Width of the output.
		///
		FullyConnected
		(
			  ActivationFunction active_function
			, int features
			, int clazz
		);
		FullyConnected
		(
			  const ActivationFunctions& active_function
			, const Shape& in
			, const Inputs& metadata
		);
		//////////////////////////////////////////////////
		virtual Layer::SPtr copy() const override;
		//////////////////////////////////////////////////
		virtual const Inputs inputs() const override;
		//////////////////////////////////////////////////
		virtual void activations(ActivationFunctions) override;
		virtual ActivationFunctions activations() const override;
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

	REGISTERED_LAYER(
		FullyConnected,
		LAYER_NAMES("fully_connected", "fc"),
		LayerShapeType(SHAPE_1D),		//shape type
		LayerDescription::MinMax { 1 },	//1 argument
		LayerDescription::MinMax { 1 }	//1 activation
	)
}