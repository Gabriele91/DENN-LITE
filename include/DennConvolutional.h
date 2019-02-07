#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"
#include "DennUtilitiesConvolution.h"

namespace Denn
{

	class Convolutional : public Layer
	{
	public:
		///////////////////////////////////////
		/// Constructor
		///
		/// \param in_width      Width of the input image in each channel.
		/// \param in_height     Height of the input image in each channel.
		/// \param in_channels   Number of input channels.
		/// \param window_width  Width of the filter.
		/// \param window_height Height of the filter.
		/// \param out_channels  Number of output channels.
		///
		Convolutional
		(
			  ActivationFunction active_function
			, int in_width, int in_height, int in_channels
			, int window_width, int window_height, int out_channels
		);
		Convolutional
		(
			  const ActivationFunctions & active_function
			, const Shape& in
			, const std::vector<int>& metadata
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
		//shape conv
		internal::ConvDims m_dim;		// dimensions of convolution

		//weight
		Matrix    m_filter_data;        // Filter parameters. Total length is
										// (in_channels x out_channels x filter_rows x filter_cols)
										// See Utils/Convolution.h for its layout

		ColVector m_df_data;            // Derivative of filters, same dimension as m_filter_data

		//bias    
		Matrix    m_bias;               // Bias term for the output channels, out_channels x 1. (One bias term per channel)
		ColVector m_db;                 // Derivative of bias, same dimension as m_bias

		//output
		Matrix m_z;       // Linear term, z = conv(in, w) + b. Each column is an observation
		Matrix m_a;       // Output of this layer, a = act(z)
		Matrix m_din;     // Derivative of the input of this layer
                          // Note that input of this layer is also the output of previous layer
		//activation
		ActivationFunction m_activation_function;
	};

	REGISTERED_LAYER(
		Convolutional,
		LAYER_NAMES("convolutional", "conv"),
		LayerShapeType(SHAPE_2D_3D),     //shape 2D/3D
		LayerDescription::MinMax{ 2,3 }, //min args filter size, max args filter size + channels
		LayerDescription::MinMax{ 1 }	 //1 activation
	)
}