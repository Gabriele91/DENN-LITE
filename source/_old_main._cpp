#include <iostream>
#include <ctime>
#include <fstream>
#include "Denn.h"
#include "DennMainUtils.h"

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
	////////////////////////////////////////////////////////////////////////////////////////////////
	// NETWORK
	size_t n_features = dataset.get_main_header_info().m_n_features;
	size_t n_class    = dataset.get_main_header_info().m_n_classes;
	NeuralNetwork nn0 = build_mlp_network(n_features, n_class, parameters);	
	////////////////////////////////////////////////////////////////////////////////////////////////
	denn_algorithm(
		parameters, dataset, ptr_thpool, output, serialize_output, nn0
	);
	////////////////////////////////////////////////////////////////////////////////////////////////
}

int main(int argc,const char** argv)
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace Denn;
	////////////////////////////////////////////////////////////////////////////////////////////////
	Parameters arguments(argc, argv);
	//test file name
	if(!(*arguments.m_dataset_filename).size()) return -1;
	////////////////////////////////////////////////////////////////////////////////////////////////
	//parallel (OpenMP)
	#ifdef EIGEN_HAS_OPENMP
	if (*arguments.m_threads_omp)
	{
		omp_set_num_threads((int)*arguments.m_threads_omp);
		Eigen::setNbThreads((int)*arguments.m_threads_omp);
		Eigen::initParallel();
	}
	#endif
	//parallel (Thread Pool)
	//ptr
	std::unique_ptr<ThreadPool> uptr_thpool;
	//alloc new ThreadPool
	if(*arguments.m_threads_pop)
	{
		uptr_thpool = std::make_unique<ThreadPool>(*arguments.m_threads_pop);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	if(!Denn::Filesystem::exists((const std::string&)arguments.m_dataset_filename))
	{
		std::cerr << "input file: \"" << *arguments.m_dataset_filename << "\" not exists!" << std::endl;
		return 1; //exit
	}
	//get loader
	DataSetLoader::SPtr dataset = get_datase_loader((const std::string&)arguments.m_dataset_filename);
	//test loader
	if(!dataset)
	{
		std::cerr << "input file: \"" << *arguments.m_dataset_filename << "\" not supported!" << std::endl;
		return 1; //exit
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	std::ostream   runtime_output_stream(nullptr);
	std::ofstream  runtime_output_file_stream;
	std::string    runtime_output_arg = (*arguments.m_runtime_output_file);
	//output
	if(runtime_output_arg.size())
	{
		if(runtime_output_arg == "::cout")
		{
			runtime_output_stream.rdbuf(std::cout.rdbuf());
		}
		else if(runtime_output_arg == "::cerr")
		{
			runtime_output_stream.rdbuf(std::cerr.rdbuf());
		}
		else if(!Denn::Filesystem::exists((const std::string&)arguments.m_runtime_output_file) ||
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
	if(Denn::Filesystem::exists((const std::string&)arguments.m_output_filename) &&
	  !Denn::Filesystem::is_writable((const std::string&)arguments.m_output_filename))
	{
		std::cerr << "can't write into the file: \"" << *arguments.m_output_filename << "\"" << std::endl;
		return 1; //exit
	}
	std::ofstream	ofile((const std::string&)arguments.m_output_filename);
	//extension
	std::string ext = Denn::Filesystem::get_extension(*arguments.m_output_filename); 
	std::transform(ext.begin(),ext.end(), ext.begin(), ::tolower);
	if (!SerializeOutputFactory::exists(ext))
	{
		std::cerr << "can't serialize a file with extension \"" << ext << "\"" << std::endl;
		return 1; //exit
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	SerializeOutput::SPtr serialize_output = SerializeOutputFactory::create(ext, ofile, arguments);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//execute test
 	execute(arguments, *dataset, uptr_thpool.get(), runtime_output_stream, serialize_output);
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	return 0;
}
