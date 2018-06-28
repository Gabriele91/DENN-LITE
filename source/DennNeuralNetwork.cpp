#include "DennNeuralNetwork.h"
#include "DennCostFunction.h"
namespace Denn
{	
	////////////////////////////////////////////////////////////////
	//  default constructor 
	NeuralNetwork::NeuralNetwork()
	{
	}
	//  default copy constructor  and assignment operator
	NeuralNetwork::NeuralNetwork(const NeuralNetwork& nn)
	{
		//alloc
		m_layers.clear();
		//copy all layers
		for (size_t i = 0; i != nn.size(); ++i)
		{
			m_layers.push_back(nn[i].copy()->get_ptr());
		}
	}
	NeuralNetwork& NeuralNetwork::operator= (const NeuralNetwork & nn)
	{
		//alloc
		m_layers.clear();
		//copy all layers
		for (size_t i = 0; i != nn.size(); ++i)
		{
			m_layers.push_back(nn[i].copy()->get_ptr());
		}
		//self return
		return *this;
	}
	/////////////////////////////////////////////////////////////////////////
	Matrix NeuralNetwork::apply(const std::vector< Matrix >& input) const
	{
		//no layer?
		denn_assert(m_layers.size());
		//input layer
		auto output = m_layers[0]->apply(input);
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			output = m_layers[i]->apply(output);
		}
		//return
		return output.back();
	}	
	Matrix NeuralNetwork::apply(const Matrix& input) const
	{
		//no layer?
		denn_assert(m_layers.size());
		//input layer
		Matrix output = m_layers[0]->apply(input);
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			output = m_layers[i]->apply(output);
		}
		//return
		return output;
	}	
	/////////////////////////////////////////////////////////////////////////
	NeuralNetwork::BackpropagationDelta operator + (  const NeuralNetwork::BackpropagationDelta& left
													, const NeuralNetwork::BackpropagationDelta& right)
	{
        denn_assert(left.size()==right.size());
        NeuralNetwork::BackpropagationDelta output(left.size());
        for(size_t i = 0; i != left.size(); ++i) output[i] = left[i] + right[i];
        return output;		
	}

	NeuralNetwork::BackpropagationGradient operator + (  const NeuralNetwork::BackpropagationGradient& left
													   , const NeuralNetwork::BackpropagationGradient& right)
	{
        denn_assert(left.size()==right.size());
		//alloc
        NeuralNetwork::BackpropagationGradient output(left.size());
		//compute
        for(size_t i = 0; i != left.size(); ++i)
		{
			denn_assert(left[i].size()==right[i].size());
			//alloc
			output[i].resize(left[i].size());
			//compute
			for(size_t j = 0; j != left[i].size(); ++j)
			{
				output[i][j] = left[i][j] + right[i][j];
			} 
		}
        return output;		
	}

	NeuralNetwork::BackpropagationContext operator + ( const NeuralNetwork::BackpropagationContext& left
											       	 , const NeuralNetwork::BackpropagationContext& right)
	{
		NeuralNetwork::BackpropagationDelta    left_delta;
		NeuralNetwork::BackpropagationGradient left_gradient;
		NeuralNetwork::BackpropagationDelta    right_delta;
		NeuralNetwork::BackpropagationGradient right_gradient;
		std::tie(left_delta, left_gradient) = left;
		std::tie(right_delta, right_gradient) = left;
		return std::make_tuple( left_delta+right_delta, left_gradient+right_gradient );
		
	}

	NeuralNetwork::BackpropagationContext NeuralNetwork::compute_gradient(  const Matrix& input 
																		  , const Matrix& labels
																		  , Scalar regular_param) const
	{
		//////////////////////////////////////////////////////////////////////
		BackpropagationDelta    deltas(m_layers.size());
		BackpropagationGradient gradients(m_layers.size());
		//////////////////////////////////////////////////////////////////////
		//FEED-FORWAORD PROPAGATION
		//output of layers
		std::vector< Matrix > z(m_layers.size());
		std::vector< Matrix > s(m_layers.size());
		//output
		z[0] = m_layers[0]->feedforward(input, s[0]);
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			z[i] = m_layers[i]->feedforward(z[i-1], s[i]);
		}
		//////////////////////////////////////////////////////////////////////
		//BACK-PROPAGATION
		Matrix        output_nn = CostFunction::softmax_row_samples(z.back());
		Matrix::Index nsamples  = input.rows();
		//compute delta	
		for (long i = 0, j = m_layers.size()-1; i < m_layers.size(); ++i, --j)
		{
			if(!i)
			{
				deltas[j] = m_layers[j]->backpropagate_derive( ( output_nn-labels ).transpose(), s[j] ) / nsamples;
			}
			else
			{
				deltas[j] = m_layers[j]->backpropagate_derive(m_layers[j+1]->backpropagate_delta( deltas[j+1] ), s[j]);
			}
		}
		//compute gradient
		for (long i = 0, j = -1; i < m_layers.size(); ++i, ++j)
		{	
			if(!i)
			{
				gradients[i] = m_layers[i]->backpropagate_gradient(deltas[i], input);
			}
			else
			{
				gradients[i] = m_layers[i]->backpropagate_gradient(deltas[i], z[j]);
			}
		}
		//////////////////////////////////////////////////////////////////////
		//get values
		return std::make_tuple(deltas, gradients);
	}

	void NeuralNetwork::gradient_descent(BackpropagationContext&& bpcontext, Scalar learn_rate)
	{
		//////////////////////////////////////////////////////////////////////
		BackpropagationDelta    deltas;
		BackpropagationGradient gradients;
		//////////////////////////////////////////////////////////////////////
		std::tie(deltas,gradients) = bpcontext;
		//////////////////////////////////////////////////////////////////////
		//Mu
		Scalar mu = Scalar(-1.0) * learn_rate;
		//////////////////////////////////////////////////////////////////////
		//GD
		for (size_t i = 0; i < m_layers.size(); ++i)
		for (size_t m = 0; m < m_layers[i]->size(); ++m)
		{
			(*m_layers[i])[m] += mu * gradients[i][m];
		}
	}

	void NeuralNetwork::backpropagation_gradient_descent
    (
		  const Matrix& input
        , const Matrix& labels
		, const Scalar learn_rate  
	    , const Scalar regular_param
	)
	{
		gradient_descent(compute_gradient(input, labels, regular_param), learn_rate);
	}	
	/////////////////////////////////////////////////////////////////////////
	size_t NeuralNetwork::size() const
	{
		return m_layers.size();
	}

	Layer& NeuralNetwork::operator [] (size_t i)
	{
		return *(m_layers[i].get());
	}

	const Layer& NeuralNetwork::operator [] (size_t i) const
	{
		return *(m_layers[i].get());
	}

	NeuralNetwork::LayerIterator NeuralNetwork::begin()
	{
		return m_layers.begin();
	}

	NeuralNetwork::LayerIterator NeuralNetwork::end()
	{
		return m_layers.end();
	}

	NeuralNetwork::LayerConstIterator NeuralNetwork::begin() const
	{
		return m_layers.begin();
	}

	NeuralNetwork::LayerConstIterator NeuralNetwork::end() const
	{
		return m_layers.end();
	}
}
