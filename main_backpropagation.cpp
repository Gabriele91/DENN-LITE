#include <iostream>
#include <ctime>
#include <fstream>
#include "Denn.h"


void execute
(
	const Denn::Parameters&     parameters
	, Denn::DataSetLoader&        dataset
	, Denn::ThreadPool*           ptr_thpool
	, std::ostream&               output
	, Denn::SerializeOutput::SPtr serialize_output
)
{
	using namespace Denn;
	// NETWORK
	size_t n_features = dataset.get_main_header_info().m_n_features;
	size_t n_class = dataset.get_main_header_info().m_n_classes;
	NeuralNetwork nn0;
	//hidden layer list
	const auto& hidden_layers = (*parameters.m_hidden_layers);
	const auto& active_layers = (*parameters.m_active_functions);
	const auto& active_output = (*parameters.m_output_active_function);
	//push all hidden layers
	if (hidden_layers.size())
	{
		//add first layer
		nn0.add_layer(PerceptronLayer(
			active_layers.size() ? ActiveFunctionFactory::get(active_layers[0]) : nullptr
			, n_features
			, hidden_layers[0]
		));
		//add next layers
		for (size_t i = 0; i != hidden_layers.size() - 1; ++i)
		{
			nn0.add_layer(PerceptronLayer(
				(i + 1 < active_layers.size()) ? ActiveFunctionFactory::get(active_layers[i + 1]) : nullptr
				, hidden_layers[i]
				, hidden_layers[i + 1]
			));
		}
		//add last layer
		nn0.add_layer(PerceptronLayer(
			ActiveFunctionFactory::get(active_output)
			, hidden_layers[hidden_layers.size() - 1]
			, n_class
		));
	}
	//else add only input layer
	else
	{
		nn0.add_layer(PerceptronLayer(ActiveFunctionFactory::get(active_output), n_features, n_class));
	}
	//random
	for (auto& layer  : nn0)
	for (auto& matrix : *layer)
	{
		matrix  = (Matrix::Random(matrix.rows(), matrix.cols()) * 2.0);
	}
	//Function ptr
#if 1
	auto cost_function = CostFunction::softmax_cross_entropy_with_logit< Matrix >;
#elif 0
	auto cost_function = CostFunction::softmax_cross_entropy< Matrix >;
#else
	auto cost_function = CostFunction::inverse_accuracy< Matrix >;
#endif
	////////////////////////////////////////////////////////////////////////////////////////////////
	NeuralNetwork result = nn0;
	double execute_time = Time::get_time();
	size_t n_batch = 1000;
	size_t n_on_a_batch = 1;
	Scalar learn_rate = 0.1;
	Scalar regular_param = 1.0;
	dataset.start_read_batch();
	//test
	Denn::DataSetScalar test;
	dataset.read_test(test);
	//accuracy
	auto acc_init_test = Denn::CostFunction::accuracy(result.apply(test.features()), test.labels());
	//backpropagation
	for(int b_i = 0; b_i != n_batch; ++b_i)
	{
		Denn::DataSetScalar batch;
		dataset.read_batch(batch);
		if (batch.labels().rows())
		{
			for (int a_i = 0; a_i != n_on_a_batch; ++a_i)
			{
				result.backpropagation_with_sgd(
					[](const Matrix& predict, const Matrix& y)
					{
						return predict - y;
					}
					, batch.features()
					, batch.labels()
					, learn_rate
					, regular_param
				);
			}
		}
		else
		{
			output << "Wrong, batch[" << b_i << "] have 0 samples" << std::endl;
		}
	}
	execute_time = Time::get_time() - execute_time;
	//accuracy
	auto acc_end_test = Denn::CostFunction::accuracy(result.apply(test.features()), test.labels());
	output << "Accuracy start: " << acc_init_test << ", end: "<< acc_end_test << std::endl;
	//output
	serialize_output->serialize_parameters(parameters);
	serialize_output->serialize_best
	(
		execute_time
		, acc_end_test
		, 0.0
		, 0.0
		, nn0
	);
}

int main(int argc, const char** argv)
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace Denn;
	////////////////////////////////////////////////////////////////////////////////////////////////
	Parameters arguments(argc, argv);
	//test file name
	if (!(*arguments.m_dataset_filename).size()) return -1;
	////////////////////////////////////////////////////////////////////////////////////////////////
	//parallel (OpenMP)
#ifdef EIGEN_HAS_OPENMP
	if (*arguments.m_threads_omp)
	{
		omp_set_num_threads(*arguments.m_threads_omp);
		Eigen::setNbThreads(*arguments.m_threads_omp);
		Eigen::initParallel();
	}
#endif
	//parallel (Thread Pool)
	//ptr
	std::unique_ptr<ThreadPool> uptr_thpool;
	//alloc new ThreadPool
	if (*arguments.m_threads_pop)
	{
		uptr_thpool = std::make_unique<ThreadPool>(*arguments.m_threads_pop);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	if (!Denn::Filesystem::exists((const std::string&)arguments.m_dataset_filename))
	{
		std::cerr << "input file: \"" << *arguments.m_dataset_filename << "\" not exists!" << std::endl;
		return 1; //exit
	}
	DataSetLoaderGZ dataset((const std::string&)arguments.m_dataset_filename);
	////////////////////////////////////////////////////////////////////////////////////////////////
	std::ostream   runtime_output_stream(nullptr);
	std::ofstream  runtime_output_file_stream;
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
			return 1; //exit
		}
	}
	else
	{
		runtime_output_stream.rdbuf(std::cout.rdbuf());
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	if (Denn::Filesystem::exists((const std::string&)arguments.m_output_filename) &&
		!Denn::Filesystem::is_writable((const std::string&)arguments.m_output_filename))
	{
		std::cerr << "can't write into the file: \"" << *arguments.m_output_filename << "\"" << std::endl;
		return 1; //exit
	}
	std::ofstream	ofile((const std::string&)arguments.m_output_filename);
	//extension
	std::string ext = Denn::Filesystem::get_extension(*arguments.m_output_filename);
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
	if (!SerializeOutputFactory::exists(ext))
	{
		std::cerr << "can't serialize a file with extension \"" << ext << "\"" << std::endl;
		return 1; //exit
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	SerializeOutput::SPtr serialize_output = SerializeOutputFactory::create(ext, ofile, arguments);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//execute test
	execute(arguments, dataset, uptr_thpool.get(), runtime_output_stream, serialize_output);
	////////////////////////////////////////////////////////////////////////////////////////////////

	return 0;
}
