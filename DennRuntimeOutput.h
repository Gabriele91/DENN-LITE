#pragma once
#include "Config.h"

namespace Denn
{
	//
	class Parameters;
	//
	class RuntimeOutput : public std::enable_shared_from_this< RuntimeOutput >
	{
	protected:

		struct Initialization
		{
			size_t m_n_g_pass{ 0 };
			size_t m_n_s_pass{ 0 };
		};
		struct GlobalPassInfo
		{
			size_t m_g_pass         { size_t(-1) };
			size_t m_n_restart      {  0         };
			double m_validation_eval{ 0.0        };
			double m_target_eval    { 0.0        };
			double m_test_eval      { 0.0        };	
		};
		struct PassInfo
		{
			size_t m_g_pass             { size_t(-1) };
			size_t m_s_pass             { 0          };
			size_t m_minimum_on_pop_id  { 0          };
			double m_minimum_on_pop_eval{ 0          };
		};
		struct EndOfIterations
		{
			double m_test_result;
		};

		PassInfo m_pass;
		GlobalPassInfo    m_global_pass;
		Initialization    m_initialization;
		EndOfIterations   m_end_of_iterations;
		std::ostream&     m_stream;
		const Parameters& m_params;

	public:
		using SPtr = std::shared_ptr<RuntimeOutput>;

		RuntimeOutput(std::ostream& stream,const Parameters& params):m_stream(stream),m_params(params) {}

		SPtr get_ptr(){ return shared_from_this(); }

		virtual bool is_enable()	         { return false;             }
		virtual bool is_enable_best()	     { return this->is_enable(); }
		virtual bool is_enable_pass()	     { return this->is_enable(); }
		virtual bool is_enable_compute_test(){ return this->is_enable(); }
		virtual std::ostream&     output()     const { return m_stream;      }
		virtual const Parameters& parameters() const { return m_params;      }

		virtual void start()
		{
			//none
		}

		virtual void update_best()
		{
			//none
		}

		virtual void update_pass()
		{
			//none
		}

		virtual void end()
		{ 
			//none
		}

		virtual void send_start(
			size_t n_g_pass
			, size_t n_s_pass
		)
		{
			m_initialization.m_n_g_pass = n_g_pass;
			m_initialization.m_n_s_pass = n_s_pass;
			//call start
			start();
		}

		virtual void send_end(
			double test_result
		)
		{
			m_end_of_iterations.m_test_result = test_result;
			//call start
			end();
		}

		virtual void send_best(
			size_t g_pass
			, size_t n_restart
			, double validation_eval
			, double target_eval
			, double test_eval
		)
		{
			m_global_pass.m_g_pass = g_pass;
			m_global_pass.m_n_restart = n_restart;
			m_global_pass.m_validation_eval = validation_eval;
			m_global_pass.m_target_eval = target_eval;
			m_global_pass.m_test_eval = test_eval;
			//
			update_best();
		}

		virtual void sent_pass(
			size_t g_pass
			, size_t s_pass
			, size_t minimum_on_pop_id
			, double minimum_on_pop_eval
		)
		{
			m_pass.m_g_pass = g_pass; //is equal to m_global_pass.m_g_pass + 1
			m_pass.m_s_pass = s_pass;
			m_pass.m_minimum_on_pop_id = minimum_on_pop_id;
			m_pass.m_minimum_on_pop_eval = minimum_on_pop_eval;
			//
			update_pass();
		}

		virtual void write_global_pass() 
		{
			output() << (m_initialization.m_n_s_pass * (m_global_pass.m_g_pass+1));
		}

		virtual void write_local_pass() 
		{
			output() << (m_initialization.m_n_s_pass * m_pass.m_g_pass + m_pass.m_s_pass);
		}

		virtual void write_global_best(
			const std::string& open="[ ", 
			const std::string& separetor=", ", 
			const std::string& closer=" ]"
		) 
		{
			output() 
			<< open 
			<< m_global_pass.m_validation_eval
			<< separetor 
			<< m_global_pass.m_target_eval
			<< separetor 
			<< m_global_pass.m_test_eval
			<< closer;
		}

		virtual void write_pass_best(
			const std::string& open="[ ", 
			const std::string& separetor=", ", 
			const std::string& closer=" ]"
		) 
		{
			output() 
			<< open 
			<< m_pass.m_minimum_on_pop_id
			<< separetor 
			<< m_pass.m_minimum_on_pop_eval 
			<< closer;
		}

	};

	//class factory of RuntimeOutput
	class RuntimeOutputFactory
	{

	public:
		//Crossover classes map
		typedef RuntimeOutput::SPtr(*CreateObject)(std::ostream& stream,const Parameters& params);

		//public
		static RuntimeOutput::SPtr create(const std::string& name, std::ostream& stream,const Parameters& params);
		static void append(const std::string& name, CreateObject fun, size_t size);

		//list of methods
		static std::vector< std::string > list_of_runtime_outputs();
		static std::string names_of_runtime_outputs(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	protected:

		static std::unique_ptr< std::map< std::string, CreateObject > > m_cmap;

	};

	//class used for static registration of a object class
	template<class T>
	class RuntimeOutputItem
	{

		static RuntimeOutput::SPtr create(std::ostream& stream,const Parameters& params)
		{
			return (std::make_shared< T >(stream,params))->get_ptr();
		}

		RuntimeOutputItem(const std::string& name, size_t size)
		{
			RuntimeOutputFactory::append(name, RuntimeOutputItem<T>::create, size);
		}

	public:


		static RuntimeOutputItem<T>& instance(const std::string& name, size_t size)
		{
			static RuntimeOutputItem<T> objectItem(name, size);
			return objectItem;
		}

	};


	#define REGISTERED_RUNTIME_OUTPUT(class_,name_)\
	namespace\
	{\
		static const RuntimeOutputItem<class_>& _Denn_ ## class_ ## _RuntimeOutputItem= RuntimeOutputItem<class_>::instance( name_, sizeof(class_) );\
	}

}