#include <iostream>
#include <ctime>
#include <fstream>
#include <emscripten/emscripten.h>
#include "Denn.h"

namespace BuildTest
{

	class OutputData
	{
	public:

		OutputData() 
		{
			m_temp_output << "{" << std::endl;
		}

		virtual ~OutputData()
		{		
			m_temp_output << "}" << std::endl;
			//pass
			EM_ASM_({
				Module.print_results(Module.UTF8ToString($0));
			}, m_temp_output.str().c_str());
		}

		template < typename Parameters >
		void serialize_parameters
		(
			const Parameters& args
		)
		{
			Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
			m_temp_output << "\t\"arguments\" :" << std::endl;
			m_temp_output << "\t{" << std::endl;
			m_temp_output << "\t\t \"generations\" : "   << *args.m_generations   			           << ","   << std::endl;
			m_temp_output << "\t\t \"sub_gens\" : "      << *args.m_sub_gens     		    	       << ","   << std::endl;
			m_temp_output << "\t\t \"number_parents\" : "<< *args.m_np            			           << ","   << std::endl;
			m_temp_output << "\t\t \"mutation\" : \""    << mutation_to_string(*args.m_mutation_type)  << "\"," << std::endl;
			m_temp_output << "\t\t \"crossover\" : \""   << crossover_to_string(*args.m_crossover_type)<< "\"," << std::endl;
			m_temp_output << "\t\t \"f_default\" : "     << *args.m_default_f      		 	           << ","   << std::endl;
			m_temp_output << "\t\t \"cr_default\" : "    << *args.m_default_cr   			           << ","   << std::endl;
			m_temp_output << "\t\t \"f_jde\" : "         << *args.m_jde_f        		 	           << ","   << std::endl;
			m_temp_output << "\t\t \"cr_jde\" : "        << *args.m_jde_cr       	                   << ","   << std::endl;
			m_temp_output << "\t\t \"clamp_max\" : "     << *args.m_clamp_max  		                   << ","   << std::endl;
			m_temp_output << "\t\t \"clamp_min\" : "     << *args.m_clamp_min   	                   << ","   << std::endl;
			m_temp_output << "\t\t \"range_max\" : "     << *args.m_range_max    			 	       << ","   << std::endl;
			m_temp_output << "\t\t \"range_min\" : "     << *args.m_range_min    			 	       << ","   << std::endl;
			m_temp_output << "\t\t \"restart_count\" : " << *args.m_restart_count			 	       << ","   << std::endl;
			m_temp_output << "\t\t \"restart_delta\" : " << *args.m_restart_delta 			           << ","   << std::endl;
			m_temp_output << "\t\t \"threads_omp\" : "   << *args.m_threads_omp   				       << ","   << std::endl;
			m_temp_output << "\t\t \"threads_pop\" : "   << *args.m_threads_pop   				       << std::endl;
			m_temp_output << "\t}," << std::endl;
		}

		template < typename ScalarType >
		void serialize_a_individual
		(
			 double time,
			 ScalarType accuracy,
			 ScalarType f,
			 ScalarType cr,
			 const Denn::PerceptronNetwork< Denn::Matrix< ScalarType > >& network
		)
		{
			Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
			m_temp_output << "\t\"time\" : "     << time << "," << std::endl;
			m_temp_output << "\t\"accuracy\" : " << accuracy << "," << std::endl;
			m_temp_output << "\t\"f\" : " << f << "," << std::endl;
			m_temp_output << "\t\"cr\" : " << cr << "," << std::endl;
			m_temp_output << "\t\"network\" : [" << std::endl;
			for (size_t i = 0; i != network.size(); ++i)
			{
				m_temp_output
				<< "\t\t["
				<< network[i].weights().format(matrix_to_json_array)
				<< ","
				<< network[i].baias().format(matrix_to_json_array)
				<< ((i != network.size() - 1) ? "]," : "]")
				<< std::endl;
			}
			m_temp_output << "\t]" << std::endl;
		}

	private:

		std::stringstream m_temp_output;

		std::string mutation_to_string(Denn::MutationType type)
		{
			switch(type) 
			{ 
				case Denn::MutationType::MT_RAND_ONE: return "rand/1";	
				case Denn::MutationType::MT_RAND_TWO: return "rand/2";	
				case Denn::MutationType::MT_BEST_ONE: return "best/1";	
				case Denn::MutationType::MT_BEST_TWO: return "best/2";	
				default: return "unknown";
			}
		}
		std::string crossover_to_string(Denn::CrossoverType type)
		{
			switch(type) 
			{ 
				case Denn::CrossoverType::CT_EXP: return "exp";	
				case Denn::CrossoverType::CT_BIN: return "bin";	
				default: return "unknown";
			}
		}

	};

	template 
	< typename ScalarType, typename Parameters, typename DSLoader >
	void execute
	(
		  const Parameters& parameters
		, DSLoader& dataset
		, OutputData& output
		, Denn::ThreadPool* ptr_thpool
		, Denn::RuntimeOutput::SPtr runtime_output
	)
	{
		using namespace Denn;
		using MLP           = PerceptronNetwork< Matrix< ScalarType > >;
		using LP	        = PerceptronLayer  < Matrix< ScalarType > >;
		using DennAlgo 	    = DennAlgorithm< MLP, Parameters, DSLoader >;

		// NETWORK
		size_t n_features = dataset.get_main_header_info().m_n_features;
		size_t n_class    = dataset.get_main_header_info().m_n_classes;
		MLP nn0( LP(n_features, n_class) );

		//Function ptr
		auto cost_function = CostFunction::softmax_cross_entropy_with_logit< typename MLP::MatrixType >;

		//DENN
		DennAlgo denn
		(
			  &dataset
			, parameters
			, nn0
			, cost_function
			//output
			, runtime_output
		);
		//Init population
		denn.init();
		//execute
		double execute_time = Time::get_time();
		auto result = denn.execute(ptr_thpool);
		execute_time = Time::get_time() - execute_time;

		//output
		output.serialize_parameters< Parameters >(parameters);
		output.serialize_a_individual< ScalarType >
		(
		  execute_time
		, denn.execute_test(*result)
		, result->m_f
		, result->m_cr
		, result->m_network
		);
	}
}

class CustomRuntimeOutput : public Denn::RuntimeOutput
{
public:

	CustomRuntimeOutput() : Denn::RuntimeOutput(m_buffer){}
	
	virtual bool is_enable() override       { return true;  }
	virtual bool is_enable_pass() override  { return false; }

	virtual void start() override
	{ 
		EM_ASM(
			Module.print('Denn start');
		);
		m_start_time = Denn::Time::get_time();
	}

	virtual void update_best() override 
	{ 
		//output
		write_output(); 
		//pass
		EM_ASM_({
			Module.print(Module.UTF8ToString($0));
		}, m_buffer.str().c_str());
		//clear
		m_buffer.str( std::string() );
		m_buffer.clear();
	}

	virtual void update_pass() override 
	{ 
		//void
	}

	virtual void end() override
	{ 
		EM_ASM_({
			Module.print('Denn end [ test: '+ $0 + ', time: ' + $1+' ]');
		}, m_end_of_iterations.m_test_result , Denn::Time::get_time() - m_start_time);
	}

	virtual void write_output()
	{
		write_global_pass();
		output() << " best: ";
		write_global_best("[ acc: ",", cross: ");
	}

protected:

	double m_start_time;
	std::stringstream m_buffer;

};

int main(int argc,const char** argv)
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace Denn;
	////////////////////////////////////////////////////////////////////////////////////////////////
	using ScalarArgument = double;
	using Parameters = Denn::Parameters<ScalarArgument>;
	Parameters arguments(argc, argv);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//parallel (OpenMP)
	#ifdef EIGEN_HAS_OPENMP
	if (arguments.m_threads_omp)
	{
		omp_set_num_threads(arguments.m_threads_omp);
		Eigen::setNbThreads(arguments.m_threads_omp);
		Eigen::initParallel();
	}
	#endif
	//parallel (Thread Pool)
	#ifdef PTHREADS_SUPPORTED
		std::unique_ptr<ThreadPool> uptr_thpool;
		//alloc new ThreadPool
		if(arguments.m_threads_pop)
		{
			uptr_thpool = std::make_unique<ThreadPool>(size_t(arguments.m_threads_pop));
		}	
	#else
		std::unique_ptr<ThreadPool> uptr_thpool = nullptr;
	#endif
	////////////////////////////////////////////////////////////////////////////////////////////////
	using DataSetLoader = Denn::DataSetLoader< Denn::IOFileWrapper::zlib_file<> >;
	DataSetLoader dataset((const std::string&)arguments.m_dataset_filename);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//custom
	BuildTest::OutputData output;
	auto runtime_out = std::make_shared<CustomRuntimeOutput>()->get_ptr(); 
	////////////////////////////////////////////////////////////////////////////////////////////////
	//double or float?
	switch (dataset.get_main_header_info().m_type)
	{
		case DataSetType::DS_FLOAT:  BuildTest::execute<float, Parameters, DataSetLoader> (arguments, dataset, output, uptr_thpool.get(), runtime_out); break;
		case DataSetType::DS_DOUBLE: BuildTest::execute<double, Parameters, DataSetLoader>(arguments, dataset, output, uptr_thpool.get(), runtime_out); break;
		default: break;
	} 

	return 0;
}
