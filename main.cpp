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

	protected:

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

		struct ArgumentAndAction
		{
			std::string				   		  m_description;
			std::vector< std::string  > 	  m_arg_key;
			std::function< void(Arguments&) > m_action;
		};

	public:

		ReadArguments() 
		:m_arguments_and_actions
		({
			ArgumentAndAction{ "Global number of generation", { "--gen_tot",    "-t"  }, 
								[this](Arguments& args) { m_gen_tot = args.get_int() ; } },
			ArgumentAndAction{ "Number of generation per batch", { "--gen_step",    "-s"  }, 
								[this](Arguments& args) { m_gen_step = args.get_int() ; } },
			ArgumentAndAction{ "Size of population", { "--pop_size",    "-p"  }, 
								[this](Arguments& args) { m_pop_size = args.get_int() ; } },

			ArgumentAndAction{ "Default F factor for DE", { "--f_default",    "-f"  }, 
								[this](Arguments& args) { m_f_default = args.get_double() ; } },
			ArgumentAndAction{ "Default CR factor for DE", { "--cr_default",    "-cr"  }, 
								[this](Arguments& args) { m_cr_default = args.get_double() ; } },
								
			ArgumentAndAction{ "Probability of change of F (JDE)", { "--f_jde",    "-jf"  }, 
								[this](Arguments& args) { m_f_jde = args.get_double() ; } },
			ArgumentAndAction{ "Probability of change of CR (JDE)", { "--cr_jde",    "-jcr"  }, 
								[this](Arguments& args) { m_cr_jde = args.get_double() ; } },

			ArgumentAndAction{ "Minimum size of weight", { "--clamp_min",    "-cmin"  }, 
								[this](Arguments& args) { m_clamp_min = args.get_double() ; } },
			ArgumentAndAction{ "Maximum size of weight", { "--clamp_min",    "-cmax"  }, 
								[this](Arguments& args) { m_clamp_max = args.get_double() ; } },

			ArgumentAndAction{ "Minimum size of weight in random initialization", { "--random_min",    "-rmin"  }, 
								[this](Arguments& args) { m_range_min = args.get_double() ; } },
			ArgumentAndAction{ "Maximum size of weight in random initialization", { "--random_max",    "-rmax"  }, 
								[this](Arguments& args) { m_range_max = args.get_double() ; } },

			ArgumentAndAction{ "Number of change of batches before restart (if accuracy not increase)", { "--restart_count",    "-rc"  }, 
								[this](Arguments& args) { m_restart_count = args.get_int() ; } },
			ArgumentAndAction{ "Delta factor to determine if accuracy is increased", { "--restart_delta",    "-rd"  }, 
								[this](Arguments& args) { m_restart_delta = args.get_double() ; } },

			ArgumentAndAction{ "Path of dataset file (gz)", { "--dataset", "-d", "-i" }, 
								[this](Arguments& args) {  m_dataset_filename = args.get_string(); } },
			ArgumentAndAction{ "Path of output file (json)", { "--output", "-o" }, 
								[this](Arguments& args) {  m_output_filename = args.get_string(); } },

			ArgumentAndAction{ "Number of threads using by OpenMP", { "--threads_omp",    "-omp"  }, 
								[this](Arguments& args) { m_threads_omp = args.get_int() ; } },
			ArgumentAndAction{ "Number of threads using for  generate a new population", { "--threads_pop",    "-tp"  }, 
								[this](Arguments& args) { m_threads_pop = args.get_int() ; } },

			ArgumentAndAction{ "Print the help", { "--help",    "-h"  }, 
								[this](Arguments& args) { std::cout << make_help(); } },
		})
		{
		}

		ReadArguments(int nargs, const char **vargs, bool jump_first = true):ReadArguments()
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
				bool is_a_valid_arg = false;
				const char *p = args.get_string();
				for(auto& action : m_arguments_and_actions)
				{
					if(compare_n_args(action.m_arg_key, p))
					{
						action.m_action(args);
						is_a_valid_arg = true;
						break;
					} 
				}
				if(!is_a_valid_arg) 
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
	
	private:

		static inline bool compare_n_args(const std::vector< std::string >& keys, const char* arg)
		{
			for (const std::string& key : keys) if (strcmp(key.c_str(), arg) == 0) return true;
			return false;
		}
		static inline std::string return_n_space(size_t n)
		{
			std::string out;
			while(n--) out+=" ";
			return std::move(out);
		}

		std::string make_help() const
		{
			std::stringstream s_out;
			//header
			s_out << "denn [<args>]" ;
			s_out << std::endl;				
			s_out << std::endl;
			//
			for(auto& action : m_arguments_and_actions)
			{
				size_t space_line = 25;
				s_out << "\t";
				for(auto& key : action.m_arg_key)
				{
					s_out << key << ", ";
					space_line -= key.size() + 2;
				}
				s_out << return_n_space(Denn::clamp<size_t>(space_line,0,50));
				s_out << action.m_description;
				s_out << std::endl;
				s_out << std::endl;
			}
			s_out << std::endl;
			return s_out.str();
		}

		std::vector< ArgumentAndAction > m_arguments_and_actions;


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

}

namespace BuildTest
{

	template < typename ScalarType, typename ScalarArgument = double, typename DSLoader >
	void execute
	(
		  const LineInput::ReadArguments<ScalarArgument>& arguments
		, DSLoader& dataset
		, LineInput::OutputData& output
		, Denn::ThreadPool* ptr_thpool
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
		typename DennAlgo::CostFunction 
		cost_function = CostFunction::softmax_cross_entropy_with_logit< typename MLP::MatrixType >;

		//DENN
		DennAlgo denn
		(
			  &dataset
			, arguments.m_pop_size
			, nn0
			// jde f, cr
			, typename DennAlgo::JDEFCrInfo 
			( arguments.m_f_jde
			, arguments.m_cr_jde
			)	
			//default f,cr
			, typename DennAlgo::FCrInfo    
			( arguments.m_f_default
			, arguments.m_cr_default 
			)
			//default clamp
			, typename DennAlgo::ClampInfo  
			( arguments.m_clamp_min
			, arguments.m_clamp_max 
			)
			//restart
			, typename DennAlgo::RestartInfo
			( true
			, arguments.m_restart_count
			, arguments.m_restart_delta 
			)
			//random
			, typename DennAlgo::RandomRangeInfo
			( arguments.m_range_min
			, arguments.m_range_max
			)
			//const function
			, cost_function
			//output
			, runtime_output
		);
		//Init population
		denn.init();
		//execute
		double execute_time = Time::get_time();
		auto result = denn.execute(arguments.m_gen_tot, arguments.m_gen_step, ptr_thpool);
		execute_time = Time::get_time() - execute_time;

		//output
		output.serialize_arguments< ScalarArgument >(arguments);
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
		-p 42 \
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
		-p 96 \
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
		-p 96 \
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
	using ScalarArgument = double;
	LineInput::ReadArguments<ScalarArgument> arguments(argc, argv);
	////////////////////////////////////////////////////////////////////////////////////////////////
	//std::srand(std::time(NULL));
	////////////////////////////////////////////////////////////////////////////////////////////////
	using namespace Denn;
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
	LineInput::OutputData output(ofile);
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
		case DataSetType::DS_FLOAT:  BuildTest::execute<float, ScalarArgument, DataSetLoader> (arguments, dataset, output, uptr_thpool.get(), runtime_out); break;
		case DataSetType::DS_DOUBLE: BuildTest::execute<double, ScalarArgument, DataSetLoader>(arguments, dataset, output, uptr_thpool.get(), runtime_out); break;
		default: break;
	} 

	return 0;
}
