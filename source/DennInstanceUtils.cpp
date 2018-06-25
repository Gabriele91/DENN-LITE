#include <fstream>
#include "DennInstanceUtils.h"
#include "DennLayer.h"

namespace Denn
{
	//build a mlp network from parameters
	NeuralNetwork build_mlp_network
	(
		  size_t n_features
		, size_t n_class
		, const Denn::Parameters& parameters
	)
	{
		//mlp network
		NeuralNetwork mlp_nn;
		//hidden layer list
		const auto& hidden_layers = (*parameters.m_hidden_layers);
		const auto& active_layers = (*parameters.m_activation_functions);
		const auto& active_output = (*parameters.m_output_activation_function);
		//return NeuralNetwork
		return build_mlp_network(n_features,n_class, hidden_layers, active_layers, active_output);
	}

	//build a mlp network from parameters
	NeuralNetwork build_mlp_network
	(
		  size_t n_features
		, size_t n_class
		, const std::vector<unsigned int>& hidden_layers
		, const std::vector<std::string>& active_layers
		, const std::string& active_output
	)
	{
		//mlp network
		NeuralNetwork mlp_nn;
		//push all hidden layers
		if (hidden_layers.size())
		{
			//add first layer
			mlp_nn.add_layer(PerceptronLayer(
				active_layers.size() ? ActivationFunctionFactory::get(active_layers[0]) : nullptr
				, n_features
				, hidden_layers[0]
			));
			//add next layers
			for (size_t i = 0; i != hidden_layers.size() - 1; ++i)
			{
				mlp_nn.add_layer(PerceptronLayer(
					(i + 1 < active_layers.size()) ? ActivationFunctionFactory::get(active_layers[i + 1]) : nullptr
					, hidden_layers[i]
					, hidden_layers[i + 1]
				));
			}
			//add last layer
			mlp_nn.add_layer(PerceptronLayer(
				ActivationFunctionFactory::get(active_output)
				, hidden_layers[hidden_layers.size() - 1]
				, n_class
			));
		}
		//else add only input layer
		else
		{
			mlp_nn.add_layer(PerceptronLayer(ActivationFunctionFactory::get(active_output), n_features, n_class));
		}
		//return NeuralNetwork
		return mlp_nn;
	}
	//build a network from parameters
	NeuralNetwork build_network
	(
		  size_t n_features
		, size_t n_class
		, const Denn::Parameters& parameters
	)
	{
		//mlp network
		NeuralNetwork mlp_nn;
		//hidden layer list
		const auto& hidden_layers = (*parameters.m_hidden_layers);
		const auto& active_layers = (*parameters.m_activation_functions);
		const auto& hidden_types  = (*parameters.m_hidden_layers_types);
		const auto& active_output = (*parameters.m_output_activation_function);
		//return NeuralNetwork
		return build_network(n_features,n_class, hidden_layers, active_layers, hidden_types, active_output);
	}
	//build a mlp network from parameters
	NeuralNetwork build_network
	(
		  size_t n_features
		, size_t n_class
		, const std::vector<unsigned int>& hidden_layers
		, const std::vector<std::string>& active_layers
		, const std::vector<std::string>& types_layers
		, const std::string& active_output
	)
	{ 
		//input of next layer
		size_t input_size = n_features;
		//layer gen
		auto new_layer = [&](size_t i) -> Layer::SPtr 
		{ 
			//get activation function
			auto function = (i < active_layers.size()) ? ActivationFunctionFactory::get(active_layers[i]) : nullptr;
			//build layer
			if(types_layers.size() <= i || types_layers[i] == "perceptron")
			{
				return  LayerFactory::create(
					  types_layers[i]
					, function
					, { input_size , hidden_layers[i] }
				);
				input_size = hidden_layers[i];
			}
			else if (types_layers[i] == "recurrent")
			{
				return  LayerFactory::create(
					  types_layers[i]
					, function
					, { input_size , hidden_layers[i] }
				);
				//next same size
			}
			return nullptr; 
		};	
		//network
		NeuralNetwork nn;
		//push all hidden layers
		if (hidden_layers.size())
		{
			//add hiddens
			for (size_t i = 0; i != hidden_layers.size() - 1; ++i)
			{
				nn.add(new_layer(i));
			}
		}
		//add last layer
		nn.add(Layer::SPtr((Layer*)new PerceptronLayer(
			  ActivationFunctionFactory::get(active_output)
			, input_size
			, n_class
		)));
		//return NeuralNetwork
		return nn;
	}
	//build output stream
	bool build_outputstream(std::ostream& runtime_output_stream, std::ofstream& runtime_output_file_stream, const Denn::Parameters& arguments)
	{
		//get argument
		std::string    runtime_output_arg = (*arguments.m_runtime_output_file);
		//output
		if (runtime_output_arg.size())
		{
			if (runtime_output_arg == "::cout")
			{
				runtime_output_stream.rdbuf(std::cout.rdbuf());
			}
			else if (runtime_output_arg == "::cerr")
			{
				runtime_output_stream.rdbuf(std::cerr.rdbuf());
			}
			else if (!Denn::Filesystem::exists((const std::string&)arguments.m_runtime_output_file) ||
				Denn::Filesystem::is_writable((const std::string&)arguments.m_runtime_output_file))
			{
				runtime_output_file_stream.open(*arguments.m_runtime_output_file);
				runtime_output_stream.rdbuf(runtime_output_file_stream.rdbuf());
			}
			else
			{
				std::cerr << "can't write into the file: \"" << *arguments.m_runtime_output_file << "\"" << std::endl;
				return false;
			}
		}
		else
		{
			runtime_output_stream.rdbuf(std::cout.rdbuf());
		}
		return true;
	}

	//build output stream
	bool build_serialize(SerializeOutput::SPtr& serialize, std::ofstream& ofile, const Denn::Parameters& arguments)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////
		if (Denn::Filesystem::exists((const std::string&)arguments.m_output_filename) &&
			!Denn::Filesystem::is_writable((const std::string&)arguments.m_output_filename))
		{
			std::cerr << "can't write into the file: \"" << *arguments.m_output_filename << "\"" << std::endl;
			return false;
		}
		//open output file
		ofile.open((const std::string&)arguments.m_output_filename);
		//extension
		std::string ext = Denn::Filesystem::get_extension(*arguments.m_output_filename);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (!SerializeOutputFactory::exists(ext))
		{
			std::cerr << "can't serialize a file with extension \"" << ext << "\"" << std::endl;
			return false;
		}
		////////////////////////////////////////////////////////////////////////////////////////////////
		serialize = SerializeOutputFactory::create(ext, ofile, arguments);
		////////////////////////////////////////////////////////////////////////////////////////////////
		return serialize != nullptr;
	}

	//build thread pool
	bool build_thread_pool(std::unique_ptr<ThreadPool>& thpool, const Denn::Parameters& parameters)
	{
		//parallel (OpenMP)
		#ifdef EIGEN_HAS_OPENMP
		if (*parameters.m_threads_omp)
		{
		omp_set_num_threads((int)*parameters.m_threads_omp);
		Eigen::setNbThreads((int)*parameters.m_threads_omp);
		Eigen::initParallel();
		}
		#endif
		//parallel (Thread Pool)
		//ptr
		std::unique_ptr<ThreadPool> uptr_thpool;
		//alloc new ThreadPool
		if (*parameters.m_threads_pop)
		{
			thpool = std::make_unique<ThreadPool>(*parameters.m_threads_pop);
		}
		return true;
	}
}
