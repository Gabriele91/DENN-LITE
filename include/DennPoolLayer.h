#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

namespace Denn
{
	//Conv2D
	class DennPoolLayer : public Layer
	{
	public:
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
		///////////////////////////////////////
		DennPoolLayer
		(
			  InputShape in_shape
			, KernelsShape k_size
			, Stride strides = Stride(1)
		);

		DennConv2DLayer
		(
			ActivationFunction active_function_c_h
			, InputShape in_shape
			, KernelsShape k_size
			, Stride strides = Stride(1)
		);

		DennConv2DLayer
		(
			const std::vector< ActivationFunction >& active_function
			, const std::vector< size_t >& input_output
		);
		//////////////////////////////////////////////////
		Matrix& K(size_t size);
		//////////////////////////////////////////////////
		const Matrix& K(size_t size) const;
		//////////////////////////////////////////////////
		virtual Layer::SPtr copy() const override;
		//////////////////////////////////////////////////
		virtual VMatrix  apply(const VMatrix& input) const override;
		//////////////////////////////////////////////////
		//Backpropagation stuff
		virtual VMatrix   feedforward(const VMatrix& input) { assert(0); return {}; }
		virtual VMatrix   backpropagate(const VMatrix& input) { assert(0); }
		virtual void      optimizer(BPOptimizer& optimizer) { assert(0); }
		//////////////////////////////////////////////////
		virtual VActivationFunction get_activation_functions() override { assert(0); };
		virtual void                set_activation_functions(const VActivationFunction& active_function) override { assert(0); };
		//////////////////////////////////////////////////
		virtual size_t size() const override;
		virtual size_t ouput_paramters() const override;
		virtual Matrix& operator[](size_t i) operator_override;
		virtual const Matrix& operator[](size_t i) const operator_override;
		//////////////////////////////////////////////////

	protected:
		//parameters
		std::vector<Matrix> m_kernels;
		InputShape     m_input_shape;
		KernelsShape   m_kernel_shape;
		Stride         m_stride;
		//function
		ActivationFunction m_activation_function{ nullptr };
	};
	REGISTERED_LAYER(
		DennPoolLayer
		, "pool"
		, LayerMinMax(2 + 1, 3 + 3 + 2)  /* number of hyper parameters as hidden layer */
		, LayerMinMax(1)   		  /* number of activation functions */
		, LayerMinMax(2 + 1, 3 + 3 + 2)  /* number of hyper parameters as output layer */
		, DENN_CAN_BE_AN_INPUT_LAYER  /* input/output mode, can be first layer, same input/output */
		| DENN_CAN_BE_AN_HIDDEN_LAYER
	)
}