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

	//crossover
	class Crossover : public std::enable_shared_from_this< Crossover >
	{
		public:
		//Crossover info
		Crossover(const DennAlgorithm& algorithm);
		//ref to Crossover
		using SPtr = std::shared_ptr<Crossover>;
		//operator
		virtual void operator()(const Population& population, size_t id_target, Individual& mutant)= 0;
		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }

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

		#ifndef RANDOM_SAFE_CROSSOVER
		Random& main_random()  const;
		Random& random()  const;
		#endif
	};

	//class factory of Crossover methods
	class CrossoverFactory
	{

	public:
		//Crossover classes map
		typedef Crossover::SPtr(*CreateObject)(const DennAlgorithm& algorithm);

		//public
		static Crossover::SPtr create(const std::string& name, const DennAlgorithm& algorithm);
		static void append(const std::string& name, CreateObject fun, size_t size);

		//list of methods
		static std::vector< std::string > list_of_crossovers();
		static std::string names_of_crossovers(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	};

	//class used for static registration of a object class
	template<class T>
	class CrossoverItem
	{

		static Crossover::SPtr create(const DennAlgorithm& algorithm)
		{
			return (std::make_shared< T >(algorithm))->get_ptr();
		}

		CrossoverItem(const std::string& name, size_t size)
		{
			CrossoverFactory::append(name, CrossoverItem<T>::create, size);
		}

	public:


		static CrossoverItem<T>& instance(const std::string& name, size_t size)
		{
			static CrossoverItem<T> objectItem(name, size);
			return objectItem;
		}

	};


	#define REGISTERED_CROSSOVER(class_,name_)\
	namespace\
	{\
		static const CrossoverItem<class_>& _Denn_ ## class_ ## _CrossoverItem= CrossoverItem<class_>::instance( name_, sizeof(class_) );\
	}

}