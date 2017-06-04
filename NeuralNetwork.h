#pragma once
#include "Config.h"
#include "PerceptronLayer.h"

namespace Denn
{
template < typename Layer >
class NeuralNetwork
{
public:
	///////////////////////////////////////
	using LayerType		     = typename Layer;
	using MatrixType		 = typename Layer::MatrixType;
	using ScalarType		 = typename Layer::ScalarType;
	using LayerList			 = std::vector < Layer >;
	using LayerIterator		 = typename LayerList::iterator;
	using LayerConstIterator = typename LayerList::const_iterator;
	///////////////////////////////////////

	NeuralNetwork()
	{
	}

	template < class ...Layers >
	NeuralNetwork(Layers ...layers)
	{
		add_layer(layers...);
	}

	/////////////////////////////////////////////////////////////////////////
	void add_layer(const Layer& layer)
	{
		m_layers.push_back(layer);
	}

	template < typename ...Layers >
	void add_layer(const Layer& layer, Layers ...layers)
	{
		add_layer(layer);
		add_layer(layers...);
	}
	/////////////////////////////////////////////////////////////////////////

	MatrixType apply(const MatrixType& input)
	{
		//no layer?
		assert(m_layers.size());
		//input layer
		MatrixType output = m_layers[0].typename apply(input);
		//hidden layers
		for (size_t i = 1; i < m_layers.size(); ++i)
		{
			output = m_layers[i].typename apply(output);
		}
		//return
		return output;
	}

	/////////////////////////////////////////////////////////////////////////
	size_t size() const
	{
		return m_layers.size();
	}

	Layer& operator [] (size_t i)
	{
		return m_layers[i];
	}

	const Layer& operator [] (size_t i) const
	{
		return m_layers[i];
	}

	LayerIterator begin()
	{
		return m_layers.typename begin();
	}	

	LayerIterator end()
	{
		return m_layers.typename end();
	}

	LayerConstIterator begin() const
	{
		return m_layers.typename begin();
	}

	LayerConstIterator end() const
	{
		return m_layers.typename end();
	}
	/////////////////////////////////////////////////////////////////////////

protected:

	LayerList m_layers;

};

//////////////////////////////////////////////////////ALIAS
using PerceptronNetworkD = NeuralNetwork< typename PerceptronLayerD >;
using PerceptronNetworkF = NeuralNetwork< typename PerceptronLayerF >;

}