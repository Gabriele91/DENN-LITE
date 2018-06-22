#include "DennCostFunction.h"
#include "DennActivationFunction.h"
#include "DennRecurrentLayer.h"

namespace Denn
{
	///////////////////////////////////////	
	RecurrentLayer::RecurrentLayer
	(
		  int features
		, int clazz
	)
	{		
        U().resize(features, clazz);
		W().resize(clazz, clazz);
		V().resize(clazz, features);
	}

	RecurrentLayer::RecurrentLayer
	(
		  ActivationFunction active_function
		, size_t features
		, size_t clazz
	)
	{
		set_activation_function(active_function);
        U().resize(features, clazz);
		W().resize(clazz, clazz);
		V().resize(clazz, features);
	}
	//////////////////////////////////////////////////
	Matrix& RecurrentLayer::U() { return m_U; }
	Matrix& RecurrentLayer::W() { return m_W; }
	Matrix& RecurrentLayer::V() { return m_V; }
	//////////////////////////////////////////////////
	const Matrix& RecurrentLayer::U() const { return m_U; }
	const Matrix& RecurrentLayer::W() const { return m_W; }
	const Matrix& RecurrentLayer::V() const { return m_V; }	
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
    Layer::VMatrix  RecurrentLayer::apply(const VMatrix& inputs) const
    {
        //outputs
        Layer::VMatrix o;
        //h0
        Matrix h = Matrix::Zero(inputs.size(), inputs[0].rows()); 
        //
        for(size_t t = 0; t!=inputs.size(); ++t)
        {
            //get state
            h = (U() * inputs[t]) +  (W() * h);
            if (m_activation_function) h = m_activation_function(h);
            //output
            Matrix out = V()*h;
            o.push_back(CostFunction::softmax_row_samples(out));
        }
        return o;
    }
    //////////////////////////////////////////////////
    size_t RecurrentLayer::size() const
	{
		return 3;
	}
	Matrix& RecurrentLayer::operator[](size_t i)
	{
		denn_assert(i < 3);
        switch(i)
        {
            default:
            case 1: return U();
            case 2: return W();
            case 3: return V();
        }
	}
	const Matrix& RecurrentLayer::operator[](size_t i) const
	{
		denn_assert(i < 3);
        switch(i)
        {
            default:
            case 1: return U();
            case 2: return W();
            case 3: return V();
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