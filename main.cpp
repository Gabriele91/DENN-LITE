#include <iostream>
#include <ctime>
#include <fstream>
#include "Denn.h"

#define STR_PRINT_ATTRIBUTES(...)  #__VA_ARGS__; __VA_ARGS__
namespace BuildTest
{

	class OutputData
	{
	public:

		OutputData(std::ostream* ostream) :OutputData(*ostream) {}
		OutputData(std::ostream& ostream) :m_ostream(ostream) 
		{
			m_ostream << "{" << std::endl;
		}

		virtual ~OutputData()
		{
			m_ostream << "}" << std::endl;
		}

		void serialize_parameters
		(
			const Denn::Parameters& args
		)
		{
			Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
			m_ostream << "\t\"arguments\" :" << std::endl;
			m_ostream << "\t{" << std::endl;
			m_ostream << "\t\t \"generations\" : "   << *args.m_generations   			           << ","   << std::endl;
			m_ostream << "\t\t \"sub_gens\" : "      << *args.m_sub_gens     		    	       << ","   << std::endl;
			m_ostream << "\t\t \"number_parents\" : "<< *args.m_np            			           << ","   << std::endl;
			m_ostream << "\t\t \"mutation\" : \""    << *args.m_mutation_type                      << "\"," << std::endl;
			m_ostream << "\t\t \"crossover\" : \""   << *args.m_crossover_type                     << "\"," << std::endl;
			m_ostream << "\t\t \"f_default\" : "     << *args.m_default_f      		 	           << ","   << std::endl;
			m_ostream << "\t\t \"cr_default\" : "    << *args.m_default_cr   			           << ","   << std::endl;
			m_ostream << "\t\t \"f_jde\" : "         << *args.m_jde_f        		 	           << ","   << std::endl;
			m_ostream << "\t\t \"cr_jde\" : "        << *args.m_jde_cr       	                   << ","   << std::endl;
			m_ostream << "\t\t \"clamp_max\" : "     << *args.m_clamp_max  		                   << ","   << std::endl;
			m_ostream << "\t\t \"clamp_min\" : "     << *args.m_clamp_min   	                   << ","   << std::endl;
			m_ostream << "\t\t \"range_max\" : "     << *args.m_range_max    			 	       << ","   << std::endl;
			m_ostream << "\t\t \"range_min\" : "     << *args.m_range_min    			 	       << ","   << std::endl;
			m_ostream << "\t\t \"restart_count\" : " << *args.m_restart_count			 	       << ","   << std::endl;
			m_ostream << "\t\t \"restart_delta\" : " << *args.m_restart_delta 			           << ","   << std::endl;
			m_ostream << "\t\t \"threads_omp\" : "   << *args.m_threads_omp   				       << ","   << std::endl;
			m_ostream << "\t\t \"threads_pop\" : "   << *args.m_threads_pop   				       << std::endl;
			m_ostream << "\t}," << std::endl;
		}

		void serialize_a_individual
		(
			 double time,
			 Denn::Scalar accuracy,
			 Denn::Scalar f,
			 Denn::Scalar cr,
			 const Denn::NeuralNetwork& network
		)
		{
			Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
			m_ostream << "\t\"time\" : "     << time << "," << std::endl;
			m_ostream << "\t\"accuracy\" : " << accuracy << "," << std::endl;
			m_ostream << "\t\"f\" : " << f << "," << std::endl;
			m_ostream << "\t\"cr\" : " << cr << "," << std::endl;
			m_ostream << "\t\"network\" : [" << std::endl;
			for (size_t i = 0; i != network.size(); ++i)
			{
				m_ostream
				<< "\t\t["
				<< network[i][0].format(matrix_to_json_array)
				<< ","
				<< network[i][1].format(matrix_to_json_array)
				<< ((i != network.size() - 1) ? "]," : "]")
				<< std::endl;
			}
			m_ostream << "\t]" << std::endl;
		}

	private:

		std::ostream& m_ostream;
		
	};

	void execute
	(
		  const Denn::Parameters& parameters
		, Denn::DataSetLoader& dataset
		, OutputData& output
		, Denn::ThreadPool* ptr_thpool
		, Denn::RuntimeOutput::SPtr runtime_output
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
		output.serialize_parameters(parameters);
		output.serialize_a_individual
		(
		  execute_time
		, denn.execute_test(*result)
		, result->m_f 
		, result->m_cr
		, result->m_network
		);
	}
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
	std::ofstream		  ofile((const std::string&)arguments.m_output_filename);
	BuildTest::OutputData output(ofile);
	////////////////////////////////////////////////////////////////////////////////////////////////
	class CustomRuntimeOutput : public Denn::RuntimeOutput
	{
	public:

		CustomRuntimeOutput(std::ostream& stream=std::cerr) : Denn::RuntimeOutput(stream){}
		
		virtual bool is_enable() override  { return true; }

		virtual void start() override
		{ 
			output() << "Denn start" << std::endl;
			m_start_time = Time::get_time();
			m_pass_time  = Time::get_time(); 
			m_n_pass = 0;
		}

		virtual void update_best() override 
		{ 
			//reset
			m_pass_time  = Time::get_time(); 
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
			double pass_per_sec = (double(m_n_pass) / (Time::get_time() - m_pass_time));
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
			<< Time::get_time() - m_start_time 
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
	//custom
	auto runtime_out = std::make_shared<CustomRuntimeOutput>()->get_ptr(); //default std::cerr
	//execute test
 	BuildTest::execute(arguments, dataset, output, uptr_thpool.get(), runtime_out);
	
	return 0;
}
