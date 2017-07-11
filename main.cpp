#include <iostream>
#include <ctime>
#include <fstream>
#include "Denn.h"
class CustomRuntimeOutput : public Denn::RuntimeOutput
{
public:

	CustomRuntimeOutput(std::ostream& stream=std::cerr) : Denn::RuntimeOutput(stream){}
	
	virtual bool is_enable() override  { return true; }

	virtual void start() override
	{ 
		output() << "Denn start" << std::endl;
		m_start_time = Denn::Time::get_time();
		m_pass_time  = Denn::Time::get_time(); 
		m_n_pass = 0;
	}

	virtual void update_best() override 
	{ 
		//reset
		m_pass_time  = Denn::Time::get_time(); 
		m_n_pass 	 = 0;
		//clean line
		clean_line();
		//output
		write_output(); 
		output() << std::endl;
	}

	virtual void update_pass() override 
	{ 
		++m_n_pass;
		//compute pass time
		double pass_per_sec = (double(m_n_pass) / (Denn::Time::get_time() - m_pass_time));
		//clean line
		clean_line();
		//write output
		output() << double(long(pass_per_sec*10.))/10.0 << " [it/s], ";
		write_output(); 
		output() << "\r";
	}

	virtual void end() override
	{ 
		output() 
		<< "Denn end [ test: " 
		<< m_end_of_iterations.m_test_result 
		<< ", time: " 
		<< Denn::Time::get_time() - m_start_time 
		<< " ]" 
		<< std::endl;
	}

	virtual void write_output()
	{
		write_local_pass();
		output() << " -> on population: ";
		write_pass_best("[ id: ",", cross: ");
		output() << ", best: ";
		write_global_best("[ acc: ",", cross: ");
	}

	virtual void clean_line()
	{
		//clean line
		for(short i=0;i!=11;++i) 
			output() << "          ";
		//end row
		output() << "\r";
	}

protected:

	double m_start_time;
	double m_pass_time;
	long   m_n_pass;

};

void execute
(
	  const Denn::Parameters& parameters
	, Denn::DataSetLoader& dataset
	, Denn::ThreadPool* ptr_thpool
	, Denn::RuntimeOutput::SPtr runtime_output
	, Denn::SerializeOutput::SPtr serialize_output
)
{
	using namespace Denn;
	// NETWORK
	size_t n_features = dataset.get_main_header_info().m_n_features;
	size_t n_class    = dataset.get_main_header_info().m_n_classes;
	NeuralNetwork nn0;
	//hidden layer list
	const auto& hidden_layers = (*parameters.m_hidden_layers);
	//push all hidden layers
	if(hidden_layers.size())
	{
		//add first layer
		nn0.add_layer( PerceptronLayer(n_features, hidden_layers[0]) );
		//add next layers
		for(size_t i = 0; i != hidden_layers.size() - 1;++i)
		{
			nn0.add_layer( PerceptronLayer(hidden_layers[i], hidden_layers[i+1]) );
		}
		//add last layer
		nn0.add_layer( PerceptronLayer(hidden_layers[hidden_layers.size()-1], n_class) );
	}
	//else add only input layer
	else 
	{
		nn0.add_layer( PerceptronLayer(n_features, n_class) );
	}
	//Function ptr
	auto cost_function = CostFunction::softmax_cross_entropy_with_logit< Matrix >;

	//DENN
	DennAlgorithm denn
	(
			&dataset
		, parameters
		, nn0
		, cost_function
		//output
		, runtime_output
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
		omp_set_num_threads(*arguments.m_threads_omp);
		Eigen::setNbThreads(*arguments.m_threads_omp);
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
	DataSetLoaderGZ dataset((const std::string&)arguments.m_dataset_filename);
	////////////////////////////////////////////////////////////////////////////////////////////////
	std::ofstream	ofile((const std::string&)arguments.m_output_filename);
	////////////////////////////////////////////////////////////////////////////////////////////////
	auto runtime_out      = std::make_shared<CustomRuntimeOutput>()->get_ptr(); //default std::cerr
	auto serialize_output = std::make_shared<JSONSerializeOutput>(ofile)->get_ptr();
	//execute test
 	execute(arguments, dataset, uptr_thpool.get(), runtime_out, serialize_output);
	
	return 0;
}
