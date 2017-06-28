#pragma once
#include "Config.h"
#include "DennPopulation.h"

namespace Denn
{	
	//parameters
	class Parameters;
	//mutation
    class Mutation : public std::enable_shared_from_this< Mutation >
	{ 
		public:
		//ref to Mutation
		using SPtr = std::shared_ptr<Mutation>;
		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }
		//Mutation
		Mutation(const Parameters& parameters);
		//operation
		virtual void operator()(const Population& population,int id_target,Individual& output)= 0; 

		protected:
		//utils
		std::function<Scalar(Scalar)> m_clamp;
		//attributes
		const Parameters& m_parameters;
	};

	//class factory of Mutation methods
	class MutationFactory
	{

	public:
		//Mutation classes map
		typedef Mutation::SPtr(*CreateObject)(const Parameters& parameters);

		//public
		static Mutation::SPtr create(const std::string& name, const Parameters& parameters);
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

		static Mutation::SPtr create(const Parameters& parameters)
		{
			return (std::make_shared< T >(parameters))->get_ptr();
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