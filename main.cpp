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

		template < typename Parameters >
		void serialize_parameters
		(
			const Parameters& args
		)
		{
			Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
			m_ostream << "\t\"arguments\" :" << std::endl;
			m_ostream << "\t{" << std::endl;
			m_ostream << "\t\t \"generations\" : "   << *args.m_generations   << "," << std::endl;
			m_ostream << "\t\t \"sub_gens\" : "      << *args.m_sub_gens      << "," << std::endl;
			m_ostream << "\t\t \"pop_size\" : "      << *args.m_np            << "," << std::endl;
			m_ostream << "\t\t \"f_default\" : "     << *args.m_default_f     << "," << std::endl;
			m_ostream << "\t\t \"cr_default\" : "    << *args.m_default_cr    << "," << std::endl;
			m_ostream << "\t\t \"f_jde\" : "         << *args.m_jde_f         << "," << std::endl;
			m_ostream << "\t\t \"cr_jde\" : "        << *args.m_jde_cr        << "," << std::endl;
			m_ostream << "\t\t \"clamp_max\" : "     << *args.m_clamp_max     << "," << std::endl;
			m_ostream << "\t\t \"clamp_min\" : "     << *args.m_clamp_min     << "," << std::endl;
			m_ostream << "\t\t \"range_max\" : "     << *args.m_range_max     << "," << std::endl;
			m_ostream << "\t\t \"range_min\" : "     << *args.m_range_min     << "," << std::endl;
			m_ostream << "\t\t \"restart_count\" : " << *args.m_restart_count << "," << std::endl;
			m_ostream << "\t\t \"restart_delta\" : " << *args.m_restart_delta << "," << std::endl;
			m_ostream << "\t\t \"threads_omp\" : "   << *args.m_threads_omp   << "," << std::endl;
			m_ostream << "\t\t \"threads_pop\" : "   << *args.m_threads_pop   <<		std::endl;
			m_ostream << "\t}," << std::endl;
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
			m_ostream << "\t\"time\" : "     << time << "," << std::endl;
			m_ostream << "\t\"accuracy\" : " << accuracy << "," << std::endl;
			m_ostream << "\t\"f\" : " << f << "," << std::endl;
			m_ostream << "\t\"cr\" : " << cr << "," << std::endl;
			m_ostream << "\t\"network\" : [" << std::endl;
			for (size_t i = 0; i != network.size(); ++i)
			{
				m_ostream
				<< "\t\t["
				<< network[i].weights().format(matrix_to_json_array)
				<< ","
				<< network[i].baias().format(matrix_to_json_array)
				<< ((i != network.size() - 1) ? "]," : "]")
				<< std::endl;
			}
			m_ostream << "\t]" << std::endl;
		}

	private:

		std::ostream& m_ostream;

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
			//mutation
			, MutationType::MT_RAND_ONE
			//crossover
			, CrossOverType::CR_BIN
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

int main(int argc,const char** argv)
{
	/*input example
		///////////////////////////////////////////////////////////IRIS
		-t 5000 \
		-s 40 \
		-np 42 \
		-f 0.6 \
		-cr 0.8 \
		-jf 0.1 \
		-jcr 0.1 \
		-cmin -15 \
		-cmax 15 \
		-rmin -2.0 \
		-rmax 2.0 \
		-i "assets/iris_105x6_5s.gz" \
		-o "iris_105x6_5s.json" \
		-omp 2 \
		-tp 4
		///////////////////////////////////////////////////////////MNIST
		-t 8000 \
		-s 1000 \
		-np 96 \
		-f 0.9 \
		-cr 0.9 \
		-jf 0.1 \
		-jcr 0.1 \
		-cmin -30.0 \
		-cmax 30.0 \
		-rmin -2.0 \
		-rmax 2.0 \
		-i "assets/ARTICLE_MNIST_5_12x4000_1s.gz" \
		-o "ARTICLE_MNIST_5_12x4000_1s.json" \
		-omp 0 \
		-tp 4
		///////////////////////////////////////////////////////////MNIST-MINI
		-t 12500 \
		-rc 3 \
		-s 1250 \
		-np 96 \
		-f 0.9 \
		-cr 0.9 \
		-jf 0.2 \
		-jcr 0.2 \
		-cmin -30.0 \
		-cmax 30.0 \
		-rmin -2.0 \
		-rmax 2.0 \
		-i "assets/ARTICLE_mnist_minibatch_v004_576x100_1s.gz" \
		-o "ARTICLE_mnist_minibatch_v004_576x100_1s.json" \
		-omp 0 \
		-tp 4
	*/
	////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace Denn;
	////////////////////////////////////////////////////////////////////////////////////////////////
	using ScalarArgument = double;
	using Parameters = Denn::Parameters<ScalarArgument>;
	Parameters arguments(argc, argv);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//std::srand(std::time(NULL));
	////////////////////////////////////////////////////////////////////////////////////////////////
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
	//ptr
	std::unique_ptr<ThreadPool> uptr_thpool;
	//alloc new ThreadPool
	if(arguments.m_threads_pop)
	{
		uptr_thpool = std::make_unique<ThreadPool>(size_t(arguments.m_threads_pop));
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	using DataSetLoader = Denn::DataSetLoader< Denn::IOFileWrapper::zlib_file<> >;
	DataSetLoader dataset((const std::string&)arguments.m_dataset_filename);
	////////////////////////////////////////////////////////////////////////////////////////////////
	std::ofstream		  ofile((const std::string&)arguments.m_output_filename);
	BuildTest::OutputData output(ofile);
	////////////////////////////////////////////////////////////////////////////////////////////////
	class CustomRuntimeOutput : public Denn::RuntimeOutput
	{
	public:

		CustomRuntimeOutput(std::ostream& stream=std::cerr) : Denn::RuntimeOutput(stream){}

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
			//
			write_output(); 
			output() << std::endl;
		}

		virtual void update_pass() override 
		{ 
			++m_n_pass;
			//compute pass time
			double pass_per_sec = (double(m_n_pass) / (Time::get_time() - m_pass_time));
			//clean line
			for(short i=0;i!=10;++i) output() << "\t";
			output() << "\r";
			//write output
			output() << double(long(pass_per_sec*10.))/10.0 << " [it/s], ";
			write_output(); 
			output() << "\r";
		}

		virtual void end(double test_eval) override
		{ 
			write_output(); 
			output() << std::endl;
			output() << "Denn end [ test: " << test_eval << ", time: " << Time::get_time() - m_start_time << " ]" << std::endl;
		}

	protected:

		double m_start_time;
		double m_pass_time;
		long   m_n_pass;

	};
	//standard
	//auto runtime_out = std::make_shared<Denn::RuntimeOutput>(/* srd::cerr or std::cout or file */); //default std::cerr
	//custom
	auto runtime_out = std::make_shared<CustomRuntimeOutput>()->get_ptr(); //default std::cerr


	//double or float?
	switch (dataset.get_main_header_info().m_type)
	{
		case DataSetType::DS_FLOAT:  BuildTest::execute<float, Parameters, DataSetLoader> (arguments, dataset, output, uptr_thpool.get(), runtime_out); break;
		case DataSetType::DS_DOUBLE: BuildTest::execute<double, Parameters, DataSetLoader>(arguments, dataset, output, uptr_thpool.get(), runtime_out); break;
		default: break;
	} 

	return 0;
}
