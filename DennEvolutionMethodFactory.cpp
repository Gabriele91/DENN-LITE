#include "DennEvolutionMethod.h"
#include "DennParameters.h"
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

	//map
	std::unique_ptr< std::map< std::string, EvolutionMethodFactory::CreateObject > > EvolutionMethodFactory::m_cmap;
	//public
	EvolutionMethod::SPtr EvolutionMethodFactory::create(const std::string& name, const DennAlgorithm& algorithm)
	{
		//map is alloc?
		if (!m_cmap) return nullptr;
		//find
		auto it = m_cmap->find(name);
		//return
		return it->second(algorithm);
	}
	void EvolutionMethodFactory::append(const std::string& name, EvolutionMethodFactory::CreateObject fun, size_t size)
	{
		//alloc
		if (!m_cmap) m_cmap = std::move(std::make_unique< std::map< std::string, EvolutionMethodFactory::CreateObject > >());
		//add
		m_cmap->operator[](name) = fun;
	}
	//list of methods
	std::vector< std::string > EvolutionMethodFactory::list_of_of_evolution_methods()
	{
		std::vector< std::string > list;
		for (const auto & pair : *m_cmap) list.push_back(pair.first);
		return list;
	}
	std::string  EvolutionMethodFactory::names_of_evolution_methods(const std::string& sep)
	{
		std::stringstream sout;
		auto list = list_of_of_evolution_methods();
		std::copy(list.begin(), list.end() - 1, std::ostream_iterator<std::string>(sout, sep.c_str()));
		sout << *(list.end() - 1);
		return sout.str();
	}
	//info
	bool EvolutionMethodFactory::exists(const std::string& name)
	{
		//find
		auto it = m_cmap->find(name);
		//return 
		return it != m_cmap->end();
	}
}