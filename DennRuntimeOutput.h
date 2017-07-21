#pragma once
#include "Config.h"

namespace Denn
{
	//
	class Parameters;
	class DennAlgorithm;
	class EvolutionMethod;
	class DoubleBufferPopulation;
	//
	class RuntimeOutput : public std::enable_shared_from_this< RuntimeOutput >
	{
	protected:
		//attributes
		const DennAlgorithm& m_algorithm;
		std::ostream&        m_stream;

	public:
		using SPtr = std::shared_ptr<RuntimeOutput>;

		RuntimeOutput
		(
		  std::ostream& stream
		, const DennAlgorithm& algorithm
		)
		:m_stream(stream)
		,m_algorithm(algorithm) 
		{
		}

		SPtr get_ptr(){ return shared_from_this(); }
		//Output / Parameters
		virtual std::ostream&     output()     const { return m_stream;      }
		//easy access		
		const EvolutionMethod&        evolution_method() const;
		const Parameters&             parameters()       const;	
		const DoubleBufferPopulation& population()       const;
		const size_t                  current_np()       const;

		virtual void start()
		{
			//none
		}

		virtual void start_a_pass()
		{
			//none
		}

		virtual void start_a_sub_pass()
		{
			//none
		}		

		virtual void end_a_sub_pass()
		{
			//none
		}		
		
		virtual void restart()
		{
			//none
		}		

		virtual void end_a_pass()
		{
			//none
		}

		virtual void end()
		{ 
			//none
		}

	};

	//class factory of RuntimeOutput
	class RuntimeOutputFactory
	{

	public:
		//Crossover classes map
		typedef RuntimeOutput::SPtr(*CreateObject)(std::ostream& stream,const DennAlgorithm& algorithm);

		//public
		static RuntimeOutput::SPtr create(const std::string& name, std::ostream& stream,const DennAlgorithm& algorithm);
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

		static RuntimeOutput::SPtr create(std::ostream& stream,const DennAlgorithm& algorithm)
		{
			return (std::make_shared< T >(stream,algorithm))->get_ptr();
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