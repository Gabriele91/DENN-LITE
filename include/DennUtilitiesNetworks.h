#pragma once
#include "Denn.h"

namespace Denn
{
	//build a network from parameters
	NeuralNetwork build_network(size_t n_features, size_t n_class, const Denn::Parameters& parameters);
	//build a network from parameters
	NeuralNetwork build_network
	(
		  size_t n_features
		, size_t n_class
		, Layer::Shape shape
		, Layer::Input layers
		, const std::vector<std::string>& activation_functions
		, const std::vector<std::string>& layers_types
	);
	//build output stream
	bool build_outputstream(std::ostream& output, std::ofstream& file, const Denn::Parameters& parameters);
	//build output stream
	bool build_serialize(SerializeOutput::SPtr& serialize, std::ofstream& file, const Denn::Parameters& parameters);
	//build thread pool
	bool build_thread_pool(std::unique_ptr<ThreadPool>& tpool, const Denn::Parameters& parameters);
	//parser arg input
	bool get_network_from_string
	(
		  const std::string&		network
		, std::vector<long>&        shape
		, std::vector<long>&        layers
		, std::vector<std::string>& activation_functions
		, std::vector<std::string>& layers_types
	);
	//get arg from nn
	bool get_string_from_args
	(
		        std::string&			  network
		, const std::vector<long>&        layers
		, const std::vector<std::string>& activation_functions
		, const std::vector<std::string>& layers_types
	);
}