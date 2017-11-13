#include "DennEvolutionMethod.h"
#include "DennAlgorithm.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn
{
	//EvolutionMethod
	EvolutionMethod::EvolutionMethod(const DennAlgorithm& algorithm)
	: m_algorithm(algorithm)
	{
	}
    
    //virtual
    void EvolutionMethod::start() {};
    void EvolutionMethod::start_a_gen_pass(DoubleBufferPopulation& population) {};
    void EvolutionMethod::start_a_subgen_pass(DoubleBufferPopulation& population) {};
    void EvolutionMethod::end_a_subgen_pass(DoubleBufferPopulation& population) {};
    void EvolutionMethod::end_a_gen_pass(DoubleBufferPopulation& population) {};
    bool EvolutionMethod::can_reset()            { return true; }
    bool EvolutionMethod::best_from_validation() { return *parameters().m_use_validation; }
    const VariantRef EvolutionMethod::get_context_data() const { return VariantRef(); }
    
	//easy access
	const Parameters& EvolutionMethod::parameters()            const { return m_algorithm.parameters();        }
	const EvolutionMethod& EvolutionMethod::evolution_method() const { return m_algorithm.evolution_method();  }

	const size_t EvolutionMethod::current_np()                  const   { return m_algorithm.current_np(); }
	const DoubleBufferPopulation& EvolutionMethod::population() const   { return m_algorithm.population(); }

	Random& EvolutionMethod::population_random(size_t i)       const { return m_algorithm.population_random(i);}
	Random& EvolutionMethod::random(size_t i)			       const { return m_algorithm.random(i); }

	Random& EvolutionMethod::main_random()					   const { return m_algorithm.main_random(); }

	#ifndef RANDOM_SAFE_EVOLUTION_METHOD
	Random& EvolutionMethod::random()					       const { return m_algorithm.random(); }
	#endif
	//map
	static std::map< std::string, EvolutionMethodFactory::CreateObject >& em_map()
	{
		static std::map< std::string, EvolutionMethodFactory::CreateObject > em_map;
		return em_map;
	}
	//public
	EvolutionMethod::SPtr EvolutionMethodFactory::create(const std::string& name, const DennAlgorithm& algorithm)
	{
		//find
		auto it = em_map().find(name);
		//return
		return it->second(algorithm);
	}
	void EvolutionMethodFactory::append(const std::string& name, EvolutionMethodFactory::CreateObject fun, size_t size)
	{
		//add
		em_map()[name] = fun;
	}
	//list of methods
	std::vector< std::string > EvolutionMethodFactory::list_of_evolution_methods()
	{
		std::vector< std::string > list;
		for (const auto & pair : em_map()) list.push_back(pair.first);
		return list;
	}
	std::string  EvolutionMethodFactory::names_of_evolution_methods(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_evolution_methods();
		std::copy(list.begin(), list.end() - 1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	//info
	bool EvolutionMethodFactory::exists(const std::string& name)
	{
		//find
		auto it = em_map().find(name);
		//return 
		return it != em_map().end();
	}
}
