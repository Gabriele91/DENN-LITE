#pragma once
#include "Config.h"
#include <string>
#include <vector>
#include <sstream>

namespace Denn
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
	class Parameters
	{

		template <class T>
		class read_only 
		{
		public:

			//cast
			template < class X >
			inline operator X() const { return X(m_data); }
			//no cast
			const T& operator *() const { return m_data; }
			
		private:


			//init
			read_only() {}
			
			template < typename I > read_only(const I& arg) { m_data = T(arg); }
			
			template < typename I > T operator=(const I& arg) { m_data = T(arg); return m_data; }

			//sata
			T m_data;

			friend class Parameters<Scalar>;

		};

		struct ParameterInfo
		{
			std::string				   		  m_description;
			std::vector< std::string  > 	  m_arg_key;
			std::function< void(Arguments&) > m_action;
		};

	public:

		read_only<std::string>					  m_dataset_filename;
		read_only<std::string> 					  m_output_filename;
		std::vector< ParameterInfo >              m_params_info;
		
		read_only<size_t>	   m_generations   { size_t(1000)  };
		read_only<size_t>	   m_sub_gens      { size_t(100)   };
		read_only<size_t>	   m_np            { size_t(12)    };
		read_only<Scalar>	   m_default_f     { Scalar(1.0)   };
		read_only<Scalar>	   m_default_cr    { Scalar(1.0)   };
		read_only<Scalar>	   m_jde_f         { Scalar(0.1)   };
		read_only<Scalar>	   m_jde_cr        { Scalar(0.1)   };
		read_only<Scalar>	   m_clamp_max     { Scalar( 10.0) };
		read_only<Scalar>	   m_clamp_min     { Scalar(-10.0) };
		read_only<Scalar>	   m_range_max     { Scalar( 1.0 ) };
		read_only<Scalar>	   m_range_min     { Scalar(-1.0)  };
		read_only<bool>	       m_restart_enable{ true          };
		read_only<size_t>	   m_restart_count { size_t(2)    };
		read_only<Scalar>	   m_restart_delta { Scalar(0.02) };
		read_only<int>	       m_threads_omp   { size_t(2) };
		read_only<size_t>	   m_threads_pop   { size_t(2) };
	
		Parameters() 
		:m_params_info
		({
			ParameterInfo{ 
                "Global number of generation", { "--generation",    "-t"  }, 
				 [this](Arguments& args) { m_generations = args.get_int() ; } 
            },
			ParameterInfo{ 
                "Number of generation per batch", { "--sub_gens",    "-s"  }, 
                [this](Arguments& args) { m_sub_gens = args.get_int() ; }
            },
			ParameterInfo{
                 "Number of parents", { "--number_parents",    "-np"  }, 
				 [this](Arguments& args) { m_np = args.get_int() ; }
            },
			ParameterInfo{ 
                "Default F factor for DE", { "--f_default",    "-f"  }, 
				[this](Arguments& args) { m_default_f = args.get_double() ; } 
            },
			ParameterInfo{ 
                "Default CR factor for DE", { "--cr_default",    "-cr"  }, 
				[this](Arguments& args) { m_default_cr = args.get_double() ; }
            },
								
			ParameterInfo{ 
                "Probability of change of F (JDE)", { "--f_jde",    "-jf"  }, 
				[this](Arguments& args) { m_jde_f = args.get_double() ; } 
            },
			ParameterInfo{ 
                "Probability of change of CR (JDE)", { "--cr_jde",    "-jcr"  }, 
				[this](Arguments& args) { m_jde_cr = args.get_double() ; } 
            },
			ParameterInfo{ 
                "Minimum size of weight", { "--clamp_min",    "-cmin"  }, 
				[this](Arguments& args) { m_clamp_min = args.get_double() ; } 
            },
			ParameterInfo{ 
                "Maximum size of weight", { "--clamp_min",    "-cmax"  }, 
				[this](Arguments& args) { m_clamp_max = args.get_double() ; } 
            },
			ParameterInfo{ 
                "Minimum size of weight in random initialization", { "--random_min",    "-rmin"  }, 
				[this](Arguments& args) { m_range_min = args.get_double() ; } 
            },
			ParameterInfo{ 
                "Maximum size of weight in random initialization", { "--random_max",    "-rmax"  }, 
				[this](Arguments& args) { m_range_max = args.get_double() ; } 
            },
			ParameterInfo{ 
                "Number of change of batches before restart (if accuracy not increase)", { "--restart_count",    "-rc"  }, 
				[this](Arguments& args) 
				{ 
					m_restart_count = args.get_int() ; 
					if(*m_restart_count<0) m_restart_enable = false;
				} 
            },
			ParameterInfo{ 
                "Delta factor to determine if accuracy is increased", { "--restart_delta",    "-rd"  }, 
				[this](Arguments& args) 
				{
					m_restart_delta = args.get_double() ; 
					if(*m_restart_delta<0) m_restart_enable = false;
				} 
            },
			ParameterInfo{
                 "Path of dataset file (gz)", { "--dataset", "-d", "-i" }, 
				 [this](Arguments& args) {  m_dataset_filename = args.get_string(); } 
            },
			ParameterInfo{ 
                "Path of output file (json)", { "--output", "-o" }, 
				[this](Arguments& args) {  m_output_filename = args.get_string(); } 
            },
			ParameterInfo{ 
                "Number of threads using by OpenMP", { "--threads_omp",    "-omp"  }, 
				[this](Arguments& args) { m_threads_omp = args.get_int() ; } 
            },
			ParameterInfo{ 
                "Number of threads using for  generate a new population", { "--threads_pop",    "-tp"  }, 
				[this](Arguments& args) { m_threads_pop = args.get_int() ; } 
            },
			ParameterInfo{
                "Print the help", { "--help",    "-h"  }, 
				[this](Arguments& args) { std::cout << make_help(); } 
            },
		})
		{
		}

		Parameters(int nargs, const char **vargs, bool jump_first = true) : Parameters()
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
				for(auto& action : m_params_info)
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
			for(auto& param : m_params_info)
			{
				size_t space_line = 25;
				s_out << "\t";
				for(auto& key : param.m_arg_key)
				{
					s_out << key << ", ";
					space_line -= key.size() + 2;
				}
				s_out << return_n_space(Denn::clamp<size_t>(space_line,0,50));
				s_out << param.m_description;
				s_out << std::endl;
				s_out << std::endl;
			}
			s_out << std::endl;
			return s_out.str();
		}


	};

}