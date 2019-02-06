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
	const Matrix& NeuralNetwork::feedforward(const Matrix& input) const
	{
		//no layer?
		denn_assert(m_layers.size());
		//input layer
		const Matrix* output = &m_layers[0]->feedforward(input);
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			output = &m_layers[i]->feedforward(*output);
		}
		//return
		return *output;
	}	
	void NeuralNetwork::backpropagate(const Matrix& input, OutputLoss oltype)
	{
		//ptrs
		Layer::SPtr first_layer = m_layers[0];
		Layer::SPtr last_layer = m_layers[size() - 1];

		// Let output layer compute back-propagation data
		// MSE
		Matrix eval;
		switch (oltype)
		{
		default:
		case Denn::NeuralNetwork::MSE:
			eval.noalias() = last_layer->ff_output() - input;
		break;
		case Denn::NeuralNetwork::MULTICLASS_CROSS_ENTROPY:
		{
			// Compute the derivative of the input of this layer
			// L = -sum(log(phat) * y)
			// in = phat
			// d(L) / d(in)= -y / phat
			const int nobs = last_layer->ff_output().cols();
			const int nclass = last_layer->ff_output().rows();
			eval.resize(nclass, nobs);
			eval.noalias() = -input.cwiseQuotient(last_layer->ff_output());
		}
		break;
		case Denn::NeuralNetwork::BINARY_CROSS_ENTROPY:
		{
			// Compute the derivative of the input of this layer
			// L = -y * log(phat) - (1 - y) * log(1 - phat)
			// in = phat
			// d(L) / d(in)= -y / phat + (1 - y) / (1 - phat), y is either 0 or 1
#if 0
			const int nobs = last_layer->ff_output().cols();
			const int nclass = last_layer->ff_output().rows();
			eval.resize(nclass, nobs);
			eval.noalias() = (input.array() < Scalar(0.5)).select(
				(Scalar(1) - last_layer->ff_output().array()).cwiseInverse(),
				-last_layer->ff_output().cwiseInverse()
			);
#endif
		}
		break;
		}

		// If there is only one hidden layer, "prev_layer_data" will be the input data
		if (size() == 1)
		{
			first_layer->backpropagate(input, eval);
			return;
		}

		// Compute gradients for the last hidden layer
		last_layer->backpropagate(m_layers[size() - 2]->ff_output(), eval);

		// Compute gradients for all the hidden layers except for the first one and the last one
		for (int i = size() - 2; i > 0; i--)
		{
			m_layers[i]->backpropagate(m_layers[i - 1]->ff_output(), m_layers[i + 1]->bp_output());
		}

		// Compute gradients for the first layer
		first_layer->backpropagate(input, m_layers[1]->bp_output());
	}
	void NeuralNetwork::fit(const Matrix& input, const Matrix& output, 
							const Optimizer& opt, OutputLoss type)
	{
		//->
		feedforward(input);
		//<-
		backpropagate(output, type);
		//update
		for (auto layer : *this) 
			layer->update(opt);
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
