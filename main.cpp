#include <iostream>
#include <ctime>
#include <fstream>
#include "Denn.h"

#define STR_PRINT_ATTRIBUTES(...)  #__VA_ARGS__; __VA_ARGS__

namespace LineInput
{

	class Arguments
	{
	public:

		Arguments(int nargs, const char** vargs)
		: m_rem_arg(nargs)
		, m_pointer(vargs)
		{
		}

		const char* get_string()
		{
			assert(m_rem_arg);
			--m_rem_arg;
			return *(m_pointer++);
		}

		int get_int() 
		{
			return atoi(get_string());
		}

		double get_double()
		{
			return atof(get_string());
		}

		int remaining() const
		{
			return m_rem_arg;
		}

	public:

		int		      m_rem_arg;
		const char**  m_pointer;

	};

	template < typename Scalar = double >
	class ReadArguments
	{

		template <class T>
		class read_only 
		{

		public:

			//default
			operator const T&() const { return m_data;    }
			//cast
			template < class X >
			operator const X() const { return X(m_data); }

			//default
			const T& operator *() const { return m_data; }
			
		private:

			read_only() {}

			template < typename I > read_only(const I& arg) { m_data = T(arg); }

			T m_data;

			template < typename I > T operator=(const I& arg) { m_data = T(arg); return m_data; }

			friend class ReadArguments<Scalar>;

		};

		static inline bool compare_n_args(std::initializer_list<const char*> keys, const char* arg)
		{
			for (const char* key : keys) if (strcmp(key, arg) == 0) return true;
			return false;
		}

	public:

		ReadArguments() {}
		ReadArguments(int nargs, const char **vargs, bool jump_first = true)
		{
			get_params_from_args(nargs, vargs, jump_first);
		}

		void get_params_from_args(int nargs, const char **vargs, bool jump_first = true)
		{
			Arguments args(nargs, vargs);
			//jump first
			if (args.remaining() && jump_first) args.get_string();
			//start
			while (args.remaining())
			{
				const char *p = args.get_string();
				     if (compare_n_args({ "--gen_tot",    "-t"  }, p))      m_gen_tot = args.get_int();
				else if (compare_n_args({ "--gen_step",   "-s"  }, p))      m_gen_step = args.get_int();
				else if (compare_n_args({ "--pop_size",   "-p"  }, p))      m_pop_size = args.get_int();
				else if (compare_n_args({ "--f_default",  "-f"  }, p))      m_f_default  = args.get_double();
				else if (compare_n_args({ "--cr_default", "-cr" }, p))      m_cr_default = args.get_double();
				else if (compare_n_args({ "--f_jde",      "-jf" }, p))      m_f_jde      = args.get_double();
				else if (compare_n_args({ "--cr_jde",     "-jcr" }, p))     m_cr_jde     = args.get_double();
				else if (compare_n_args({ "--clamp_min",  "-cmin" }, p))    m_clamp_min  = args.get_double();
				else if (compare_n_args({ "--clamp_max",  "-cmax" }, p))    m_clamp_max  = args.get_double();
				else if (compare_n_args({ "--random_min",  "-rmin" }, p))   m_range_min  = args.get_double();
				else if (compare_n_args({ "--random_max",  "-rmax" }, p))   m_range_max  = args.get_double();
				else if (compare_n_args({ "--restart_count",  "-rc" }, p))  m_restart_count = args.get_int();
				else if (compare_n_args({ "--restart_delta",  "-rd" }, p))  m_restart_delta = args.get_double();
				else if (compare_n_args({ "--dataset", "-d", "-i" }, p))    m_dataset_filename = args.get_string();
				else if (compare_n_args({ "--output",        "-o" }, p))    m_output_filename  = args.get_string();
				else if (compare_n_args({ "--threads_omp",   "-omp" }, p))  m_threads_omp    = args.get_int();
				else if (compare_n_args({ "--threads_pop",   "-tp" }, p))   m_threads_pop    = args.get_int();
				else 
				{
					std::cerr << "parameter " << p << " not found\n";
					exit(1);
				}
			}
		}

		read_only<std::string> m_dataset_filename;
		read_only<std::string> m_output_filename;
		read_only<size_t>	   m_gen_tot   { size_t(1000)  };
		read_only<size_t>	   m_gen_step  { size_t(100)   };
		read_only<size_t>	   m_pop_size  { size_t(12)    };
		read_only<Scalar>	   m_f_default { Scalar(1.0)   };
		read_only<Scalar>	   m_cr_default{ Scalar(1.0)   };
		read_only<Scalar>	   m_f_jde     { Scalar(0.1)   };
		read_only<Scalar>	   m_cr_jde    { Scalar(0.1)   };
		read_only<Scalar>	   m_clamp_max { Scalar( 10.0) };
		read_only<Scalar>	   m_clamp_min { Scalar(-10.0) };
		read_only<Scalar>	   m_range_max { Scalar( 1.0 ) };
		read_only<Scalar>	   m_range_min { Scalar(-1.0)  };
		read_only<size_t>	   m_restart_count{ size_t(2)    };
		read_only<Scalar>	   m_restart_delta{ Scalar(0.02) };
		read_only<int>	       m_threads_omp   { size_t(2) };
		read_only<size_t>	   m_threads_pop   { size_t(2) };

	};

	class FileRuntimeOutput : public Denn::RuntimeOutput
	{
	public:

		FileRuntimeOutput(const std::string& pathfile):m_file(pathfile){}
		virtual bool is_enable() { return true; }
		virtual std::ostream& output() { return m_file; }
	
	private:

		std::ofstream m_file;

	};

	class CoutRuntimeOutput : public Denn::RuntimeOutput
	{
	public:

		virtual bool is_enable() { return true; }
		virtual std::ostream& output() { return std::cout; }
	};

	class VoidRuntimeOutput : public Denn::RuntimeOutput
	{
	public:

		virtual bool is_enable() { return false; }
	};

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

		template < typename ScalarType >
		void serialize_arguments
		(
			const ReadArguments< ScalarType >& args
		)
		{
			Eigen::IOFormat matrix_to_json_array(Eigen::FullPrecision, Eigen::DontAlignCols, ", ", ", ", "[", "]", "[", "]");
			m_ostream << "\t\"arguments\" :" << std::endl;
			m_ostream << "\t{" << std::endl;
			m_ostream << "\t\t \"gen_tot\" : "       << *args.m_gen_tot       << "," << std::endl;
			m_ostream << "\t\t \"gen_step\" : "      << *args.m_gen_step      << "," << std::endl;
			m_ostream << "\t\t \"pop_size\" : "      << *args.m_pop_size      << "," << std::endl;
			m_ostream << "\t\t \"f_default\" : "     << *args.m_f_default     << "," << std::endl;
			m_ostream << "\t\t \"cr_default\" : "    << *args.m_cr_default    << "," << std::endl;
			m_ostream << "\t\t \"f_jde\" : "         << *args.m_f_jde         << "," << std::endl;
			m_ostream << "\t\t \"cr_jde\" : "        << *args.m_cr_jde        << "," << std::endl;
			m_ostream << "\t\t \"clamp_max\" : "     << *args.m_clamp_max     << "," << std::endl;
			m_ostream << "\t\t \"clamp_min\" : "     << *args.m_clamp_min     << "," << std::endl;
			m_ostream << "\t\t \"range_max\" : "     << *args.m_range_max     << "," << std::endl;
			m_ostream << "\t\t \"range_min\" : "     << *args.m_range_min     << "," << std::endl;
			m_ostream << "\t\t \"restart_count\" : " << *args.m_restart_count << "," << std::endl;
			m_ostream << "\t\t \"restart_delta\" : " << *args.m_restart_delta << "," << std::endl;
			m_ostream << "\t\t \"threads_omp\" : "   << *args.m_threads_omp   << "," << std::endl;
			m_ostream << "\t\t \"threads_pop\" : "   << *args.m_threads_pop   <<		std::endl;
			m_ostream << "\t}" << std::endl;
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

}

namespace BuildTest
{

	template < typename ScalarType, typename ScalarArgument = double, typename DSLoader >
	void execute
	(
		  const LineInput::ReadArguments<ScalarArgument>& arguments
		, DSLoader& dataset
		, LineInput::OutputData& output
		, Denn::ThreadPool& thpool
		, Denn::RuntimeOutput::SPtr runtime_output
	)
	{
		using namespace Denn;
		using MLP      = PerceptronNetwork< Matrix< ScalarType > >;
		using LP	   = PerceptronLayer  < Matrix< ScalarType > >;
		using DennAlgo = DennAlgorithm< MLP, DSLoader >;

		// NETWORK
		size_t n_features = dataset.get_main_header_info().m_n_features;
		size_t n_class    = dataset.get_main_header_info().m_n_classes;
		MLP nn0( LP(n_features, n_class) );

		//Function ptr
		DennAlgo::CostFunction cost_function = CostFunction::softmax_cross_entropy_with_logit<MLP::MatrixType>;

		//DENN
		DennAlgo denn
		(
			  &dataset
			, arguments.m_pop_size
			, nn0
			// jde f, cr
			, DennAlgo::JDEFCrInfo 
			{ arguments.m_f_jde
			, arguments.m_cr_jde
			}	
			//default f,cr
			, DennAlgo::FCrInfo    
			{ arguments.m_f_default
			, arguments.m_cr_default 
			}
			//default clamp
			, DennAlgo::ClampInfo  
			{ arguments.m_clamp_min
			, arguments.m_clamp_max 
			}
			//restart
			, DennAlgo::RestartInfo
			{ true
			, arguments.m_restart_count
			, arguments.m_restart_delta 
			}
			//random
			, DennAlgo::RandomRangeInfo
			{ arguments.m_range_min
			, arguments.m_range_max
			}
			//const function
			, cost_function
			//output
			, runtime_output
		);
		//Init population
		denn.init();
		//execute
		double execute_time = Time::get_time();
		auto result = denn.execute(arguments.m_gen_tot, arguments.m_gen_step, &thpool);
		execute_time = Time::get_time() - execute_time;

		//output
		output.serialize_arguments< ScalarArgument >(arguments);
		output.serialize_a_individual< ScalarType >
		(
		  execute_time
		, denn.execute_test(result)
		, result.m_f
		, result.m_cr
		, result.m_network
		);
	}
}

int main(int argc,const char** argv)
{
	/*input example
		///////////////////////////////////////////////////////////IRIS
		-t 5000 
		-s 40
		-p 42 
		-f 0.6 
		-cr 0.8
		-jf 0.1 
		-jcr 0.1
		-cmin -15 
		-cmax 15 
		-rmin -2.0 
		-rmax 2.0 
		-i "assets/iris_105x6_5s.gz"
		-o "iris_105x6_5s.json"
		-omp 2 
		-tp 8
		///////////////////////////////////////////////////////////MNIST
		-t 7000 
		-s 1000
		-p 96 
		-f 0.9 
		-cr 0.9
		-jf 0.1 
		-jcr 0.1
		-cmin -30.0 
		-cmax 30.0 
		-rmin -2.0 
		-rmax 2.0 
		-i "assets/ARTICLE_MNIST_5_12x4000_1s.gz"
		-o "ARTICLE_MNIST_5_12x4000_1s.json"
		-omp 4 
		-tp 8
	*/
	LineInput::ReadArguments<> arguments(argc, argv);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//std::srand(std::time(NULL));
	////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace Denn;
	////////////////////////////////////////////////////////////////////////////////////////////////
	//parallel (OpenMP)
	#ifdef _OPEN_MP_SUPPORTED_
	int n_openMP_threads = arguments.m_threads_omp;
	if (n_openMP_threads)
	{
		omp_set_num_threads(n_openMP_threads);
		Eigen::setNbThreads(n_openMP_threads);
		Eigen::initParallel();
	}
	#endif
	//parallel (Thread Pool)
	size_t n_denn_threads = arguments.m_threads_pop;
	ThreadPool thpool(n_denn_threads);
	////////////////////////////////////////////////////////////////////////////////////////////////
	Denn::DataSetLoader< Denn::IOFileWrapper::zlib_file<> > dataset(arguments.m_dataset_filename);
	////////////////////////////////////////////////////////////////////////////////////////////////
	std::ofstream		  ofile((std::string)arguments.m_output_filename);
	LineInput::OutputData output(ofile);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//auto runtime_out = std::make_shared<Denn::RuntimeOutput>(); //default std::cerr
	//auto runtime_out = std::make_shared<LineInput::FileRuntimeOutput>("runtime.log")->get_ptr(); //file
	//auto runtime_out = std::make_shared<LineInput::CoutRuntimeOutput>()->get_ptr(); //cout
	auto runtime_out = std::make_shared<LineInput::VoidRuntimeOutput>()->get_ptr(); //none
	//double or float?
	switch (dataset.get_main_header_info().m_type)
	{
		case DataSetType::DS_FLOAT:  BuildTest::execute<float> (arguments, dataset, output, thpool, runtime_out); break;
		case DataSetType::DS_DOUBLE: BuildTest::execute<double>(arguments, dataset, output, thpool, runtime_out); break;
		default: break;
	} 

	return 0;
}
