#pragma once
#include "Config.h"
#include "DennPopulation.h"

namespace Denn
{	
	//dec class
	class DennAlgorithm;
	class EvolutionMethod;
	class Parameters;
	class Random;

	//mutation
    class Mutation : public std::enable_shared_from_this< Mutation >
	{ 
		public:
		//ref to Mutation
		using SPtr = std::shared_ptr<Mutation>;
		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }
		//Mutation
		Mutation(const DennAlgorithm& algorithm);
		//operation
		virtual void operator()(const Population& population,size_t id_target,Individual& output)= 0; 
		//required sort (default false)
		virtual bool required_sort() const { return false; }

		protected:
		
		//attributes
		const DennAlgorithm& m_algorithm;

		//easy access		
		const Parameters& parameters() const;
		const EvolutionMethod& evolution_method() const;
	
		const size_t current_np() const;
		const DoubleBufferPopulation& population() const;

		Random& population_random(size_t i)  const;
		Random& random(size_t i)  const;

		#ifndef RANDOM_SAFE_MUTATION
		Random& main_random()  const;
		Random& random()  const;
		#endif
	};

	//class factory of Mutation methods
	class MutationFactory
	{

	public:
		//Mutation classes map
		typedef Mutation::SPtr(*CreateObject)(const DennAlgorithm& algorithm);

		//public
		static Mutation::SPtr create(const std::string& name, const DennAlgorithm& algorithm);
		static void append(const std::string& name, CreateObject fun, size_t size);

		//list of methods
		static std::vector< std::string > list_of_mutations();
		static std::string names_of_mutations(const std::string& sep = ", ");
		
		//info
		static bool exists(const std::string& name);

	protected:

		static std::unique_ptr< std::map< std::string, CreateObject > > m_cmap;

	};

	//class used for static registration of a object class
	template<class T>
	class MutationItem
	{

		static Mutation::SPtr create(const DennAlgorithm& algorithm)
		{
			return (std::make_shared< T >(algorithm))->get_ptr();
		}

		MutationItem(const std::string& name, size_t size)
		{
			MutationFactory::append(name, MutationItem<T>::create, size);
		}

	public:


		static MutationItem<T>& instance(const std::string& name, size_t size)
		{
			static MutationItem<T> objectItem(name, size);
			return objectItem;
		}

	};


	#define REGISTERED_MUTATION(class_,name_)\
	namespace\
	{\
		static const MutationItem<class_>& _Denn_ ## class_ ## _MutationItem= MutationItem<class_>::instance( name_, sizeof(class_) );\
	}
}