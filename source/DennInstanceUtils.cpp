#include <fstream>
#include "DennInstanceUtils.h"

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
		const auto& active_layers = (*parameters.m_active_functions);
		const auto& active_output = (*parameters.m_output_active_function);
		//push all hidden layers
		if (hidden_layers.size())
		{
			//add first layer
			mlp_nn.add_layer(PerceptronLayer(
				active_layers.size() ? ActiveFunctionFactory::get(active_layers[0]) : nullptr
				, n_features
				, hidden_layers[0]
			));
			//add next layers
			for (size_t i = 0; i != hidden_layers.size() - 1; ++i)
			{
				mlp_nn.add_layer(PerceptronLayer(
					(i + 1 < active_layers.size()) ? ActiveFunctionFactory::get(active_layers[i + 1]) : nullptr
					, hidden_layers[i]
					, hidden_layers[i + 1]
				));
			}
			//add last layer
			mlp_nn.add_layer(PerceptronLayer(
				ActiveFunctionFactory::get(active_output)
				, hidden_layers[hidden_layers.size() - 1]
				, n_class
			));
		}
		//else add only input layer
		else
		{
			mlp_nn.add_layer(PerceptronLayer(ActiveFunctionFactory::get(active_output), n_features, n_class));
		}
		//return NeuralNetwork
		return mlp_nn;
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
	bool build_serialize(SerializeOutput::SPtr& serialize, std::ofstream& file, const Denn::Parameters& arguments)
	{
		////////////////////////////////////////////////////////////////////////////////////////////////
		if (Denn::Filesystem::exists((const std::string&)arguments.m_output_filename) &&
			!Denn::Filesystem::is_writable((const std::string&)arguments.m_output_filename))
		{
			std::cerr << "can't write into the file: \"" << *arguments.m_output_filename << "\"" << std::endl;
			return false;
		}
		std::ofstream	ofile((const std::string&)arguments.m_output_filename);
		//extension
		std::string ext = Denn::Filesystem::get_extension(*arguments.m_output_filename);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (!SerializeOutputFactory::exists(ext))
		{
			std::cerr << "can't serialize a file with extension \"" << ext << "\"" << std::endl;
			return false;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		SerializeOutput::SPtr serialize_output = SerializeOutputFactory::create(ext, ofile, arguments);
		return true;
	}

	//build thread pool
	bool build_thread_pool(std::unique_ptr<ThreadPool>& thpool, const Denn::Parameters& parameters)
	{
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
