#include <cmath>
#include "DennCostFunction.h"
#include "DennActivationFunction.h"
#include "DennConv2DLayer.h"

namespace Denn
{
	enum CONV_MODE
	{
		CONV2D_VALID,
		CONV2D_FULL
	};

	static Matrix Convolution
	(
		  const Matrix& input
		, const Matrix& conv
		, const ActivationFunction& function
		, CONV_MODE mode = CONV2D_FULL
	)
	{
		//output matrix
		Matrix output;
		/* The actual formulation can be seen in .md file */
		switch(mode)
		{
			default:
			case CONV2D_VALID:
				output.resize(input.rows() - conv.rows() + 1, input.cols() - conv.cols() + 1);
				for (int i = 0; i < output.rows(); i++)
				{
					for (int j = 0; j < output.cols(); j++)
					{
						output(i, j) = (input.block(i, j, conv.rows(), conv.cols()).array() * conv.array()).sum();
					}
				}
				return function.apply(output);
			break;
			case CONV2D_FULL:
				output = Matrix::Zero(input.rows() + 2 * conv.rows() - 2, input.cols() + 2 * conv.cols() - 2);
				output.block(conv.rows() - 1, conv.cols() - 1, input.rows(), input.cols()) = input;
				return Convolution(output, conv, function, CONV2D_VALID);
			break;
		}
	}
	///////////////////////////////////////	
	DennConv2DLayer::DennConv2DLayer
	(
		  Size size
		, Stride strides
	)
	{
		m_activation_function = ActivationFunctionFactory::get("sigmoid");
		m_kernel_size = size;
		m_stride = strides;
		//build
		m_kernels.resize(size.m_count);
		for(size_t i = 0; i != size.m_count; ++i)
		{
			m_kernels[i].resize(size.m_height, size.m_weight);
		}

	}

	DennConv2DLayer::DennConv2DLayer
	(
		  ActivationFunction active_function
		, Size size
		, Stride strides
	)
	{
		m_activation_function = active_function;
		m_kernel_size = size;
		m_stride = strides;
		//build
		m_kernels.resize(size.m_count);
		for(size_t i = 0; i != size.m_count; ++i)
		{
			m_kernels[i].resize(size.m_height, size.m_weight);
		}
	}

	DennConv2DLayer::DennConv2DLayer
	(
		  const std::vector< ActivationFunction >& active_functions
		, const std::vector< size_t >&			   input_output
	)
	{
		//only first
		m_activation_function = active_functions.size() 
							  ? active_functions[0] 
							  : ActivationFunctionFactory::get("sigmoid");

		switch(input_output.size())
		{
			default:
			case 5:
				m_kernel_size.m_weight = active_functions[0];
				m_kernel_size.m_height = active_functions[1];
				m_kernel_size.m_count  = active_functions[2];
				m_stride.m_x 		   = active_functions[3];
				m_stride.m_y 		   = active_functions[4];
			break;
			case 4:
				m_kernel_size.m_weight = active_functions[0];
				m_kernel_size.m_height = active_functions[1];
				m_kernel_size.m_count  = active_functions[2];
				m_stride.m_x 		   = 
				m_stride.m_y 		   = active_functions[3];
			break;
			case 3:
				m_kernel_size.m_weight = active_functions[0];
				m_kernel_size.m_height = active_functions[1];
				m_kernel_size.m_count  = active_functions[2];
			break;
			case 2:
				m_kernel_size.m_weight = active_functions[0];
				m_kernel_size.m_height = active_functions[1];
			break;
			case 1:
				m_kernel_size.m_weight =
				m_kernel_size.m_height = active_functions[1];
			break;
			case 0:
				denn_assert(active_functions.size());
			break;
		}
		//build
		m_kernels.resize(m_kernel_size.m_count);
		for(size_t i = 0; i != m_kernel_size.m_count; ++i)
		{
			m_kernels[i].resize(m_kernel_size.m_height, m_kernel_size.m_weight);
		}
	}
    //////////////////////////////////////////////////
	Layer::VActivationFunction DennConv2DLayer::get_activation_functions()
	{
		return { m_activation_function };
	}
	void DennConv2DLayer::set_activation_functions(const Layer::VActivationFunction& active_functions)
	{
		if (active_functions.size())
		{
			m_activation_function = active_functions[0];
		}
	}
	//////////////////////////////////////////////////
	Layer::SPtr DennConv2DLayer::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<DennConv2DLayer>(*this));
	}
	//////////////////////////////////////////////////    
	Matrix  DennConv2DLayer::apply(const Matrix& input) const
	{
		return apply(VMatrix{input}).back();
	}
    Layer::VMatrix  DennConv2DLayer::apply(const VMatrix& inputs) const
    {
		//outputs
        Layer::VMatrix o;
		//for each input
		for(size_t i = 0; i != inputs.size(); ++i)
		{
			//for each kernels
			for(size_t k = 0; k != m_kernels.size(); ++k)
			{
				//apply kernel
				o.push_back(Convolution(inputs[i], m_kernels[k], m_activation_function));
			}
		}
		//ok
        return o;
    }
    //////////////////////////////////////////////////
    size_t DennConv2DLayer::size() const
	{
		return m_kernels.size();
	}
	size_t DennConv2DLayer::size_ouput() const
	{
		return 0;
	}
	Matrix& DennConv2DLayer::operator[](size_t i)
	{
		denn_assert(i < size());
		return m_kernels[i];
	}
	const Matrix& DennConv2DLayer::operator[](size_t i) const
	{
		denn_assert(i < size());
		return m_kernels[i];
	}
    /////////////////////////////////////////////////////////////////////////////////////////////
    /// TODO
    /////////////////////////////////////////////////////////////////////////////////////////////
	Matrix DennConv2DLayer::feedforward(const Matrix& input, Matrix& l_out)
	{
        denn_assert(false);
		//return
		return {};
	}
	Matrix DennConv2DLayer::backpropagate_delta(const Matrix& loss)
    {
        denn_assert(false);
		//return
		return {};
    }
    Matrix DennConv2DLayer::backpropagate_derive(const Matrix& delta, const Matrix& l_out)
    {
        denn_assert(false);
		//return
		return {};
    }
    Layer::VMatrix DennConv2DLayer::backpropagate_gradient(const Matrix& delta, const Matrix& l_in, Scalar regular)
    {
        denn_assert(false);
		//return
		return {};
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /// TODO
    /////////////////////////////////////////////////////////////////////////////////////////////
    Layer::VMatrix DennConv2DLayer::feedforward(const VMatrix& inputs, VMatrix& linear_outs)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VMatrix DennConv2DLayer::backpropagate_delta(const VMatrix& vloss)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VMatrix DennConv2DLayer::backpropagate_derive(const VMatrix& deltas, const VMatrix& linear_outs)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VVMatrix DennConv2DLayer::backpropagate_gradient(const VMatrix& deltas, const VMatrix& linear_inputs, Scalar regular)
	{
        denn_assert(false);
		//return
		return {};
	}
    /////////////////////////////////////////////////////////////////////////////////////////////
}