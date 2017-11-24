#include "DennMainUtils.h"

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

	//apply denn 
	bool denn_algorithm
	(
		const Denn::Parameters&       parameters
		, Denn::DataSetLoader&        dataset
		, Denn::ThreadPool*           ptr_thpool
		, std::ostream&               output
		, Denn::SerializeOutput::SPtr serialize_output
		, NeuralNetwork&			  neural_netowrk
	)
	{
		//DENN
		DennAlgorithm denn
		(
			&dataset
			, parameters
			, neural_netowrk
			, EvaluationFactory::create("cross_entropy_logistic",denn) //train (loss function)
			, EvaluationFactory::create("accuracy",denn) //validation
			, EvaluationFactory::create("accuracy",denn) //test
			//output
			, output
			//thread pool
			, ptr_thpool
		);
		//execute
		double execute_time = Time::get_time();
		auto result = denn.execute();
		execute_time = Time::get_time() - execute_time;
		//output
		serialize_output->serialize_parameters(parameters);
		serialize_output->serialize_best
		(
			execute_time
			, denn.execute_test(*result)
			, result->m_f
			, result->m_cr
			, result->m_network
		);
		//output
		neural_netowrk = result->m_network;
		return true;
	}
}
