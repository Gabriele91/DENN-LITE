#pragma once
#include "Config.h"
#include "DennRandom.h"
#include "DennVariant.h"
#include "DennArguments.h"
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace Denn
{
	//////////////////////////////////////////////
	class  Parameters;
	class  GenericReadOnly;
	struct ParameterDomain;
	struct ParameterOwner;
	struct ParameterInfo;
	//////////////////////////////////////////////
	class GenericReadOnly
	{
	public:
		virtual std::string name()  const = 0;
		virtual Variant variant()   const = 0;
		virtual bool serializable() const = 0;
	};

	template <class T>
	class ReadOnly : public GenericReadOnly
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
		ReadOnly() {}
		ReadOnly(const std::string& name) { m_name = name; }
		//init + value
		template < typename I >
		ReadOnly(const std::string& name, const I& arg, bool serializable = true)
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
		bool m_serializable{ true };

		//friends
		friend class Parameters;
		friend struct ParameterInfo;

	};

	struct ParameterDomain
	{
		enum TypeDomain
		{
			NONE,
			MONO,
			CHOISE,
		};
		TypeDomain  m_type{ NONE };
		std::string m_domain{ "void" };
		std::vector< std::string > m_choises;

		ParameterDomain() {}

		ParameterDomain(const std::string& domain)
			: m_type(MONO)
			, m_domain(domain)
		{
		}

		ParameterDomain(const std::string& domain, const std::vector< std::string >& choises)
			: m_type(CHOISE)
			, m_domain(domain)
			, m_choises(choises)
		{
		}
	};

	struct ParameterOwner
	{
		enum Filter
		{
			NONE,
			ALL,
			EXCEPT,
			ONLY
		};
		const Filter			     m_filter_type;
		const std::vector< Variant > m_filter_values;
		const GenericReadOnly*       m_owner_variable;

		ParameterOwner()
			: m_filter_type(NONE)
			, m_owner_variable(nullptr)
		{
		}

		ParameterOwner(const GenericReadOnly& owner)
			: m_filter_type(ALL)
			, m_owner_variable(&owner)
		{
		}

		ParameterOwner(const Filter filter, std::vector< Variant >& value, const GenericReadOnly& owner)
			: m_filter_type(filter)
			, m_filter_values(value)
			, m_owner_variable(&owner)
		{
		}
	};

	struct ParameterInfo
	{

		const GenericReadOnly*		      m_associated_variable;
		const ParameterOwner			  m_owener;
		std::string				   		  m_description;
		std::vector< std::string  > 	  m_arg_key;
		std::function< bool(Arguments&) > m_action;
		ParameterDomain                   m_domain;


		ParameterInfo() { }

		template< class T >
		ParameterInfo(ReadOnly< T >&			            associated_variable
			, const std::string&				   	description
			, const std::vector< std::string  >& 	arg_key
			, std::function< bool(Arguments&) >   action = nullptr
			, const ParameterDomain&              domain = ParameterDomain(cpp_type_to_arg_type<T>())
		)
			: m_associated_variable(&associated_variable)
			, m_description(description)
			, m_arg_key(arg_key)
			, m_domain(domain)
		{
			m_action = action ? action : [&](Arguments& args) -> bool { associated_variable = args.get<T>(); return true; };
			m_arg_key.insert(m_arg_key.begin(), "--" + associated_variable.name());
		}

		template< class T >
		ParameterInfo(ReadOnly< T >&			            associated_variable
			, const ParameterOwner			    owener
			, const std::string&				   	description
			, const std::vector< std::string  >& 	arg_key
			, std::function< bool(Arguments&) >   action = nullptr
			, const ParameterDomain&              domain = ParameterDomain(cpp_type_to_arg_type<T>())
		)
			: m_associated_variable(&associated_variable)
			, m_owener(owener)
			, m_description(description)
			, m_arg_key(arg_key)
			, m_domain(domain)
		{
			m_action = action ? action : [&](Arguments& args) -> bool { associated_variable = args.get<T>(); return true; };
			m_arg_key.insert(m_arg_key.begin(), "--" + associated_variable.name());
		}

		ParameterInfo(const std::string&				   	description
			, const std::vector< std::string  >& 	arg_key
			, std::function< bool(Arguments&) >   action
			, const ParameterDomain&              domain = ParameterDomain(std::string())
		)
			: m_associated_variable(nullptr)
			, m_description(description)
			, m_arg_key(arg_key)
			, m_action(action)
			, m_domain(domain)
		{
		}

		ParameterInfo(const std::string&				   	description
			, const ParameterOwner			    owener
			, const std::vector< std::string  >& 	arg_key
			, std::function< bool(Arguments&) >   action
			, const ParameterDomain&              domain = ParameterDomain(std::string())
		)
			: m_associated_variable(nullptr)
			, m_owener(owener)
			, m_description(description)
			, m_arg_key(arg_key)
			, m_action(action)
			, m_domain(domain)
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

	protected:

		template< class T > static const char* cpp_type_to_arg_type() { return "void"; }

	};
	//////////////////////////////////////////////
	class Parameters
	{
	public:

		ReadOnly<std::string>			m_dataset_filename        { "dataset" };
		ReadOnly<size_t>			    m_batch_size			  { "batch_size", size_t(100) };
		ReadOnly<size_t>			    m_batch_offset			  { "batch_offset", size_t(0) };
		ReadOnly<std::string> 			m_output_filename         { "output"  };
		ReadOnly<std::string>		    m_runtime_output_type     { "runtime_output",            "bench",    true /* false? */ };
		ReadOnly<std::string>		    m_runtime_output_file     { "runtime_output_file",            "",    true /* false? */ };
		ReadOnly<bool>				    m_compute_test_per_pass   { "compute_test_per_pass",    bool(true),  true /* false? */ };
		ReadOnly<bool>				    m_serialize_neural_network{ "serialize_neural_network", bool(true),  false /* true? */ };
        ReadOnly<bool>                  m_use_validation          { "use_validation",           bool(true),  true /* false? */ };

		ReadOnly<unsigned int>	         m_seed			 { "seed", (unsigned int)(std::random_device{}())  };

		ReadOnly<std::string>	         m_instance      { "instance", "default" };
		ReadOnly<size_t>	             m_generations   { "generation", size_t(1000)  };
		ReadOnly<size_t>	             m_sub_gens      { "sub_gens"  , size_t(100)   };
		ReadOnly<size_t>	             m_np            { "number_parents",size_t(16) };
		//DE
		ReadOnly<Scalar>	             m_default_f     { "f_default",Scalar(1.0)   };
		ReadOnly<Scalar>	             m_default_cr    { "cr_default",Scalar(1.0)   };
		//JDE
		ReadOnly<Scalar>	             m_jde_f         { "f_jde", Scalar(0.1)   };
		ReadOnly<Scalar>	             m_jde_cr        { "cr_jde", Scalar(0.1)   };
		//JADE/SHADE/LSHADE
		ReadOnly<size_t>	             m_archive_size { "archive_size", size_t(0) };
		ReadOnly<Scalar>	             m_f_cr_adapt   { "f_cr_adapt", Scalar(0.1) };
		//SHADE/LSHADE
		ReadOnly<size_t>	             m_shade_h      { "shade_h", size_t(10) };
		//LSHADE
		ReadOnly<size_t>	             m_min_np              { "min_number_parents"     ,size_t(4)   };
		ReadOnly<size_t>	             m_max_nfe             { "max_number_fitness_eval",size_t(160) };
		ReadOnly<Scalar>	             m_mu_cr_terminal_value{ "mu_cr_terminal_value"   ,Scalar(0.0) };
		//Current to P best
		ReadOnly<Scalar>	             m_perc_of_best { "perc_of_best", Scalar(0.1) };
		//DEGL
		ReadOnly<Scalar>                m_degl_scalar_weight{ "degl_scalar_weight",Scalar(0.5) };
		ReadOnly<size_t>                m_degl_neighborhood { "degl_neighborhood", Scalar(1)   };
		//TDE
		ReadOnly<Scalar>				 m_trig_m        { "trig_m", Scalar(0.05) };
		//DE common info
		ReadOnly<Scalar>	             m_clamp_max     { "clamp_max",  Scalar( 10.0) };
		ReadOnly<Scalar>	             m_clamp_min     { "clamp_min",  Scalar(-10.0) };
		ReadOnly<Scalar>	             m_range_max     { "random_max", Scalar( 1.0 ) };
		ReadOnly<Scalar>	             m_range_min     { "random_min", Scalar(-1.0)  };
		ReadOnly<bool>	                 m_restart_enable{ "restart_enable", bool(false), false };
		ReadOnly<long>	                 m_restart_count { "restart_count", size_t(-1)    };
		ReadOnly<Scalar>	             m_restart_delta { "restart_delta", Scalar(0.001) };
		ReadOnly<size_t>	             m_threads_omp   { "threads_omp", size_t(2) };
		ReadOnly<size_t>	             m_threads_pop   { "threads_pop", size_t(2) };
		ReadOnly<size_t>	             m_history_size  { "history_size", size_t(1) };
		//type of DE
		ReadOnly<std::string>           m_mutation_type { "mutation","rand/1" };
		ReadOnly<std::string>           m_crossover_type{ "crossover","bin" };
		ReadOnly<std::string>           m_evolution_type    { "evolution_method","JDE" };
		ReadOnly<std::string>           m_sub_evolution_type{ "sub_evolution_method","JDE" };
		//network
		ReadOnly<Scalar>					   m_learning_rate		   { "learning_rate", Scalar(0.05) };
		ReadOnly<Scalar>					   m_regularize			   { "regularize", Scalar(0.0) };
		ReadOnly< std::vector<unsigned int> > m_hidden_layers         { "hidden_layers"         /* , none */ };
		ReadOnly< std::vector<std::string> >  m_active_functions      { "active_functions"      /* , none */ };
		ReadOnly< std::string >               m_output_active_function{ "output_active_function" ,  "softmax" };
		//nram attribute
		ReadOnly< size_t >					   m_max_int			   { "max_int"                  , size_t(10) };
		ReadOnly< size_t >					   m_n_registers		   { "n_registers"              , size_t(4) };
		ReadOnly< size_t >					   m_time_steps			   { "time_steps"               , size_t(1) };
		ReadOnly< std::string >               m_task				   { "task"                  /* , none */ };
		ReadOnly< std::vector<std::string> >  m_gates				   { "gates"                 /* , none */ };
		//nram test 
		ReadOnly< size_t >					   m_n_test				   { "n_test", size_t(1) };
		//params info
		std::vector< ParameterInfo >     m_params_info;
	
		Parameters();
		Parameters(int nargs, const char **vargs, bool jump_first = true);

		void get_params_from_args(int nargs, const char **vargs, bool jump_first = true);

	private:

		static bool compare_n_args(const std::vector< std::string >& keys, const char* arg);
		static std::string return_n_space(size_t n);
		std::string make_help() const;
        std::string make_help_json() const;

	};

}
