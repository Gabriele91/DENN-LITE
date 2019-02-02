#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

namespace Denn
{
    //Conv2D
	class DennConv2DLayer : public Layer
	{
	public:
		///////////////////////////////////////
		DennConv2DLayer
		(
			  const Layer::Shape&			    shape
			, const Layer::Input&			    input
			, const Layer::VActivationFunction& active_functions
		);
		//////////////////////////////////////////////////
		Matrix& K(size_t size);
		//////////////////////////////////////////////////
		const Matrix& K(size_t size) const; 
		//////////////////////////////////////////////////
		virtual Layer::SPtr copy() const override;
		//////////////////////////////////////////////////
		virtual VActivationFunction get_activation_functions()										     override;
		virtual void                set_activation_functions(const VActivationFunction& active_function) override;
		//////////////////////////////////////////////////
		virtual VMatrix  apply(const VMatrix& input) const override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;
		virtual size_t ouput_paramters() const override;		
		virtual size_t input_shape_dims() const override { return 3; }
		virtual Layer::Shape output_shape() const override;
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////

	protected:
		//size
		struct KernelsShape
		{
			size_t m_weight{ 3 };
			size_t m_height{ 3 };
			size_t m_count{ 1 };
			KernelsShape() = default;

			KernelsShape(size_t weight, size_t height)
				: m_weight(weight)
				, m_height(height)
			{}

			KernelsShape(size_t weight, size_t height, size_t num)
				: m_weight(weight)
				, m_height(height)
				, m_count(num)
			{}
		};
		//Shape
		struct InputShape
		{
			size_t m_weight{ 3 };
			size_t m_height{ 3 };
			size_t m_channels{ 1 };
			InputShape() = default;

			InputShape(size_t weight, size_t height)
				: m_weight(weight)
				, m_height(height)
			{}

			InputShape(size_t weight, size_t height, size_t channels)
				: m_weight(weight)
				, m_height(height)
				, m_channels(channels)
			{}
		};
		//stride
		struct Stride
		{
			size_t m_x{ 1 };
			size_t m_y{ 1 };
			Stride() = default;

			Stride(size_t xy)
				: m_x(xy)
				, m_y(xy) {}

			Stride(size_t x, size_t y)
				: m_x(x)
				, m_y(x)
			{}
		};
        //parameters
		std::vector<Matrix> m_kernels;
		InputShape     m_input_shape;
		KernelsShape   m_kernel_shape;
		Stride         m_stride;
        //function
		ActivationFunction m_activation_function{ nullptr };
		//friends
		friend Matrix ImageSetConvolution
		(
			  const InputShape& in_shape
			, const Stride& in_stride
			, const Matrix& input
			, const Matrix& conv
			, const ActivationFunction& function
		);
		friend Matrix Convolution
		(
			  const Stride& strides
			, const Matrix& input
			, const Matrix& kernel
			, const ActivationFunction& function
		);
	};
	REGISTERED_LAYER(
		  DennConv2DLayer
		, "conv2d"
		, LayerMinMax(2, 3)           /* shape */
		, LayerMinMax(0, 3+2)		  /* number of hyper parameters as hidden layer */
		, LayerMinMax(1)   			  /* number of activation functions */
		, LayerMinMax(0, 3+2)	      /* number of hyper parameters as output layer */
		, DENN_CAN_BE_AN_INPUT_LAYER  /* input/output mode, can be first layer, same input/output */ 
		| DENN_CAN_BE_AN_HIDDEN_LAYER
	)
}