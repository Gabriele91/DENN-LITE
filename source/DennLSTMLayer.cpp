#include <cmath>
#include "DennCostFunction.h"
#include "DennActivationFunction.h"
#include "DennLSTMLayer.h"
//http://www.wildml.com/2015/09/recurrent-neural-networks-tutorial-part-2-implementing-a-language-model-rnn-with-python-numpy-and-theano/

namespace Denn
{
	///////////////////////////////////////	
	LSTMLayer::LSTMLayer
	(
		  size_t features
		, size_t weights
	)
	{
		m_activation_function[FuncType::G] = ActivationFunctionFactory::get("sigmoid");
		m_activation_function[FuncType::C] = ActivationFunctionFactory::get("tanh");
		m_activation_function[FuncType::H] = ActivationFunctionFactory::get("tanh");
		for(size_t i = 0; i!=GateType::NGATE; ++i)
		{
			m_W[i].resize(features, weights);
			m_U[i].resize(weights,weights);
			m_B[i].resize(1, weights);
		}
	}

	LSTMLayer::LSTMLayer
	(
		  ActivationFunction active_function
		, size_t features
		, size_t weights
	)
	{
		m_activation_function[FuncType::G] = ActivationFunctionFactory::get("sigmoid");
		m_activation_function[FuncType::C] = active_function;
		m_activation_function[FuncType::H] = active_function;
		for(size_t i = 0; i!=GateType::NGATE; ++i)
		{
			m_W[i].resize(features, weights);
			m_U[i].resize(weights,weights);
			m_B[i].resize(1, weights);
		}
	}

	LSTMLayer::LSTMLayer
	(
		  ActivationFunction active_function_g
		, ActivationFunction active_function_c_h
		, size_t features
		, size_t weights
	)
	{
		//g
		m_activation_function[FuncType::G] = active_function_g;
		//h/c
		m_activation_function[FuncType::C] = active_function_c_h;
		m_activation_function[FuncType::H] = active_function_c_h;

		for(size_t i = 0; i!=GateType::NGATE; ++i)
		{
			m_W[i].resize(features, weights);
			m_U[i].resize(weights,weights);
			m_B[i].resize(1, weights);
		}
	}


	LSTMLayer::LSTMLayer
	(
		  ActivationFunction active_function_g
		, ActivationFunction active_function_c
		, ActivationFunction active_function_h
		, size_t features
		, size_t weights
	)
	{
		m_activation_function[FuncType::G] = active_function_g;
		m_activation_function[FuncType::C] = active_function_c;
		m_activation_function[FuncType::H] = active_function_h;
		for(size_t i = 0; i!=GateType::NGATE; ++i)
		{
			m_W[i].resize(features, weights);
			m_U[i].resize(weights,weights);
			m_B[i].resize(1, weights);
		}
	}

	LSTMLayer::LSTMLayer
	(
		  const std::vector< ActivationFunction >& active_functions
		, const std::vector< size_t >&			   input_output
	)
	{
		size_t features = input_output[0];
		size_t weights  = input_output[1];

		if (active_functions.size() >= 3)
		{
			m_activation_function[FuncType::G] = active_functions[0];
			m_activation_function[FuncType::C] = active_functions[1];
			m_activation_function[FuncType::H] = active_functions[2];
		}
		if (active_functions.size() == 2)
		{
			m_activation_function[FuncType::G] = active_functions[0];
			m_activation_function[FuncType::C] = active_functions[1];
			m_activation_function[FuncType::H] = active_functions[1];
		}
		else
		{
			m_activation_function[FuncType::G] = ActivationFunctionFactory::get("sigmoid");
			m_activation_function[FuncType::C] = active_functions[0];
			m_activation_function[FuncType::H] = active_functions[0];
		}
		for(size_t i = 0; i!=GateType::NGATE; ++i)
		{
			m_W[i].resize(features, weights);
			m_U[i].resize(weights,weights);
			m_B[i].resize(1, weights);
		}
	}
    //////////////////////////////////////////////////
	Layer::VActivationFunction LSTMLayer::get_activation_functions()
	{
		return { m_activation_function[FuncType::G], m_activation_function[FuncType::C] };
	}
	void LSTMLayer::set_activation_functions(const Layer::VActivationFunction& active_functions)
	{
		if (active_functions.size() >= 3)
		{
			m_activation_function[FuncType::G] = active_functions[0];
			m_activation_function[FuncType::C] = active_functions[1];
			m_activation_function[FuncType::H] = active_functions[2];
		}
		if (active_functions.size() == 2)
		{
			m_activation_function[FuncType::G] = active_functions[0];
			m_activation_function[FuncType::H] = active_functions[1];
			m_activation_function[FuncType::C] = active_functions[1];
		}
		else
		{
			m_activation_function[FuncType::H] = active_functions[0];
			m_activation_function[FuncType::C] = active_functions[0];
		}
	}
	//////////////////////////////////////////////////
	Layer::SPtr LSTMLayer::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<LSTMLayer>(*this));
	}
	//////////////////////////////////////////////////    
	Matrix  LSTMLayer::apply(const Matrix& input) const
	{
		return apply(VMatrix{input}).back();
	}
    Layer::VMatrix  LSTMLayer::apply(const VMatrix& inputs) const
    {
		//alias		
		const Eigen::Map<RowVector> bias[]
		{
			Eigen::Map<RowVector>((Scalar*)m_B[0].data(), m_B[0].cols()*m_B[0].rows()),
			Eigen::Map<RowVector>((Scalar*)m_B[1].data(), m_B[1].cols()*m_B[1].rows()),
			Eigen::Map<RowVector>((Scalar*)m_B[2].data(), m_B[2].cols()*m_B[2].rows()),
			Eigen::Map<RowVector>((Scalar*)m_B[3].data(), m_B[3].cols()*m_B[3].rows()),
			Eigen::Map<RowVector>((Scalar*)m_B[4].data(), m_B[4].cols()*m_B[4].rows()),
		};
		//outputs
        Layer::VMatrix o;
        //h0
        Matrix h = Matrix::Zero(inputs[0].rows(), m_U[0].cols());
        Matrix c = Matrix::Zero(inputs[0].rows(), m_U[0].cols());
		//alias
		const auto& Fg = m_activation_function[FuncType::G].m_function;
		const auto& Fc = m_activation_function[FuncType::C].m_function;
		const auto& Fh = m_activation_function[FuncType::H].m_function;
        //
        for(size_t t = 0; t!=inputs.size(); ++t)
        {

			auto Fv = Matrix((inputs[t] * m_W[GateType::F] + h * m_U[GateType::F]).rowwise() + bias[GateType::F]);
			auto F = Fg( Fv );
			
			auto Iv = Matrix((inputs[t] * m_W[GateType::I] + h * m_U[GateType::I]).rowwise() + bias[GateType::I]);
			auto I = Fg( Iv );

			auto Ov = Matrix((inputs[t] * m_W[GateType::O] + h * m_U[GateType::O]).rowwise() + bias[GateType::O]);
			auto O = Fg( Ov );
			//update states
			auto Cv = Matrix((inputs[t] * m_W[GateType::C] + h * m_U[GateType::C]).rowwise() + bias[GateType::C]);
			c = F.cwiseProduct(c)  + I.cwiseProduct(Fc( Cv ));
            //output
			h = O.cwiseProduct(Fh(c));
			//push to output
            o.push_back(h);
        }
        return o;
    }
    //////////////////////////////////////////////////
    size_t LSTMLayer::size() const
	{
		return GateType::NGATE * 3;
	}
	size_t LSTMLayer::size_ouput() const
	{
		//output = weights
		return m_U[0].cols();
	}
	Matrix& LSTMLayer::operator[](size_t i)
	{
		denn_assert(i < size());
		size_t id = i / 3;
        size_t type = i % 3;
		switch(type)
		{
			default:
			case 0: return m_W[id];
			case 1: return m_U[id];
			case 2: return m_B[id];
		}
	}
	const Matrix& LSTMLayer::operator[](size_t i) const
	{
		denn_assert(i < size());
		size_t id = i / 3;
        size_t type = i % 3;
		switch(type)
		{
			default:
			case 0: return m_W[id];
			case 1: return m_U[id];
			case 2: return m_B[id];
		}
	}
    /////////////////////////////////////////////////////////////////////////////////////////////
    /// TODO
    /////////////////////////////////////////////////////////////////////////////////////////////
	Matrix LSTMLayer::feedforward(const Matrix& input, Matrix& l_out)
	{
        denn_assert(false);
		//return
		return {};
	}
	Matrix LSTMLayer::backpropagate_delta(const Matrix& loss)
    {
        denn_assert(false);
		//return
		return {};
    }
    Matrix LSTMLayer::backpropagate_derive(const Matrix& delta, const Matrix& l_out)
    {
        denn_assert(false);
		//return
		return {};
    }
    Layer::VMatrix LSTMLayer::backpropagate_gradient(const Matrix& delta, const Matrix& l_in, Scalar regular)
    {
        denn_assert(false);
		//return
		return {};
    }
    /////////////////////////////////////////////////////////////////////////////////////////////
    /// TODO
    /////////////////////////////////////////////////////////////////////////////////////////////
    Layer::VMatrix LSTMLayer::feedforward(const VMatrix& inputs, VMatrix& linear_outs)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VMatrix LSTMLayer::backpropagate_delta(const VMatrix& vloss)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VMatrix LSTMLayer::backpropagate_derive(const VMatrix& deltas, const VMatrix& linear_outs)
	{
        denn_assert(false);
		//return
		return {};
	}
	Layer::VVMatrix LSTMLayer::backpropagate_gradient(const VMatrix& deltas, const VMatrix& linear_inputs, Scalar regular)
	{
        denn_assert(false);
		//return
		return {};
	}
    /////////////////////////////////////////////////////////////////////////////////////////////
}