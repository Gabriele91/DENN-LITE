#include <cmath>
#include "DennCostFunction.h"
#include "DennActivationFunction.h"
#include "DennRecurrentLayer.h"
//http://www.wildml.com/2015/09/recurrent-neural-networks-tutorial-part-2-implementing-a-language-model-rnn-with-python-numpy-and-theano/

namespace Denn
{
	///////////////////////////////////////	
	RecurrentLayer::RecurrentLayer
	(
		  const Layer::Shape&			    shape
		, const Layer::Input&			    input
		, const Layer::VActivationFunction& active_functions
	)
	{

		//get layout
		size_t features = shape[0];

		//info
		size_t weights  = input[0];
		size_t output   = input.size() > 1 ? input[1] : features;

		if (active_functions.size() > 1)
		{
			m_activation_function_inside = ActivationFunctionFactory::name_of(active_functions[0]) == "linear" ? nullptr : active_functions[0];
			m_activation_function_output = ActivationFunctionFactory::name_of(active_functions[1]) == "linear" ? nullptr : active_functions[1];
		}
		else
		{
			m_activation_function_inside = ActivationFunctionFactory::get("tanh");
			m_activation_function_output = ActivationFunctionFactory::name_of(active_functions[0]) == "linear" ? nullptr : active_functions[0];
		}

		U().resize(features, weights);
		W().resize(weights, weights);
		B().resize(1, weights);
		V().resize(weights, output);
		C().resize(1, output);
	}
	//////////////////////////////////////////////////
	Matrix& RecurrentLayer::U() { return m_U; }
	Matrix& RecurrentLayer::W() { return m_W; }
	Matrix& RecurrentLayer::B() { return m_B; }
	Matrix& RecurrentLayer::V() { return m_V; }
	Matrix& RecurrentLayer::C() { return m_C; }
	//////////////////////////////////////////////////
	const Matrix& RecurrentLayer::U() const { return m_U; }
	const Matrix& RecurrentLayer::W() const { return m_W; }
	const Matrix& RecurrentLayer::B() const { return m_B; }
	const Matrix& RecurrentLayer::V() const { return m_V; }	
	const Matrix& RecurrentLayer::C() const { return m_C; }	
    //////////////////////////////////////////////////
	Layer::VActivationFunction RecurrentLayer::get_activation_functions()
	{
		return { m_activation_function_inside, m_activation_function_output };
	}
	void RecurrentLayer::set_activation_functions(const Layer::VActivationFunction& active_functions)
	{
		if (active_functions.size() > 1)
		{
			m_activation_function_inside = ActivationFunctionFactory::name_of(active_functions[0]) == "linear" ? nullptr : active_functions[0];
			m_activation_function_output = ActivationFunctionFactory::name_of(active_functions[1]) == "linear" ? nullptr : active_functions[1];
		}
		else
		{
			m_activation_function_output = ActivationFunctionFactory::name_of(active_functions[0]) == "linear" ? nullptr : active_functions[0];
		}
	}
	//////////////////////////////////////////////////
	Layer::SPtr RecurrentLayer::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<RecurrentLayer>(*this));
	}
	//////////////////////////////////////////////////    
    Layer::VMatrix  RecurrentLayer::apply(const VMatrix& inputs) const
    {
		//alias		
		const Eigen::Map<ColVector>& b_bais_c = Eigen::Map<ColVector>((Scalar*)B().data(), B().cols()*B().rows());
		const Eigen::Map<RowVector>& b_bais_r = Eigen::Map<RowVector>((Scalar*)B().data(), B().cols()*B().rows());
		const Eigen::Map<ColVector>& c_bais_c = Eigen::Map<ColVector>((Scalar*)C().data(), C().cols()*C().rows());
		const Eigen::Map<RowVector>& c_bais_r = Eigen::Map<RowVector>((Scalar*)C().data(), C().cols()*C().rows());
		//outputs
        Layer::VMatrix o;
        //h0
        Matrix h = Matrix::Zero(inputs[0].rows(), W().cols());
        //
        for(size_t t = 0; t!=inputs.size(); ++t)
        {
            //get state
			auto hW = h * W();
			h = (inputs[t] * U() + hW).rowwise() + b_bais_r;
            if (m_activation_function_inside) h = m_activation_function_inside(h);
            //output
			Matrix out = (h * V()).rowwise() + c_bais_r;
			if (m_activation_function_output) out = m_activation_function_output(out);
            o.push_back(out);
        }
        return o;
    }
    //////////////////////////////////////////////////
    size_t RecurrentLayer::size() const
	{
		return 5;
	}
	size_t RecurrentLayer::ouput_paramters() const
	{
		return V().cols();
	}
	size_t RecurrentLayer::input_shape_dims() const
	{
		return 1;
	}
	Layer::Shape RecurrentLayer::output_shape() const
	{
		return Layer::Shape{ long(V().cols()) };
	}
	Matrix& RecurrentLayer::operator[](size_t i)
	{
		denn_assert(i < 5);
        switch(i)
        {
            default:
            case 0: return U();
            case 1: return W();
            case 2: return B();
            case 3: return V();
			case 4: return C();
        }
	}
	const Matrix& RecurrentLayer::operator[](size_t i) const
	{
		denn_assert(i < 5);
        switch(i)
        {
            default:
            case 0: return U();
            case 1: return W();
            case 2: return B();
            case 3: return V();
            case 4: return C();
        }
	}    
    /////////////////////////////////////////////////////////////////////////////////////////////
}