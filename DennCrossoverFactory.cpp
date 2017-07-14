#include "DennCrossover.h"
#include "DennAlgorithm.h"
#include <algorithm>
#include <sstream>
#include <iterator>

namespace Denn
{
	//EvolutionMethod
	Crossover::Crossover(const DennAlgorithm& algorithm)
	: m_algorithm(algorithm)
	{
	}
	//easy access
	const Parameters& Crossover::parameters()            const { return m_algorithm.parameters();        }

	const EvolutionMethod& Crossover::evolution_method() const	{ return m_algorithm.evolution_method();  }

	Random& Crossover::population_random(size_t i)       const { return m_algorithm.population_random(i);}

	Random& Crossover::main_random()					 const { return m_algorithm.main_random(); }

	Random& Crossover::random(size_t i)			         const { return m_algorithm.random(i); }

	Random& Crossover::random()					         const { return m_algorithm.random(); }
	//map
	std::unique_ptr< std::map< std::string, CrossoverFactory::CreateObject > > CrossoverFactory::m_cmap;
	//public
	Crossover::SPtr CrossoverFactory::create(const std::string& name, const DennAlgorithm& algorithm)
	{
		//map is alloc?
		if (!m_cmap) return nullptr;
		//find
		auto it = m_cmap->find(name);
		//return
		return it->second(algorithm);
	}
	void CrossoverFactory::append(const std::string& name, CrossoverFactory::CreateObject fun, size_t size)
	{
		//alloc
		if (!m_cmap) m_cmap = std::move(std::make_unique< std::map< std::string, CrossoverFactory::CreateObject > >());
		//add
		m_cmap->operator[](name) = fun;
	}
	//list of methods
	std::vector< std::string > CrossoverFactory::list_of_crossovers()
	{
		std::vector< std::string > list;
		for (const auto & pair : *m_cmap) list.push_back(pair.first);
		return list;
	}
	std::string                CrossoverFactory::names_of_crossovers(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_crossovers();
		std::copy(list.begin(), list.end()-1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	bool CrossoverFactory::exists(const std::string& name)
	{
		//find
		auto it = m_cmap->find(name);
		//return 
		return it != m_cmap->end();
	}
}