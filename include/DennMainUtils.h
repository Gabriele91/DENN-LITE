#pragma once
#include "Denn.h"

namespace Denn
{
	//build a mlp network from parameters
	NeuralNetwork build_mlp_network(size_t n_features, size_t n_class, const Denn::Parameters& parameters);
	//apply denn 
	bool denn_algorithm
	(
		  const Denn::Parameters&     parameters
		, Denn::DataSetLoader&        dataset
		, Denn::ThreadPool*           ptr_thpool
		, std::ostream&               output
		, Denn::SerializeOutput::SPtr serialize_output
		, NeuralNetwork&			  neural_netowrk
	);
}