#include <fstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "DennInstanceUtils.h"
#include "DennLayer.h"

namespace Denn
{
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
		const auto& hidden_layers = (*parameters.m_layers);
		const auto& active_layers = (*parameters.m_activation_functions);
		const auto& hidden_types  = (*parameters.m_layers_types);
		//return NeuralNetwork
		return build_network(n_features,n_class, hidden_layers, active_layers, hidden_types);
	}
	//build a mlp network from parameters
	NeuralNetwork build_network
	(
		  size_t n_features
		, size_t n_class
		,       std::vector<long>         layers
		, const std::vector<std::string>& functions
		, const std::vector<std::string>& types
	)
	{ 
		//input of next layer
		size_t input_size = n_features;
		size_t t= 0;
		size_t a = 0;
		size_t f = 0;
		//layer gen
		auto new_layer = [&]() -> Layer::SPtr 
		{ 
			//build layer
			if(t < types.size())
			{
				//is the last	
				bool is_the_first = t == 0;
				bool is_the_last  = t == (types.size()-1);
				//get size
				int  min_i = LayerFactory::min_input_size(types[t]);
				int  max_i = LayerFactory::max_input_size(types[t]);
				int  min_f = LayerFactory::min_activation_size(types[t]);
				int  max_f = LayerFactory::max_activation_size(types[t]);
				int  flags = LayerFactory::flags(types[t]);
				//pass
			    bool not_pass_trought = !(flags & DENN_PASS_TROUGHT);
				//failed
				if (is_the_first && !( flags & DENN_CAN_GET_THE_INPUT )) return nullptr;
				if (is_the_last && !( flags & DENN_CAN_RETURN_OUTPUT ))  return nullptr;
				//
				if (is_the_last)
				{
					min_i = LayerFactory::min_output_size(types[t]);
					max_i = LayerFactory::max_output_size(types[t]);
				}
				//build input
				std::vector<size_t>			    l_inputs;
				std::vector<ActivationFunction> l_functions;
				//add first input
				if(not_pass_trought) l_inputs.push_back(input_size);
				//add inputs
				for(int cargs=0; cargs != max_i; ++cargs, ++a)
				{	
					//args in pool
					if(layers.size() <= a) return nullptr;
					//get size
					long size = layers[a];
					//more then min?
					     if (size <= 0 && cargs < min_i) return nullptr;
					else if (size > 0)					 l_inputs.push_back(size);
					else break;
				}
				//add functions
				for (int cfuncs = 0; cfuncs != max_f; ++cfuncs, ++f)
				{
					//args in pool
					if (functions.size() <= f) return nullptr;
					//exits
					bool exists = ActivationFunctionFactory::exists(functions[f]);
					//more then min?
						 if (!exists && cfuncs < min_f)  return nullptr;
					else if (exists) l_functions.push_back(ActivationFunctionFactory::get(functions[f]));
					else break;
				}
				//add output size
				if (is_the_last) l_inputs.push_back(n_class);
				//build input
				auto layer = LayerFactory::create(
					  types[t]
					, l_functions
					, l_inputs
				);
				//update
				if(not_pass_trought)  input_size = layer->size_ouput();
				//go to next layer
				++t;
				//return
				return layer;
			}
			return nullptr; 
		};
		//network
		NeuralNetwork nn;
		//push all hidden layers
		if (types.size())
		{
			//add
			Layer::SPtr layer = nullptr;
			//add hiddens
			while ((layer = new_layer()).get())
			{
				nn.add(layer);
			}
		}
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
	//parser network arg
	bool get_network_from_string
	(
		  const std::string&		network
		, std::vector<long>&        layers
		, std::vector<std::string>& activation_functions
		, std::vector<std::string>& layers_types
	)
	{	
		//parser state
		enum State
		{
			S_READ_TYPE,
			S_READ_INPUT,
			S_READ_ACTIVATIONS,
			S_FINALIZE
		};
		//source ptr
		const char* ptr = network.data();
		//name layer
		State state{ S_READ_TYPE };
		std::string type;
		std::string function;
		std::vector< long > sizes;
		std::vector< std::string > functions;
		//ptr
		while (*ptr)
		{
			//jump space
			while (std::isspace(*ptr)) ++ptr;
			//change parse state
			if (state == S_READ_TYPE)
			{
				if (std::isalpha(*ptr))
				{
					type += *ptr;
				}
				else if (*ptr == '(')
				{
					state = S_READ_INPUT;
					//all lower case
					std::transform(type.begin(), type.end(), type.begin(), ::tolower);
					//test
					if (!LayerFactory::exists(type)) return false;
				}
				//next
				++ptr; continue;
			}
			else if (state == S_READ_INPUT)
			{
				if (std::isdigit(*ptr))
				{
					//parse
					char *outptr = nullptr;
					sizes.push_back(std::strtol(ptr, &outptr, 10));
					if (ptr == outptr) return false;
					//next
					ptr = outptr; continue;
				}
				else if (',')
				{
					state = S_READ_ACTIVATIONS;
					//next
					continue; ++ptr;
				}
				else if (*ptr == ')')
				{
					state = S_FINALIZE;
					//next
					continue; ++ptr;
				}
			}
			else if (state == S_READ_ACTIVATIONS)
			{
				if (std::isalpha(*ptr))
				{
					function += *ptr;
				}
				else if (*ptr == ')')
				{
					state = S_FINALIZE;
					//next
					continue; ++ptr;
				}
				//next
				++ptr;
				//is a space/)?
				if (std::isspace(*ptr) || *ptr == ')')
				{
					//all lower case
					std::transform(function.begin(), function.end(), function.begin(), ::tolower);
					//test
					if (!ActivationFunctionFactory::exists(function)) return false;
					//push
					functions.push_back(function);
					//clean
					function.clear();
				}
				//end state
				continue;
			}
			else if (state == S_FINALIZE)
			{
				//get if is the last
				while (std::isspace(*ptr)) ++ptr;
				//is first
				bool is_the_first = layers_types.size();
				//last == '\0'
				bool is_the_last = *ptr == '\0';
				//flags
				auto min_i = LayerFactory::min_input_size(type);
				auto max_i = LayerFactory::max_input_size(type);
				auto min_f = LayerFactory::min_input_size(type);
				auto max_f = LayerFactory::max_input_size(type);
				int  flags = LayerFactory::flags(type);
				//pass
			    bool can_input = (flags & DENN_CAN_GET_THE_INPUT);
			    bool can_output = (flags & DENN_CAN_RETURN_OUTPUT);
				//if is the last, a paramater must to be omittet
				if (is_the_last)
				{
					min_i = LayerFactory::min_output_size(type);
					max_i = LayerFactory::max_output_size(type);
				}
				//test
				if (is_the_first && !can_input) return false;
				if (is_the_last && !can_output) return false;

				if (sizes.size() < min_i) return false;
				if (sizes.size() > max_i) return false;

				if (functions.size() < min_f) return false;
				if (functions.size() > max_f) return false;
				//add
				for (auto s : sizes) layers.push_back(s);
				for (auto& f : functions) activation_functions.push_back(f);
				//add void
				for (int v = sizes.size(); v < max_i; ++v) layers.push_back(0);
				for (int v = functions.size(); v < max_f; ++v) activation_functions.push_back("-");
				//add type
				layers_types.push_back(type);
				//ok
				type.clear();
				function.clear();
				sizes.clear();
				functions.clear();
			}
		}
		//ok
		return true;
	}
	bool get_string_from_args
	(
				std::string&			  network
		, const std::vector<long>&        layers
		, const std::vector<std::string>& activation_functions
		, const std::vector<std::string>& layers_types
	)
	{
		network = "";
		size_t a = 0;
		size_t f = 0;

		for (size_t t = 0; t != layers_types.size(); ++t)
		{
			//is first
			bool is_the_first = t == 0;
			//last == '\0'
			bool is_the_last = t == (layers_types.size() - 1);
			//get size
			int  max_i = LayerFactory::max_input_size(layers_types[t]);
			int  max_f = LayerFactory::max_activation_size(layers_types[t]);
			int  flags = LayerFactory::flags(layers_types[t]);
			//pass
			bool pass_trought = (flags & DENN_PASS_TROUGHT);
			bool can_input = (flags & DENN_CAN_GET_THE_INPUT);
			bool can_output = (flags & DENN_CAN_RETURN_OUTPUT);
			//failed
			if (is_the_first && !can_input) return false;
			//failed
			if (is_the_last && !can_output) return false;
			//
			if (is_the_last && !pass_trought)
			{
				max_i = LayerFactory::max_output_size(layers_types[t]);
			}
			//flags
			bool remove_last_space_input = false;
			bool remove_last_space_function = false;
			///////////////////////////////////////////////////////
			network += layers_types[t];
			network += '(';
			//args
			for (int cargs = 0; cargs != max_i; ++cargs, ++a)
			{ 
				if (layers[a] <= 0) { ++a; break; }
				network += std::to_string(layers[a]);
				network += ' '; 
				remove_last_space_input = true;
			}
			if(remove_last_space_input) network.pop_back();
			//,
			network += ", ";
			//activationfunctions
			for (int cfuncs = 0; cfuncs != max_f; ++cfuncs, ++f)
			{ 
				if (activation_functions[f] == "-") { ++f; break; }
				network += activation_functions[f];
				network += ' ';  
				remove_last_space_function = true;
			}
			if (remove_last_space_function) network.pop_back();
			//end
			network += ") ";
			///////////////////////////////////////////////////////
		}
		//remove last space
		if(layers_types.size()) network.pop_back();
		//ok
		return true;
	}
}
