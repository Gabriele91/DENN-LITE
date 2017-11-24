#include "DennEvaluation.h"
#include "DennAlgorithm.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn
{
	//EvolutionMethod
	Evaluation::Evaluation(const DennAlgorithm& algorithm)
	: m_algorithm(algorithm)
	{
	}
	//easy access
	const Parameters& Evaluation::parameters()            const { return m_algorithm.parameters();        }
	const EvolutionMethod& Evaluation::evolution_method() const	{ return m_algorithm.evolution_method();  }

	const size_t Evaluation::current_np()                  const   { return m_algorithm.current_np(); }
	const DoubleBufferPopulation& Evaluation::population() const   { return m_algorithm.population(); }

	Random& Evaluation::population_random(size_t i)       const { return m_algorithm.population_random(i);}
	Random& Evaluation::random(size_t i)			         const { return m_algorithm.random(i); }

	#ifndef RANDOM_SAFE_CROSSOVER
	Random& Evaluation::main_random()					 const { return m_algorithm.main_random(); }
	Random& Evaluation::random()					         const { return m_algorithm.random(); }
	#endif
	//map
	static std::map< std::string, EvaluationFactory::CreateObject >& c_map()
	{
		static std::map< std::string, EvaluationFactory::CreateObject > c_map;
		return c_map;
	}
	//public
	Evaluation::SPtr EvaluationFactory::create(const std::string& name, const DennAlgorithm& algorithm)
	{
		//find
		auto it = c_map().find(name);
		//return
		return it->second(algorithm);
	}
	void EvaluationFactory::append(const std::string& name, EvaluationFactory::CreateObject fun, size_t size)
	{
		//add
		c_map()[name] = fun;
	}
	//list of methods
	std::vector< std::string > EvaluationFactory::list_of_evaluators()
	{
		std::vector< std::string > list;
		for (const auto & pair : c_map()) list.push_back(pair.first);
		return list;
	}
	std::string EvaluationFactory::names_of_evaluators(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_evaluators();
		std::copy(list.begin(), list.end()-1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	bool EvaluationFactory::exists(const std::string& name)
	{
		//find
		auto it = c_map().find(name);
		//return 
		return it != c_map().end();
	}
}
