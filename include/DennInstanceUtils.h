#pragma once
#include "Denn.h"

namespace Denn
{
	//build a mlp network from parameters
	NeuralNetwork build_mlp_network(size_t n_features, size_t n_class, const Denn::Parameters& parameters);
	//build a mlp network from parameters
	NeuralNetwork build_mlp_network
	(
		  size_t n_features
		, size_t n_class
		, const std::vector<unsigned int>& hidden_layers
		, const std::vector<std::string>& active_layers
		, const std::string& active_output
	);
	//build output stream
	bool build_outputstream(std::ostream& output, std::ofstream& file, const Denn::Parameters& parameters);
	//build output stream
	bool build_serialize(SerializeOutput::SPtr& serialize, std::ofstream& file, const Denn::Parameters& parameters);
	//build thread pool
	bool build_thread_pool(std::unique_ptr<ThreadPool>& tpool, const Denn::Parameters& parameters);
}