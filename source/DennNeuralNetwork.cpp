#include <iostream> 
#include "DennNeuralNetwork.h"
#include "DennCostFunction.h"
#include "DennDump.h"
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
			m_layers.push_back(nn[i].copy());
		}
	}
	NeuralNetwork& NeuralNetwork::operator= (const NeuralNetwork & nn)
	{
		//alloc
		m_layers.clear();
		//copy all layers
		for (size_t i = 0; i != nn.size(); ++i)
		{
			m_layers.push_back(nn[i].copy());
		}
		//self return
		return *this;
	}
	/////////////////////////////////////////////////////////////////////////
	Matrix NeuralNetwork::apply(const VMatrix& input) const
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
		VMatrix output = m_layers[0]->apply({ input });
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			output = m_layers[i]->apply(output);
		}
		//return
		return output.back();
	}
	void NeuralNetwork::backpropagation
	(
		  const VMatrix& input
		, const VMatrix& labels
		, BPOptimizer& optimizer
	)
	{
		//no layer?
		denn_assert(m_layers.size());
		//set optimizer
		for (size_t i = 0; i < m_layers.size(); ++i) m_layers[i]->optimizer(optimizer);
		//feedforward pass
		VMatrix ffout = m_layers[0]->feedforward(input);
		for (size_t i = 1; i < m_layers.size(); ++i) ffout = m_layers[i]->apply(ffout);
		//error
		for (size_t i = 0; i < labels.size(); ++i) ffout[i] = labels[i] - ffout[i];
		//backpropagation pass
		VMatrix bpout = m_layers.back()->backpropagate(ffout);
		for (size_t i = 1; i < m_layers.size(); ++i) bpout = m_layers[m_layers.size()-i-1]->apply(bpout);
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

	NeuralNetwork::LayerIterator NeuralNetwork::back()
	{
		return --m_layers.end();
	}

	NeuralNetwork::LayerConstIterator NeuralNetwork::begin() const
	{
		return m_layers.begin();
	}

	NeuralNetwork::LayerConstIterator NeuralNetwork::end() const
	{
		return m_layers.end();
	}

	NeuralNetwork::LayerConstIterator NeuralNetwork::back() const
	{
		return --m_layers.end();
	}
}
