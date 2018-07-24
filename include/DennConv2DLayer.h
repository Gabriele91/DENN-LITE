#pragma once
#include "Config.h"
#include "DennLayer.h"
#include "DennActivationFunction.h"

namespace Denn
{
    //RNN
	class DennConv2DLayer : public Layer
	{
	public:
		//size
		struct KernelsShape
		{
			size_t m_weight{3};
			size_t m_height{3};
			size_t m_count{1};
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
			size_t m_weight{3};
			size_t m_height{3};
			size_t m_channels{1};
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
		DennConv2DLayer
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
		virtual Matrix  apply(const Matrix& input) const override;
		//////////////////////////////////////////////////
		virtual Matrix  feedforward(const Matrix& input, Matrix& linear_out)				                               override;
		virtual Matrix  backpropagate_delta(const Matrix& loss)       			               							   override;
		virtual Matrix  backpropagate_derive(const Matrix& delta, const Matrix& linear_out)       			               override;
		virtual VMatrix backpropagate_gradient(const Matrix& delta, const Matrix& linear_inpu, Scalar regular=Scalar(0.0)) override;
		//////////////////////////////////////////////////
		virtual VActivationFunction get_activation_functions()										     override;
		virtual void                set_activation_functions(const VActivationFunction& active_function) override;
		//////////////////////////////////////////////////
		virtual VMatrix  apply(const VMatrix& input) const														  			   override;
		virtual VMatrix  feedforward(const VMatrix& input, VMatrix& linear_out)				  							       override;
		virtual VMatrix  backpropagate_delta(const VMatrix& loss)     							                               override;		
		virtual VMatrix  backpropagate_derive(const VMatrix& delta, const VMatrix& linear_out)       			               override;
		virtual VVMatrix backpropagate_gradient(const VMatrix& delta, const VMatrix& linear_input, Scalar regular=Scalar(0.0)) override;
		//////////////////////////////////////////////////
		virtual size_t size() const override;
		virtual size_t size_ouput() const override;
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
		  DennConv2DLayer
		, "conv2D"
		, LayerMinMax(2+1,3+3+2)  /* number of hyper parameters as hidden layer */
		, LayerMinMax(1)   		  /* number of activation functions */
		, LayerMinMax(2+1,3+3+2)  /* number of hyper parameters as output layer */
		, DENN_CAN_GET_THE_INPUT  /* input/output mode, can be first layer, same input/output */ 
		| DENN_PASS_TROUGHT
	)
}