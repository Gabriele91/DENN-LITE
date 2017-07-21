#pragma once
#include "Config.h"
#include "DennLayer.h"

namespace Denn
{
class NeuralNetwork
{
public:
	////////////////////////////////////////////////////////////////
	using LayerList			 = std::vector < Layer::SPtr >;
	using LayerIterator		 = typename LayerList::iterator;
	using LayerConstIterator = typename LayerList::const_iterator;
	////////////////////////////////////////////////////////////////
	//  default constructor 
	NeuralNetwork();
	//  default copy constructor  and assignment operator
	NeuralNetwork(const NeuralNetwork& nn);
	NeuralNetwork& operator= (const NeuralNetwork & nn);
	////////////////////////////////////////////////////////////////
	// add layers
	template < class ...Layers >
	NeuralNetwork(Layers ...layers)
	{
		add_layer(layers...);
	}
	template < typename DerivateLayer >
	void add_layer(const DerivateLayer& layer)
	{
		m_layers.push_back(std::static_pointer_cast<Layer>(std::make_shared<DerivateLayer>(layer)));
	}
	template < typename ...Layers >
	void add_layer(const Layer& layer, Layers ...layers)
	{
		add_layer(layer);
		add_layer(layers...);
	}
	/////////////////////////////////////////////////////////////////////////
	Matrix apply(const Matrix& input);
	/////////////////////////////////////////////////////////////////////////
	size_t size() const;

	Layer& operator [] (size_t i);

	const Layer& operator [] (size_t i) const;

	LayerIterator begin();

	LayerIterator end();

	LayerConstIterator begin() const;

	LayerConstIterator end() const;
	/////////////////////////////////////////////////////////////////////////
protected:

	LayerList m_layers;

};

}