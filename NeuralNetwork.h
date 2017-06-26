#pragma once
#include "Config.h"
#include "Layer.h"

namespace Denn
{
class NeuralNetwork
{
public:
	////////////////////////////////////////////////////////////////
	using LayerList			 = std::vector < std::unique_ptr<Layer> >;
	using LayerIterator		 = typename LayerList::iterator;
	using LayerConstIterator = typename LayerList::const_iterator;
	////////////////////////////////////////////////////////////////

	NeuralNetwork()
	{
	}

	template < class ...Layers >
	NeuralNetwork(Layers ...layers)
	{
		add_layer(layers...);
	}


    //  default copy constructor  and assignment operator
	NeuralNetwork(const NeuralNetwork& nn)
	{
		//alloc
		m_layers.resize(nn.size());
		//copy all layers
		for(size_t i=0; i!=size(); ++i) 
		{
			m_layers[i] = std::move(std::unique_ptr<Layer>(nn[i].copy()));
		}
	}
    NeuralNetwork& operator= (const NeuralNetwork & nn)
	{
		//alloc
		m_layers.resize(nn.size());
		//copy all layers
		for(size_t i=0; i!=size(); ++i) 
		{
			m_layers[i] = std::move(std::unique_ptr<Layer>(nn[i].copy()));
		}
		//self return
		return *this;
	}
	/////////////////////////////////////////////////////////////////////////
	template < typename DerivateLayer >
	void add_layer(const DerivateLayer& layer)
	{
		m_layers.push_back(std::move(
			std::unique_ptr<Layer>(std::make_unique<DerivateLayer>(layer))
		));
	}

	template < typename ...Layers >
	void add_layer(const Layer& layer, Layers ...layers)
	{
		add_layer(layer);
		add_layer(layers...);
	}
	/////////////////////////////////////////////////////////////////////////
	Matrix apply(const Matrix& input)
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
	size_t size() const
	{
		return m_layers.size();
	}

	Layer& operator [] (size_t i)
	{
		return *(m_layers[i].get());
	}

	const Layer& operator [] (size_t i) const
	{
		return *(m_layers[i].get());
	}

	LayerIterator begin()
	{
		return m_layers.begin();
	}	

	LayerIterator end()
	{
		return m_layers.end();
	}

	LayerConstIterator begin() const
	{
		return m_layers.begin();
	}

	LayerConstIterator end() const
	{
		return m_layers.end();
	}
	/////////////////////////////////////////////////////////////////////////

protected:

	LayerList m_layers;

};

}