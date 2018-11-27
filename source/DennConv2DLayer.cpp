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
		  const DennConv2DLayer::Stride& strides
		, const Matrix& input
		, const Matrix& kernel
		, const ActivationFunction& function
	)
	{
		//output matrix
		Matrix output;

		//info
		Index input_w = input.cols();
		Index input_h = input.rows();
		Index kernel_w = kernel.cols();
		Index kernel_h = kernel.rows();

		//output
		Index output_width = Index(std::ceil(float(input_w) / float(strides.m_x)));
		Index output_height = Index(std::ceil(float(input_h) / float(strides.m_y)));

		// Calculate the number of zeros which are needed to add as padding
		Index pad_along_width  = std::max<Index>((output_width - 1) * strides.m_x + kernel_w - input_w, 0);
		Index pad_along_height = std::max<Index>((output_height - 1) * strides.m_y + kernel_h - input_h, 0);
		Index pad_top      = std::floor(pad_along_height / 2);// amount of zero padding on the top
		Index pad_bottom   = pad_along_height - pad_top;      // amount of zero padding on the bottom
		Index pad_left     = std::floor(pad_along_width / 2); // amount of zero padding on the left
		Index pad_right    = pad_along_width - pad_left;      // amount of zero padding on the right
		//output
		output.resize(output_height, output_width);
		//input padded
		Matrix input_padded(input.rows() + pad_along_height, input.cols() + pad_along_width);
		input_padded.setZero();
		input_padded.block(pad_top, pad_left, input_h, input_w) = input;
		
		for (Index i = 0; i < output.rows(); i++)
		for (Index j = 0; j < output.cols(); j++)
		{
			output(i, j) = (input_padded.block(
				  i *  strides.m_y
				, j * strides.m_x
				, kernel.rows()
				, kernel.cols()
			).array() * kernel.array()).sum();
		}
		return std::move(function.apply(output));
	}
		
	static Matrix ImageSetConvolution
	(
		  const DennConv2DLayer::InputShape& in_shape
		, const DennConv2DLayer::Stride& in_stride
		, const Matrix& input
		, const Matrix& conv
		, const ActivationFunction& function
		, CONV_MODE mode = CONV2D_FULL
	)
	{
		//assert
		denn_assert( input.cols() == in_shape.m_height*in_shape.m_weight );
		//output matrix
		Matrix output; 
		//allocs
		output.resize(input.rows(),input.cols()) ;
		/* The actual formulation can be seen in .md file */
		for(size_t i = 0; i != input.rows(); ++i)
		{
			//Cast to matrix
			Matrix row = input.row(i);
			//auto pass = Eigen::Stride<Eigen::Dynamic, 1>(Index(in_shape.m_channels)); //todo
			auto image = MapMatrix(row.data(), Index(in_shape.m_height),  Index(in_shape.m_weight));
			//execute
			auto example = Convolution(in_stride, image, conv, function);
			//vector
			auto rowend = MapRowVector(example.array().data(), example.size());
			//save
			output.row(i) = rowend;
		}
		return output;
	}
	///////////////////////////////////////	
	DennConv2DLayer::DennConv2DLayer
	(
		  InputShape in_shape
		, KernelsShape k_shape
		, Stride strides
	)
	{
		m_activation_function = ActivationFunctionFactory::get("sigmoid");
		m_input_shape = in_shape;
		m_kernel_shape = k_shape;
		m_stride = strides;
		//build
		m_kernels.resize(m_kernel_shape.m_count);
		for(size_t i = 0; i != m_kernel_shape.m_count; ++i)
		{
			m_kernels[i].resize(m_kernel_shape.m_height, m_kernel_shape.m_weight);
		}

	}

	DennConv2DLayer::DennConv2DLayer
	(
		  ActivationFunction active_function
		, InputShape in_shape
		, KernelsShape k_shape
		, Stride strides
	)
	{
		m_activation_function = active_function;
		m_input_shape = in_shape;
		m_kernel_shape = k_shape;
		m_stride = strides;
		//build
		m_kernels.resize(m_kernel_shape.m_count);
		for(size_t i = 0; i != m_kernel_shape.m_count; ++i)
		{
			m_kernels[i].resize(m_kernel_shape.m_height, m_kernel_shape.m_weight);
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
			case 8:
				m_input_shape.m_weight = input_output[0];
				m_input_shape.m_height = input_output[1];
				m_input_shape.m_channels = input_output[2];
				m_kernel_shape.m_weight = input_output[3];
				m_kernel_shape.m_height = input_output[4];
				m_kernel_shape.m_count  = input_output[5];
				m_stride.m_x 		   = input_output[6];
				m_stride.m_y 		   = input_output[7];
			break;

			case 7:
				m_input_shape.m_weight = input_output[0];
				m_input_shape.m_height = input_output[1];
				m_input_shape.m_channels = input_output[2];
				m_kernel_shape.m_weight = input_output[3];
				m_kernel_shape.m_height = input_output[4];
				m_kernel_shape.m_count  = input_output[5];
				m_stride.m_x 		   = 
				m_stride.m_y 		   = input_output[6];
			break;

			case 6:
				m_input_shape.m_weight = input_output[0];
				m_input_shape.m_height = input_output[1];
				m_input_shape.m_channels = input_output[2];
				m_kernel_shape.m_weight = input_output[3];
				m_kernel_shape.m_height = input_output[4];
				m_kernel_shape.m_count  = input_output[5];
			break;

			case 5:
				m_input_shape.m_weight = input_output[0];
				m_input_shape.m_height = input_output[1];
				m_input_shape.m_channels = input_output[2];
				m_kernel_shape.m_weight = input_output[3];
				m_kernel_shape.m_height = input_output[4];
			break;

			case 4:
				m_input_shape.m_weight = input_output[0];
				m_input_shape.m_height = input_output[1];
				m_kernel_shape.m_weight = input_output[2];
				m_kernel_shape.m_height = input_output[3];
			break;

			case 3:
				m_input_shape.m_weight = input_output[0];
				m_input_shape.m_height = input_output[1];
				m_kernel_shape.m_weight = 
				m_kernel_shape.m_height = input_output[3];
			break;

			default:
			case 0:
				denn_assert(input_output.size());
			break;
		}
		//build
		m_kernels.resize(m_kernel_shape.m_count);
		for(size_t i = 0; i != m_kernel_shape.m_count; ++i)
		{
			m_kernels[i].resize(m_kernel_shape.m_height, m_kernel_shape.m_weight);
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
		//for each kernels
 		for(size_t k = 0; k != m_kernels.size(); ++k)
		{
			//apply kernel
			o.push_back(ImageSetConvolution(m_input_shape, m_stride, inputs[i], m_kernels[k], m_activation_function));
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