#pragma once
#include "Config.h"
#include "DennIndividual.h"
#include "DennDataset.h"

namespace Denn
{
	//dec class
	class DennAlgorithm;
	class EvolutionMethod;
	class Parameters;
	class Random;
	class DataSet;
	class Individual;
	class DoubleBufferPopulation;

    //Evaluate function
	class Evaluation : public std::enable_shared_from_this<Evaluation>
	{		
    public:
        //ref to Evaluation
        using SPtr = std::shared_ptr<Evaluation>;
		//return ptr
		SPtr get_ptr() { return this->shared_from_this(); }
		//Evaluation info
        Evaluation(const DennAlgorithm& algorithm);
        //methods
        virtual bool minimize() const = 0;
        virtual Scalar operator () (const Individual&, const DataSet&) = 0;	
    
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
	};

	//class factory of Evaluation methods
	class EvaluationFactory
	{

	public:
		//Evaluation classes map
		typedef Evaluation::SPtr(*CreateObject)(const DennAlgorithm& algorithm);

		//public
		static Evaluation::SPtr create(const std::string& name, const DennAlgorithm& algorithm);
		static void append(const std::string& name, CreateObject fun, size_t size);

		//list of methods
		static std::vector< std::string > list_of_evaluators();
		static std::string names_of_evaluators(const std::string& sep = ", ");

		//info
		static bool exists(const std::string& name);

	};

	//class used for static registration of a object class
	template<class T>
	class EvaluationItem
	{

		static Evaluation::SPtr create(const DennAlgorithm& algorithm)
		{
			return (std::make_shared< T >(algorithm))->get_ptr();
		}

		EvaluationItem(const std::string& name, size_t size)
		{
			EvaluationFactory::append(name, EvaluationItem<T>::create, size);
		}

	public:


		static EvaluationItem<T>& instance(const std::string& name, size_t size)
		{
			static EvaluationItem<T> objectItem(name, size);
			return objectItem;
		}

	};


	#define REGISTERED_EVALUATION(class_,name_)\
	namespace\
	{\
		static const EvaluationItem<class_>& _Denn_ ## class_ ## _EvaluationItem= EvaluationItem<class_>::instance( name_, sizeof(class_) );\
	}
}