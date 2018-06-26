#include "DennCostFunction.h"
#include "DennActivationFunction.h"
#include "DennRecurrentLayer.h"
//http://\
  www.wildml.com/2015/09/\
  recurrent-neural-networks-tutorial-part-2-implementing-a-language-model-rnn-with-python-numpy-and-theano/

namespace Denn
{
	///////////////////////////////////////	
	RecurrentLayer::RecurrentLayer
	(
		  int features
		, int iweigth
	)
	{		
        U().resize(features, iweigth);
		W().resize(iweigth, iweigth);
		B().resize(1, iweigth);
		V().resize(iweigth, features);
		C().resize(1, features);
	}

	RecurrentLayer::RecurrentLayer
	(
		  ActivationFunction active_function
		, size_t features
		, size_t iweigth
	)
	{
		set_activation_function(active_function);
        U().resize(features, iweigth);
		W().resize(iweigth, iweigth);
		B().resize(1, iweigth);
		V().resize(iweigth, features);
		C().resize(1, features);
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
	ActivationFunction RecurrentLayer::get_activation_function() 
	{
		return m_activation_function;
	}
	void RecurrentLayer::set_activation_function(ActivationFunction active_function)
	{
		m_activation_function = ActivationFunctionFactory::name_of(active_function) == "linear" ? nullptr : active_function;
	}
	//////////////////////////////////////////////////
	Layer::SPtr RecurrentLayer::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<RecurrentLayer>(*this));
	}
	//////////////////////////////////////////////////    
	Matrix  RecurrentLayer::apply(const Matrix& input) const
	{
		return apply(VMatrix{input}).back();
	}
    Layer::VMatrix  RecurrentLayer::apply(const VMatrix& inputs) const
    {
		//alias
		const Eigen::Map<RowVector>& b_bais = Eigen::Map<RowVector>((Scalar*)B().data(), B().cols()*B().rows());
		const Eigen::Map<RowVector>& c_bais = Eigen::Map<RowVector>((Scalar*)C().data(), C().cols()*C().rows());
        //outputs
        Layer::VMatrix o;
        //h0
        Matrix h = Matrix::Zero(inputs.size(), inputs[0].rows()); 
        //
        for(size_t t = 0; t!=inputs.size(); ++t)
        {
            //get state
            h = ((U() * inputs[t]) +  (W() * h)).rowwise() + b_bais;
            if (m_activation_function) h = m_activation_function(h);
            //output
            Matrix out = (V()*h).rowwise() + c_bais;
            o.push_back(CostFunction::softmax_row_samples(out));
        }
        return o;
    }
    //////////////////////////////////////////////////
    size_t RecurrentLayer::size() const
	{
		return 5;
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
    /// TODO
    /////////////////////////////////////////////////////////////////////////////////////////////
	Matrix RecurrentLayer::feedforward(const Matrix& input, Matrix& l_out)
	{
        denn_assert(false);
		//return
		return {};
	}
	Matrix RecurrentLayer::backpropagate_delta(const Matrix& loss)
    {
        denn_assert(false);
		//return
		return {};
    }
    Matrix RecurrentLayer::backpropagate_derive(const Matrix& delta, const Matrix& l_out)
    {
        denn_assert(false);
		//return
		return {};
    }
    Layer::VMatrix RecurrentLayer::backpropagate_gradient(const Matrix& delta, const Matrix& l_in, Scalar regular)
    {
        denn_assert(false);
		//return
		return {};
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /// TODO
    /////////////////////////////////////////////////////////////////////////////////////////////
    Layer::VMatrix RecurrentLayer::feedforward(const VMatrix& inputs, VMatrix& linear_outs)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VMatrix RecurrentLayer::backpropagate_delta(const VMatrix& vloss)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VMatrix RecurrentLayer::backpropagate_derive(const VMatrix& deltas, const VMatrix& linear_outs)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VVMatrix RecurrentLayer::backpropagate_gradient(const VMatrix& deltas, const VMatrix& linear_inputs, Scalar regular)
	{
        denn_assert(false);
		//return
		return {};
	}
    /////////////////////////////////////////////////////////////////////////////////////////////
}