#pragma once
#include "Config.h"
#include "DennVariant.h"
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
	public:
	
		class generic_read_only
		{
		public:
			virtual std::string name()  const = 0;
			virtual Variant variant()   const = 0;
			virtual bool serializable() const = 0;
		};

		template <class T>
		class read_only : public generic_read_only
		{
		public:

			//cast
			template < class X >
			inline operator X() const { return X(m_data); }
			//no cast
			const T& operator *() const { return m_data; }
			//no cast
			const T& get() const { return m_data; }
			//get variant
			virtual Variant variant() const	  override { return get(); }
			//get name
			virtual std::string name() const  override { return m_name; }
			//get if serializable
			virtual bool serializable() const override { return m_serializable; }
			
		private:


			//init
			read_only() {}
			read_only(const std::string& name) { m_name = name; }
			//init + value
			template < typename I > 
			read_only(const std::string& name, const I& arg, bool serializable=true) 
			{ 
				m_name = name;
				m_data = T(arg); 
				m_serializable = serializable;
			}
			//set value
			template < typename I >
			T operator=(const I& arg) 
			{ 
				m_data = T(arg); 
				return m_data; 
			}
			
			//get (no const) value
			T& get() { return m_data; }

			//name/data/serializable
			std::string m_name;
			T m_data;
			bool m_serializable { true };

			friend class Parameters;

		};

		struct ParameterInfo
		{
			const generic_read_only*		  m_associated_variable;
			std::string				   		  m_description;
			std::vector< std::string  > 	  m_arg_key;
			std::function< bool(Arguments&) > m_action;

			ParameterInfo() { }

			ParameterInfo(  const generic_read_only&			associated_variable
						  , const std::string&				   	description
						  , const std::vector< std::string  >& 	arg_key
						  , std::function< bool(Arguments&) >   action
						  )
			: m_associated_variable(&associated_variable)
			, m_description(description)
			, m_arg_key(arg_key)
			, m_action(action)
			{				
			}

			ParameterInfo(  const std::string&				   	description
						  , const std::vector< std::string  >& 	arg_key
						  , std::function< bool(Arguments&) >   action
						  )
			: m_associated_variable(nullptr)
			, m_description(description)
			, m_arg_key(arg_key)
			, m_action(action)
			{				
			}

			bool has_an_associated_variable() const 
			{
				return m_associated_variable != nullptr;
			}

			bool serializable() const
			{
				return has_an_associated_variable() && m_associated_variable->serializable();
			}
		};

		read_only<std::string>			m_dataset_filename        { "dataset" };
		read_only<std::string> 			m_output_filename         { "output"  };
		read_only<std::string>		    m_runtime_output_type     { "runtime_output",            "bench",    true /*false?*/ };
		read_only<bool>				    m_compute_test_per_pass   { "compute_test_per_pass",    bool(true),  true /*false?*/ };
		read_only<bool>				    m_serialize_neural_network{ "serialize_neural_network", bool(true),  false /*true?*/ };
		
		read_only<unsigned int>	         m_seed          { "seed", (unsigned int)(std::random_device{}())  };

		read_only<size_t>	             m_generations   { "generation", size_t(1000)  };
		read_only<size_t>	             m_sub_gens      { "sub_gens",size_t(100)   };
		read_only<size_t>	             m_np            { "number_parents",size_t(12)    };
		read_only<Scalar>	             m_default_f     { "f_default",Scalar(1.0)   };
		read_only<Scalar>	             m_default_cr    { "cr_default",Scalar(1.0)   };
		//JDE
		read_only<Scalar>	             m_jde_f         { "f_jde", Scalar(0.1)   };
		read_only<Scalar>	             m_jde_cr        { "cr_jde", Scalar(0.1)   };
		//JADE/SHADE
		read_only<size_t>	             m_archive_size { "archive_size", size_t(0) };
		read_only<Scalar>	             m_f_cr_adapt   { "f_cr_adapt", Scalar(0.1) };
		//SHADE
		read_only<size_t>	             m_shade_h      { "shade_h", size_t(10) };
		//Current to P best
		read_only<Scalar>	             m_perc_of_best { "perc_of_best", Scalar(0.1) };
		//DEGL
		read_only<Scalar>                m_degl_scalar_weight{ "degl_scalar_weight",Scalar(0.5) };
		read_only<size_t>                m_degl_neighborhood { "degl_neighborhood", Scalar(1)   };

		read_only<Scalar>	             m_clamp_max     { "clamp_max",  Scalar( 10.0) };
		read_only<Scalar>	             m_clamp_min     { "clamp_min",  Scalar(-10.0) };
		read_only<Scalar>	             m_range_max     { "random_max", Scalar( 1.0 ) };
		read_only<Scalar>	             m_range_min     { "random_min", Scalar(-1.0)  };
		read_only<bool>	                 m_restart_enable{ "restart_enable", bool(true), false };
		read_only<long>	                 m_restart_count { "restart_count", size_t(2)    };
		read_only<Scalar>	             m_restart_delta { "restart_delta", Scalar(0.02) };
		read_only<int>	                 m_threads_omp   { "threads_omp", size_t(2) };
		read_only<size_t>	             m_threads_pop   { "threads_pop", size_t(2) };
		read_only<std::string>           m_mutation_type { "mutation","rand/1" };
		read_only<std::string>           m_crossover_type{ "crossover","bin" };
		read_only<std::string>           m_evolution_type{ "evolution_method","JDE" };
		read_only< std::vector<int> >    m_hidden_layers { "hidden_layers" /* , none */ };

		//params info
		std::vector< ParameterInfo >     m_params_info;
	
		Parameters();
		Parameters(int nargs, const char **vargs, bool jump_first = true);

		void get_params_from_args(int nargs, const char **vargs, bool jump_first = true);

	private:

		static bool compare_n_args(const std::vector< std::string >& keys, const char* arg);
		static std::string return_n_space(size_t n);
		std::string make_help() const;


	};

}