#pragma once
#include "Config.h"
#include "DennPopulation.h"

namespace Denn
{
	//parameters
	class Parameters;
	//mutation
	class Crossover : public std::enable_shared_from_this< Crossover >
	{
		public:
		//Crossover info
		Crossover(const Parameters& parameters) : m_parameters(parameters) {}
		//ref to Crossover
		using SPtr = std::shared_ptr<Crossover>;
		//operator
		virtual void operator()(const Population& population, int id_target, Individual& mutant)= 0;
		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }
		//attributes
		const Parameters& m_parameters;
	};

	//class factory of Crossover methods
	class CrossoverFactory
	{

	public:
		//Crossover classes map
		typedef Crossover::SPtr(*CreateObject)(const Parameters& parameters);

		//public
		static Crossover::SPtr create(const std::string& name, const Parameters& parameters);
		static void append(const std::string& name, CreateObject fun, size_t size);

		//list of methods
		static std::vector< std::string > list_of_crossovers();
		static std::string names_of_crossovers(const std::string& sep = ", ");
		static bool exists(const std::string& name);

	protected:

		static std::unique_ptr< std::map< std::string, CreateObject > > m_cmap;

	};

	//class used for static registration of a object class
	template<class T>
	class CrossoverItem
	{

		static Crossover::SPtr create(const Parameters& parameters)
		{
			return (std::make_shared< T >(parameters))->get_ptr();
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