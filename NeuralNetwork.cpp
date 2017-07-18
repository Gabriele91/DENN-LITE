#include "NeuralNetwork.h"

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
	Matrix NeuralNetwork::apply(const Matrix& input)
	{
		//no layer?
		assert(m_layers.size());
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