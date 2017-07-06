#pragma once
#include "Config.h"
#include "DennMutation.h"
#include "DennCrossover.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace Denn
{
	class Arguments
	{
	public:

		Arguments(int nargs, const char** vargs);
		
		const char* get_string();
		int get_int() ;
		double get_double();
		
		int remaining() const;

		void back_of_one();
		bool start_with_minus() const;

	protected:

		int		      m_rem_arg;
		const char**  m_pointer;

	};

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
			//no cast
			const T& get() const { return m_data; }
			
		private:


			//init
			read_only() {}
			
			template < typename I > read_only(const I& arg) { m_data = T(arg); }
			
			template < typename I > T operator=(const I& arg) { m_data = T(arg); return m_data; }

			T& get() { return m_data; }

			//sata
			T m_data;

			friend class Parameters;

		};

		struct ParameterInfo
		{
			std::string				   		  m_description;
			std::vector< std::string  > 	  m_arg_key;
			std::function< bool(Arguments&) > m_action;
		};

	public:

		read_only<std::string>					  m_dataset_filename;
		read_only<std::string> 					  m_output_filename;
		std::vector< ParameterInfo >              m_params_info;
		
		read_only<size_t>	             m_generations   { size_t(1000)  };
		read_only<size_t>	             m_sub_gens      { size_t(100)   };
		read_only<size_t>	             m_np            { size_t(12)    };
		read_only<Scalar>	             m_default_f     { Scalar(1.0)   };
		read_only<Scalar>	             m_default_cr    { Scalar(1.0)   };
		read_only<Scalar>	             m_jde_f         { Scalar(0.1)   };
		read_only<Scalar>	             m_jde_cr        { Scalar(0.1)   };
		read_only<Scalar>	             m_clamp_max     { Scalar( 10.0) };
		read_only<Scalar>	             m_clamp_min     { Scalar(-10.0) };
		read_only<Scalar>	             m_range_max     { Scalar( 1.0 ) };
		read_only<Scalar>	             m_range_min     { Scalar(-1.0)  };
		read_only<bool>	                 m_restart_enable{ true          };
		read_only<long>	                 m_restart_count { size_t(2)    };
		read_only<Scalar>	             m_restart_delta { Scalar(0.02) };
		read_only<int>	                 m_threads_omp   { size_t(2) };
		read_only<size_t>	             m_threads_pop   { size_t(2) };
		read_only<std::string>           m_mutation_type { "rand/1" };
		read_only<std::string>           m_crossover_type{ "bin" };
		read_only< std::vector<size_t> > m_hidden_layers { /* none */ };
	
		Parameters();
		Parameters(int nargs, const char **vargs, bool jump_first = true);

		void get_params_from_args(int nargs, const char **vargs, bool jump_first = true);

	private:

		static bool compare_n_args(const std::vector< std::string >& keys, const char* arg);
		static std::string return_n_space(size_t n);
		std::string make_help() const;


	};

}