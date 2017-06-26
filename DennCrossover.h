#pragma once
#include "Config.h"
#include "DennPopulation.h"

namespace Denn
{
	class Crossover : public std::enable_shared_from_this< Crossover >
	{
		public:
		//ref to individual
		using SPtr = std::shared_ptr<Crossover>;
		//operator
		virtual void operator()(Individual& target,Individual& mutant)= 0;
		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }
	};

	//class factory of Crossover methods
	class CrossoverFactory
	{

	public:
		//Crossover classes map
		typedef Crossover::SPtr(*CreateObject)();

		//public
		static Crossover::SPtr create(const std::string& name);
		static void append(const std::string& name, CreateObject fun, size_t size);

		//list of methods
		static std::vector< std::string > list_of_crossovers();
		static std::string names_of_crossovers(const std::string& sep = ", ");

	protected:

		static std::unique_ptr< std::map< std::string, CreateObject > > m_cmap;

	};

	//class used for static registration of a object class
	template<class T>
	class CrossoverItem
	{

		static Crossover::SPtr create()
		{
			return (std::make_shared< T >())->get_ptr();
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


	#define REGISTERED_CROSSOVER(classname)\
	namespace\
	{\
		static const CrossoverItem<classname>& _Denn_ ## classname ## _ObjectItem= CrossoverItem<classname>::instance( #classname, sizeof(classname) );\
	}

	#define CREATE_CROSSOVER_METHOD(classname, target, mutant, ...)\
	class classname : public Crossover\
	{\
	public:\
		virtual void operator()(Individual& target, Individual& mutant)\
		{\
			__VA_ARGS__\
		}\
	};\
	REGISTERED_CROSSOVER(classname);
}